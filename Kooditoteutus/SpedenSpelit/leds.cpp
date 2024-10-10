#include "leds.h"

const int ledPins[] = {12, 11, 10, 9}; // Pins for LEDs

void initializeLeds()
{ 
    if (debug) {  
    Serial.println("Initializing leds");
    }
    for (int i = 0; i < 4; i++)
    { 
        if (debug) {
        Serial.print("LED pin: ");
        Serial.println(ledPins[i]);
        }
        pinMode(ledPins[i], OUTPUT);
    }
    setAllLeds();
    delay(3000);
    clearAllLeds();
}

void setLed(byte ledNumber)
{
    digitalWrite(ledPins[ledNumber], HIGH); 
}

void clearAllLeds()
{
    for (int i = 0; i < 4; i++)
    {
        digitalWrite(ledPins[i], LOW);
    } 
}

void setAllLeds()
{
    for (int i = 0; i < 4; i++)
    {
        digitalWrite(ledPins[i], HIGH);
    }
}
// Display binary numbers 0-15 on the LEDs. 
void show1() {
  for (int i = 0; i <= 15; i++) {
    // Convert the number to binary and display it on the LEDs.
    digitalWrite(ledPins[9], i & 0x1);  // `ledPins[9]` corresponds to the least significant bit (LSB).
    digitalWrite(ledPins[10], (i >> 1) & 0x1);  // `ledPins[10]` corresponds to the second bit.
    digitalWrite(ledPins[11], (i >> 2) & 0x1);  // `ledPins[11]` corresponds to the third bit.
    digitalWrite(ledPins[12], (i >> 3) & 0x1);  // `ledPins[12]` corresponds to the fourth bit.

    delay(2000);  // A delay to ensure the numbers are clearly visible.
  }
  clearAllLeds();
}

// Display the LEDs and increase the speed after each cycle.
void show2(int rounds) {
  int delayTime = 5000;  // Initial delay.
  for (int r = 0; r < rounds; r++) {
    for (int i = 0; i < 4; i++) {
      setLed(i);  // Light up one LED at a time.
      delay(delayTime);  // Delaytime
    }
    delayTime = max(5000, delayTime - 1000);  // Decrease the delay, but not below 1000 ms.
  }
}
