#include "synth.h"

Synth::Synth() {
  // The actual initialization happens in the setup() method.
}

Synth::~Synth() {
  
}

void Synth::setup() {
   
   // Audio setup.
  AudioMemory(30);

  this->codec.enable();
  
  this->setMasterVolume(0.2);

  this->codec.surroundSound(7, 1);
  this->codec.surroundSoundEnable();
}

void Synth::setMasterVolume(float vol) {
  this->masterVolume = vol;
  this->codec.volume(this->masterVolume);
}

