#include "synth.h"
#include "voice.h"
#include "frequencies.h"

using namespace std;

AudioOutputI2S  audioOut;

Synth::Synth() {

  this->waveform1 = 0;
  this->waveform2 = 0;

  this->detune = 1;

  this->sustained = false;

  int patchCordIndex = 0;

  this->finalMixer = new AudioMixer4();
  this->filter = new AudioFilterStateVariable();

  this->patchCords[patchCordIndex++] = new AudioConnection(*this->finalMixer, 0, *this->filter, 0);

  this->lfo = new AudioSynthWaveform();
  this->lfo->begin(1.0, 2.0, WAVEFORM_SINE);

  this->filterSignalMixer = new AudioMixer4();
  this->filterSignalMixer->gain(0, 1.0);
  this->filterSignalMixer->gain(1, 1);
  this->patchCords[patchCordIndex++] = new AudioConnection(*this->lfo, 0, *this->filterSignalMixer, 0);

  this->filterSignalDc = new AudioSynthWaveformDc();
  this->filterSignalDc->amplitude(1);
  this->filterSignalEnvelope = new AudioEffectEnvelope();

  this->patchCords[patchCordIndex++] = new AudioConnection(*this->filterSignalDc, 0, *this->filterSignalEnvelope, 0);
  this->patchCords[patchCordIndex++] = new AudioConnection(*this->filterSignalEnvelope, 0, *this->filterSignalMixer, 1);
  this->patchCords[patchCordIndex++] = new AudioConnection(*this->filterSignalMixer, 0, *this->filter, 1);


  this->amplitudeMixer = new AudioMixer4();
  this->amplitudeDc = new AudioSynthWaveformDc();
  this->amplitudeDc->amplitude(1);
  this->amplitudeMixer->gain(0, 0);
  this->patchCords[patchCordIndex++] = new AudioConnection(*this->lfo, 0, *this->amplitudeMixer, 0);
  this->amplitudeMixer->gain(1, 1);
  this->patchCords[patchCordIndex++] = new AudioConnection(*this->amplitudeDc, 0, *this->amplitudeMixer, 1);

  this->amplitudeModulation = new AudioEffectMultiply();
  this->patchCords[patchCordIndex++] = new AudioConnection(*this->filter, 0, *this->amplitudeModulation, 0);
  this->patchCords[patchCordIndex++] = new AudioConnection(*this->amplitudeMixer, 0, *this->amplitudeModulation, 1);

  this->patchCords[patchCordIndex++] = new AudioConnection(*this->amplitudeModulation, 0, audioOut, 0);
  this->patchCords[patchCordIndex++] = new AudioConnection(*this->amplitudeModulation, 0, audioOut, 1);

  for (int i = 0; i < mergeMixerCount; i++) {
    this->mergeMixers[i] = new AudioMixer4();
    this->mergeMixers[i]->gain(0, 0.25);
    this->mergeMixers[i]->gain(1, 0.25);
    this->mergeMixers[i]->gain(2, 0.25);
    this->mergeMixers[i]->gain(3, 0.25);

    this->patchCords[patchCordIndex++] = new AudioConnection(*this->mergeMixers[i], 0, *this->finalMixer, i);
    this->finalMixer->gain(i, 1.0/mergeMixerCount);
  }

  for (int i = 0; i < voiceCount; i++) {
    this->voices[i] = new Voice();

    this->patchCords[patchCordIndex++] = new AudioConnection(*this->voices[i]->output, 0, *this->mergeMixers[i / channelsPerMixer], i % channelsPerMixer);
  }

  // Start with all voices unplayed.
  for (int i = 0; i < voiceCount; i++) {
    this->unplayedNotes.push_front(i);
  }
}

Synth::~Synth() {

}

