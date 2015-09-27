#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

// GUItool: begin automatically generated code
AudioSynthWaveformDc     dc1;            //xy=211,443
AudioEffectEnvelope      envelope1;      //xy=374,446
AudioSynthWaveformSine   sine1;          //xy=385,391.5
AudioSynthNoiseWhite     noise1;         //xy=488.75000762939453,301.2500057220459
AudioMixer4              mixer1;         //xy=521,426
AudioFilterStateVariable filter1;        //xy=688.75,371.25
AudioOutputI2S           i2s1;           //xy=861.2500152587891,358.0000057220459
AudioConnection          patchCord1(dc1, envelope1);
AudioConnection          patchCord2(envelope1, 0, mixer1, 1);
AudioConnection          patchCord3(sine1, 0, mixer1, 0);
AudioConnection          patchCord4(noise1, 0, filter1, 0);
AudioConnection          patchCord5(mixer1, 0, filter1, 1);
AudioConnection          patchCord6(filter1, 0, i2s1, 0);
AudioConnection          patchCord7(filter1, 0, i2s1, 1);
// GUItool: end automatically generated code



void setup() {
  AudioMemory(18);

  noise1.amplitude(0.5);

  sine1.amplitude(0.1);
  sine1.frequency(0.1);

  dc1.amplitude(1);
}

unsigned long last_time = millis();

void loop() {
  if (millis() - last_time >= 1000) {

    AudioNoInterrupts();

    envelope1.noteOff();
    AudioInterrupts();
  }
  
  if (millis() - last_time >= 2000) {

    AudioNoInterrupts();

    envelope1.noteOn();

    AudioInterrupts();

    last_time = millis();
  }

  

}
