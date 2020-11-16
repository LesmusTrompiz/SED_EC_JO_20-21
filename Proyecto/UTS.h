#ifndef _ULTRASONIDOS

#define _ULTRASONIDOS
#define Fpclk 25e6	     // Fcpu/4 (defecto despues del reset)
#define TH_UTS 10e-6     // T

#include "GLCD/GLCD.h"
#include "stdio.h"

void config_UTS(void);
void UTS_trigger(void);

extern float distancia;


#endif 