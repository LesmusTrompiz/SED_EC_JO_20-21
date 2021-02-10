// Preprocessor Directives to include the library only once
#ifndef _DAC
#define _DAC

// Necessary libraries:
#include <LPC17xx.H>
#include <math.h>
#include "state.h"

// Necessary global variable
extern uint16_t samples[N_SAMPLES];                  

// New defines:
#define PI 3.14                                       // Pi Number

// Avaible functions that can be called:
void config_DAC(void);                                // Enables the DAC, the output pin is P0.26.
void config_timer_dac(void);                          // Configure the Timer1 to change the sample of the DAC.
void generate_samples(void);                          // Generate the samples of the sinusoidal signal. 

#endif 
