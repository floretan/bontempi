#include "synth.h"
#include "voice.h"
#include "frequencies.h"

using namespace std;

bool compare_note_entry (const NoteEntry& first, const NoteEntry& second) {
  return ( first.note < second.note );
}

Synth::Synth() {

  this->waveform1 = 0;
  this->waveform2 = 0;

  this->detune = 1;

  this->sustained = false;

  int patchCordIndex = 0;

  this->finalMixer = new AudioMixer4();
  this->filter = new AudioFilterStateVariable();

  this->patchCords[patchCordIndex++] = new AudioConnection(*this->finalMixer, 0, *this->filter, 0);

  this->lfo = new AudioSynthWaveform();
  this->lfo->begin(1.0, 2.0, WAVEFORM_SINE);

  this->filterSignalMixer = new AudioMixer4();
  this->filterSignalMixer->gain(0, 1.0);
  this->filterSignalMixer->gain(1, 1);
  this->patchCords[patchCordIndex++] = new AudioConnection(*this->lfo, 0, *this->filterSignalMixer, 0);

  this->filterSignalDc = new AudioSynthWaveformDc();
  this->filterSignalDc->amplitude(1);
  this->filterSignalEnvelope = new AudioEffectEnvelope();

  this->patchCords[patchCordIndex++] = new AudioConnection(*this->filterSignalDc, 0, *this->filterSignalEnvelope, 0);
  this->patchCords[patchCordIndex++] = new AudioConnection(*this->filterSignalEnvelope, 0, *this->filterSignalMixer, 1);
  this->patchCords[patchCordIndex++] = new AudioConnection(*this->filterSignalMixer, 0, *this->filter, 1);


  this->amplitudeMixer = new AudioMixer4();
  this->amplitudeDc = new AudioSynthWaveformDc();
  this->amplitudeDc->amplitude(1);
  this->amplitudeMixer->gain(0, 0);
  this->patchCords[patchCordIndex++] = new AudioConnection(*this->lfo, 0, *this->amplitudeMixer, 0);
  this->amplitudeMixer->gain(1, 1);
  this->patchCords[patchCordIndex++] = new AudioConnection(*this->amplitudeDc, 0, *this->amplitudeMixer, 1);

  this->amplitudeModulation = new AudioEffectMultiply();
  this->patchCords[patchCordIndex++] = new AudioConnection(*this->filter, 0, *this->amplitudeModulation, 0);
  this->patchCords[patchCordIndex++] = new AudioConnection(*this->amplitudeMixer, 0, *this->amplitudeModulation, 1);

  this->patchCords[patchCordIndex++] = new AudioConnection(*this->amplitudeModulation, 0, this->outputMixer, 0);

  for (int i = 0; i < mergeMixerCount; i++) {
    this->mergeMixers[i] = new AudioMixer4();
    this->mergeMixers[i]->gain(0, 0.25);
    this->mergeMixers[i]->gain(1, 0.25);
    this->mergeMixers[i]->gain(2, 0.25);
    this->mergeMixers[i]->gain(3, 0.25);

    this->patchCords[patchCordIndex++] = new AudioConnection(*this->mergeMixers[i], 0, *this->finalMixer, i);
    this->finalMixer->gain(i, 1.0/mergeMixerCount);
  }

  for (int i = 0; i < voiceCount; i++) {
    this->voices[i] = new Voice();

    this->patchCords[patchCordIndex++] = new AudioConnection(*this->voices[i]->output, 0, *this->mergeMixers[i / channelsPerMixer], i % channelsPerMixer);
  }

  // Initialize the mode.
  this->mode = 0;

  // Start with all voices unplayed.
  for (int i = 0; i < voiceCount; i++) {
    struct NoteEntry n;
    n.voiceIndex = i;
    n.note = 0;
    this->unplayedNotes.push_front(n);
  }

  this->arpeggiatorPosition = 0;
}

Synth::~Synth() {

}

void Synth::setup() {

  this->filter->octaveControl(3);
}

