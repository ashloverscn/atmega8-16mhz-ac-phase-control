/*
this library is intended to be use with atmega8
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <Arduino.h>
#include "atmega8_16mhz_ac_phase_control.h"

#define MIN_PHASE_DELAY 1
#define MAX_PHASE_DELAY 623
#define TRIAC_PULSE_TICKS 2

static volatile int current_phase_delay = MAX_PHASE_DELAY;

void atmega8_16mhz_ac_phase_controlClass::init()
{
  // Zero-crossing pin (PD2 / INT0) as input with pull-up
  DDRD &= ~(1 << PD2);
  PORTD |= (1 << PD2);

  // TRIAC gate pin (PB1 / OC1A) as output, initially LOW
  DDRB |= (1 << PB1);
  PORTB &= ~(1 << PB1);

  // Configure Timer1 interrupts for Compare Match A and B
  TIMSK = (1 << OCIE1A) | (1 << OCIE1B);

  TCCR1A = 0x00;
  TCCR1B = 0x00;

  TCNT1 = 0x00;
  OCR1A = 0x00;
  OCR1B = 0x00;

  // Configure INT0 for falling edge trigger
  MCUCR &= ~((1 << ISC01) | (1 << ISC00));
  MCUCR |= (1 << ISC01);

  GICR |= (1 << INT0);

  sei();
}

void atmega8_16mhz_ac_phase_controlClass::uninit()
{
  PORTB &= ~(1 << PB1);

  DDRD = 0x00;
  PORTD = 0x00;

  DDRB = 0x00;
  PORTB = 0x00;

  TIMSK = 0x00;

  TCCR1A = 0x00;
  TCCR1B = 0x00;

  TCNT1 = 0x00;
  OCR1A = 0x00;
  OCR1B = 0x00;

  GICR = 0x00;
  MCUCR = 0x00;

  cli();
}

void atmega8_16mhz_ac_phase_controlClass::set_ac_power(int value)
{
  if (value < MIN_PHASE_DELAY)
  {
    value = MIN_PHASE_DELAY;
  }

  if (value > MAX_PHASE_DELAY)
  {
    value = MAX_PHASE_DELAY;
  }

  current_phase_delay = value;
}

// Zero-crossing Interrupt
ISR(INT0_vect)
{
  PORTB &= ~(1 << PB1); // Ensure TRIAC is off

  TCCR1B = 0x00;        // Stop timer
  TCNT1 = 0x00;         // Reset counter
  OCR1A = current_phase_delay; // Load phase delay target

  TIFR |= (1 << OCF1A) | (1 << OCF1B); // Clear pending match flags
  TCCR1B = 0x04;        // Start Timer1 with prescaler 256
}

// Phase delay reached -> Fire TRIAC pulse
ISR(TIMER1_COMPA_vect)
{
  PORTB |= (1 << PB1); // Turn on TRIAC pulse
  OCR1B = TCNT1 + TRIAC_PULSE_TICKS; // Schedule pulse cutoff
}

// Pulse width ticks reached -> Turn off TRIAC pulse
ISR(TIMER1_COMPB_vect)
{
  PORTB &= ~(1 << PB1); // Turn off TRIAC pulse
  TCCR1B = 0x00;        // Stop timer until next zero cross
}

atmega8_16mhz_ac_phase_controlClass atmega8_16mhz_ac_phase_control;
