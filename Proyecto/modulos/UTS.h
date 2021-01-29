#ifndef _ULTRASONIDOS

#define _ULTRASONIDOS
#define Fpclk 25e6	     // Fcpu/4 (defecto despues del reset)
#define TH_UTS 10e-6     // T
#define UMBRAL 100       // T
#define N_MUESTRAS 32    // T



#include "GLCD/GLCD.h"
#include "stdio.h"
#include "uart.h"
#include <LPC17xx.H>
#include <stdlib.h>
void config_UTS(void);
void UTS_trigger(void);

extern struct sonar_status sonar;


#endif 

