#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

const byte firstPin = 2; // First PinChangeInterrupt on D-bus
const byte lastPin =  13; // Last PinChangeInterrupt on D-bus

extern volatile int buttonNumber;
/* 
  initButtonsAndButtonInterrupts subroutine is called from Setup() function
  during the initialization of Speden Spelit. This function does the following:
  1) Initializes 4 button pins for the game = Arduino pins 2,3,4,5
  2) Initializes 1 button pin for starting the game = Arduino pin 6
  3) Enables PinChangeInterrupt on D-bus in a way that interrupt
     is generated whenever some of pins 2,3,4,5,6 is connected to LOW state
*/
void initButtonsAndButtonInterrupts(void);

/*
  PCINT2_vect Interrupt Service Routine (ISR) function for handling 
  Pin Change Interrupts on pins 2, 3, 4, 5 (D-bus on Arduino Uno).
*/
ISR(PCINT0_vect);

#endif
