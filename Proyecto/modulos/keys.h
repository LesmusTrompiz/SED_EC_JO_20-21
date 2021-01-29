// Preprocessor Directives to include the library only once
#ifndef _KEYS                                   
#define _KEYS

// Necessary libraries:
#include <LPC17xx.H>
#include "state.h"
#include "uart.h"
#include "servo.h"

// Necessary global variable
extern struct sonar_status sonar;                     // Sonar state is modified by Eint_Handlers

// Avaible functions that can be called:
void config_keys(void);                               // Enables the buttons and their interruptions

#endif
