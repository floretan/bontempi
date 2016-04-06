#ifndef voice_H
#define voice_H

#define WAVEFORM_NOISE 13

#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>

#include "AKWF_cello.h"
#define WAVEFORM_CELLO 0x10

#include "AKWF_piano.h"
#define WAVEFORM_PIANO 0x11

#include "AKWF_eorgan.h"
#define WAVEFORM_EORGAN 0x12

class Voice {
    byte currentNote;

    byte waveform1;
    byte waveform2;
    float pw; // Pulse width for the first oscillator.
    float detune; // Detune for the second oscillator.
    AudioSynthWaveform *osc1;
    AudioSynthWaveform *osc2;
    AudioSynthNoiseWhite *noise;
    // AudioPlaySerialflashRaw *sampler;
    AudioPlaySdRaw *sampler;

    AudioMixer4 *noteMixer;

    AudioEffectEnvelope *env;

    AudioConnection* patchCords[7];

  public:
    Voice();
    ~Voice();

    AudioMixer4 *output;


    void noteOn(byte midiNote);
    void noteOff();

    void setWaveForm1(byte waveform);
    void setWaveForm2(byte waveform);
    void setPulseWidth(float pw);
    void setDetune(float detune);
};

#endif
