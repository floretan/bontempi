#ifndef synth_H
#define synth_H
 
#include <Arduino.h>
// Audio
#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
 
class Synth {
  AudioControlSGTL5000 codec;
  float masterVolume;
public:
  Synth();
  ~Synth();
  void setup();
  void setMasterVolume(float vol);
};
 
#endif
