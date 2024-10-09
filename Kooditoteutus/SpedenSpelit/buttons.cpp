#include "buttons.h"

const int buttonPins[] = {8, 7, 6, 5};

static unsigned long lastInterruptTimestamp = 0;

// Funktiojulistus
void handleButtonInterrupt();

void initButtonsAndButtonInterrupts(void) {
  // Ota käyttöön pin change -keskeytykset Portti B:lle (pin 8) ja Portti D:lle (pinnit 5-7)
  PCICR |= (1 << PCIE0) | (1 << PCIE2);

  // Määritä keskeytykset pinneille 5,6,7 (Portti D) ja 8 (Portti B)
  PCMSK0 |= (1 << PCINT0);  // Pin 8
  PCMSK2 |= (1 << PCINT21) | (1 << PCINT22) | (1 << PCINT23);  // Pinnit 5-7

  Serial.println("Initializing buttons");
  
  for (int i = 0; i < 4; i++) {
    Serial.print("Button pin: ");
    Serial.println(buttonPins[i]);
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
}

// Määritetään handleButtonInterrupt()-funktio ennen keskeytyskäsittelijöitä
void handleButtonInterrupt() {
  unsigned long thisInterruptTimestamp = millis();

  if (thisInterruptTimestamp - lastInterruptTimestamp > 50) {  // Debounce 50 ms
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
