#ifndef synth_H
#define synth_H

#include <Arduino.h>
#include <list>

// Audio
#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>

#include "voice.h"

// Use the std namespace so we have access to lists.
using namespace std;

// Mixers
const byte mergeMixerCount = 2;
const byte channelsPerMixer = 4;
const byte voiceCount = channelsPerMixer * mergeMixerCount;

// Modes
const byte MODE_MONOPHONIC = 0;
const byte MODE_POLYPHONIC = 1;
const byte MODE_CHORDS = 2;
const byte MODE_ARPEGGIATOR = 3;

struct NoteEntry {
  byte note;
  byte voiceIndex;
};

class Synth {
    float masterVolume;
    byte waveform1;
    byte waveform2;
    float pw;
    float detune;

    byte lfoWaveform;
    byte lfoTarget;

    boolean sustained;



    byte mode;
    list<NoteEntry> playedNotes;
    list<NoteEntry> unplayedNotes;

    list<NoteEntry> ::iterator currentNote;
    byte arpeggiatorPosition;

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
    AudioMixer4 outputMixer;

    Synth();
    ~Synth();
    void setup();

    void noteOn(byte midiNote);
    void noteOff(byte midiNote);
    void sustain(boolean pressed);

    void tick();
    void setMode(byte mode) {
      if (this->mode != mode) {
        this->mode = mode;
        switch (mode) {
          case MODE_MONOPHONIC:
            Serial.println("Mode: Monophonic");
            break;

          case MODE_POLYPHONIC:
            Serial.println("Mode: Polyphonic");
            break;

          case MODE_CHORDS:
            Serial.println("Mode: Chords");
            break;

          case MODE_ARPEGGIATOR:
            Serial.println("Mode: Arpeggiator");
            break;
        }
      }
    }

    void setWaveForm1(byte waveform);
    void setWaveForm2(byte waveform);
    void setMix(float mix);
    void setDetune(float detune);
    void setTranspose(int offset);

    void setAmpEnvAttack(float attack) {
      for (int i = 0; i < voiceCount; i++) {
        this->voices[i]->env->attack(attack);
      }
    }
    void setAmpEnvDecay(float decay) {
      for (int i = 0; i < voiceCount; i++) {
        this->voices[i]->env->decay(decay);
      }
    }
    void setAmpEnvSustain(float sustain) {
      for (int i = 0; i < voiceCount; i++) {
        this->voices[i]->env->sustain(sustain);
      }
    }
    void setAmpEnvRelease(float release) {
      for (int i = 0; i < voiceCount; i++) {
        this->voices[i]->env->release(release);
      }
    }

    void setFilterFrequency(float freq);
    void setFilterResonance(float q);


    void setLFOWaveform(byte waveform) {
      if (waveform == this->lfoWaveform) {
        return;
      }
      else {
        this->lfoWaveform = waveform;
      }

      switch(waveform) {
        case 0:
          this->lfo->begin(WAVEFORM_SINE);
          break;
        case 1:

          this->lfo->begin(WAVEFORM_SAWTOOTH);
          break;
        case 2:
          this->lfo->begin(WAVEFORM_SAWTOOTH_REVERSE);
          break;
        case 3:
          this->lfo->begin(WAVEFORM_SQUARE);
          break;
        case 4:
          this->lfo->begin(WAVEFORM_SAMPLE_HOLD);
          break;

        // TODO: add remaining waveforms
      }
    }
    void setLFORate(float freq) {
      this->lfo->frequency(freq);
    }
    void setLFOAmplitude(float amplitude) {
      if (amplitude > 1) amplitude = 1;
      if (amplitude < 0) amplitude = 0;
      this->lfo->amplitude(amplitude);

      switch (this->lfoTarget) {
        case 0:
        case 2:
          // Amplitude modulation
          this->amplitudeDc->amplitude(1, 1 - amplitude * 0.5);
          break;
      }
    }
    void setLFOTarget(byte target) {
      if (target == this->lfoTarget) {
        return;
      }
      else {
        this->lfoTarget = target;
      }

      // Reset all targets.
      this->filterSignalMixer->gain(0, 0);
      this->amplitudeMixer->gain(0, 0.0);
      this->amplitudeMixer->gain(1, 1.0);

      switch(target) {
        case 0:
          // Amplitude modulation
          this->amplitudeMixer->gain(0, 1);
          this->amplitudeMixer->gain(1, 1);
          Serial.println("Amplitude modulation");
          break;

        case 1:
          // Filter modulation
          this->filterSignalMixer->gain(0, 1.0);
          Serial.println("Filter modulation");
          break;

        case 2:
          // Amplitude and Filter modulation
          this->amplitudeMixer->gain(0, 1);
          this->amplitudeMixer->gain(1, 1);

          this->filterSignalMixer->gain(0, 1.0);

          Serial.println("Filter and amplitude modulation");
          break;

      }
    }

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
};

#endif
