#ifndef main_mixer_H
#define main_mixer_H

#include <Arduino.h>

#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>

#include "synth.h"
#include "sampler.h"

class MainMixer {

  AudioControlSGTL5000 codec;
  AudioOutputI2S  audioOut;
  AudioConnection* patchCords[6];

public:
  AudioMixer4 inputMixer;
  AudioEffectReverb reverb;
  AudioMixer4 reverbMixer;
  AudioSynthWaveformDc dc;

  MainMixer() {
    int patchCordIndex = 0;

    // Make sure there is always audio data, otherwise the reverb gets cut off.
    this->patchCords[patchCordIndex++] = new AudioConnection(dc, 0, inputMixer, 3);

    // The reverbMixer gets both the dry and wet signals.
    this->patchCords[patchCordIndex++] = new AudioConnection(inputMixer, 0, reverb, 0);
    this->patchCords[patchCordIndex++] = new AudioConnection(inputMixer, 0, reverbMixer, 0);
    this->patchCords[patchCordIndex++] = new AudioConnection(reverb, 0, reverbMixer, 1);

    this->patchCords[patchCordIndex++] = new AudioConnection(reverbMixer, 0, audioOut, 0);
    this->patchCords[patchCordIndex++] = new AudioConnection(reverbMixer, 0, audioOut, 1);

  };

  void enable() {
    this->codec.enable();
    Serial.println("Codec enabled");

    this->setMasterVolume(0.5);
    this->codec.surroundSound(7, 1);
    this->codec.surroundSoundEnable();
  }

  void setMasterVolume(float vol) {
    // Protect my ears.
    if (vol > 0.8) {
      vol = 0.8;
    }
    this->codec.volume(vol);
  }

  void setReverbTime(float time) {
    this->reverb.reverbTime(time);
  }

  void setReverbVolume(float volume) {
    this->reverbMixer.gain(1, volume);
  }
};


#endif
