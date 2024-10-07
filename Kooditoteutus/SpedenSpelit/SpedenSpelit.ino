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
int highScore = 0;  // Korkein pistemäärä, joka tallennetaan EEPROMiin

// Peliin liittyvät muuttujat
byte randomLed = 0;
int timerInterruptCount = 0;
int gameSpeed = 5000;  // Alustava pelin nopeus
bool gameStarted = false;  // Pelin tila
unsigned long buttonPressTime = 0;  // Painikkeen painalluksen aloitusaika

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
  
  // Ladataan korkein pistemäärä EEPROMista
  highScore = EEPROM.read(0);
  
  // Näytetään korkein pistemäärä alussa
  updateDisplay(highScore);
  
  // Aloitetaan peli
  initializeGame();
}

void loop() {
  if (buttonNumber >= 0) {
    if (!gameStarted) {
      startTheGame();
      gameStarted = true;
    } else {
      // check the game if 0 <= buttonNumber < 4
      if (buttonNumber >= 0 && buttonNumber < 4) {
        checkGame(buttonNumber);
      }
    }

    buttonNumber = -1;  // Reset button state after processing
  }

  if (newTimerInterrupt == true) {
    if (gameStarted) {
      // Generate a new random number for LED
      clearAllLeds();
      randomLed = random(0, 4);  // Random LED between 0 and 3
      setLed(randomLed);  // Sytytetään sattumanvarainen LED
    } else {
      // Näytetään korkein pistemäärä, kun peli ei ole käynnissä
      updateDisplay(highScore);
    }

    newTimerInterrupt = false;  // Nollataan keskeytyksen tilamuuttuja
  }

  // Tarkistetaan, onko kahta painiketta painettu 2 sekuntia high scoren nollaamiseksi
  if (digitalRead(2) == LOW && digitalRead(3) == LOW) {
    if (buttonPressTime == 0) {
      buttonPressTime = millis();
    } else if (millis() - buttonPressTime >= 2000) {
      highScore = 0;
      EEPROM.write(0, highScore);
      updateDisplay(highScore);
      Serial.println("High score reset!");
      buttonPressTime = 0;
    }
  } else {
    buttonPressTime = 0;
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

void initializeGame() {
  currentScore = 0;
  gameSpeed = 5000;
  timerInterruptCount = 0;
  gameStarted = false;
  clearAllLeds();
  updateDisplay(highScore);  // Näytetään korkein pistemäärä alussa
  Serial.println("Game initialized!");
}

void startTheGame() {
  currentScore = 0;
  gameSpeed = 5000;
  timerInterruptCount = 0;
  gameStarted = true;
  updateDisplay(currentScore);  // Näytetään 0, kun peli alkaa
  Serial.println("Game started!");
}

void checkGame(int button) {
  if (button == randomLed) {
    currentScore++;
    Serial.print("Correct button! Score: ");
    Serial.println(currentScore);
    updateDisplay(currentScore);  // Päivitetään näyttö
    if (currentScore % 5 == 0) {
      gameSpeed = gameSpeed * 0.9;  // Peli nopeutuu 10%
      OCR1A = (gameSpeed * 16) / 1000;  // Asetetaan uusi nopeus
    }
  } else {
    Serial.println("Wrong button! Game over.");
    gameStarted = false;
    if (currentScore > highScore) {
      highScore = currentScore;
      EEPROM.write(0, highScore);  // Tallennetaan uusi korkein pistemäärä EEPROMiin
    }
    updateDisplay(currentScore);  // Näytetään pelin tulos
  }
}