// Preprocessor Directives to include the library only once
#ifndef _SERVO
#define _SERVO

// Necessary libraries:
#include <LPC17xx.H>
#include "state.h"

// New defines:
#define Tpwm 15e-3	                                  // Period of the PWM signal of the servo(15ms)

// Avaible functions that can be called:
void config_servo(void);                              // Enables the PWM, the output pin is P1.20.
void set_servo(float grados);                         // Moves the servo to the position passed in the argument.

#endif 

