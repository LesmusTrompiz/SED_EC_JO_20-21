#ifndef _TIMER05

#define _TIMER05


#include <LPC17xx.H>
#include "state.h"
#define Fpclk 25e6	// Fcpu/4 (defecto despu?s del reset)
#include "UTS.h"
#include "servo.h"
void config_timer05(void);



extern struct sonar_status sonar;


#endif 
