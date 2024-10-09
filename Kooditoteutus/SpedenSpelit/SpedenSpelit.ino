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
volatile int numberOfTimerInterrupts = 0; // Increased on every timer interrupt. Used to decrease timer interrupt interval
volatile int timerInterruptSpeed = 15624; // Timer interrupt interval (15624 = 1Hz)
volatile int currentScore = 0;  // Oletusarvo voi olla 0, tai muu alkuarvo
volatile int highScore = 0;  // Korkein pistemäärä, joka tallennetaan EEPROMiin

// Peliin liittyvät muuttujat
byte randomLed = 0;
volatile int timerInterruptCount = 0;
volatile int gameSpeed = 1000;  // Alustava pelin nopeus (1 Hz)
volatile bool gameStarted = false;  // Pelin tila
volatile int buttonPressTime = 0;  // Painikkeen painalluksen aloitusaika
volatile int gameStartTime = 0;  // Pelin käynnistysaika
volatile bool countdownStarted = false;  // Laskennan tila
volatile bool waitingForStart = true;  // Odotetaan pelin käynnistystä
volatile unsigned long lastActivityTime = 0;  // Viimeisin aktiivisuusaika
volatile unsigned long ledLitTime = 0;        // Aika, jolloin LED syttyi

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
  if (millis() - lastActivityTime > 60000) {
    // Siirrytään virransäästötilaan, jos ei ole ollut aktiivisuutta minuuttiin
    enterPowerSaveMode();
  }

  if (waitingForStart) {
    if (buttonNumber >= 0) {
      startCountdown();
      waitingForStart = false;
      gameStarted = true;
      buttonNumber = -1;  // Reset button state after processing
      lastActivityTime = millis();  // Päivitetään viimeisin aktiivisuusaika
    }
  } else {
    if (buttonNumber >= 0) {
      Serial.print("Pressed button: ");
      Serial.println(buttonNumber);
      if (gameStarted) {
        // check the game if 0 <= buttonNumber < 4
        if (buttonNumber >= 0 && buttonNumber < 4) {
          checkGame(buttonNumber);
        }
      }
      buttonNumber = -1;  // Reset button state after processing
      lastActivityTime = millis();  // Päivitetään viimeisin aktiivisuusaika
    }

    if (gameStarted && !countdownStarted && (millis() - ledLitTime >= 5000)) {
      Serial.println("Time's up! Game over.");
      gameStarted = false;
      if (currentScore > highScore) {
        highScore = currentScore;
        EEPROM.write(0, highScore);
      }
      updateDisplay(currentScore);
      waitingForStart = true;
      clearAllLeds();
    }

    if (newTimerInterrupt == true) {
      if (gameStarted) {
        if (countdownStarted) {
          // Näytetään laskenta 5, 4, 3, 2, 1, 0
          int countdown = 5 - (millis() - gameStartTime) / 1000;
          if (countdown >= 0) {
            updateDisplay(countdown);
          } else {
            countdownStarted = false;
            startTheGame();
          }
        } else {
          // Generate a new random number for LED
          // Tämä osa voidaan poistaa, koska uusi LED sytytetään checkGame()-funktion sisällä
        }
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
}

void initializeTimer(void) {
  // Set up Timer1 for generating regular interrupts
  TCCR1A = 0;  // Normal operation, no PWM
  TCCR1B = (1 << WGM12) | (1 << CS12);  // CTC mode, prescaler 256
  OCR1A = 62499;  // Initial compare value for 1 second intervals
  TIMSK1 |= (1 << OCIE1A);  // Enable Timer1 compare interrupt
}

ISR(TIMER1_COMPA_vect) {
  /*
  Communicate to loop() that it's time to make new random number.
  */
  newTimerInterrupt = true;  // Ilmoitetaan, että on aika luoda uusi satunnaisluku
}

void initializeGame() {
  currentScore = 0;
  gameSpeed = 1000;  // 1 Hz
  timerInterruptCount = 0;
  gameStarted = false;
  countdownStarted = false;
  waitingForStart = true;
  clearAllLeds();
  updateDisplay(highScore);  // Näytetään korkein pistemäärä alussa
  Serial.println("Game initialized!");
}

void startCountdown() {
  gameStartTime = millis();
  countdownStarted = true;
  Serial.println("Countdown started!");
}

void startTheGame() {
  currentScore = 0;
  gameSpeed = 1000;  // 1 Hz
  timerInterruptCount = 0;
  gameStarted = true;
  updateDisplay(currentScore);  // Näytetään 0, kun peli alkaa
  Serial.println("Game started!");
  // Sytytetään ensimmäinen LED ja tallennetaan syttymisaika
  clearAllLeds();
  randomLed = random(0, 4);  // Random LED between 0 and 3
  setLed(randomLed);  // Sytytetään sattumanvarainen LED
  ledLitTime = millis();  // Tallennetaan syttymisaika
  Serial.print("LED lit: ");
  Serial.println(randomLed);
}

void checkGame(int button) {
  if (button == randomLed) {
    currentScore++;
    Serial.print("Correct button! Score: ");
    Serial.println(currentScore);
    updateDisplay(currentScore);  // Päivitetään näyttö
    if (currentScore % 5 == 0) {
      gameSpeed = gameSpeed * 0.9;  // Peli nopeutuu 10%
      if (gameSpeed < 100) gameSpeed = 100;  // Varmistetaan miniminopeus
      OCR1A = ((unsigned long)gameSpeed * 62500UL) / 1000 - 1;  // Asetetaan uusi nopeus
    }
    // Sytytetään uusi LED ja tallennetaan syttymisaika
    clearAllLeds();
    randomLed = random(0, 4);  // Random LED between 0 and 3
    setLed(randomLed);  // Sytytetään sattumanvarainen LED
    ledLitTime = millis();  // Tallennetaan syttymisaika
    Serial.print("LED lit: ");
    Serial.println(randomLed);
  } else {
    Serial.println("Wrong button! Game over.");
    gameStarted = false;
    if (currentScore > highScore) {
      highScore = currentScore;
      EEPROM.write(0, highScore);  // Tallennetaan uusi korkein pistemäärä EEPROMiin
    }
    updateDisplay(currentScore);  // Näytetään pelin tulos
    waitingForStart = true;  // Odotetaan pelin uudelleenkäynnistystä
    clearAllLeds();
  }
}

void enterPowerSaveMode() {
  Serial.println("Entering power save mode...");
  clearAllLeds();
  clearDisplay();
  while (digitalRead(3) != LOW) {
    // Odotetaan, että oikeanpuoleista alanappia painetaan
  }
  Serial.println("Exiting power save mode...");
  lastActivityTime = millis();  // Päivitetään viimeisin aktiivisuusaika
  initializeGame();  // Alustetaan peli uudelleen
}
