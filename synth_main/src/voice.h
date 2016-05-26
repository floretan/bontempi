#ifndef voice_H
#define voice_H

#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>

#include "AKWF_cello.h"
#include "AKWF_piano.h"
#include "AKWF_eorgan.h"

class Voice {

    float detune; // Detune for the second oscillator.
    int transpose;  // Transpose the second oscillator.
    AudioSynthWaveform *osc1;
    AudioSynthWaveform *osc2;
    AudioSynthNoiseWhite *noise;
    // AudioPlaySerialflashRaw *sampler;
    AudioPlaySdRaw *sampler;

    AudioMixer4 *noteMixer;

    AudioConnection* patchCords[7];

  public:
    Voice();
    ~Voice();

    byte currentNote; // The midi note currently being played.
    bool is_pressed;
    bool is_playing;

    AudioMixer4 *output;
    AudioEffectEnvelope *env;

    void noteOn(byte midiNote);
    void noteOff();

    void setWaveForm1(byte waveform);
    void setWaveForm2(byte waveform);
    void setMix(float mix);
    void setDetune(float detune);
    void setTranspose(int offset);
};

#endif
