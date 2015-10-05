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
    float detune;

    AudioControlSGTL5000 codec;

    byte notes[voiceCount];
    Voice *voices[voiceCount];
    AudioMixer4 *mergeMixers[voiceCount / 4];
    AudioMixer4 *finalMixer;
    AudioFilterStateVariable *filter;
    AudioConnection* patchCords[voiceCount + mergeMixerCount + 3];

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

    void setFilterFrequency(float freq);
    void setFilterResonance(float q);
};

#endif
