// Preprocessor Directives to include the library only once
#ifndef _UTS
#define _UTS


// Necessary libraries:
#include <LPC17xx.H>
#include "state.h"

// New defines
#define TH_UTS 10e-6                                  // Period of the trigger signal.
#define THRESHOLD 100                                 // Measurement threshold.

// Necessary global variable:
extern struct sonar_status sonar;                     // Sonar state is modified by the UTS measure.

// Avaible functions that can be called:
void config_UTS(void);                                // Configure the Timer3 to control the UTS. 
void UTS_trigger(void);                               // Start with the measurement sequence by triggering the UTS.

#endif 

