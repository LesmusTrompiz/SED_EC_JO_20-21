
#ifndef _DAC

#define N_MUESTRAS 32    // T
#define PI 3.14
#include <LPC17xx.H>

void config_DAC(void);
void config_timer_dac(void);
void generar_muestras(void);

extern uint16_t muestras[N_MUESTRAS];

#endif 
