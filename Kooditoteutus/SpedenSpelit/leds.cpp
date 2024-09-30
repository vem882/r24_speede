#include "leds.h"  // Tämä riittää, älä sisällytä toista kertaa!

// LEDien alustaminen: A2, A3, A4, A5 määritellään ulostuloiksi
void initializeLeds() {
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
  clearAllLeds();  // Varmista, että kaikki LEDit ovat aluksi pois päältä
}

// Yksittäisen LEDin sytyttäminen: Annetaan numero väliltä 0-3
void setLed(byte ledNumber) {
  clearAllLeds();  // Sammutetaan kaikki LEDit ennen uuden sytyttämistä
  
  switch(ledNumber) {
    case 0: 
      digitalWrite(A2, HIGH);
      break;
    case 1: 
      digitalWrite(A3, HIGH);
      break;
    case 2: 
      digitalWrite(A4, HIGH);
      break;
    case 3: 
      digitalWrite(A5, HIGH);
      break;
    default:
      // Virheellinen numero, ei tehdä mitään
      break;
  }
}

// Kaikkien LEDien sammuttaminen
void clearAllLeds() {
  digitalWrite(A2, LOW);
  digitalWrite(A3, LOW);
  digitalWrite(A4, LOW);
  digitalWrite(A5, LOW);
}

// Kaikkien LEDien sytyttäminen
void setAllLeds() {
  digitalWrite(A2, HIGH);
  digitalWrite(A3, HIGH);
  digitalWrite(A4, HIGH);
  digitalWrite(A5, HIGH);
}

// Näyttää binäärinumerot 0-15 LEDeillä A2-A5
void show1() {
  for (int i = 0; i <= 15; i++) {
    // Muunnetaan numero binääriksi ja näytetään LEDeillä
    digitalWrite(A2, i & 0x1);  // A2 vastaa vähiten merkitsevää bittiä
    digitalWrite(A3, (i >> 1) & 0x1);  // A3 vastaa toista bittiä
    digitalWrite(A4, (i >> 2) & 0x1);  // A4 vastaa kolmatta bittiä
    digitalWrite(A5, (i >> 3) & 0x1);  // A5 vastaa neljättä bittiä

    delay(250);  // Viive, jotta numerot näkyvät selkeästi
  }
}

// Näyttää LEDit 0-1-2-3-0-1-2-3 ... ja lisää nopeutta jokaisen kierroksen jälkeen
void show2(int rounds) {
  int delayTime = 500;  // Alustava viive
  for (int r = 0; r < rounds; r++) {
    for (int i = 0; i < 4; i++) {
      setLed(i);  // Sytytetään yksi LED kerrallaan
      delay(delayTime);  // Viive
    }
    delayTime = max(50, delayTime - 50);  // Pienennetään viivettä, mutta ei alle 50 ms
  }
}
