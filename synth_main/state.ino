
void setupState() {
  // Initialize key state.
  for (byte i = 0; i < sizeof(keyState); i++) {
    keyState[i] = false;
  }

  // Initialise controls.
  waveform1 = WAVEFORM_SINE;
  waveform2 = WAVEFORM_SINE;

  volume = 0.5;
}

