#ifndef ATMEGA8_16MHZ_AC_PHASE_CONTROL_H
#define ATMEGA8_16MHZ_AC_PHASE_CONTROL_H

#include <Arduino.h>

class atmega8_16mhz_ac_phase_controlClass
{
  public:
    void init();
    void uninit();
    void set_ac_power(int value);
};

extern atmega8_16mhz_ac_phase_controlClass atmega8_16mhz_ac_phase_control;

#endif
