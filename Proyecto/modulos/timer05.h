// Preprocessor Directives to include the library only once:
#ifndef _TIMER05
#define _TIMER05

// Necessary libraries:
#include <LPC17xx.H>
#include "state.h"
#include "UTS.h"
#include "servo.h"

// Necessary global variable:
extern struct sonar_status sonar;                     // Sonar state is modified by timer 0.5 handler.

// Avaible functions that can be called:
void config_timer05(void);                            // Configure the Timer0 to interrupt every 0.5 seconds.	

#endif 
