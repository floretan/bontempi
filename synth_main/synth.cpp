#include "synth.h"
#include "frequencies.h"

// First oscillator
AudioSynthWaveform sine0, sine1, sine2, sine3;
AudioSynthWaveform sine4, sine5, sine6, sine7;

// Second oscillator
AudioSynthWaveform sine10, sine11, sine12, sine13;
AudioSynthWaveform sine14, sine15, sine16, sine17;

// Mix the two oscillators together
AudioMixer4 noteMixer0, noteMixer1, noteMixer2, noteMixer3;
AudioMixer4 noteMixer4, noteMixer5, noteMixer6, noteMixer7;

AudioConnection patchCord01(sine0, 0, noteMixer0, 0);
AudioConnection patchCord02(sine1, 0, noteMixer1, 0);
AudioConnection patchCord03(sine2, 0, noteMixer2, 0);
AudioConnection patchCord04(sine3, 0, noteMixer3, 0);
AudioConnection patchCord05(sine4, 0, noteMixer4, 0);
AudioConnection patchCord06(sine5, 0, noteMixer5, 0);
AudioConnection patchCord07(sine6, 0, noteMixer6, 0);
AudioConnection patchCord08(sine7, 0, noteMixer7, 0);

AudioConnection patchCord11(sine10, 0, noteMixer0, 1);
AudioConnection patchCord12(sine11, 0, noteMixer1, 1);
AudioConnection patchCord13(sine12, 0, noteMixer2, 1);
AudioConnection patchCord14(sine13, 0, noteMixer3, 1);
AudioConnection patchCord15(sine14, 0, noteMixer4, 1);
AudioConnection patchCord16(sine15, 0, noteMixer5, 1);
AudioConnection patchCord17(sine16, 0, noteMixer6, 1);
AudioConnection patchCord18(sine17, 0, noteMixer7, 1);

// Each waveform will be shaped by an envelope
AudioEffectEnvelope env0, env1, env2, env3;
AudioEffectEnvelope env4, env5, env6, env7;

// Route each waveform through its own envelope effect
AudioConnection patchCord21(noteMixer0, env0);
AudioConnection patchCord22(noteMixer1, env1);
AudioConnection patchCord23(noteMixer2, env2);
AudioConnection patchCord24(noteMixer3, env3);
AudioConnection patchCord25(noteMixer4, env4);
AudioConnection patchCord26(noteMixer5, env5);
AudioConnection patchCord27(noteMixer6, env6);
AudioConnection patchCord28(noteMixer7, env7);

// Two mixers are needed to handle 8 channels of music
AudioMixer4     mixer1;
AudioMixer4     mixer2;

// Mix the 8 channels down to 2 audio streams
AudioConnection patchCord31(env0, 0, mixer1, 0);
AudioConnection patchCord32(env1, 0, mixer1, 1);
AudioConnection patchCord33(env2, 0, mixer1, 2);
AudioConnection patchCord34(env3, 0, mixer1, 3);
AudioConnection patchCord35(env4, 0, mixer2, 0);
AudioConnection patchCord36(env5, 0, mixer2, 1);
AudioConnection patchCord37(env6, 0, mixer2, 2);
AudioConnection patchCord38(env7, 0, mixer2, 3);

// Now create 2 mixers for the main output
AudioMixer4     mixerLeft;
AudioMixer4     mixerRight;
AudioOutputI2S  audioOut;

// Mix all channels to both the outputs
AudioConnection patchCord41(mixer1, 0, mixerLeft, 0);
AudioConnection patchCord42(mixer2, 0, mixerLeft, 1);
AudioConnection patchCord43(mixer1, 0, mixerRight, 0);
AudioConnection patchCord44(mixer2, 0, mixerRight, 1);

AudioConnection patchCord51(mixerLeft, 0, audioOut, 0);
AudioConnection patchCord52(mixerRight, 0, audioOut, 1);

