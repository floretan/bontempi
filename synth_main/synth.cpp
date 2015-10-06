#include "synth.h"
#include "voice.h"
#include "frequencies.h"

AudioOutputI2S  audioOut;

Synth::Synth() {

  this->waveform1 = WAVEFORM_SINE;
  this->waveform2 = WAVEFORM_SINE;

  this->detune = 1;

  int patchCordIndex = 0;

  this->finalMixer = new AudioMixer4();
  this->filter = new AudioFilterStateVariable();

  

  this->patchCords[patchCordIndex++] = new AudioConnection(*this->finalMixer, 0, *this->filter, 0);
  this->patchCords[patchCordIndex++] = new AudioConnection(*this->filter, 0, audioOut, 0);
  this->patchCords[patchCordIndex++] = new AudioConnection(*this->filter, 0, audioOut, 1);

  this->lfo = new AudioSynthWaveformSine();
  this->patchCords[patchCordIndex++] = new AudioConnection(*this->lfo, 0, *this->filter, 1);

  for (int i = 0; i < mergeMixerCount; i++) {
    this->mergeMixers[i] = new AudioMixer4();
    this->mergeMixers[i]->gain(0, 0.25);
    this->mergeMixers[i]->gain(1, 0.25);
    this->mergeMixers[i]->gain(2, 0.25);
    this->mergeMixers[i]->gain(3, 0.25);

    this->patchCords[patchCordIndex++] = new AudioConnection(*this->mergeMixers[i], 0, *this->finalMixer, i);
  }

  for (int i = 0; i < voiceCount; i++) {
    this->voices[i] = new Voice();
    this->notes[i] = 0;

    this->patchCords[patchCordIndex++] = new AudioConnection(*this->voices[i]->output, 0, *this->mergeMixers[i / channelsPerMixer], i % channelsPerMixer);
  }
}

Synth::~Synth() {

}

void Synth::setup() {

  // Audio setup.
  AudioMemory(30);

  this->codec.enable();

  this->setMasterVolume(0.2);

  this->codec.surroundSound(7, 1);
  this->codec.surroundSoundEnable();
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
  for (int i = 0; i < voiceCount; i++) {
    if (this->notes[i] == 0) {

      this->notes[i] = midiNote;
      this->voices[i]->noteOn(midiNote);

      break;
    }
  }
}

void Synth::noteOff(byte midiNote) {
  for (int i = 0; i < voiceCount; i++) {
    if (this->notes[i] == midiNote) {

      this->voices[i]->noteOff();

      // Setting the amplitude to 0 frees CPU,
      // but results in a click because it comes too early.
      // waves[i]->amplitude(0);
      this->notes[i] = 0;

      break;
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

void Synth::setDetune(float detune) {
  this->detune = detune;

  for (int i = 0; i < voiceCount; i++) {
    this->voices[i]->setDetune(detune);
  }
}

void Synth::setFilterFrequency(float freq) {
  this->filter->frequency(freq);
}

void Synth::setFilterResonance(float q) {
  this->filter->resonance(q);
}


void Synth::setLFORate(float freq) {
  this->lfo->frequency(freq);
}

void Synth::setFilterLFOAmount(float octaves) {
  this->filter->octaveControl(octaves);
}
    
