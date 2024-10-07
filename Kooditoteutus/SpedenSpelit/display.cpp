#include "display.h"

// Define pins for 7-segment display control
const int shiftClockPin = 3; 
const int latchClockPin = 4;
const int outEnablePin = 13;
const int serialInputPin = 2;

// Segment bits for numbers 0-9 (common cathode)
const uint8_t digitBits[] = {
  0b00111111, // 0
  0b00000110, // 1
  0b01011011, // 2
  0b01001111, // 3
  0b01100110, // 4
  0b01101101, // 5
  0b01111101, // 6
  0b00000111, // 7
  0b01111111, // 8
  0b01101111  // 9
};

void initializeDisplay(void) {
  // Initialize all the control pins as outputs
  Serial.println("Ititializing displays");
  pinMode(shiftClockPin, OUTPUT);
  pinMode(latchClockPin, OUTPUT);
  pinMode(outEnablePin, OUTPUT);
  pinMode(serialInputPin, OUTPUT);
  
  // Ensure the display is reset and disabled initially
  digitalWrite(outEnablePin, HIGH);  // Disable output
  digitalWrite(latchClockPin, LOW);
  
  // Reset the shift registers
  digitalWrite(outEnablePin, LOW);  // Enable output
}

void writeByte(uint8_t number, bool last) {
  // Send the byte corresponding to the 7-segment digit
  shiftOut(serialInputPin, shiftClockPin, MSBFIRST, digitBits[number]);
  
  // If this is the last byte, latch the data
  if (last) {
    digitalWrite(latchClockPin, HIGH);   // Pulse the latch to make the output visible
    delayMicroseconds(100);               // Short delay for stability
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
