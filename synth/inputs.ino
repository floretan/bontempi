

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
}

void readInputs() {
  // 0
  digitalWrite(multiplexCPin, LOW);
  delayMicroseconds(propagationDelay);

  int value = analogRead(multiplexPotPin);
  if (value != p0) {
    p0 = value;

    switch(p0 * 4 / 1023 ) {
      case 0:
        waveform1 = WAVEFORM_SINE;
        break;
    
      case 1:
        waveform1 = WAVEFORM_SQUARE;
        break;
        
      case 2:
        waveform1 = WAVEFORM_SAWTOOTH;
        break;
      case 3: 
        waveform1 = WAVEFORM_TRIANGLE;
        break;
    }
  }
  
  digitalWrite(multiplexDataPin, LOW);
  readInputKeyRow(67);



  // 1
  digitalWrite(multiplexAPin, HIGH);
  delayMicroseconds(propagationDelay);

  value = analogRead(multiplexPotPin);
  if (value != p1) {
    p1 = value;

    switch(p1 * 4 / 1023 ) {
      case 0:
        waveform2 = WAVEFORM_SINE;
        break;
    
      case 1:
        waveform2 = WAVEFORM_SQUARE;
        break;
        
      case 2:
        waveform2 = WAVEFORM_SAWTOOTH;
        break;
      case 3: 
        waveform2 = WAVEFORM_TRIANGLE;
        break;
    }
  }

  digitalWrite(multiplexDataPin, LOW);
  readInputKeyRow(74);

  //  3
  digitalWrite(multiplexBPin, HIGH);
  delayMicroseconds(propagationDelay);
  digitalWrite(multiplexDataPin, LOW);
  readInputKeyRow(60);

  value = analogRead(multiplexPotPin);
  if (value != p3) {
    p3 = value;
    lfo_depth = (float)p3 / 1023;
  }

  // 2
  digitalWrite(multiplexAPin, LOW);
  delayMicroseconds(propagationDelay);
  digitalWrite(multiplexDataPin, LOW);
  readInputKeyRow(81);

  value = analogRead(multiplexPotPin);
  if (value != p2) {
    p2 = value;

    // 1.0293022 = 24th root of 2 = quarter-step.
    detune = fscale(1, 1023, 1, 1.0293022, p2, 0);
  }

  // 6
  digitalWrite(multiplexCPin, HIGH);
  delayMicroseconds(propagationDelay);
  digitalWrite(multiplexDataPin, LOW);
  readInputKeyRow(46);

  // 7
  digitalWrite(multiplexAPin, HIGH);
  delayMicroseconds(propagationDelay);
  digitalWrite(multiplexDataPin, LOW);
  readInputKeyRow(53);

  value = analogRead(multiplexPotPin);
  if (value != p7) {
    p7 = value;
    volume = (float)p7 / 1023;
    // TODO: move this to the state.
    codec.volume(volume);
  }

  // 5
  digitalWrite(multiplexBPin, LOW);
  delayMicroseconds(propagationDelay);
  digitalWrite(multiplexDataPin, LOW);
//  readInputKeyRow(0);

  // 4
  digitalWrite(multiplexAPin, LOW);
  delayMicroseconds(propagationDelay);
  digitalWrite(multiplexDataPin, LOW);
  readInputKeyRow(39);
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
      doNoteOn(note);
      keyState[note] = true;
    }
  }
  else {
    if (keyState[note]) {
      usbMIDI.sendNoteOff(note, velocity, channel);
      doNoteOff(note);
      keyState[note] = false;
    }
  }
}
