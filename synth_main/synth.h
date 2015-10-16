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

    AudioSynthWaveformSine *lfo;
    AudioMixer4 *amplitudeMixer;
    AudioSynthWaveformDc *amplitudeDc;
    AudioConnection* patchCords[voiceCount + mergeMixerCount + 15];

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
    void setPulseWidth(float pw);
    void setDetune(float detune);

    void setFilterFrequency(float freq);
    void setFilterResonance(float q);

    void setLFORate(float freq);
    void setFilterLFOAmount(float octaves);
    void setAmplitudeModulationLFOAmount(float amount);
};

#endif
