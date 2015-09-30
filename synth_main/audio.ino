

void setupAudio() {
  // Audio setup.
//  AudioMemory(30);
//
//  codec.enable();
//  codec.volume(0.45);
//
//  codec.surroundSound(7, 1);
//  codec.surroundSoundEnable();


  // set envelope parameters, for pleasing sound :-)
  for (int i = 0; i < 8; i++) {
    envs[i]->attack(2.0);
    envs[i]->hold(2.1);
    envs[i]->decay(31.4);
    envs[i]->sustain(1.0);
    envs[i]->release(284.5);
  }
}

//int lastNoteIndex = 0;
void doNoteOn(byte note) {

//  notes[lastNoteIndex] = note;
//
//  AudioNoInterrupts();
//
//  waves[lastNoteIndex]->begin(0.2, tune_frequencies2_PGM[note], waveform1);
//
//
//  envs[lastNoteIndex]->noteOn();
//
//  AudioInterrupts();
//
//  lastNoteIndex = (lastNoteIndex + 1) % 8;


  for (int i = 0; i < 8; i++) {
    if (notes[i] == 0) {

      notes[i] = note;

      AudioNoInterrupts();

      waves[i]->begin(0.2, tune_frequencies2_PGM[note], waveform1);
      waves2[i]->begin(0.2, tune_frequencies2_PGM[note]*detune/2, waveform2);

      envs[i]->noteOn();

      AudioInterrupts();
      break;
    }
  }
}

void doNoteOff(byte note) {

  for (int i = 0; i < 8; i++) {
    if (notes[i] == note) {

      AudioNoInterrupts();
      envs[i]->noteOff();
      AudioInterrupts();

      // Setting the amplitude to 0 frees CPU,
      // but results in a click because it comes too early.
      // waves[i]->amplitude(0);
      notes[i] = 0;

      break;
    }
  }
}

