#ifndef _ULTRASONIDOS

#define _ULTRASONIDOS
#define Fpclk 25e6	     // Fcpu/4 (defecto despu�s del reset)
#define TH_UTS 10e-6     // T
#define F_UTS  40        // 40 Hz

#include "GLCD/GLCD.h"
#include "stdio.h"

void config_UTS(void);
void UTS_trigger(void);

extern float distancia;


#endif 