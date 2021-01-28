#include "timer05.h"

void config_timer05()
{
  /*
    Configuro un timer para que salte cada 0.5 S 
    para que interrumpa y salte el flag.
  */
  LPC_SC->PCONP |=(1<<1);  							// Alimento Timer 0 
  LPC_TIM0->PR   = 0;	  								// Ponemos el prescaler a 0 
  LPC_TIM0->MR0  = (Fpclk*0.5-1);
  LPC_TIM0->MCR  = 3; 								  // El match interrumpe, deja de contar y se reinicia a 0 el Timer Counter	
  LPC_TIM0->TCR |= (1<<0);				  		// Que empiece a contar timer 0
  NVIC_EnableIRQ(TIMER0_IRQn);				
}


void TIMER0_IRQHandler()
{
  static char sentido = POSITIVO;
  static int  loop_iter = 0;

  LPC_TIM0->IR=1<<0;
  
  switch(sonar.state)
  {
    case(ST_SETUP):
      sonar.state = ST_MANUAL;
      break;

    case(ST_AUTOMATICO):
      UTS_trigger();
      loop_iter++;
      if(!sonar.f_block_move && (loop_iter == sonar.servo_period))
      {	
        if(sentido == POSITIVO)
        {
          if((sonar.servo_pose + sonar.servo_resolution) > 180)
            sentido = NEGATIVO;	
          
          else
            set_servo(sonar.servo_pose += sonar.servo_resolution);	
        }
        
        else
        {
          if((sonar.servo_pose - sonar.servo_resolution) < 0)
          {
            sentido = POSITIVO;
            set_servo(sonar.servo_pose += sonar.servo_resolution);	
          }
          else
            set_servo(sonar.servo_pose -= sonar.servo_resolution);			
        }
        loop_iter = 0;
      }
      break;
		
    case(ST_MANUAL):
      if(sonar.f_block_measure)
        UTS_trigger();
      break;
  }
}
