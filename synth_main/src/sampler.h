#ifndef sampler_H
#define sampler_H

#include <Arduino.h>

#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>

class Sampler {

private:
  AudioPlaySdRaw sdPlayer;
  AudioPlaySerialflashRaw flashPlayer;
  AudioConnection* patchCords[2];

  byte sound;

public:
  AudioMixer4 outputMixer;

  Sampler(void) {
    int patchCordIndex = 0;

    this->patchCords[patchCordIndex++] = new AudioConnection(this->sdPlayer, 0, this->outputMixer, 0);
    this->patchCords[patchCordIndex++] = new AudioConnection(this->flashPlayer, 0, this->outputMixer, 1);

    this->sound = 0;
  }

  void enable() {
    // Initialize the SD card
    SPI.setMOSI(7);
    SPI.setSCK(14);
    if (!(SD.begin(10))) {
      // stop here if no SD card, but print a message
      while (1) {
        Serial.println("Unable to access the SD card");
        delay(500);
      }
    }

    // Initialize the serial flash chip.
    if (!SerialFlash.begin()) {
      Serial.println("Unable to access SPI Flash chip");
    }
  }
  
  void setSound(byte sound);

  String getFilename(byte note) {
    String filename = "";
    filename.concat(note-12);
    filename.concat(".RAW");

    return filename;
  }

  String getDirectory() {
    String directory;
    switch (this->sound) {
      case 0:
        directory = "accor/";
        break;
      case 1:
        directory = "celes/";
        break;
      case 2:
        directory = "epian/";
        break;
      case 3:
        directory = "hto/";
        break;
      case 4:
        directory = "mbox/";
        break;
      case 5:
        directory = "piano/";
        break;
      case 6:
        directory = "strin/";
        break;
    }

    return directory;
  }

  void noteOn(byte note);

  void noteOff(byte note) {
    this->sdPlayer.stop();
    this->flashPlayer.stop();
  }
};


#endif
