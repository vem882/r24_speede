#include "display.h"
#include "buttons.h"
#include "leds.h"
#include "SpedenSpelit.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <EEPROM.h>

int debug = 1; // // Set to 1 to enable debug messages, 0 to disable
volatile int buttonNumber = -1;           // for buttons interrupt handler
volatile bool newTimerInterrupt = false;  // for timer interrupt handler
volatile int numberOfTimerInterrupts = 0; // Increased on every timer interrupt. Used to decrease timer interrupt interval
volatile int timerInterruptSpeed = 15624; // Timer interrupt interval (15624 = 1Hz)
volatile int currentScore = 0;  // CurrectScore for default 0
volatile int highScore = 0;  // HighScore by default 0, but will be readed from  EEPROM correct one
const int buttonPins[4] = {5,6,7,8}; // Buttons pins

// Variables related to the game
byte randomLed = 0; // randomLed function
volatile int timerInterruptCount = 0; 
volatile int gameSpeed = 1000;  // default gamespeed for start  (1 Hz)
volatile bool gameStarted = false;  // Game status 
volatile int buttonPressTime = 0;  // Button press time
volatile int gameStartTime = 0;  // Game starttime 
volatile bool countdownStarted = false;  // Countdown status 
volatile bool waitingForStart = true;  // waiting for game start
volatile unsigned long lastActivityTime = 0;  // last activity time
volatile unsigned long ledLitTime = 0;        // Time , when  LED turn on

void setup() {
  /*
    Initialize here all modules
  */
  if (debug) {
  Serial.begin(9600);  // Serial Monitor debugging
  delay(1000);
  // // Alustetaan kaikki moduulit
  Serial.println("********************** ");
  Serial.println("***** Debug mode ***** ");
  Serial.println("********************** ");
    }
  initializeDisplay();
  initializeLeds();
  initButtonsAndButtonInterrupts();
  initializeTimer();
  
  // loading highscore from  EEPROM memory
  highScore = EEPROM.read(0);
  
  // Show highscore on display before game starts
  updateDisplay(highScore);
  delay(2000);
  
  // initialize Game
  initializeGame();
}

