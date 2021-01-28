/*                 _____ ____  _   _____    ____     ______  __
                  / ___// __ \/ | / /   |  / __ \   / __ ) \/ /
                  \__ \/ / / /  |/ / /| | / /_/ /  / __  |\  / 
                 ___/ / /_/ / /|  / ___ |/ _, _/  / /_/ / / /  
                /____/\____/_/ |_/_/  |_/_/ |_|  /_____/ /_/   
                                                               
                _______    _____       _________    __ _    ______ 
               / ____/ |  / /   |     / ____/   |  / /| |  / / __ \
              / __/  | | / / /| |    / /   / /| | / / | | / / / / /
             / /___  | |/ / ___ |   / /___/ ___ |/ /__| |/ / /_/ / 
            /_____/  |___/_/  |_|   \____/_/  |_/_____/___/\____/  
                                                                   
                                 ___    _   ______ 
                                /   |  / | / / __ \
                               / /| | /  |/ / / / /
                              / ___ |/ /|  / /_/ / 
                             /_/  |_/_/ |_/_____/  
                                                   
              _____ _    ________________     ____  ____  _______________
             / /   | |  / /  _/ ____/ __ \   / __ \/ __ \/_  __/  _/__  /
        __  / / /| | | / // // __/ / /_/ /  / / / / /_/ / / /  / /   / / 
       / /_/ / ___ | |/ // // /___/ _, _/  / /_/ / _, _/ / / _/ /   / /__
       \____/_/  |_|___/___/_____/_/ |_|   \____/_/ |_| /_/ /___/  /____/

*/


// Biblioteca de la LPC17.xx
#include <LPC17xx.H>

// Bibliotecas propias:
#include "modulos/servo.h"
#include "modulos/UTS.h"
#include "modulos/dac.h"
#include "modulos/uart.h"
#include "modulos/state.h"

// Bibliotecas externas:
#include "modulos/GLCD/GLCD.h"
#include <stdio.h>

// Estructura globales:
struct sonar_status sonar;
uint16_t muestras[N_MUESTRAS];  									    // Array que contiene la señal del DAC 


void config_botones(void)
{
	/*
		Configura los botones
		para que interrumpan si
		son impresionados.
	
		Estos botones se encargan
		de mover el servo en modo 
		manual, configurar el modo
		en el inicio, activar
		la medición automática en 
		modo manual y activar el 
	  sonar.f_block_move en modo 
		automático. 
	*/
	
  LPC_PINCON->PINSEL4|=(1<<20)|(1<<22)|(1<<24);		    // Configuro el modo de funcionamiento de los pines.
  NVIC_EnableIRQ(EINT0_IRQn);											    // Hablitamos interrupción ISP
  NVIC_EnableIRQ(EINT1_IRQn);											    // Hablitamos interrupción KEY1
  NVIC_EnableIRQ(EINT2_IRQn);										      // Hablitamos interrupción KEY2
}

void config_prioridades(void)
{
	/*
		Configura de todas las 
		prioridades a excepción
		de la UART.
	
		Se considera el orden 
		de prioridad
	
	
	*/
  NVIC_SetPriority(TIMER3_IRQn,0);								// UTS
  NVIC_SetPriority(TIMER0_IRQn,1);								// 0.5 Timer	
  NVIC_SetPriority(EINT0_IRQn,2);	    						// KEY ISP
  NVIC_SetPriority(EINT1_IRQn,3);									// KEY 1
  NVIC_SetPriority(EINT2_IRQn,4);									// KEY 2
	NVIC_SetPriority(TIMER1_IRQn,5);				   			// DAC
}

void EINT0_IRQHandler()
{
  LPC_SC->EXTINT |= (1<<0);												// Borramos flag interrupción
  if(!sonar.f_block_keys)																																			
  {																								// Este flag es para evirtar 
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
  if(
		(sonar.state == ST_MANUAL) 
			&& 
		 (!((sonar.servo_pose - 10) < 0)) 
			&& 
	   !(sonar.f_block_keys)
	)
	{	
    set_servo(sonar.servo_pose -= 10);
    sonar.f_block_keys = 1;
  }
}

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


int main(void)
{
  char msg [50];
  
  sonar.state            = ST_SETUP;              // El sonar empieza en modo configurable
  sonar.distance         = 0;							        
  sonar.servo_pose       = 0; 
  sonar.servo_period     = 1;
  sonar.servo_resolution = 10;
  sonar.f_block_keys     = 0;							
  sonar.f_block_move     = 0;							
  sonar.f_block_measure  = 0;	
	
	// Configuraciones:
  config_timer05();
  config_botones();
  config_servo();
  config_UTS();
	config_DAC();
	config_timer_dac();
	
	generar_muestras(); 
  config_prioridades();
  
	LCD_Init();
  LCD_Clear(Blue);
  set_servo(0);
  
  sprintf(msg, "Distancia medida =  %3.2f",sonar.distance);
  GUI_Text(20,40,(uint8_t *)msg,White,Black);
  GUI_Text(10,20,(uint8_t *)"Hola",Cyan,Black);
  
  while(1)
	{
    sonar.f_block_keys = 0;
		sprintf(msg, "Distancia medida =  %3.2f",sonar.distance);
		//tx_cadena_UART0(msg);
		GUI_Text(20,40,(uint8_t *)msg,White,Black);
	}
  
}	
