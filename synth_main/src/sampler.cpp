
#include "sampler.h"

void Sampler::setSound(byte sound) {

  sound = sound % 12;

  if (sound != this->sound) {
    this->sound = sound;
  }
}

void Sampler::noteOn(byte note) {
  String filename = this->getFilename(note);
  // Convert filename to a character array, which is what AudioPlaySdRaw expects.
  char __dataFileName[sizeof(filename)];
  filename.toCharArray(__dataFileName, sizeof(__dataFileName));

  if (!this->sdPlayer.isPlaying()) {

    String filename;
    filename.concat(this->getDirectory());
    filename.concat(this->getFilename(note));

    // Convert filename to a character array, which is what AudioPlaySdRaw expects.
    char __dataFileName[sizeof(filename)];
    filename.toCharArray(__dataFileName, sizeof(__dataFileName));
    this->sdPlayer.play(__dataFileName);
  }
}