void loop() {
  if (millis() - lastActivityTime > 60000) {
    // Switch to power-saving mode if there has been no activity for a minute.
    enterPowerSaveMode();
  }

  if (waitingForStart) {
    if (buttonNumber >= 0) {
      startCountdown();
      updateDisplay(highScore);
      waitingForStart = false;
      gameStarted = true;
      buttonNumber = -1;  // Reset button state after processing
      lastActivityTime = millis();  // Update the latest activity time.
    }
  } else {
    if (buttonNumber >= 0) {
      if (debug) {
      Serial.print("Pressed button: ");
      Serial.println(buttonNumber);
      }
      if (gameStarted) {
        // check the game if 0 <= buttonNumber < 4
        if (buttonNumber >= 0 && buttonNumber < 4) {
          checkGame(buttonNumber);
        }
      }
      buttonNumber = -1;  // Reset button state after processing
      lastActivityTime = millis();  // Update the latest activity time.
    }

    if (gameStarted && !countdownStarted && (millis() - ledLitTime >= 5000)) {
      if (debug) {
      Serial.println("Time's up! Game over.");
      Serial.println("Press any button for play again");
      }
      gameStarted = false;
      // If we have new highscore update highscore to EEPROM memory
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
          // Show Countdown before game starts on displays from 5 to 0
          int countdown = 5 - (millis() - gameStartTime) / 1000;
          if (countdown >= 0) {
            updateDisplay(countdown);
          } else {
            countdownStarted = false;
            startTheGame();
          }
        } else { }
      } else {
        // Show highscore before game starts
        updateDisplay(highScore);
      }

      newTimerInterrupt = false;  // Reset the interrupt state variable.
    }

    // Check if two buttons have been pressed for 2 seconds to reset the high score.
    if (digitalRead(2) == LOW && digitalRead(3) == LOW) {
      if (buttonPressTime == 0) {
        buttonPressTime = millis();
      } else if (millis() - buttonPressTime >= 2000) {
        highScore = 0;
        EEPROM.write(0, highScore);
        //updateDisplay(highScore);
        if (debug) {
        Serial.println("Checking if need to update Highscore");
        }
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
  newTimerInterrupt = true;  // Indicate that it's time to generate a new random number.
}

void initializeGame() {
  currentScore = 0;
  gameSpeed = 1000;  // 1 Hz
  timerInterruptCount = 0;
  gameStarted = false;
  countdownStarted = false;
  waitingForStart = true;
  clearAllLeds();
  //updateDisplay(highScore);  // Display the highest score at the start.
  if (debug) {
  Serial.println("Game initialized!");
  }
}

void startCountdown() {
  gameStartTime = millis();
  countdownStarted = true;
  if (debug) {
  Serial.println("Countdown started!");
  }
}

void startTheGame() {
  currentScore = 0;
  gameSpeed = 1000;  // 1 Hz
  timerInterruptCount = 0;
  gameStarted = true;
  updateDisplay(currentScore);  // Display 0 when the game starts.
  if (debug) {
  Serial.println("Game started!");
  }
  // Turn on the first LED and record the time it was lit.
  clearAllLeds();
  randomLed = random(0, 4);  // Random LED between 0 and 3
  setLed(randomLed);  // Turn on a random LED.
  ledLitTime = millis();  // Record lit Time
  if (debug) {
  Serial.print("LED lit: ");
  Serial.println(randomLed);
  }
}

void checkGame(int button) {
  if (button == randomLed) {
    currentScore++;
    if (debug) {
    Serial.print("Correct button! Score: ");
    Serial.println(currentScore);
    }
    updateDisplay(currentScore);  // Update score on display
    if (currentScore % 5 == 0) {
      gameSpeed = gameSpeed * 0.9;  // Game speeds up by 10%.
      if (gameSpeed < 100) gameSpeed = 100;  // Ensure the minimum speed.
      OCR1A = ((unsigned long)gameSpeed * 62500UL) / 1000 - 1;  // Set a new speed.
    }
    // Turn on a new LED and record the time it was lit.
    clearAllLeds();
    randomLed = random(0, 4);  // Random LED between 0 and 3
    setLed(randomLed);  // Turn on a random LED.
    ledLitTime = millis();  // Record the time the LED was lit.
    if (debug) {
    Serial.print("LED lit: ");
    Serial.println(randomLed);
    }
  } else {
    if (debug) {
    Serial.println("Wrong button! Game over.");
    }
    gameStarted = false;
    if (currentScore > highScore) {
      highScore = currentScore;
      EEPROM.write(0, highScore);  // Save the new highest score to the EEPROM.
    }
    updateDisplay(currentScore);  // Save the new highest score to the EEPROM.
    waitingForStart = true;  // Wait for the game to restart.
    clearAllLeds();
  }
}

void enterPowerSaveMode() {
  if (debug) {
  Serial.println("Entering power save mode...");
  }
  clearAllLeds();
  clearDisplay();

  unsigned long lastBlinkTime = 0;  // Tracks when the last LED blink occurred
  const unsigned long blinkInterval = 5000;  // Blink every 5 seconds
  int randomLed = 0;  // Variable to store the random LED index

  // Wait for any button press to exit power save mode
  while (true) {
    bool buttonPressed = false;

    // Check for button press
    for (int i = 0; i < 4; i++) {  // Assuming 4 buttons
      if (digitalRead(buttonPins[i]) == LOW) {  // Button press detected (active LOW)
        buttonPressed = true;
        break;  // Exit the for loop as soon as any button is pressed
      }
    }

    // Exit the power save mode if a button is pressed
    if (buttonPressed) {
      break;
    }

    // Check if it's time to blink a random LED
    if (millis() - lastBlinkTime >= blinkInterval) {
      // Clear all LEDs
      clearAllLeds();

      // Light up a random LED
      randomLed = random(0, 4);  // Random LED between 0 and 3
      setLed(randomLed);  // Turn on the random LED
      if (debug) {
      Serial.print("Random LED blinking: ");
      Serial.println(randomLed);
      }

      delay(200);  // Keep the LED on for 200ms fast blink in safemode

      // Turn off all LEDs after the blink
      clearAllLeds();

      // Update the last blink time
      lastBlinkTime = millis();
    }
  }
  if (debug) {
  Serial.println("Exiting power save mode...");
  }
  lastActivityTime = millis();  // Update last activity time
  startTheGame();  // Reinitialize the game
}
