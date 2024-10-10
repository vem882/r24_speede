#include "buttons.h"



static unsigned long lastInterruptTimestamp = 0;

// Funktiojulistus
void handleButtonInterrupt();
void initButtonsAndButtonInterrupts(void) {
  // Enable pin change interrupts for buttonPins {5, 6, 7, 8}.
  PCICR |= (1 << PCIE0) | (1 << PCIE2);

  // Set up interrupts for pins 5, 6, 7 (Port D) and pin 8 (Port B).
  PCMSK0 |= (1 << PCINT0);  // Pin 8
  PCMSK2 |= (1 << PCINT21) | (1 << PCINT22) | (1 << PCINT23);  // Pins  5-7
  if (debug) {
  Serial.println("Initializing buttons");
  }
  for (int i = 0; i < 4; i++) {
    if (debug) {
    Serial.print("Button pin: ");
    Serial.println(buttonPins[i]);
    }
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
}

// Define the `handleButtonInterrupt()` function before the interrupt handlers.
void handleButtonInterrupt() {
  unsigned long thisInterruptTimestamp = millis();

  if (thisInterruptTimestamp - lastInterruptTimestamp > 200) {  // Debounce 200 ms
    for (int i = 0; i < 4; i++) {
      if (digitalRead(buttonPins[i]) == LOW) {
        buttonNumber = i;
        lastInterruptTimestamp = thisInterruptTimestamp;
        break;
      }
    }
  }
}

ISR(PCINT0_vect) {
  handleButtonInterrupt();
}

ISR(PCINT2_vect) {
  handleButtonInterrupt();
}
