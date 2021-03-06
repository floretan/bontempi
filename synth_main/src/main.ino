
// For some reason these need to be included here or Arduino won't find them.
#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>

#include "synth.h"
#include "sampler.h"
#include "main_mixer.h"
#include "fscale.h"

#include "Arduino.h"

// Our main synth object.
Synth synth;
Sampler sampler;
MainMixer mainMixer;

AudioConnection patchCord1(synth.outputMixer, 0, mainMixer.inputMixer, 0);
AudioConnection patchCord2(sampler.outputMixer, 0, mainMixer.inputMixer, 1);


void setup() {
  Serial.begin(115200);
  // wait for Arduino Serial Monitor
  delay(1000);

  // Audio setup.
  AudioMemory(20);

  synth.setup();
  mainMixer.enable();
  sampler.enable();


  setupInputs();

  // Midi setup.
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleControlChange(OnControlChange);
}

unsigned long last_time = millis();

// Keep track of the frame count so we can read inputs less often.
byte frameCount = 0;

// How often to read inputs.
byte frameStep = 8;


unsigned long tickInterval = 1000;
float tickSignature = 0.25;
unsigned long lastTick = 0;
unsigned long lastTap = 0;
bool tapPressed = false;

void loop() {

  // Change this to if(1) for measurement output every 2 seconds
  if (1) {
    if (millis() - last_time >= 2000) {
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

  usbMIDI.read();

  readInputs();

  if (millis() - lastTick >= tickInterval * tickSignature) {
    synth.tick();
    lastTick = millis();
  }

  frameCount++;
}

void OnNoteOn(byte channel, byte note, byte velocity) {
  Serial.print("Note On, ch=");
  Serial.print(channel, DEC);
  Serial.print(", note=");
  Serial.print(note, DEC);
  Serial.print(", velocity=");
  Serial.print(velocity, DEC);
  Serial.println();

  synth.noteOn(note);
}

void OnNoteOff(byte channel, byte note, byte velocity) {

  synth.noteOff(note);
}

void OnControlChange(byte channel, byte control, byte value) {
  // Sustain pedal.
  if (control == 64) {
    // TODO: respond to sustain pedal message.
    if (value >= 64) {
      synth.sustain(true);
    }
    else {
      synth.sustain(false);
    }
  }
}

// Inputs
const byte multiplexAPin = 17;
const byte multiplexBPin = 16;
const byte multiplexCPin = 15;

const byte multiplexInputPin1 = 21;
const byte multiplexInputPin2 = 20;
const byte multiplexInputPin3 = A10;
const byte multiplexInputPin4 = A11;

const byte keyInputPins[7] = {
  8, 5, 4, 3, 2, 1, 0
};

int p0, p1, p2, p3, p4, p5, p6, p7;

const byte propagationDelay = 50;

// Keep track of which keys are pressed.
bool keyState[100];


void readAdditionalInputs() {

  byte pressed = digitalRead(keyInputPins[0]);

  byte channel = 4;

  if (pressed == LOW) {
    if (!keyState[0]) {
      // Send sustain pedal message.
      usbMIDI.sendControlChange(64, 127, channel);
      synth.sustain(true);
      keyState[0] = true;
    }
  }
  else {
    if (keyState[0]) {
      usbMIDI.sendControlChange(64, 0, channel);
      synth.sustain(false);
      keyState[0] = false;
    }
  }
}

/**
 * Read a 12-position selector.
 */
byte readSelectorPin(byte pin) {
  int interval = (1023 - 1) / 12; // There are twelve positions, with 11 intervals and one extra position to prevent short circuits.
  return (analogRead(pin)+interval/2)/interval - 1; // Substract the additional position.
}

void readInputKey(byte baseNote, byte keyOffset) {
  byte pressed = digitalRead(keyInputPins[keyOffset]);
  byte note = baseNote + keyOffset;
  byte velocity = 83;
  byte channel = 4;

  if (pressed == LOW) {
    if (!keyState[note]) {

      usbMIDI.sendNoteOn(note, velocity, channel);
      synth.noteOn(note);

      // TODO: this doesn't belong here.
      sampler.noteOn(note);
      keyState[note] = true;
    }
  }
  else {
    if (keyState[note]) {
      usbMIDI.sendNoteOff(note, velocity, channel);
      synth.noteOff(note);
      sampler.noteOff(note);
      keyState[note] = false;
    }
  }
}

void readInputKeyRow(byte baseNote) {
  for (unsigned int i = 0; i < sizeof(keyInputPins); i++) {
    readInputKey(baseNote, i);
  }
}


void setupInputs() {
  // Setup multiplex selector pins
  pinMode(multiplexAPin, OUTPUT);
  pinMode(multiplexBPin, OUTPUT);
  pinMode(multiplexCPin, OUTPUT);

  // Initialize multiplex selector pins.
  digitalWrite(multiplexAPin, LOW);
  digitalWrite(multiplexBPin, LOW);
  digitalWrite(multiplexCPin, LOW);


  // Setup input pins
  pinMode(multiplexInputPin1, INPUT);
  pinMode(multiplexInputPin2, INPUT);
  pinMode(multiplexInputPin3, INPUT);
  pinMode(multiplexInputPin4, INPUT);

  for (byte i = 0; i < sizeof(keyInputPins); i++) {
    pinMode(keyInputPins[i], INPUT_PULLUP);
  }

  // Initialize key state.
  for (byte i = 0; i < sizeof(keyState); i++) {
    keyState[i] = false;
  }
}

void readInputs() {

  /**
   * Multiplexer pin 0
   */
  digitalWrite(multiplexCPin, LOW);
  delayMicroseconds(propagationDelay);

  readInputKeyRow(39);

  if (frameCount % frameStep == 0) {
    // Mode depth
    int value = readSelectorPin(multiplexInputPin1);

    switch (value) {
      case 0:
      case 1:
        tickSignature = 1.0;
        break;

      case 2:
      case 3:
        tickSignature = 1.0/2;
        break;

      case 4:
      case 5:
        tickSignature = 1.0/4;
        break;

      case 6:
      case 7:
        tickSignature = 1.0/8;
        break;

      case 8:
      case 9:
        tickSignature = 1.0/16;
        break;

      case 10:
      case 11:
        tickSignature = 1.0/32;
        break;
    }

    // Amplitude envelope attack
    value = analogRead(multiplexInputPin2);
    synth.setAmpEnvAttack(fscale(1, 1023, 1, 2000, value, -5));

    // Filter cutoff
    value = analogRead(multiplexInputPin3);
    if (value != p3) {
     p3 = value;
     float freq = fscale(1, 1023, 200, 8000, p3, -3);
     synth.setFilterFrequency(freq);
    }

    // Preset selector
    value = readSelectorPin(multiplexInputPin4);
  }


  /**
   * Multiplexer pin 1
   */
  digitalWrite(multiplexAPin, HIGH);
  delayMicroseconds(propagationDelay);

  readInputKeyRow(53);

  if (frameCount % frameStep == 1) {

    // Synth osc 2 voice
    synth.setWaveForm2(readSelectorPin(multiplexInputPin1));

    // Filter envelope sustain.
    int value = analogRead(multiplexInputPin2);
    synth.setFilterEnvelopeSustain(fscale(1, 1023, 0.0, 1.0, value, 0));

    // LFO Waveform
    value = readSelectorPin(multiplexInputPin3);
    synth.setLFOWaveform(value);

    // Sampler selector
    sampler.setSound(readSelectorPin(multiplexInputPin4));
  }


  /**
   * Multiplexer pin 3
   */
  digitalWrite(multiplexBPin, HIGH);
  delayMicroseconds(propagationDelay);

  readInputKeyRow(74);

  if (frameCount % frameStep == 2) {

    // Transpose
    int value = analogRead(multiplexInputPin1);
    synth.setTranspose(map(value, 1, 1022, -12, 12));

    // Amplitude envelope sustain
    value = analogRead(multiplexInputPin2);
    synth.setAmpEnvSustain(fscale(1, 1023, 0.0, 1.0, value, 0));

    // Filter envelope amount
    value = analogRead(multiplexInputPin3);
    synth.setFilterEnvelopeAmount(fscale(1, 1023, -1.0, 1.0, value, 0));

    // Master volume
    value = analogRead(multiplexInputPin4);
    if (value != p7) {
     p7 = value;

     mainMixer.setMasterVolume((float)p7 / 1023 * 0.8);
    }
  }


  /**
   * Multiplexer pin 2
   */
  digitalWrite(multiplexAPin, LOW);
  delayMicroseconds(propagationDelay);

  if (frameCount % frameStep == 3) {

    // Synth osc 1
    synth.setWaveForm1(readSelectorPin(multiplexInputPin1));

    // Filter envelope attack
    int value = analogRead(multiplexInputPin2);
    synth.setFilterEnvelopeAttack(fscale(1, 1023, 1, 2000, value, -5));

    // LFO amplitude
    value = analogRead(multiplexInputPin3);
    synth.setLFOAmplitude(fscale(1, 1023, 0.0, 1.0, value, 0));

    // Sub volume
    value = analogRead(multiplexInputPin4);
    mainMixer.setReverbVolume(fscale(1, 1023, 0.0, 1.0, value, 0));
  }

  /**
   * Multiplexer pin 6
   */
  digitalWrite(multiplexCPin, HIGH);
  delayMicroseconds(propagationDelay);

  readInputKeyRow(81);

// int value = analogRead(multiplexInputPin1);
//   if (value > 30 && value < 500) {
//     Serial.println(value);
//   }

  // Tap tempo
  if (analogRead(multiplexInputPin1) > 500) {
    // Only count when the tap button is first pressed
    if (!tapPressed) {
      Serial.println("TAP");
      tickInterval = millis() - lastTap;
      lastTap = millis();

      tapPressed = true;
    }
  }
  else {
    tapPressed = false;
  }

  if (frameCount % frameStep == 4) {

    // Amplitude envelope release
    synth.setAmpEnvRelease(fscale(1, 1023, 1, 2000, analogRead(multiplexInputPin2), -5));

    // LFO target
    synth.setLFOTarget(readSelectorPin(multiplexInputPin3));

    // Sampler volume
    mainMixer.inputMixer.gain(1, fscale(1, 1023, 0.0, 1.0, analogRead(multiplexInputPin4), 0));
  }


  /**
  * Multiplexer pin 7
  */
  digitalWrite(multiplexAPin, HIGH);
  delayMicroseconds(propagationDelay);

  readInputKeyRow(67);

  if (frameCount % frameStep == 5) {

    // Detune
    int value = analogRead(multiplexInputPin1);
    // 1.0293022 = 24th root of 2 = quarter-step.
    float detune = fscale(1, 1023, 1, 1.0293022, value, 0);
    synth.setDetune(detune);

    // Amplitude envelope decay
    value = analogRead(multiplexInputPin2);
    synth.setAmpEnvDecay(fscale(1, 1023, 1, 2000, value, -5));

    // Filter resonance
    value = analogRead(multiplexInputPin3);
    float res = fscale(1, 1023, 0.0, 5.0, value, 0);
    synth.setFilterResonance(res);
  }


  /**
   * Multiplexer pin 5
   */
  digitalWrite(multiplexBPin, LOW);
  delayMicroseconds(propagationDelay);

  readInputKeyRow(60);

  if (frameCount % frameStep == 6) {
    // Mode selector
    int value = readSelectorPin(multiplexInputPin1);
    synth.setMode(value);

    // Filter envelope release
    value = analogRead(multiplexInputPin2);
    synth.setFilterEnvelopeRelease(fscale(1, 1023, 1, 2000, value, -5));

    // Sustain pedal?
    value = analogRead(multiplexInputPin3);

    // Sampler velocity
    value = analogRead(multiplexInputPin4);
    mainMixer.setReverbTime(fscale(1, 1023, 0, 10, value, -3));
  }

  /**
   * Multiplexer pin 4
   */
  digitalWrite(multiplexAPin, LOW);
  delayMicroseconds(propagationDelay);

  readInputKeyRow(46);

  if (frameCount % frameStep == 7) {
    // Synth mix
    int value = analogRead(multiplexInputPin1);
    synth.setMix(fscale(1, 1023, 0.0, 1.0, value, 0));

    // Filter envelope decay
    value = analogRead(multiplexInputPin2);
    synth.setFilterEnvelopeDecay(fscale(1, 1023, 1, 2000, value, -5));

    // LFO rate
    value = analogRead(multiplexInputPin3);
    synth.setLFORate(fscale(1, 1023, 0.05, 20.0, value, -2));

    // Mixer: Synth volume
    value = analogRead(multiplexInputPin4);
    mainMixer.inputMixer.gain(0, fscale(1, 1023, 0.0, 1.0, value, 0));
  }
}
