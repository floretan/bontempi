#ifndef voice_H
#define voice_H

#define WAVEFORM_NOISE 13

#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>

class Voice {
    byte waveform1;
    byte waveform2;
    float detune;
    AudioSynthWaveform *osc1;
    AudioSynthWaveform *osc2;
    AudioSynthNoiseWhite *noise;

    AudioMixer4 *noteMixer;

    AudioEffectEnvelope *env;

    AudioConnection* patchCords[5];

  public:
    Voice();
    ~Voice();

    AudioMixer4 *output;


    void noteOn(byte midiNote);
    void noteOff();

    void setWaveForm1(byte waveform);
    void setWaveForm2(byte waveform);
    void setDetune(float detune);
};

#endif
