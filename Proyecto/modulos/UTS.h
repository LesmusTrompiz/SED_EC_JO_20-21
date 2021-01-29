// Preprocessor Directives to include the library only once
#ifndef _ULTRASONIDOS
#define _ULTRASONIDOS


// Necessary libraries:
#include <LPC17xx.H>
#include "state.h"

// New defines
#define Fpclk 25e6	     // Fcpu/4 (defecto despues del reset)
#define TH_UTS 10e-6     // T
#define UMBRAL 100       // T
#define N_samples 32    // T


// Necessary global variable:
extern struct sonar_status sonar;                     // Sonar state is modified by timer 0.5 handler.

// Avaible functions that can be called:
void config_UTS(void);                                // Configure the Timer3 to control the UTS. 
void UTS_trigger(void);                               // Start with the measurement sequence by triggering the UTS.




#endif 