Synth::Synth() {

  this->waveform1 = WAVEFORM_SINE;
  this->waveform2 = WAVEFORM_SINE;

  this->waves1[0] = &sine0;
  this->waves1[1] = &sine1;
  this->waves1[2] = &sine2;
  this->waves1[3] = &sine3;
  this->waves1[4] = &sine4;
  this->waves1[5] = &sine5;
  this->waves1[6] = &sine6;
  this->waves1[7] = &sine7;

  this->waves2[0] = &sine10;
  this->waves2[1] = &sine11;
  this->waves2[2] = &sine12;
  this->waves2[3] = &sine13;
  this->waves2[4] = &sine14;
  this->waves2[5] = &sine15;
  this->waves2[6] = &sine16;
  this->waves2[7] = &sine17;

  // avoid clipping.
  noteMixer0.gain(0, 0.5);
  noteMixer0.gain(1, 0.5);
  noteMixer1.gain(0, 0.5);
  noteMixer1.gain(1, 0.5);
  noteMixer2.gain(0, 0.5);
  noteMixer2.gain(1, 0.5);
  noteMixer3.gain(0, 0.5);
  noteMixer3.gain(1, 0.5);
  noteMixer4.gain(0, 0.5);
  noteMixer4.gain(1, 0.5);
  noteMixer5.gain(0, 0.5);
  noteMixer5.gain(1, 0.5);
  noteMixer6.gain(0, 0.5);
  noteMixer6.gain(1, 0.5);
  noteMixer7.gain(0, 0.5);
  noteMixer7.gain(1, 0.5);

  this->envs[0] = &env0;
  this->envs[1] = &env1;
  this->envs[2] = &env2;
  this->envs[3] = &env3;
  this->envs[4] = &env4;
  this->envs[5] = &env5;
  this->envs[6] = &env6;
  this->envs[7] = &env7;

  mixer1.gain(0, 0.25);
  mixer1.gain(1, 0.25);
  mixer1.gain(2, 0.25);
  mixer1.gain(3, 0.25);
  mixer2.gain(0, 0.25);
  mixer2.gain(1, 0.25);
  mixer2.gain(2, 0.25);
  mixer2.gain(3, 0.25);

  // set envelope parameters, for pleasing sound :-)
  for (int i = 0; i < 8; i++) {
    this->envs[i]->attack(2.0);
    this->envs[i]->hold(2.1);
    this->envs[i]->decay(31.4);
    this->envs[i]->sustain(1.0);
    this->envs[i]->release(284.5);
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
  for (int i = 0; i < 8; i++) {
    if (this->notes[i] == 0) {

      this->notes[i] = midiNote;

      AudioNoInterrupts();

      this->waves1[i]->begin(0.2, tune_frequencies2_PGM[midiNote], this->waveform1);
      this->waves2[i]->begin(0.2, tune_frequencies2_PGM[midiNote]*this->detune / 2, this->waveform2);

      this->envs[i]->noteOn();

      AudioInterrupts();
      break;
    }
  }
}

void Synth::noteOff(byte midiNote) {
  for (int i = 0; i < 8; i++) {
    if (this->notes[i] == midiNote) {

      AudioNoInterrupts();
      this->envs[i]->noteOff();
      AudioInterrupts();

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
    for (int i = 0; i < 8; i++) {
      AudioNoInterrupts();
      this->waves1[i]->begin(this->waveform1);
      AudioInterrupts();
    }
  }
}

void Synth::setWaveForm2(byte waveform) {
  if (this->waveform2 != waveform) {

    this->waveform2 = waveform;
    for (int i = 0; i < 8; i++) {
      AudioNoInterrupts();
      this->waves2[i]->begin(this->waveform2);
      AudioInterrupts();
    }
  }
}

void Synth::setDetune(float detune) {
  this->detune = detune;

  // changing the frequency while playing results in unpleasant sounds.
}

