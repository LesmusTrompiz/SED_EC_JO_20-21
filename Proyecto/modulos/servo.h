
#include <LPC17xx.H>

//////////////////////////////////
//////// Biblioteca servo ////////
//////////////////////////////////


#define Fpclk 25e6	// Fcpu/4 (defecto después del reset)
#define Tpwm 15e-3	// Perido de la señal PWM (15ms)

void config_servo(void);
void set_servo(float grados);
