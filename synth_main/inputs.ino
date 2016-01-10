

void setupInputs() {
  // Setup multiplex selector pins
  pinMode(multiplexAPin, OUTPUT);
  pinMode(multiplexBPin, OUTPUT);
  pinMode(multiplexCPin, OUTPUT);

  // Initialize multiplex selector pins.
  digitalWrite(multiplexAPin, LOW);
  digitalWrite(multiplexBPin, LOW);
  digitalWrite(multiplexCPin, LOW);

  // Is this one really needed?
  pinMode(multiplexDataPin, OUTPUT);

  // Setup input pins
  pinMode(multiplexPotPin, INPUT);

  for (byte i = 0; i < sizeof(keyInputPins); i++) {
    pinMode(keyInputPins[i], INPUT_PULLUP);
  }

  // Initialize key state.
  for (byte i = 0; i < sizeof(keyState); i++) {
    keyState[i] = false;
  }
}

void readInputs() {
  // 0
  digitalWrite(multiplexCPin, LOW);
  delayMicroseconds(propagationDelay);

  int value = analogRead(multiplexPotPin);

  if (value != p0) {
    p0 = value;

    if (value < 512) {
      switch (p0 * 4 / 512 ) {
        case 0:
          synth.setWaveForm1(WAVEFORM_SINE);
          break;

        case 1:
          synth.setWaveForm1(WAVEFORM_TRIANGLE);

          break;

        case 2:
          synth.setWaveForm1(WAVEFORM_SAWTOOTH);
          break;

        case 3:
          synth.setWaveForm1(WAVEFORM_SQUARE);
          break;
      }
    }
    else {
      synth.setWaveForm1(WAVEFORM_PULSE);

      float pw = fscale(512, 1023, 1.0, 0.1, p0, 0);
      synth.setPulseWidth(pw);
    }
  }

  digitalWrite(multiplexDataPin, LOW);
  readInputKeyRow(39);



  // 1
  digitalWrite(multiplexAPin, HIGH);
  delayMicroseconds(propagationDelay);

  value = analogRead(multiplexPotPin);
  if (value != p1) {
    p1 = value;

    switch (p1 * 5 / 1023 ) {
      case 0:
        synth.setWaveForm2(WAVEFORM_NOISE);
        break;

      case 1:
        synth.setWaveForm2(WAVEFORM_SINE);
        break;

      case 2:
        synth.setWaveForm2(WAVEFORM_TRIANGLE);
        break;

      case 3:
        synth.setWaveForm2(WAVEFORM_SAWTOOTH);
        break;

      case 4:
        synth.setWaveForm2(WAVEFORM_SQUARE);
        break;
    }
  }

  digitalWrite(multiplexDataPin, LOW);
  readInputKeyRow(53);

  //  3
  digitalWrite(multiplexBPin, HIGH);
  delayMicroseconds(propagationDelay);
  digitalWrite(multiplexDataPin, LOW);
  readInputKeyRow(74);

  value = analogRead(multiplexPotPin);
  if (value != p3) {
    p3 = value;
    float freq = fscale(1, 1023, 200, 8000, p3, -3);
    synth.setFilterFrequency(freq);
  }

  // 2
  digitalWrite(multiplexAPin, LOW);
  delayMicroseconds(propagationDelay);
  digitalWrite(multiplexDataPin, LOW);
//  readAdditionalInputs();

  value = analogRead(multiplexPotPin);

  // 1.0293022 = 24th root of 2 = quarter-step.
  float detune = fscale(1, 1023, 1, 1.0293022, value, 0);
  synth.setDetune(detune);

  // 6
  digitalWrite(multiplexCPin, HIGH);
  delayMicroseconds(propagationDelay);
  digitalWrite(multiplexDataPin, LOW);
  readInputKeyRow(81);

  value = analogRead(multiplexPotPin);

  if (value >= 512) {
    synth.setFilterLFOAmount(fscale(512, 1023, 0, 3, value, -3));
  }
  else {
    synth.setAmplitudeModulationLFOAmount(fscale(1, 512, 1.0, 0.0, value, 3));
  }

  // 7
  digitalWrite(multiplexAPin, HIGH);
  delayMicroseconds(propagationDelay);
  digitalWrite(multiplexDataPin, LOW);
  readInputKeyRow(67);

  value = analogRead(multiplexPotPin);
  if (value != p7) {
    p7 = value;

    synth.setMasterVolume((float)p7 / 1023 * 0.8);
  }

  // 5
  digitalWrite(multiplexBPin, LOW);
  delayMicroseconds(propagationDelay);
  digitalWrite(multiplexDataPin, LOW);
  readInputKeyRow(60);

  value = analogRead(multiplexPotPin);
  synth.setLFORate(fscale(1, 1023, 0.05, 10.0, value, -1));


  // 4
  digitalWrite(multiplexAPin, LOW);
  delayMicroseconds(propagationDelay);
  digitalWrite(multiplexDataPin, LOW);
  readInputKeyRow(46);

  value = analogRead(multiplexPotPin);
  float res = fscale(1, 1023, 0.0, 5.0, value, 0);
  synth.setFilterResonance(res);
}

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

void readInputKeyRow(byte baseNote) {
  for (unsigned int i = 0; i < sizeof(keyInputPins); i++) {
    readInputKey(baseNote, i);
  }
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
      keyState[note] = true;
    }
  }
  else {
    if (keyState[note]) {
      usbMIDI.sendNoteOff(note, velocity, channel);
      synth.noteOff(note);
      keyState[note] = false;
    }
  }
}
