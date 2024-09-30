#include <avr/interrupt.h>

// ========================== LEDS FUNCTIONS ==========================

// LEDien alustaminen: A2, A3, A4, A5 määritellään ulostuloiksi
void initializeLeds() {
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  clearAllLeds();  // Varmista, että kaikki LEDit ovat aluksi pois päältä
}

// Yksittäisen LEDin sytyttäminen: Annetaan numero väliltä 0-3
void setLed(byte ledNumber) {
  clearAllLeds();  // Sammutetaan kaikki LEDit ennen uuden sytyttämistä
  
  switch (ledNumber) {
    case 0:
      digitalWrite(9, HIGH);
      break;
    case 1:
      digitalWrite(10, HIGH);
      break;
    case 2:
      digitalWrite(11, HIGH);
      break;
    case 3:
      digitalWrite(12, HIGH);
      break;
    default:
      break;
  }
}

// Kaikkien LEDien sammuttaminen
void clearAllLeds() {
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
  digitalWrite(12, LOW);
}

// ========================== BUTTONS FUNCTIONS ==========================

const byte firstPin = 5;  // First PinChangeInterrupt on D-bus
const byte lastPin = 8;   // Last PinChangeInterrupt on D-bus
volatile int buttonNumber = -1;  // Track button presses

// Initialize buttons and enable Pin Change Interrupts on D-bus
void initButtonsAndButtonInterrupts(void) {
  for (byte pin = firstPin; pin <= lastPin; pin++) {
    pinMode(pin, INPUT_PULLUP);
  }
  pinMode(6, INPUT_PULLUP);  // Start button (pin 6)

  PCICR |= (1 << PCIE2);  // Enable Pin Change Interrupts for D-bus (PCINT23..16)
  PCMSK2 |= (1 << PCINT18) | (1 << PCINT19) | (1 << PCINT20) | (1 << PCINT21);
  PCMSK2 |= (1 << PCINT22);  // Enable interrupt for pin 6 (PCINT22)
  sei();  // Enable global interrupts
}

// Interrupt Service Routine for buttons
ISR(PCINT2_vect) {
  for (byte pin = firstPin; pin <= 6; pin++) {
    if (digitalRead(pin) == LOW) {
      buttonNumber = pin - 2;  // Store the button number
      break;
    }
  }
}

// ========================== DISPLAY FUNCTIONS ==========================

// Segment bits for numbers 0-9 (common cathode)
const uint8_t digitBits[10] = {
  B11111100,  // 0  ABCDEF-
  B01100000,  // 1  -BC----
  B11011010,  // 2  AB-DE-G
  B11110010,  // 3  ABCD--G
  B01100110,  // 4  -BC--FG
  B10110110,  // 5  A-CD-EF
  B10111110,  // 6  A-BCDEF
  B11100000,  // 7  ABC----
  B11111110,  // 8  ABCDEFG
  B11110110   // 9  ABCD-FG
};

// Define pins for 7-segment display control
const int resetPin = 13;
const int shiftClockPin = 3;
const int latchClockPin = 4;
const int serialInputPin = 2;

// Initialize the 7-segment display and shift registers
void initializeDisplay(void) {
  pinMode(resetPin, OUTPUT);
  pinMode(shiftClockPin, OUTPUT);
  pinMode(latchClockPin, OUTPUT);
  pinMode(serialInputPin, OUTPUT);

  digitalWrite(resetPin, HIGH);  // Release reset
}

// Shift out a byte and latch it if needed
void writeByte(uint8_t number, bool last) {
  digitalWrite(latchClockPin, LOW);  // Disable latch during shifting

  // Shift out the bits to the 74HC595s
  shiftOut(serialInputPin, shiftClockPin, MSBFIRST, digitBits[number]);

  if (last) {
    digitalWrite(latchClockPin, HIGH);  // Latch the data
    delayMicroseconds(10);  // Short delay for stability
    digitalWrite(latchClockPin, LOW);  // Bring latch back low
  }
}

// Update the 7-segment display with a 2-digit number
void writeHighAndLowNumber(uint8_t tens, uint8_t ones) {
  writeByte(tens, false);  // Write tens digit without latching
  writeByte(ones, true);   // Write ones digit and latch
}

// Show a 2-digit result on the display
void showResult(byte result) {
  uint8_t tens = result / 10;
  uint8_t ones = result % 10;
  writeHighAndLowNumber(tens, ones);
}

// ========================== GAME AND TIMER FUNCTIONS ==========================

volatile bool newTimerInterrupt = false;  // Timer interrupt flag
int timerInterruptCount = 0;
int gameSpeed = 1000;  // Initial speed of the game
byte randomLed = 0;

// Initialize Timer1 for game timing
void initializeTimer(void) {
  TCCR1A = 0;
  TCCR1B = (1 << WGM12) | (1 << CS12);  // CTC mode, prescaler 256
  OCR1A = gameSpeed * 16;  // Set compare match register
  TIMSK1 |= (1 << OCIE1A);  // Enable Timer1 compare interrupt
}

// Timer1 compare interrupt service routine
ISR(TIMER1_COMPA_vect) {
  newTimerInterrupt = true;  // Signal to loop() to generate a new random number
}

// Check if the correct button was pressed
void checkGame(byte nbrOfButtonPush) {
  if (nbrOfButtonPush == randomLed) {
    Serial.println("Oikea nappi painettu! Piste!");
    showResult(10);  // Show 10 as an example score
  } else {
    Serial.println("Väärä nappi! Peli ohi.");
    showResult(0);  // Show 0 when game is over
    initializeGame();  // Reset game
  }
}

// Initialize the game state
void initializeGame() {
  Serial.println("Pelin alustus.");
  clearAllLeds();
  showResult(0);
  gameSpeed = 1000;
  timerInterruptCount = 0;
}

// Start a new game
void startTheGame() {
  Serial.println("Peli alkaa!");
  randomLed = random(0, 4);  // Random LED
  setLed(randomLed);  // Turn on a random LED
}

// ========================== MAIN PROGRAM ==========================

void setup() {
  Serial.begin(9600);  // Initialize serial communication
  initializeDisplay();  // Initialize the 7-segment display
  initializeLeds();     // Initialize the LEDs
  initButtonsAndButtonInterrupts();  // Initialize buttons and interrupts
  initializeTimer();    // Initialize Timer1
  initializeGame();     // Initialize game state
}

void loop() {
  // Handle button presses
  if (buttonNumber >= 0) {
    if (buttonNumber == 4) {  // Button 4 (pin 6) starts the game
      startTheGame();
    } else if (buttonNumber >= 0 && buttonNumber < 4) {  // Buttons 0-3
      checkGame(buttonNumber);
    }
    buttonNumber = -1;  // Reset button state after processing
  }

  // Handle Timer1 interrupts
  if (newTimerInterrupt) {
    randomLed = random(0, 4);  // Generate a new random LED
    setLed(randomLed);  // Turn on the new random LED

    timerInterruptCount++;
    if (timerInterruptCount >= 10) {
      gameSpeed = max(200, gameSpeed - 100);  // Increase game speed over time
      OCR1A = gameSpeed * 16;  // Update the timer interval
      timerInterruptCount = 0;
    }

    newTimerInterrupt = false;  // Reset interrupt flag
  }
}
