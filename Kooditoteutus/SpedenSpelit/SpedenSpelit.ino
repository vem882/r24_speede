#include "display.h"
#include "buttons.h"
#include "leds.h"
#include "SpedenSpelit.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <EEPROM.h>

// Use these 2 volatile variables for communicating between
// loop() function and interrupt handlers
volatile int buttonNumber = -1;           // for buttons interrupt handler
volatile bool newTimerInterrupt = false;  // for timer interrupt handler
int currentScore = 0;  // Oletusarvo voi olla 0, tai muu alkuarvo


// Peliin liittyvät muuttujat
byte randomLed = 0;
int timerInterruptCount = 0;
int gameSpeed = 5000;  // Alustava pelin nopeus

void setup() {
  /*
    Initialize here all modules
  */
  Serial.begin(9600);  // Serial Monitor debugging

  // Alustetaan kaikki moduulit
  initializeDisplay();
  initializeLeds();
  initButtonsAndButtonInterrupts();
  initializeTimer();
  
  // Aloitetaan peli
  initializeGame();
}

void loop() {
  if (buttonNumber >= 0) {
    // start the game if buttonNumber == 4 (pin 6)
    if (buttonNumber == 4) {
      startTheGame();
    }
    // check the game if 0 <= buttonNumber < 4
    else if (buttonNumber >= 0 && buttonNumber < 4) {
      checkGame(buttonNumber);
    }

    buttonNumber = -1;  // Reset button state after processing
  }

  if (newTimerInterrupt == true) {
    // Generate a new random number for LED
    clearAllLeds();
    randomLed = random(0, 4);  // Random LED between 0 and 3
    setLed(randomLed);  // Sytytetään sattumanvarainen LED
    
    // Lisätään pelin nopeutta joka 10. keskeytyksessä
    timerInterruptCount++;
    if (timerInterruptCount >= 10) {
      gameSpeed = max(200, gameSpeed - 100);  // Peli nopeutuu, mutta ei alle 200 ms
      OCR1A = gameSpeed * 16;  // Asetetaan uusi nopeus
      timerInterruptCount = 0;  // Nollataan laskuri
    }

    newTimerInterrupt = false;  // Nollataan keskeytyksen tilamuuttuja
  }
}

void initializeTimer(void) {
  // Set up Timer1 for generating regular interrupts
  TCCR1A = 0;  // Normal operation, no PWM
  TCCR1B = (1 << WGM12) | (1 << CS12);  // CTC mode, prescaler 256
  OCR1A = gameSpeed * 16;  // Initial compare value (for 1 second intervals)
  TIMSK1 |= (1 << OCIE1A);  // Enable Timer1 compare interrupt
}

ISR(TIMER1_COMPA_vect) {
  /*
  Communicate to loop() that it's time to make new random number.
  Increase timer interrupt rate after 10 interrupts.
  */
  newTimerInterrupt = true;  // Ilmoitetaan, että on aika luoda uusi satunnaisluku
}

void checkGame(byte nbrOfButtonPush) {
  // Tarkistetaan, onko painettu nappi oikea (sama kuin sattumanvarainen LED)
  if (nbrOfButtonPush == randomLed) {
    Serial.println("Oikea nappi painettu! Piste!");
    showResult(currentScore);  // Esimerkiksi näytetään tulos 10
  } else {
    Serial.println("Väärä nappi! Peli ohi.");
    showResult(currentScore);  // Näytetään nolla, peli ohi
    initializeGame();  // Aloitetaan peli uudelleen
  }
}

void initializeGame() {
  Serial.println("Pelin alustus.");
  clearAllLeds();  // Sammutetaan kaikki LEDit
  showResult(currentScore);  // Näytetään alussa 0
  gameSpeed = 5000;  // Resetoi pelin nopeus
  timerInterruptCount = 0;  // Nollaa keskeytyslaskuri
}

void startTheGame() {
  Serial.println("Peli alkaa!");
  randomLed = random(0, 4);  // Valitaan satunnainen LED
  setLed(randomLed);  // Sytytetään satunnainen LED
}
