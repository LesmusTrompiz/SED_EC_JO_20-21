#include "servo.h"


void config_servo(void)
{
    LPC_PINCON->PINSEL3|=(2<<8);        // P1.20 salida PWM (PWM1.2)
    LPC_SC->PCONP|=(1<<6);
    LPC_PWM1->MR0=Fpclk*Tpwm -1;
    LPC_PWM1->PCR|=(1<<10);             //configurado el ENA2 (1.2)
    LPC_PWM1->MCR|=(1<<1);
    LPC_PWM1->TCR|=(1<<0)|(1<<3);
    LPC_PWM1->LER|= (1<<0);
}

void set_servo(float grados)
{
    LPC_PWM1->MR2=(Fpclk * 0.4e-3 + Fpclk *2e-3* grados/180); // TH
    LPC_PWM1->LER|=(1<<2);
}
