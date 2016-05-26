#include "voice.h"
#include "frequencies.h"

Voice::Voice() {
  this->currentNote = 0;
  this->is_pressed = false;
  this->is_playing = false;

  this->detune = 1;

  this->osc1 = new AudioSynthWaveform();
  this->osc1->amplitude(1.0);
  this->osc2 = new AudioSynthWaveform();
  this->osc2->amplitude(1.0);
  this->noise = new AudioSynthNoiseWhite();
  this->sampler = new AudioPlaySdRaw();
  // this->sampler = new AudioPlaySerialflashRaw();

  this->noteMixer = new AudioMixer4();

  this->env = new AudioEffectEnvelope();
  this->env->attack(2.0);
  this->env->hold(2.1);
  this->env->decay(1500.4);
  this->env->sustain(1.0);
  this->env->release(284.5);

  this->output = new AudioMixer4();

  this->patchCords[0] = new AudioConnection(*this->osc1, 0, *this->noteMixer, 0);
  this->patchCords[1] = new AudioConnection(*this->osc2, 0, *this->noteMixer, 1);
  this->patchCords[2] = new AudioConnection(*this->noise, 0, *this->noteMixer, 2);
  this->patchCords[3] = new AudioConnection(*this->sampler, 0, *this->noteMixer, 3);
  this->patchCords[4] = new AudioConnection(*this->noteMixer, 0, *this->env, 0);
  this->patchCords[5] = new AudioConnection(*this->env, 0, *this->output, 0);
}

Voice::~Voice() {

}

void Voice::noteOn(byte midiNote) {
  this->currentNote = midiNote;
  this->is_playing = true;

  AudioNoInterrupts();
  this->osc1->frequency(tune_frequencies2_PGM[this->currentNote]);
  this->osc2->frequency(tune_frequencies2_PGM[midiNote + this->transpose] * this->detune);

  // if (!this->sampler->isPlaying()) {
  //
  //   String s = "accor/";
  //   s.concat(midiNote-12);
  //   s.concat(".RAW");
  //
  //   Serial.println(s);
  //
  //   char __dataFileName[sizeof(s)];
  //   s.toCharArray(__dataFileName, sizeof(__dataFileName));
  //   this->sampler->play(__dataFileName);
  // }

  this->env->noteOn();

  AudioInterrupts();
}

void Voice::noteOff() {
  AudioNoInterrupts();
  this->env->noteOff();
  this->sampler->stop();
  AudioInterrupts();

  this->is_playing = false;
}

void Voice::setWaveForm1(byte waveform) {

    switch(waveform) {
      case 0:
        this->osc1->begin(WAVEFORM_SINE);
        break;

      case 1:
        this->osc1->begin(WAVEFORM_TRIANGLE);
        break;

      case 2:
        this->osc1->begin(WAVEFORM_SAWTOOTH);
        break;

      case 3:
        this->osc1->begin(WAVEFORM_SQUARE);
        break;

      case 4:
        this->osc1->arbitraryWaveform(AKWF_cello_0001, 880);
        this->osc1->begin(WAVEFORM_ARBITRARY);
        break;

      case 5:
        this->osc1->arbitraryWaveform(AKWF_piano_0002, 880);
        this->osc1->begin(WAVEFORM_ARBITRARY);
        break;

      case 6:
        this->osc1->arbitraryWaveform(AKWF_eorgan_0001, 880);
        this->osc1->begin(WAVEFORM_ARBITRARY);
        break;
    }
}

void Voice::setWaveForm2(byte waveform) {
  if (waveform == 0) {
    this->osc2->amplitude(0);
    this->noise->amplitude(1.0);
  }
  else {
    this->osc2->amplitude(1.0);
    this->noise->amplitude(0);

    switch(waveform) {
      case 1:
        this->osc2->begin(WAVEFORM_SINE);
        break;

      case 2:
        this->osc2->begin(WAVEFORM_TRIANGLE);
        break;

      case 3:
        this->osc2->begin(WAVEFORM_SAWTOOTH);
        break;

      case 4:
        this->osc2->begin(WAVEFORM_SQUARE);
        break;
    }
  }
}

void Voice::setMix(float mix) {
  if (mix > 1) mix = 1;
  if (mix < 0) mix = 0;
  this->noteMixer->gain(1, mix);
  this->noteMixer->gain(2, mix);
  this->noteMixer->gain(0, 1-mix);
}

void Voice::setDetune(float detune) {
  this->detune = detune;
  this->osc2->frequency(tune_frequencies2_PGM[this->currentNote + this->transpose] * this->detune);
}
void Voice::setTranspose(int offset) {
  this->transpose = offset;
  this->osc2->frequency(tune_frequencies2_PGM[this->currentNote + this->transpose] * this->detune);
}
