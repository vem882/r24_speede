#include "display.h"

// Define pins for 7-segment display control
const int resetPin = 12;
const int shiftClockPin = 11;
const int latchClockPin = 10;
const int outEnablePin = 9;
const int serialInputPin = 8;

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

void initializeDisplay(void) {
  // Initialize all the control pins as outputs
  pinMode(resetPin, OUTPUT);
  pinMode(shiftClockPin, OUTPUT);
  pinMode(latchClockPin, OUTPUT);
  pinMode(outEnablePin, OUTPUT);
  pinMode(serialInputPin, OUTPUT);
  
  // Ensure the display is reset and disabled initially
  digitalWrite(resetPin, LOW);
  digitalWrite(outEnablePin, HIGH);  // Disable output
  digitalWrite(latchClockPin, LOW);
  
  // Reset the shift registers
  digitalWrite(resetPin, HIGH);  // Release reset pin
  digitalWrite(outEnablePin, LOW);  // Enable output
}

void writeByte(uint8_t number, bool last) {
  // Send the byte corresponding to the 7-segment digit
  shiftOut(serialInputPin, shiftClockPin, MSBFIRST, digitBits[number]);
  
  // If this is the last byte, latch the data
  if (last) {
    digitalWrite(latchClockPin, HIGH);   // Pulse the latch to make the output visible
    delayMicroseconds(10);               // Short delay for stability
    digitalWrite(latchClockPin, LOW);    // Bring the latch low again
  }
}

void writeHighAndLowNumber(uint8_t tens, uint8_t ones) {
  // Write the tens first without latching
  writeByte(tens, false);
  
  // Write the ones and latch it
  writeByte(ones, true);  // Latch this one
}

void showResult(byte result) {
  // Calculate the tens and ones from the result
  uint8_t tens = result / 10;
  uint8_t ones = result % 10;
  
  // Write the result to the display
  writeHighAndLowNumber(tens, ones);
}
