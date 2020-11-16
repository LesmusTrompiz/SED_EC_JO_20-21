#include <LPC17xx.H>
#include "servo.h"
#include "UTS.h"
#include "GLCD/GLCD.h"
#include "stdio.h"



#define ST_SETUP      0
#define ST_MANUAL     1
#define ST_AUTOMATICO 2

#define POSITIVO      0
#define NEGATIVO      1


// La lista de la verguenza 
int  posicion = 0;
char flag_bloqueante = 0;
char estado = ST_SETUP; 									// Variable de estado
char movimiento = 1;
char mediciones = 0;
float distancia = 0;

void config_botones(void)
{
  LPC_PINCON->PINSEL4|=(1<<20)|(1<<22)|(1<<24);
  NVIC_EnableIRQ(EINT0_IRQn);
  NVIC_EnableIRQ(EINT1_IRQn);
  NVIC_EnableIRQ(EINT2_IRQn);
}

void config_prioridades(void)
{
  NVIC_SetPriority(TIMER3_IRQn,0);					// UTS
  NVIC_SetPriority(TIMER0_IRQn,1);					// 0.5 Timer	
  NVIC_SetPriority(EINT0_IRQn,2);	    				// KEY ISP
  NVIC_SetPriority(EINT1_IRQn,3);						// KEY 1
  NVIC_SetPriority(EINT2_IRQn,4);						// KEY 2
}

void EINT0_IRQHandler()
{
  LPC_SC->EXTINT |= (1<<0);							// Borramos flag interrupción
  if(!flag_bloqueante)
  {
    switch(estado)
    {
      case(ST_AUTOMATICO):
        movimiento ^= 1;
        break;
      case(ST_MANUAL):
        mediciones ^= 1;			
        break;
    }
    flag_bloqueante = 1;
  }
}

void EINT1_IRQHandler()
{
  LPC_SC->EXTINT |= (1<<1);							// Borramos flag interrupción
  
  switch(estado)
  {
    case(ST_SETUP):
      estado = ST_AUTOMATICO;
      break;

    case(ST_MANUAL):
      if((!((posicion + 10) > 180)) && !(flag_bloqueante))
      {	
        set_servo(posicion += 10);
        flag_bloqueante = 1;	
      }
      break;
  }
}

void EINT2_IRQHandler()
{
  LPC_SC->EXTINT |= (1<<2);		// Borramos flag interrupción
  if((estado == ST_MANUAL) && (!((posicion - 10) < 0)) && !(flag_bloqueante))
  {	
    set_servo(posicion -= 10);
    flag_bloqueante = 1;
  }
}

void config_timer05()
{
  /*
    Configuro un timer para que salte cada 0.5 S 
    para que interrumpa y salte el flag.
  */
  LPC_SC->PCONP |=(1<<1);  							// Alimento Timer 0 
  LPC_TIM0->PR   = 0;	  								// Ponemos el prescaler a 0 
  LPC_TIM0->MR0  = (Fpclk*0.5-1);
  LPC_TIM0->MCR  = 3; 								// El match interrumpe, deja de contar y se reinicia a 0 el Timer Counter	
  LPC_TIM0->TCR |= (1<<0);				  			// Que empiece a contar timer 0
  NVIC_EnableIRQ(TIMER0_IRQn);				
}

void TIMER0_IRQHandler()
{
  static char sentido = POSITIVO;
  LPC_TIM0->IR=1<<0;
  
  switch(estado)
  {
    case(ST_SETUP):
      estado = ST_MANUAL;
      break;

    case(ST_AUTOMATICO):
      UTS_trigger();
      if(movimiento)
      {	
        if(sentido == POSITIVO)
        {
          if((posicion + 10) > 180)
            sentido = NEGATIVO;	
          
          else
            set_servo(posicion += 10);	
        }
        
        else
        {
          if((posicion - 10) < 0)
          {
            sentido = POSITIVO;
            set_servo(posicion += 10);
          }
          
          else
            set_servo(posicion -= 10);		
        }
      }
      break;
    case(ST_MANUAL):
      if(mediciones)
        UTS_trigger();
      break;
  }
}


int main(void)
{
  char msg [50];
  // Configuraciones:
  config_timer05();
  config_botones();
  config_servo();
  config_UTS();
  config_prioridades();
  LCD_Init();
  LCD_Clear(Blue);
  set_servo(0);
  
  sprintf(msg, "Distancia medida =  %3.2f",distancia);
  GUI_Text(20,40,(uint8_t *)msg,White,Black);
  GUI_Text(10,20,(uint8_t *)"Hola",Cyan,Black);
  
  while(1)
    flag_bloqueante = 0;
  
  
}	
