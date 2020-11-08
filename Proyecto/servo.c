#include "servo.h"

    LPC_PWM1->MCR|=(1<<1);
    LPC_PWM1->TCR|=(1<<0)|(1<<3);
    LPC_PWM1->LER|= (1<<0);
}

void set_servo(float grados)
{
    LPC_PWM1->MR2=(Fpclk * 0.4e-3 + Fpclk *2e-3* grados/180); // TH
    LPC_PWM1->LER|=(1<<2);
}
