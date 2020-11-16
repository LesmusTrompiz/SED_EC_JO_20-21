
#include "UTS.H"
#include <LPC17xx.H>


// Timer 1 en modo captura (CAP0.0)
void config_UTS(void)
{
  /*
      Config UTS: 
  */

  // Configuración básica
  LPC_SC->PCONP |=(1<<23);              	        // Alimento Timer 3
  LPC_PINCON->PINSEL1|=(3<<14);   	   		        // P1.20 como capture 
  LPC_PINCON->PINSEL0|=(3<<20);
  LPC_TIM3-> PR = 0;                              // 25MHz
  

  // Que hacemos cuando llegamos al match:
  LPC_TIM3->EMR|=(1<<0)|(3<<4);                   // EMR toggle cuando hay match
  LPC_TIM3->MR0 = Fpclk * TH_UTS/2 -1;            // Match a los 10us -> off
	LPC_TIM3->MCR |=3; 						                  // El match interrumpe y  se reinicia a 0 el Timer Counter
  // Configurar el capture:
  LPC_TIM3->CCR=(1<<2)|(1<<0);                    // Interrumpe en flanco de subida.
  NVIC_EnableIRQ(TIMER3_IRQn);                    // Habilitamos la interrupción
}

void UTS_trigger(void)
{
  //Cambiamos a modo match
  LPC_TIM3->MCR |=3; 						                  // El match interrumpe y  se reinicia a 0 el Timer Counter
	LPC_TIM3->TCR &=~(1<<1);                        // Quitamos el bit del reset
  LPC_TIM3-> TCR|=(1<<0);                         // Habilitamos el timer
} 

void TIMER3_IRQHandler(void)
{
  //
  static float inicio = 0;

  LPC_TIM3->IR=1<<4;
  if(((LPC_TIM3->IR>>0)&1))                       // Reconfiguramos para que no interrumpa por Match.
  {
    LPC_TIM3->IR = 1<<0;                          // Limpio el flag
    LPC_TIM3-> MCR &= ~(3<<0);                    // Cuando llegue al MR0 no interrumpe.
  }
  
  else if((LPC_TIM3->CCR >> 0) & 1)
  {
    inicio = LPC_TIM3->CR0;
		LPC_TIM3-> TCR|=(1<<0);                         // Habilitamos el timer
    LPC_TIM3->CCR=(1<<1)|(1<<2);            // Interrumpe en flanco de bajada.
  }

  else
  {
    distancia = (LPC_TIM3->CR0-inicio)*(1/Fpclk)*0.5*340*100;  //Cálculo de distancia medida en cm sacando el numero de los CR y aplicando la frecuencia, el tiempo de vuelo y la velocidad del sonid (resumida en el factor 0.01715) 
    LPC_TIM3->TCR &=~(1<<0);                  // Paramos de contar
    LPC_TIM3->TCR |=(1<<1);                   // Resteamos
    LPC_TIM3->CCR  =(1<<2)|(1<<0);            // Interrumpe en flanco de subida.
    inicio = 0;
  }

}


