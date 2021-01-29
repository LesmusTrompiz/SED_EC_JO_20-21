// Link this source code with his .h file.
#include "servo.h"


void config_servo(void)
{
  /*
    config_servo :: void -> void

    Enables the PWM, the output
    pin is P1.20.

  */

  LPC_PINCON->PINSEL3|=(2<<8);                        // Configure the pin P1.20 function as a PWM.
  LPC_SC->PCONP|=(1<<6);                              // Configure power supply.
  LPC_PWM1->MR0=Fpclk*Tpwm -1;                        // The MR0 is set to the equivalent number of TC ticks of PWM's period. 
  LPC_PWM1->PCR|=(1<<10);                             // The PWM is single mode and output is enabled. 
  LPC_PWM1->MCR|=(1<<1);                              // When the time counter reachs MR0 the time counter is reset.
  LPC_PWM1->TCR|=(1<<0)|(1<<3);                       // Reset the time counter and start to count.
  LPC_PWM1->LER|= (1<<0);                             // Enables the last changes to the MR0.
}

void set_servo(float degrees)
{
  /*
    set_servo :: float -> void

    Moves the servo to the position
    passed in the argument.
  */
  if(degrees >= 0 && degrees <= 180)                  // If the angle doesn´t exceed the bounds.
  {
    LPC_PWM1->MR2 = (Fpclk * 0.4e-3                   // The MR2 is set to the equivalent number of TC ticks of the period
    + 
    Fpclk *2e-3* degrees/180);                        // that makes the servo move to the position passed in the argument

    LPC_PWM1->LER|=(1<<2);                            // Enables the last changes to the MR2
  }
}
