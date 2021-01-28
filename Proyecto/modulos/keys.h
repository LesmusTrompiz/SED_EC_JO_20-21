#ifndef _KEYS

#define _KEYS

// Defines para mejorar
// la legibilidad

#include <LPC17xx.H>
#include "state.h"
#include "uart.h"
#include "servo.h"

void config_keys(void);

extern struct sonar_status sonar;

#endif
