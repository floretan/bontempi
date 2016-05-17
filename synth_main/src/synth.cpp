#include "synth.h"
#include "voice.h"
#include "frequencies.h"

AudioOutputI2S  audioOut;

Synth::Synth() {

  this->waveform1 = WAVEFORM_EORGAN;
  this->waveform2 = WAVEFORM_CELLO;

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
    this->notes[i] = 0;
    this->sustainedNotes[i] = 0;

    this->patchCords[patchCordIndex++] = new AudioConnection(*this->voices[i]->output, 0, *this->mergeMixers[i / channelsPerMixer], i % channelsPerMixer);
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
  int emptyNoteIndex = -1;
  int existingNoteIndex = -1;
  for (int i = 0; i < voiceCount; i++) {
    if (this->notes[i] == 0) {
      emptyNoteIndex = i;
    }
    if (this->notes[i] == midiNote) {
      existingNoteIndex = i;
    }
  }

  if (existingNoteIndex != -1)  {
    this->voices[existingNoteIndex]->noteOn(midiNote);
  }
  else if (emptyNoteIndex != -1) {
    this->notes[emptyNoteIndex] = midiNote;
    this->voices[emptyNoteIndex]->noteOn(midiNote);
  }
}

void Synth::noteOff(byte midiNote) {
  this->filterSignalEnvelope->noteOff();
  for (int i = 0; i < voiceCount; i++) {
    if (this->notes[i] == midiNote) {

      if (!this->sustained) {
        this->voices[i]->noteOff();


        // Setting the amplitude to 0 frees CPU,
        // but results in a click because it comes too early.
        // waves[i]->amplitude(0);
        this->notes[i] = 0;
        this->sustainedNotes[i] = 0;
      }
      else {
        // Delay the note off yet.
        this->sustainedNotes[i] = midiNote;
      }

      break;
    }
  }
}

void Synth::sustain(boolean pressed) {
  this->sustained = pressed;

  if (!pressed) {
    for (int i = 0; i < voiceCount; i++) {
      if (this->sustainedNotes[i] != 0 && this->notes[i] != 0) {
        this->noteOff(this->sustainedNotes[i]);
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
