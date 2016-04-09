#ifndef synth_H
#define synth_H

#include <Arduino.h>
// Audio
#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>

#include "voice.h"

// Mixers
const byte mergeMixerCount = 2;
const byte channelsPerMixer = 4;
const byte voiceCount = channelsPerMixer * mergeMixerCount;

class Synth {
    float masterVolume;
    byte waveform1;
    byte waveform2;
    float pw;
    float detune;

    boolean sustained;

    AudioControlSGTL5000 codec;

    byte notes[voiceCount];
    byte sustainedNotes[voiceCount];
    Voice *voices[voiceCount];
    AudioMixer4 *mergeMixers[voiceCount / 4];
    AudioMixer4 *finalMixer;
    AudioFilterStateVariable *filter;
    AudioEffectMultiply *amplitudeModulation;

    AudioSynthWaveform *lfo;
    AudioMixer4 *amplitudeMixer;
    AudioSynthWaveformDc *amplitudeDc;
    AudioMixer4 *filterSignalMixer;
    AudioSynthWaveformDc *filterSignalDc;
    AudioEffectEnvelope *filterSignalEnvelope;
    AudioConnection* patchCords[voiceCount + mergeMixerCount + 20];

  public:
    Synth();
    ~Synth();
    void setup();
    void setMasterVolume(float vol);

    void noteOn(byte midiNote);
    void noteOff(byte midiNote);
    void sustain(boolean pressed);

    void setWaveForm1(byte waveform);
    void setWaveForm2(byte waveform);
    void setMix(float mix);
    void setPulseWidth(float pw);
    void setDetune(float detune);

    void setAmpEnvAttack(float attack);
    void setAmpEnvDecay(float decay);
    void setAmpEnvSustain(float sustain);
    void setAmpEnvRelease(float release);

    void setFilterFrequency(float freq);
    void setFilterResonance(float q);

    void setLFORate(float freq);
    void setFilterLFOAmount(float octaves);
    void setFilterEnvelopeAmount(float amount) {
      if (amount > 1) amount = 1;
      if (amount < -1) amount = -1;
      this->filterSignalDc->amplitude(amount);
    }

    void setFilterEnvelopeAttack(float attack) {
      this->filterSignalEnvelope->attack(attack);
    }
    void setFilterEnvelopeDecay(float decay) {
      this->filterSignalEnvelope->decay(decay);
    }
    void setFilterEnvelopeSustain(float sustain) {
      this->filterSignalEnvelope->sustain(sustain);
    }
    void setFilterEnvelopeRelease(float release) {
      this->filterSignalEnvelope->release(release);
    }

    void setAmplitudeModulationLFOAmount(float amount);
};

#endif
