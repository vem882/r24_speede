#include "buttons.h"

// Function to initialize buttons and enable Pin Change Interrupts on D-bus
void initButtonsAndButtonInterrupts(void) {
  // Set pins 2, 3, 4, 5 as inputs with pull-up resistors (game buttons)
  for (byte pin = firstPin; pin <= lastPin; pin++) {
    pinMode(pin, INPUT_PULLUP);
  }

  // Set pin 6 (start button) as input with pull-up resistor
  pinMode(6, INPUT_PULLUP);

  // Enable Pin Change Interrupt on D-bus (PCINT16 to PCINT23, corresponding to pins D0-D7)
  PCICR |= (1 << PCIE2);  // Enable Pin Change Interrupts for D-bus (PCINT23..16)

  // Enable interrupts for specific pins (PCINT18 to PCINT21 correspond to pins D2 to D5)
  PCMSK2 |= (1 << PCINT18) | (1 << PCINT19) | (1 << PCINT20) | (1 << PCINT21);

  // Enable interrupt for pin 6 (PCINT22 corresponds to pin D6)
  PCMSK2 |= (1 << PCINT22);

  // Enable global interrupts
  sei();
}

// Interrupt Service Routine (ISR) for handling Pin Change Interrupts on pins 2, 3, 4, 5, and 6
ISR(PCINT2_vect) {
  // Check which button is pressed by scanning the pins 2, 3, 4, 5, and 6
  for (byte pin = firstPin; pin <= 6; pin++) {
    if (digitalRead(pin) == LOW) {
      Serial.print("Button pressed on pin ");
      Serial.println(pin);
      // Perform desired actions here based on which button was pressed
    }
  }
}
