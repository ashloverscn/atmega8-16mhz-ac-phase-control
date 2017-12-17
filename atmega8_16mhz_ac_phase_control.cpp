/*
this library is intended to be use with atmega8
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <Arduino.h>
#include "atmega8_16mhz_ac_phase_control.h"


 void atmega8_16mhz_ac_phase_controlClass::init()
{
  //setup input and output
  
  DDRD &= ~(1 << PD2); //INT0 pin as input for zero cross detect 
  PORTD |= (1 << PD2); //enable pull-up resistor for PD2
  DDRB |= (1 << PB1); //triac gate control pin as output
  
  //setup timer
  
  // set up Timer 
  //(see ATMEGA 8 data sheet for more details)
  //OCR1A = 100;      //initialize the comparator
  TIMSK = 0x14;    //enable comparator A and overflow interrupts
  TCCR1A = 0x00;    //timer control registers set for
  TCCR1B = 0x00;    //normal operation, timer disabled

  //setup interrupt
  
  //PD2 INT0 set up zero crossing interrupt on RISING edge
  MCUCR |= (1 << ISC01);MCUCR |= (1 << ISC00);
  GICR |= (1 << INT0);
  //enable global interrupt
  sei();
}

 void atmega8_16mhz_ac_phase_controlClass::uninit()
{
  //set default port registers
  DDRD = 0x00;
  PORTD = 0x00;
  DDRB = 0x00;
  PORTB = 0x00;
    //clear timer control register
  TIMSK = 0x00;  
  TCCR1A = 0x00;  
  TCCR1B = 0x00; 
  TCNT1 = 0x00;
    //clear interrupt register
  GICR = 0x00;
  MCUCR = 0x00;
  //clear global interrupt
  cli();
}
 void atmega8_16mhz_ac_phase_controlClass::set_ac_power(int value)
 {
	OCR1A = value;//speed[0 to 10] or 1 to 623 	 
 }

 ISR(INT0_vect){ //zero cross detect   
  PORTB &= ~(1 << PB1);//turn off triac gate
  TCNT1 = 0x00;   //reset timer count back to zero
  TCCR1B=0x04; //start timer with divide by 256 input
}

ISR(TIMER1_COMPA_vect){ //comparator match
  PORTB |= (1 << PB1);//set triac gate to high
}

atmega8_16mhz_ac_phase_controlClass atmega8_16mhz_ac_phase_control;
