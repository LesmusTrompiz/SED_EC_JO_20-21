#include "keys.h"
 
void config_keys(void)
{
  LPC_PINCON->PINSEL4|=(1<<20)|(1<<22)|(1<<24);		
  NVIC_EnableIRQ(EINT0_IRQn);
  NVIC_EnableIRQ(EINT1_IRQn);
  NVIC_EnableIRQ(EINT2_IRQn);
}



void EINT0_IRQHandler()
{
  LPC_SC->EXTINT |= (1<<0);												// Borramos flag interrupción
  if(!sonar.f_block_keys)
  {
    switch(sonar.state)
    {
      case(ST_AUTOMATICO):
        sonar.f_block_move ^= 1;
        break;
      case(ST_MANUAL):
        sonar.f_block_measure ^= 1;			
        break;
    }
    sonar.f_block_keys = 1;
  }
}

void EINT1_IRQHandler()
{
  LPC_SC->EXTINT |= (1<<1);												// Borramos flag interrupción
  
  switch(sonar.state)
  {
    case(ST_SETUP):
      sonar.state = ST_AUTOMATICO;
			uart0_init(9600);
      break;

    case(ST_MANUAL):
      if((!((sonar.servo_pose + 10) > 180)) && !(sonar.f_block_keys))
      {	
        set_servo(sonar.servo_pose += 10);
        sonar.f_block_keys = 1;	
      }
      break;
  }
}

void EINT2_IRQHandler()
{
  LPC_SC->EXTINT |= (1<<2);												// Borramos flag interrupción
  if((sonar.state == ST_MANUAL) && (!((sonar.servo_pose - 10) < 0)) && !(sonar.f_block_keys))
  {	
    set_servo(sonar.servo_pose -= 10);
    sonar.f_block_keys = 1;
  }
}
