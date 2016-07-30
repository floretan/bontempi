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
  AudioConnection* patchCords[2];

public:
  AudioMixer4 inputMixer;

  MainMixer() {
    int patchCordIndex = 0;

    this->patchCords[patchCordIndex++] = new AudioConnection(inputMixer, 0, audioOut, 0);
    this->patchCords[patchCordIndex++] = new AudioConnection(inputMixer, 0, audioOut, 1);

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
};


#endif
