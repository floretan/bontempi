#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

#include <math.h>


const int multiplexAPin = 17;
const int multiplexBPin = 16;
const int multiplexCPin = 15;
const int multiplexDataPin = 20;

int p0, p1, p2, p3, p4, p5, p6, p7;



// GUItool: begin automatically generated code
AudioSynthNoiseWhite     noise1;
AudioFilterStateVariable filter1;
AudioConnection          patchCord1(noise1, 0, filter1, 0);

AudioSynthNoiseWhite     noise2;
AudioFilterStateVariable filter2;
AudioConnection          patchCord2(noise2, 0, filter2, 0);


AudioMixer4              mixer1;
AudioMixer4              mixer2;

AudioConnection          patchCord14(filter1, 0, mixer1, 0);
AudioConnection          patchCord15(filter2, 0, mixer1, 1);

AudioConnection          patchCord16(filter1, 0, mixer2, 0);
AudioConnection          patchCord17(filter2, 0, mixer2, 1);

AudioOutputI2S           audioOut;           //xy=1100,508
AudioConnection          patchCord18(mixer1, 0, audioOut, 0);
AudioConnection          patchCord19(mixer2, 0, audioOut, 1);

AudioControlSGTL5000 codec;

// GUItool: end automatically generated code

unsigned long last_time = millis();

void setup() {
  AudioMemory(18);

  codec.enable();
  codec.volume(0.45);

  noise1.amplitude(0.5);
  noise2.amplitude(0.5);

  mixer1.gain(0, 0.36);
  mixer2.gain(1, 0.36);

  pinMode(multiplexAPin, OUTPUT);
  pinMode(multiplexBPin, OUTPUT);
  pinMode(multiplexCPin, OUTPUT);

  digitalWrite(multiplexAPin, LOW);
  digitalWrite(multiplexBPin, LOW);
  digitalWrite(multiplexCPin, LOW);
}

float freq = 1600;
float res = 0.7;
float multiplier = 1.01;

void loop() {

  // 0
  digitalWrite(multiplexCPin, LOW);
  delayMicroseconds(50);
  p0 = analogRead(multiplexDataPin);

  // 1
  digitalWrite(multiplexAPin, HIGH);
  delayMicroseconds(50);
  p1 = analogRead(multiplexDataPin);

  //  3
  digitalWrite(multiplexBPin, HIGH);
  delayMicroseconds(50);
  p3 = analogRead(multiplexDataPin);
  
  // 2
  digitalWrite(multiplexAPin, LOW);
  delayMicroseconds(50);
  p2 = analogRead(multiplexDataPin);

  // 6
  digitalWrite(multiplexCPin, HIGH);
  p6 = analogRead(multiplexDataPin);

  // 7
  digitalWrite(multiplexAPin, HIGH);
  delayMicroseconds(50);
  p7 = analogRead(multiplexDataPin);

  // 5
  digitalWrite(multiplexBPin, LOW);
  delayMicroseconds(50);
  p5 = analogRead(multiplexDataPin);

  // 4
  digitalWrite(multiplexAPin, LOW);
  delayMicroseconds(50);
  p4 = analogRead(multiplexDataPin);

  // put your main code here, to run repeatedly:

  if (millis() - last_time >= 10) {

    AudioNoInterrupts();
    freq = fscale(1, 1023, 200, 3200, p0, -2);
    
    filter1.frequency(freq);
    filter2.frequency(freq);
    

    
      res = fscale(1, 1023, 0.0, 5.0, p1, 0);
    
      filter1.resonance(res);
      filter2.resonance(res);
     
    AudioInterrupts();

    last_time = millis();
  }
}
