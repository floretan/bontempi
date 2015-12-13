#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

byte notes[4] = {
  0, 0, 0, 0
};

// GUItool: begin automatically generated code
AudioPlaySdRaw           playSdRaw4;     //xy=170,345
AudioPlaySdRaw           playSdRaw2;     //xy=172,242
AudioPlaySdRaw           playSdRaw3;     //xy=173,288
AudioPlaySdRaw           playSdRaw1;     //xy=174,190

AudioPlaySdRaw *players[4] = {
  &playSdRaw1, &playSdRaw2, &playSdRaw3, &playSdRaw2
};

AudioMixer4              mixer1;         //xy=360,263
AudioOutputI2S           i2s1;           //xy=583,262
AudioConnection          patchCord1(playSdRaw4, 0, mixer1, 3);
AudioConnection          patchCord2(playSdRaw2, 0, mixer1, 1);
AudioConnection          patchCord3(playSdRaw3, 0, mixer1, 2);
AudioConnection          patchCord4(playSdRaw1, 0, mixer1, 0);
AudioConnection          patchCord5(mixer1, 0, i2s1, 0);
AudioConnection          patchCord6(mixer1, 0, i2s1, 1);
AudioControlSGTL5000     codec;     //xy=381,493
// GUItool: end automatically generated code

void setup() {
  Serial.begin(115200);

  AudioMemory(24);

  codec.enable();
  codec.volume(0.45);

  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);

  // Initialize the SD card
  SPI.setMOSI(7);
  SPI.setSCK(14);
  if (!(SD.begin(10))) {
    // stop here if no SD card, but print a message
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }


  delay(100);

  Serial.println(playSdRaw1.lengthMillis());

}

void loop() {
  usbMIDI.read();

  //  Serial.println(playSdRaw1.isPlaying());

  //  if (!playSdRaw1.isPlaying()) {
  //    playSdRaw1.stop();
  //  }


}

void OnNoteOn(byte channel, byte note, byte velocity) {
  Serial.println(note);

  for (int i = 0; i < 4; i++) {
    if (notes[i] == 0) {

      notes[i] = note;

      AudioNoInterrupts();


      if (!players[i]->isPlaying()) {

//        String s = "hto/";
//        
//        s.concat("hto_36.raw");
//
//        char __dataFileName[sizeof(s)];
//        s.toCharArray(__dataFileName, sizeof(__dataFileName));

        switch (note) {
          case 36:
            players[i]->play("hto/hto_36.raw");
            break;
          case 37:
            players[i]->play("hto/hto_37.raw");
            break;
          case 38:
            players[i]->play("hto/hto_38.raw");
            break;
          case 39:
            players[i]->play("hto/hto_39.raw");
            break;
          case 40:
            players[i]->play("hto/hto_40.raw");
            break;
          case 41:
            players[i]->play("hto/hto_41.raw");
            break;
          case 42:
            players[i]->play("hto/hto_42.raw");
            break;
          case 43:
            players[i]->play("hto/hto_43.raw");
            break;
          case 44:
            players[i]->play("hto/hto_44.raw");
            break;
          case 45:
            players[i]->play("hto/hto_45.raw");
            break;
          case 46:
            players[i]->play("hto/hto_46.raw");
            break;
          case 47:
            players[i]->play("hto/hto_47.raw");
            break;
          case 48:
            players[i]->play("hto/hto_48.raw");
            break;
          case 49:
            players[i]->play("hto/hto_49.raw");
            break;
          case 50:
            players[i]->play("hto/hto_50.raw");
            break;
          case 51:
            players[i]->play("hto/hto_51.raw");
            break;
          case 52:
            players[i]->play("hto/hto_52.raw");
            break;
          case 53:
            players[i]->play("hto/hto_53.raw");
            break;
          case 54:
            players[i]->play("hto/hto_54.raw");
            break;
          case 55:
            players[i]->play("hto/hto_55.raw");
            break;
          case 56:
            players[i]->play("hto/hto_56.raw");
            break;
          case 57:
            players[i]->play("hto/hto_57.raw");
            break;
          case 58:
            players[i]->play("hto/hto_58.raw");
            break;
          case 59:
            players[i]->play("hto/hto_59.raw");
            break;
        }

        AudioInterrupts();
        break;
      }
    }
  }
}

void OnNoteOff(byte channel, byte note, byte velocity) {
  for (int i = 0; i < 4; i++) {
    if (notes[i] == note) {

      players[i]->stop();

      // Setting the amplitude to 0 frees CPU,
      // but results in a click because it comes too early.
      // waves[i]->amplitude(0);
      notes[i] = 0;

      break;
    }
  }
}

