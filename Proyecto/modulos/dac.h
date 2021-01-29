
#ifndef _DAC

#define N_SAMPLES 32    // T
#define PI 3.14
#include <LPC17xx.H>
#include <math.h>
void config_DAC(void);
void config_timer_dac(void);
void generar_samples(void);

extern uint16_t muestras[N_SAMPLES];

#endif 
