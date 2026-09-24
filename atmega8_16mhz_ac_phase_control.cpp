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

void atmega8_16mhz_ac_phase_controlClass::init()
{
  DDRD &= ~(1 << PD2);
  PORTD |= (1 << PD2);

  DDRB |= (1 << PB1);
  PORTB &= ~(1 << PB1);

  TIMSK = (1 << OCIE1A) | (1 << OCIE1B);

  TCCR1A = 0x00;
  TCCR1B = 0x00;

  TCNT1 = 0x00;
  OCR1A = 0x00;
  OCR1B = 0x00;

  MCUCR &= ~((1 << ISC01) | (1 << ISC00));
  MCUCR |= (1 << ISC01) | (1 << ISC00);

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

  OCR1A = value;
}

ISR(INT0_vect)
{
  PORTB &= ~(1 << PB1);

  TCCR1B = 0x00;

  TCNT1 = 0x00;

  TCCR1B = 0x04;
}

ISR(TIMER1_COMPA_vect)
{
  PORTB |= (1 << PB1);

  OCR1B = TCNT1 + TRIAC_PULSE_TICKS;
}

ISR(TIMER1_COMPB_vect)
{
  PORTB &= ~(1 << PB1);

  TCCR1B = 0x00;
}

atmega8_16mhz_ac_phase_controlClass atmega8_16mhz_ac_phase_control;
