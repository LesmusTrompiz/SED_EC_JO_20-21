#ifndef _TIMER05

#define _TIMER05


#include <LPC17xx.H>
#include "state.h"
#include "UTS.h"
#include "servo.h"

#define   Fpclk 25e6	// Fcpu/4 (defecto despu?s del reset)

extern    struct sonar_status sonar;
void      config_timer05(void);

#endif 