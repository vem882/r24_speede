#include "buttons.h"

const int buttonPins[] = {5, 6, 7, 8};

static unsigned long lastInterruptTimestamp = 0;

void initButtonsAndButtonInterrupts(void) // Initializes all buttons
{
  PCICR |= (1 << PCIE0);  // Enable pin change interrupt for PCINT0
  PCMSK0 |= (1 << PCINT0) | (1 << PCINT1) | (1 << PCINT2) | (1 << PCINT3);  // Enable interrupts for pins 8, 9, 10, 11

  Serial.println("Ititializing buttons");
    for (int i = 0; i < sizeof(buttonPins) / sizeof(buttonPins[0]); i++)
    {
      Serial.print("Button pin: ");
      Serial.println(buttonPins[i]);
      pinMode(buttonPins[i], INPUT_PULLUP);
    }
}

ISR(PCINT0_vect) // got a button interrupt
{
  // Sources: https://docs.arduino.cc/built-in-examples/digital/Debounce/
  // Allows button presses every 50 ms to get rid of button debounce

  unsigned long thisInterruptTimestamp = millis();

  if (thisInterruptTimestamp - lastInterruptTimestamp > 200) // over 50 ms since last interrupt
  {
    for (int i = 0; i < sizeof(buttonPins) / sizeof(buttonPins[0]); i++)
    {
      if (digitalRead(buttonPins[i]) == LOW) // checks if any pins in buttonPins is LOW
      {
        buttonNumber = i; // buttonNumber will be 0-3
        lastInterruptTimestamp = thisInterruptTimestamp; // Updates the timestamp to be current interrupt time
        break;
      }
    }    
  }
}