#ifndef synth_H
#define synth_H

#include <Arduino.h>
// Audio
#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>

class Synth {
    float masterVolume;
    byte waveform1;
    byte waveform2;
    float detune;

    AudioControlSGTL5000 codec;

    byte notes[8] = {
      0, 0, 0, 0,
      0, 0, 0, 0
    };
    AudioSynthWaveform *waves1[8];
    AudioSynthWaveform *waves2[8];
    AudioEffectEnvelope *envs[8];

  public:
    Synth();
    ~Synth();
    void setup();
    void setMasterVolume(float vol);

    void noteOn(byte midiNote);
    void noteOff(byte midiNote);

    void setWaveForm1(byte waveform);
    void setWaveForm2(byte waveform);
    void setDetune(float detune);
};

#endif