void Synth::setup() {

  // Audio setup.
  AudioMemory(20);

  this->codec.enable();

  this->setMasterVolume(0.5);

  this->filter->octaveControl(3);

  this->codec.surroundSound(7, 1);
  this->codec.surroundSoundEnable();

  // Initialize the SD card
  SPI.setMOSI(7);
  SPI.setSCK(14);
  if (!(SD.begin(10))) {
    // stop here if no SD card, but print a message
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }

  // Initialize the serial flash chip.
  if (!SerialFlash.begin()) {
    Serial.println("Unable to access SPI Flash chip");
  }
}

void Synth::setMasterVolume(float vol) {
  // Protect my ears.
  if (vol > 0.8) {
    vol = 0.8;
  }
  this->masterVolume = vol;
  this->codec.volume(this->masterVolume);
}

void Synth::noteOn(byte midiNote) {

  this->filterSignalEnvelope->noteOn();

  int voiceIndex = -1;

  // First, check if the note is already playing.
  for (list<byte>::iterator it=this->playedNotes.begin(); it != this->playedNotes.end(); it++) {
    if (this->voices[*it]->currentNote == midiNote) {
      voiceIndex = *it;
    }
  }

  // If it's not playing already, find the next voice to be played.
  if (voiceIndex == -1) {

    if (!this->unplayedNotes.empty()) {
      voiceIndex = this->unplayedNotes.back();
      this->unplayedNotes.pop_back();
      this->playedNotes.push_back(voiceIndex);
    }
    else {
      voiceIndex = this->playedNotes.front();
    }
  }

  this->voices[voiceIndex]->noteOn(midiNote);
  this->voices[voiceIndex]->is_pressed = true;
}

void Synth::noteOff(byte midiNote) {
  this->filterSignalEnvelope->noteOff();

  for (list<byte>::iterator it=this->playedNotes.begin(); it != this->playedNotes.end(); it++) {
    int voiceIndex = *it;
    if (this->voices[voiceIndex]->currentNote == midiNote) {

      // The key is not pressed anymore.
      this->voices[voiceIndex]->is_pressed = false;

      if (!this->sustained) {
        this->voices[voiceIndex]->noteOff();
        this->playedNotes.erase(it);
        this->unplayedNotes.push_front(voiceIndex);
      }

      break;
    }
  }
}

void Synth::sustain(boolean pressed) {
  this->sustained = pressed;

  if (!pressed) {
    for (list<byte>::iterator it=this->playedNotes.begin(); it != this->playedNotes.end(); it++) {
      int voiceIndex = *it;
      if (this->voices[voiceIndex]->is_playing && !this->voices[voiceIndex]->is_pressed) {
        this->voices[voiceIndex]->noteOff();

        this->playedNotes.erase(it);
        this->unplayedNotes.push_front(voiceIndex);
      }
    }
  }
}

void Synth::setWaveForm1(byte waveform) {
  if (this->waveform1 != waveform) {
    this->waveform1 = waveform;

    for (int i = 0; i < voiceCount; i++) {
      this->voices[i]->setWaveForm1(this->waveform1);
    }
  }
}

void Synth::setWaveForm2(byte waveform) {
  if (this->waveform2 != waveform) {
    this->waveform2 = waveform;

    for (int i = 0; i < voiceCount; i++) {
      this->voices[i]->setWaveForm2(this->waveform2);
    }
  }
}

void Synth::setMix(float mix) {
  for (int i = 0; i < voiceCount; i++) {
    this->voices[i]->setMix(mix);
  }
}

void Synth::setDetune(float detune) {
  for (int i = 0; i < voiceCount; i++) {
    this->voices[i]->setDetune(detune);
  }
}

void Synth::setTranspose(int offset) {
  for (int i = 0; i < voiceCount; i++) {
    this->voices[i]->setTranspose(offset);
  }
}

void Synth::setFilterFrequency(float freq) {
  this->filter->frequency(freq);
}

void Synth::setFilterResonance(float q) {
  this->filter->resonance(q);
}
