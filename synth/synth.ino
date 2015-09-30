
// Inputs
const byte multiplexAPin = 17;
const byte multiplexBPin = 16;
const byte multiplexCPin = 15;
const byte multiplexDataPin = 21;
const byte multiplexPotPin = 20;

const byte keyInputPins[7] = {
  0, 1, 2, 3, 4, 5, 8
};

int p0, p1, p2, p3, p4, p5, p6, p7;

const byte propagationDelay = 50;

// State

// Keep track of which keys are pressed.
bool keyState[100];

byte waveform1;
byte waveform2;
float detune;
float volume;
float lfo_depth;
float lfo_rate;


// Audio
#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>

#include "frequencies.h"

byte notes[8] = {
  0, 0, 0, 0,
  0, 0, 0, 0
};

// First oscillator
AudioSynthWaveform sine0, sine1, sine2, sine3;
AudioSynthWaveform sine4, sine5, sine6, sine7;
AudioSynthWaveform *waves[8] = {
  &sine0, &sine1, &sine2, &sine3,
  &sine4, &sine5, &sine6, &sine7
};

// Second oscillator
AudioSynthWaveform sine10, sine11, sine12, sine13;
AudioSynthWaveform sine14, sine15, sine16, sine17;
AudioSynthWaveform *waves2[8] = {
  &sine10, &sine11, &sine12, &sine13,
  &sine14, &sine15, &sine16, &sine17
};

// Mix the two oscillators together
AudioMixer4 noteMixer0, noteMixer1, noteMixer2, noteMixer3;
AudioMixer4 noteMixer4, noteMixer5, noteMixer6, noteMixer7;

AudioConnection patchCord01(sine0, 0, noteMixer0, 0);
AudioConnection patchCord02(sine1, 0, noteMixer1, 0);
AudioConnection patchCord03(sine2, 0, noteMixer2, 0);
AudioConnection patchCord04(sine3, 0, noteMixer3, 0);
AudioConnection patchCord05(sine4, 0, noteMixer4, 0);
AudioConnection patchCord06(sine5, 0, noteMixer5, 0);
AudioConnection patchCord07(sine6, 0, noteMixer6, 0);
AudioConnection patchCord08(sine7, 0, noteMixer7, 0);

AudioConnection patchCord11(sine10, 0, noteMixer0, 1);
AudioConnection patchCord12(sine11, 0, noteMixer1, 1);
AudioConnection patchCord13(sine12, 0, noteMixer2, 1);
AudioConnection patchCord14(sine13, 0, noteMixer3, 1);
AudioConnection patchCord15(sine14, 0, noteMixer4, 1);
AudioConnection patchCord16(sine15, 0, noteMixer5, 1);
AudioConnection patchCord17(sine16, 0, noteMixer6, 1);
AudioConnection patchCord18(sine17, 0, noteMixer7, 1);


// Each waveform will be shaped by an envelope
AudioEffectEnvelope env0, env1, env2, env3;
AudioEffectEnvelope env4, env5, env6, env7;
AudioEffectEnvelope *envs[8] = {
  &env0, &env1, &env2, &env3,
  &env4, &env5, &env6, &env7
};

// Route each waveform through its own envelope effect
AudioConnection patchCord21(noteMixer0, env0);
AudioConnection patchCord22(noteMixer1, env1);
AudioConnection patchCord23(noteMixer2, env2);
AudioConnection patchCord24(noteMixer3, env3);
AudioConnection patchCord25(noteMixer4, env4);
AudioConnection patchCord26(noteMixer5, env5);
AudioConnection patchCord27(noteMixer6, env6);
AudioConnection patchCord28(noteMixer7, env7);

// Two mixers are needed to handle 8 channels of music
AudioMixer4     mixer1;
AudioMixer4     mixer2;

// Mix the 8 channels down to 2 audio streams
AudioConnection patchCord31(env0, 0, mixer1, 0);
AudioConnection patchCord32(env1, 0, mixer1, 1);
AudioConnection patchCord33(env2, 0, mixer1, 2);
AudioConnection patchCord34(env3, 0, mixer1, 3);
AudioConnection patchCord35(env4, 0, mixer2, 0);
AudioConnection patchCord36(env5, 0, mixer2, 1);
AudioConnection patchCord37(env6, 0, mixer2, 2);
AudioConnection patchCord38(env7, 0, mixer2, 3);

// Now create 2 mixers for the main output
AudioMixer4     mixerLeft;
AudioMixer4     mixerRight;
AudioOutputI2S  audioOut;

// Mix all channels to both the outputs
AudioConnection patchCord41(mixer1, 0, mixerLeft, 0);
AudioConnection patchCord42(mixer2, 0, mixerLeft, 1);
AudioConnection patchCord43(mixer1, 0, mixerRight, 0);
AudioConnection patchCord44(mixer2, 0, mixerRight, 1);

AudioConnection patchCord51(mixerLeft, 0, audioOut, 0);
AudioConnection patchCord52(mixerRight, 0, audioOut, 1);

AudioControlSGTL5000 codec;

void setup() {

  setupInputs();

  setupState();

  setupAudio();

  Serial.begin(115200);
  // wait for Arduino Serial Monitor
  delay(200);

  // Midi setup.
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
}

unsigned long last_time = millis();
void loop() {

  // Change this to if(1) for measurement output every 5 seconds
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

      Serial.print("Detune:");
      Serial.println(detune);
    }
  }

  usbMIDI.read();

  readInputs();
}

void OnNoteOn(byte channel, byte note, byte velocity) {
  Serial.print("Note On, ch=");
  Serial.print(channel, DEC);
  Serial.print(", note=");
  Serial.print(note, DEC);
  Serial.print(", velocity=");
  Serial.print(velocity, DEC);
  Serial.println();

  doNoteOn(note);
}

void OnNoteOff(byte channel, byte note, byte velocity) {

  doNoteOff(note);
}