void Synth::noteOn(byte midiNote) {

  if (this->mode == MODE_MONOPHONIC) {
    if (this->playedNotes.empty()) {

      // TODO: noteOff on previous note.
      this->filterSignalEnvelope->noteOn();
    }
  }

  int voiceIndex = -1;

  if (this->mode == MODE_MONOPHONIC) {
    voiceIndex = 0;
  } else {
    // First, check if the note is already playing.
    for (list<NoteEntry>::iterator it=this->playedNotes.begin(); it != this->playedNotes.end(); it++) {
      if (this->voices[(*it).voiceIndex]->currentNote == midiNote) {
        voiceIndex = (*it).voiceIndex;
      }
    }

    // If it's not playing already, find the next voice to be played.
    if (voiceIndex == -1) {

      if (!this->unplayedNotes.empty()) {
        struct NoteEntry n = this->unplayedNotes.back();
        voiceIndex = n.voiceIndex;
        this->unplayedNotes.pop_back();
        n.note = midiNote;
        this->playedNotes.push_back(n);
      }
      else {
        // If no voice is unplayed, replace the oldest note.
        struct NoteEntry n = this->playedNotes.front();
        n.note = midiNote;
        voiceIndex = n.voiceIndex;
      }
    }
  }

  // if (this->mode == MODE_MONOPHONIC || this->mode == MODE_POLYPHONIC) {
    this->voices[voiceIndex]->noteOn(midiNote);
  // }
  // else {
    // TODO: Set the right note for the voice.
  // }

  this->voices[voiceIndex]->is_pressed = true;
  this->playedNotes.sort(compare_note_entry);
}

void Synth::noteOff(byte midiNote) {

  for (list<NoteEntry>::iterator it=this->playedNotes.begin(); it != this->playedNotes.end(); it++) {
    struct NoteEntry n = *it;
    int voiceIndex = n.voiceIndex;
    if (this->voices[voiceIndex]->currentNote == midiNote) {

      // The key is not pressed anymore.
      this->voices[voiceIndex]->is_pressed = false;

      if (!this->sustained) {
        this->voices[voiceIndex]->noteOff();
        this->playedNotes.erase(it);
        this->unplayedNotes.push_front(n);
      }

      break;
    }
  }

  if (this->mode == MODE_MONOPHONIC) {
    if (this->playedNotes.empty()) {
      this->filterSignalEnvelope->noteOff();
    }
  }
}

void Synth::sustain(boolean pressed) {
  this->sustained = pressed;

  if (!pressed) {
    for (list<NoteEntry>::iterator it=this->playedNotes.begin(); it != this->playedNotes.end(); it++) {
      struct NoteEntry n = *it;
      int voiceIndex = n.voiceIndex;
      if (this->voices[voiceIndex]->is_playing && !this->voices[voiceIndex]->is_pressed) {
        this->voices[voiceIndex]->noteOff();

        this->playedNotes.erase(it);
        this->unplayedNotes.push_front(n);
      }
    }
  }
}

void Synth::tick() {
  if (this->mode == MODE_MONOPHONIC || this->mode == MODE_POLYPHONIC) {
    // Don't do anything.
    return;
  }

  if (this->mode == MODE_ARPEGGIATOR) {
    if (!this->playedNotes.empty()) {
      this->arpeggiatorPosition = (this->arpeggiatorPosition + 1) % this->playedNotes.size();

      int i = 0;
      list<NoteEntry>::iterator it=this->playedNotes.begin();
      while (i < this->arpeggiatorPosition && it != this->playedNotes.end()) {
        i++;
        it++;
      }

      this->voices[(*it).voiceIndex]->noteOff();

      // Go to the next played voice.
      it++;

      // If we reached the end, go back to the start.
      if (it == this->playedNotes.end()) {
        it = this->playedNotes.begin();
      }

      this->voices[(*it).voiceIndex]->noteOn();
    }
  }
}

void Synth::setWaveForm1(byte waveform) {
  if (this->waveform1 != waveform) {
    this->waveform1 = waveform;

    for (int i = 0; i < voiceCount; i++) {
      this->voices[i]->setWaveForm1(this->waveform1);
    }
  }
}

void Synth::setWaveForm2(byte waveform) {
  if (this->waveform2 != waveform) {
    this->waveform2 = waveform;

    for (int i = 0; i < voiceCount; i++) {
      this->voices[i]->setWaveForm2(this->waveform2);
    }
  }
}

void Synth::setMix(float mix) {
  for (int i = 0; i < voiceCount; i++) {
    this->voices[i]->setMix(mix);
  }
}

void Synth::setDetune(float detune) {
  for (int i = 0; i < voiceCount; i++) {
    this->voices[i]->setDetune(detune);
  }
}

void Synth::setTranspose(int offset) {
  for (int i = 0; i < voiceCount; i++) {
    this->voices[i]->setTranspose(offset);
  }
}

void Synth::setFilterFrequency(float freq) {
  this->filter->frequency(freq);
}

void Synth::setFilterResonance(float q) {
  this->filter->resonance(q);
}
