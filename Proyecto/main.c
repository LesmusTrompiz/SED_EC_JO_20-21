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

void config_prioridades(void)
{
	/*
		config_prioridades :: void -> void
    
    Esta función configura las prioridades
    de todas las interrupciones que se usan
    en el proyecto, excepto la de la UART 
    que se configura en su propia 
    configuración. 
	
	*/
  NVIC_SetPriority(TIMER3_IRQn,0);								    // UTS
  NVIC_SetPriority(TIMER0_IRQn,1);								    // 0.5 Timer	
  NVIC_SetPriority(EINT0_IRQn, 2);	    						  // KEY ISP
  NVIC_SetPriority(EINT1_IRQn, 3);									  // KEY 1
  NVIC_SetPriority(EINT2_IRQn, 4);									  // KEY 2
	NVIC_SetPriority(TIMER1_IRQn,5);				   			    // DAC
}

int main(void)
{
  char msg [50];
  
  sonar.state            = ST_SETUP;                  // El sonar empieza en modo configurable
  sonar.distance         = 0;							        
  sonar.servo_pose       = 0; 
  sonar.servo_period     = 1;
  sonar.servo_resolution = 10;
  sonar.f_block_keys     = 0;							
  sonar.f_block_move     = 0;							
  sonar.f_block_measure  = 0;	
	
	// Configuraciones:
  config_timer05();
  config_keys();
  config_servo();
  config_UTS();
	config_DAC();
	config_timer_dac();
  config_prioridades();
  
  // Iniciamos el Hardware 
  // y la señal del DAC
  generar_muestras(); 
	LCD_Init();
  LCD_Clear(Blue);
  set_servo(0);
  
  GUI_Text(10,20,(uint8_t *)"Iniciando",Cyan,Black);
  
  while(1)
	{
    sonar.f_block_keys = 0;
		sprintf(msg, "Distancia medida =  %3.2f",sonar.distance);
		//tx_cadena_UART0(msg);
		GUI_Text(20,40,(uint8_t *)msg,White,Black);
	}
  
}	
