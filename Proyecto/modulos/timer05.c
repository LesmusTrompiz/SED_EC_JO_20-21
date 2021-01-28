#include "timer05.h"

void config_timer05()
{
  /*
    Configuro un timer para que
  	salte cada 0.5 S.
	
		Este timer se utilizará
		para medir, mover el servo
		en modo automático y 
		configurar el modo tras
	  el reincio.
  */
	
  LPC_SC->PCONP |=(1<<1);  												// Alimento Timer 0 
  LPC_TIM0->PR   = 0;	  													// Ponemos el prescaler a 0 
  LPC_TIM0->MR0  = (Fpclk*0.5-1);									// Frecuencia de 0.5s
  LPC_TIM0->MCR  = 3; 								  					// Match -> interrpt,stop and reset	
  LPC_TIM0->TCR |= (1<<0);				  							// Start count
  NVIC_EnableIRQ(TIMER0_IRQn);										// Hablitamos interrupción.			
}

void TIMER0_IRQHandler()
{
  static char sentido = POSITIVO;
  static int  ciclo = 0;

  LPC_TIM0->IR=1<<0;
  
  switch(sonar.state)
  {
    case(ST_SETUP):
      sonar.state = ST_MANUAL;
      break;

    case(ST_AUTOMATICO):
      UTS_trigger();
      ciclo++;
      if(!sonar.f_block_move && (ciclo >= 1))
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
        ciclo = 0;
      }
      break;
		
    case(ST_MANUAL):
      if(sonar.f_block_measure)
        UTS_trigger();
      break;
  }
}