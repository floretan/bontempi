#include "voice.h"
#include "frequencies.h"


Voice::Voice() {

  this->waveform1 = WAVEFORM_SINE;
  this->waveform2 = WAVEFORM_SINE;

  this->detune = 1;

  this->osc1 = new AudioSynthWaveform();
  this->osc2 = new AudioSynthWaveform();

  this->noteMixer = new AudioMixer4();
  this->noteMixer->gain(0, 0.5);
  this->noteMixer->gain(1, 0.5);

  this->env = new AudioEffectEnvelope();
  this->env->attack(2.0);
  this->env->hold(2.1);
  this->env->decay(31.4);
  this->env->sustain(1.0);
  this->env->release(284.5);

  this->output = new AudioMixer4();

  this->patchCords[0] = new AudioConnection(*this->osc1, 0, *this->noteMixer, 0);
  this->patchCords[1] = new AudioConnection(*this->osc2, 0, *this->noteMixer, 1);
  this->patchCords[2] = new AudioConnection(*this->noteMixer, 0, *this->env, 0);
  this->patchCords[3] = new AudioConnection(*this->env, 0, *this->output, 0);
}

Voice::~Voice() {

}

void Voice::noteOn(byte midiNote) {

  AudioNoInterrupts();

  this->osc1->begin(0.2, tune_frequencies2_PGM[midiNote], this->waveform1);
  this->osc2->begin(0.2, tune_frequencies2_PGM[midiNote]*this->detune / 2, this->waveform2);

  this->env->noteOn();

  AudioInterrupts();
}

void Voice::noteOff() {
  this->env->noteOff();
}

void Voice::setWaveForm1(byte waveform) {
  this->waveform1 = waveform;
  this->osc1->begin(waveform);
}

void Voice::setWaveForm2(byte waveform) {
  this->waveform2 = waveform;
  this->osc2->begin(waveform);
}

void Voice::setDetune(float detune) {
  this->detune = detune;

  // changing the frequency while playing results in unpleasant sounds.
}

