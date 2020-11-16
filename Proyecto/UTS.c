
#include "UTS.H"
#include <LPC17xx.H>


// Timer 1 en modo captura (CAP0.0)
void config_UTS(void)
{
  /*
    Configuro el Timer3 . 
  */

  // Configuración básica:
  LPC_SC->PCONP |=(1<<23);              	        // Alimento Timer 3.
  LPC_PINCON->PINSEL1|=(3<<14);   	   		        // P1.20 como capture. 
  LPC_PINCON->PINSEL0|=(3<<20);                   // P0.10 como MAT.
  LPC_TIM3-> PR = 0;                              // 25MHz.
  
  // Que hacemos cuando llegamos al match:
  LPC_TIM3->MR0 = Fpclk * TH_UTS/2 -1;            // Match a los 5us -> on/off.
  LPC_TIM3->EMR|=(1<<0)|(3<<4);                   // Cuando llega al match el pin hace toggle.
	LPC_TIM3->MCR |=3; 						                  // Cuando llega al match interrumpe y se reinicia a 0 el Timer Counter
  
  // Configurar el capture:
  LPC_TIM3->CCR=(1<<2)|(1<<0);                    // Interrumpe si hay flanco de subida en el capture.
  
  // Habilito la interrupción:
  NVIC_EnableIRQ(TIMER3_IRQn);                    
}

void UTS_trigger(void)
{
  /*
    Activo el trigger para empezar
    con la secuencia de medida.
  */
  LPC_TIM3->MCR |=3; 						                  // El match interrumpe y  se reinicia a 0 el Timer Counter
	LPC_TIM3->TCR &=~(1<<1);                        // Quitamos el bit del reset
  LPC_TIM3-> TCR|=(1<<0);                         // Habilitamos el timer
} 

void TIMER3_IRQHandler(void)
{
  /*
    Handler del Timer 3 se encarga
    de generar de manera correcta
    el Trigger y medir el Echo del
    UTS.

    Además pone la medida del UTS 
    en pantalla. 
  */

  static float inicio = 0;                        // Variable auxiliar para calcular el ancho del Echo.
  char msg [10];
  // Interrupción por Match:
  if(((LPC_TIM3->IR>>0)&1))                      
  {
    LPC_TIM3->IR = 1<<0;                          // Limpio el flag
    LPC_TIM3-> MCR &= ~(3<<0);                    // Cuando llegue al MR0 no interrumpe.
  }
  
  // Interrupción por flanco de subida.
  else if((LPC_TIM3->CCR >> 0) & 1)               
  {
    LPC_TIM3->IR=1<<4;
    inicio = LPC_TIM3->CR0;                       // Almaceno el valor del CR en la varaible auxiliar.
    LPC_TIM3->CCR=(1<<1)|(1<<2);                  // Interrumpe en flanco de bajada.
  }

  // Interrupción por flanco de bajada.
  else
  {                                            
    LPC_TIM3->IR=1<<4;
    distancia = ((LPC_TIM3->CR0-inicio)           //Cálculo la distancia medida en cm.
        * (1/Fpclk)*0.5*340*100);                 
    LPC_TIM3->TCR &=~(1<<0);                      // Paramos de contar
    LPC_TIM3->TCR |=(1<<1);                       // Reseteamos
    LPC_TIM3->CCR  =(1<<2)|(1<<0);                // Interrumpe en flanco de subida.
    inicio = 0;
    
    // Pongo la medida en pantalla
    sprintf(msg, "Distancia medida =  %3.2f",distancia);
		GUI_Text(20,40,(uint8_t *)msg,White,Black);
  }
}


