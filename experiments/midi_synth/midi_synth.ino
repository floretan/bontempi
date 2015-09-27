
#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>

#include "PlaySynthMusic.h"


byte notes[16] = {
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0
};

AudioSynthWaveform sine0, sine1, sine2, sine3;
AudioSynthWaveform sine4, sine5, sine6, sine7;
AudioSynthWaveform sine8, sine9, sine10, sine11;
AudioSynthWaveform sine12, sine13, sine14, sine15;
AudioSynthWaveform *waves[16] = {
  &sine0, &sine1, &sine2, &sine3,
  &sine4, &sine5, &sine6, &sine7,
  &sine8, &sine9, &sine10, &sine11,
  &sine12, &sine13, &sine14, &sine15
};

// Each waveform will be shaped by an envelope
AudioEffectEnvelope env0, env1, env2, env3;
AudioEffectEnvelope env4, env5, env6, env7;
AudioEffectEnvelope env8, env9, env10, env11;
AudioEffectEnvelope env12, env13, env14, env15;
AudioEffectEnvelope *envs[16] = {
  &env0, &env1, &env2, &env3,
  &env4, &env5, &env6, &env7,
  &env8, &env9, &env10, &env11,
  &env12, &env13, &env14, &env15
};

// Route each waveform through its own envelope effect
AudioConnection patchCord01(sine0, env0);
AudioConnection patchCord02(sine1, env1);
AudioConnection patchCord03(sine2, env2);
AudioConnection patchCord04(sine3, env3);
AudioConnection patchCord05(sine4, env4);
AudioConnection patchCord06(sine5, env5);
AudioConnection patchCord07(sine6, env6);
AudioConnection patchCord08(sine7, env7);
AudioConnection patchCord09(sine8, env8);
AudioConnection patchCord10(sine9, env9);
AudioConnection patchCord11(sine10, env10);
AudioConnection patchCord12(sine11, env11);
AudioConnection patchCord13(sine12, env12);
AudioConnection patchCord14(sine13, env13);
AudioConnection patchCord15(sine14, env14);
AudioConnection patchCord16(sine15, env15);

// Four mixers are needed to handle 16 channels of music
AudioMixer4     mixer1;
AudioMixer4     mixer2;
AudioMixer4     mixer3;
AudioMixer4     mixer4;

// Mix the 16 channels down to 4 audio streams
AudioConnection patchCord17(env0, 0, mixer1, 0);
AudioConnection patchCord18(env1, 0, mixer1, 1);
AudioConnection patchCord19(env2, 0, mixer1, 2);
AudioConnection patchCord20(env3, 0, mixer1, 3);
AudioConnection patchCord21(env4, 0, mixer2, 0);
AudioConnection patchCord22(env5, 0, mixer2, 1);
AudioConnection patchCord23(env6, 0, mixer2, 2);
AudioConnection patchCord24(env7, 0, mixer2, 3);
AudioConnection patchCord25(env8, 0, mixer3, 0);
AudioConnection patchCord26(env9, 0, mixer3, 1);
AudioConnection patchCord27(env10, 0, mixer3, 2);
AudioConnection patchCord28(env11, 0, mixer3, 3);
AudioConnection patchCord29(env12, 0, mixer4, 0);
AudioConnection patchCord30(env13, 0, mixer4, 1);
AudioConnection patchCord31(env14, 0, mixer4, 2);
AudioConnection patchCord32(env15, 0, mixer4, 3);

// Now create 2 mixers for the main output
AudioMixer4     mixerLeft;
AudioMixer4     mixerRight;
AudioOutputI2S  audioOut;

