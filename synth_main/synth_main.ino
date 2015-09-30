#include "synth.h"

// For some reason these need to be included here or Arduino won't find them.
#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>

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

// Keep track of which keys are pressed.
bool keyState[100];

float lfo_depth;
float lfo_rate;

// Our main synth object.
Synth synth; 

void setup() {

  Serial.begin(115200);
  // wait for Arduino Serial Monitor
  delay(1000);

  synth.setup();

  setupInputs();
  
  // Midi setup.
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
}

unsigned long last_time = millis();
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




