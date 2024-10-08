#include "leds.h"  // Tämä riittää, älä sisällytä toista kertaa!

const int ledPins[] = {9, 10, 11, 12}; // Pins for LEDs
// LEDien alustaminen: 9, 10, 11, 12 määritellään ulostuloiksi
void initializeLeds()
{ 
    Serial.println("Ititializing leds");
    for (int i = 0; i < sizeof(ledPins) / sizeof(ledPins[0]); i++)
    {
        Serial.print("LED pin: ");
        Serial.println(ledPins[i]);
        pinMode(ledPins[i], OUTPUT);
    }
    setAllLeds();
    delay(3000);
    clearAllLeds();
}

// Yksittäisen LEDin sytyttäminen: Annetaan numero väliltä 0-3
void setLed(byte ledNumber)
{
    digitalWrite(ledPins[ledNumber], HIGH); 
}

void clearAllLeds()
{
    for (int i = 0; i < sizeof(ledPins) / sizeof(ledPins[0]); i++)
    {
        digitalWrite(ledPins[i], LOW);
    } 
}

void setAllLeds()
{
    for (int i = 0; i < sizeof(ledPins) / sizeof(ledPins[0]); i++)
    {
        digitalWrite(ledPins[i], HIGH);
    }
}

// Näyttää binäärinumerot 0-15 LEDeillä 
void show1() {
  for (int i = 0; i <= 15; i++) {
    // Muunnetaan numero binääriksi ja näytetään LEDeillä
    digitalWrite(ledPins[9], i & 0x1);  // ledPins[9] vastaa vähiten merkitsevää bittiä
    digitalWrite(ledPins[10], (i >> 1) & 0x1);  // ledPins[10] vastaa toista bittiä
    digitalWrite(ledPins[11], (i >> 2) & 0x1);  // ledPins[11] vastaa kolmatta bittiä
    digitalWrite(ledPins[12], (i >> 3) & 0x1);  // ledPins[12] vastaa neljättä bittiä

    delay(2000);  // Viive, jotta numerot näkyvät selkeästi
  }
  clearAllLeds();
}

// Näyttää LEDit ja lisää nopeutta jokaisen kierroksen jälkeen
void show2(int rounds) {
  int delayTime = 5000;  // Alustava viive
  for (int r = 0; r < rounds; r++) {
    for (int i = 0; i < 4; i++) {
      setLed(i);  // Sytytetään yksi LED kerrallaan
      delay(delayTime);  // Viive
    }
    delayTime = max(5000, delayTime - 1000);  // Pienennetään viivettä, mutta ei alle 1000 ms
  }
}