// Mix all channels to both the outputs
AudioConnection patchCord33(mixer1, 0, mixerLeft, 0);
AudioConnection patchCord34(mixer2, 0, mixerLeft, 1);
AudioConnection patchCord35(mixer3, 0, mixerLeft, 2);
AudioConnection patchCord36(mixer4, 0, mixerLeft, 3);
AudioConnection patchCord37(mixer1, 0, mixerRight, 0);
AudioConnection patchCord38(mixer2, 0, mixerRight, 1);
AudioConnection patchCord39(mixer3, 0, mixerRight, 2);
AudioConnection patchCord40(mixer4, 0, mixerRight, 3);
AudioConnection patchCord41(mixerLeft, 0, audioOut, 0);
AudioConnection patchCord42(mixerRight, 0, audioOut, 1);

AudioControlSGTL5000 codec;

// Initial value of the volume control
int volume = 50;

void setup() {

  Serial.begin(115200);
  //while (!Serial) ; // wait for Arduino Serial Monitor
  delay(200);

  // Audio setup.
  AudioMemory(18);

  codec.enable();
  codec.volume(0.45);

  codec.surroundSound(7, 1);
  codec.surroundSoundEnable();


  // set envelope parameters, for pleasing sound :-)
  for (int i = 0; i < 16; i++) {
    envs[i]->attack(9.2);
    envs[i]->hold(2.1);
    envs[i]->decay(31.4);
    envs[i]->sustain(0.6);
    envs[i]->release(84.5);
    // uncomment these to hear without envelope effects
    //envs[i]->attack(0.0);
    //envs[i]->hold(0.0);
    //envs[i]->decay(0.0);
    //envs[i]->release(0.0);
  }

  // Midi setup.
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);

  usbMIDI.setHandleRealTimeSystem(OnRealTimeSystem);
  usbMIDI.setHandleTimeCodeQuarterFrame(OnTimeCodeQuarterFrame);
}

unsigned long last_time = millis();
void loop() {
  usbMIDI.read(); // USB MIDI receive

  // Print statistics.
  if (1) {
    if (millis() - last_time >= 5000) {
      Serial.print("Proc = ");
      Serial.print(AudioProcessorUsage());
      Serial.print(" (");
      Serial.print(AudioProcessorUsageMax());
      Serial.print("),  Mem = ");
      Serial.print(AudioMemoryUsage());
      Serial.print(" (");
      Serial.print(AudioMemoryUsageMax());
      Serial.println(")");
      last_time = millis();
    }
  }
}


void OnNoteOn(byte channel, byte note, byte velocity) {
  Serial.print("Note On, ch=");
  Serial.print(channel, DEC);
  Serial.print(", note=");
  Serial.print(note, DEC);
  Serial.print(", velocity=");
  Serial.print(velocity, DEC);
  Serial.println();
  
  short wave_type = WAVEFORM_SINE;

  switch (channel) {
    case 1:
      wave_type = WAVEFORM_SINE;
      break;
    case 2:
      wave_type = WAVEFORM_SQUARE;
      break;
    case 3:
      wave_type = WAVEFORM_SAWTOOTH;
      break;
    case 4:
      wave_type = WAVEFORM_TRIANGLE;
      break;
  }

  for (int i = 0; i < 16; i++) {
    if (notes[i] == 0) {

      notes[i] = note;

      AudioNoInterrupts();

      waves[i]->begin(0.2,
                      tune_frequencies2_PGM[note],
                      wave_type);


      envs[i]->noteOn();

      AudioInterrupts();
      break;
    }
  }
}

void OnNoteOff(byte channel, byte note, byte velocity) {

  for (int i = 0; i < 16; i++) {
    if (notes[i] == note) {

      envs[i]->noteOff();

      // Setting the amplitude to 0 frees CPU,
      // but results in a click because it comes too early.
      // waves[i]->amplitude(0);
      notes[i] = 0;

      break;
    }
  }
}

void OnRealTimeSystem(byte realtimebyte) {
  Serial.print("Real time message: ");
  Serial.print(realtimebyte, DEC);
  Serial.println();
}

void OnTimeCodeQuarterFrame(uint16_t data) {
  Serial.print("Quarter Fram message: ");
  Serial.print(data, DEC);
  Serial.println();
}

