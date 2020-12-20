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

// Modulos propios:
#include "servo.h"
#include "UTS.h"
#include "stdio.h"
#include "dac.h"
#include "uart.h"

// Modulos extra
#include "GLCD/GLCD.h"

// Defines para mejorar
// la legibilidad
#define ST_SETUP      0			// Estado inicial		
#define ST_MANUAL     1			// Estado manual
#define ST_AUTOMATICO 2			// Estado automático
#define ST_TEST 			3			// Modo para probar nuevas funcionalidades
#define N_MUESTRAS    32    // Numero de muestras en la señal sinusoidal
#define POSITIVO      0			// Movimiento del servo en sentido horario.
#define NEGATIVO      1			// Movimiento del servo en sentido antihorario.


// Variables globales:
// tambien conocido como
// la lista de la verguenza 
int  posicion = 0;																// Posición actual del servo esta en grados.
char flag_bloqueante = 0;													// Flag para impedir que el servo se mueva.
char estado = ST_SETUP; 														// Variable que indica el estado.
char movimiento = 1;															// Flag que habilita/deshabilita el servo, se modifica con el ISP.
char mediciones = 0;															// Flag que habilita/deshabilita el UTS, se modifica con el ISP.
float distancia = 0;															// Distancia medida por el ultrasonidos en cm.
int periodo_servo = 1;
int resolucion_servo = 10;
uint16_t* ptr_relleno;
uint16_t muestras[N_MUESTRAS];  									// Array que contiene la señal del DAC 


void config_botones(void)
{
  LPC_PINCON->PINSEL4|=(1<<20)|(1<<22)|(1<<24);		//
  NVIC_EnableIRQ(EINT0_IRQn);
  NVIC_EnableIRQ(EINT1_IRQn);
  NVIC_EnableIRQ(EINT2_IRQn);
}

void config_prioridades(void)
{
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
  LPC_SC->EXTINT |= (1<<1);												// Borramos flag interrupción
  
  switch(estado)
  {
    case(ST_SETUP):
      estado = ST_AUTOMATICO;
			uart0_init(9600);
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
  LPC_SC->EXTINT |= (1<<2);												// Borramos flag interrupción
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
  LPC_TIM0->MCR  = 3; 								  // El match interrumpe, deja de contar y se reinicia a 0 el Timer Counter	
  LPC_TIM0->TCR |= (1<<0);				  		// Que empiece a contar timer 0
  NVIC_EnableIRQ(TIMER0_IRQn);				
}

void TIMER0_IRQHandler()
{
  static char sentido = POSITIVO;
  static int  ciclo = 0;

  LPC_TIM0->IR=1<<0;
  
  switch(estado)
  {
    case(ST_SETUP):
      estado = ST_MANUAL;
      break;

    case(ST_AUTOMATICO):
      UTS_trigger();
      ciclo++;
      if(movimiento && (ciclo == periodo_servo))
      {	
        if(sentido == POSITIVO)
        {
          if((posicion + resolucion_servo) > 180)
            sentido = NEGATIVO;	
          
          else
            set_servo(posicion += resolucion_servo);	
        }
        
        else
        {
          if((posicion - resolucion_servo) < 0)
          {
            sentido = POSITIVO;
            set_servo(posicion += resolucion_servo);
          }
          else
            set_servo(posicion -= resolucion_servo);		
        }
        ciclo = 0;
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
	config_DAC();
	config_timer_dac();
	
	generar_muestras(); 
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
