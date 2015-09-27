
const int ledPin = 13;


void setup() {
  Serial.begin(115200);

  usbMIDI.setHandleRealTimeSystem(OnRealTimeSystem);
  usbMIDI.setHandleProgramChange(OnProgramChange);

  pinMode(ledPin, OUTPUT);
}

void loop() {
  usbMIDI.read(); // USB MIDI receive
}

int beatFragment = 0;
int beat = 0;
int beats_per_measure = 4;

void OnRealTimeSystem(byte realtimebyte) {

  // Clock message
  if (realtimebyte == 248) {
    beatFragment = (beatFragment + 1) % 24;
    if (beatFragment == 0) {
      beat = (beat + 1) % beats_per_measure;
      Serial.print(beat, DEC);
      Serial.println();
    }
  }

  // Start.
  else if (realtimebyte == 250) {
    beatFragment = 0;
    beat = 0;
  }
  // Other messages
  else {
    Serial.print("Realtime message: ");
    Serial.print(realtimebyte, DEC);
    Serial.println();
  }

  if (beatFragment == 0) {
    if (beat == 0) {
      analogWrite(ledPin, 1023);
    }
    else {
      analogWrite(ledPin, 128);
    }
  }
  else {
    analogWrite(ledPin, 0);
  }
}

void OnProgramChange(byte channel, byte program) {
  // Use program changes to set the number of beats per measure.
  beats_per_measure = program;
}

