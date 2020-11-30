#include <LPC17xx.H>
#include "dac.h"

void config_DAC(void)										
{
	/*
		Configuración del DAC
	*/
	LPC_PINCON->PINSEL1  |= (2<<20);		// AOUT en el pin P0.26
	LPC_PINCON->PINMODE1 |= (2<<20);		// Pull-up-pull-down deshabilitado
	LPC_DAC->DACCTRL      = 0;				  // Deshabilita el modo DMA
}


void config_timer_dac(void)
{
  /*
    Configuro un timer para que salte cada 0.5 S 
    para que interrumpa y salte el flag.
  */
  LPC_SC->PCONP |=(1<<1);  					  // Alimento Timer 0 
  LPC_TIM1->PR   = 0;	  					    // Ponemos el prescaler a 0 
  LPC_TIM1->MCR  = 3; 						    // El match interrumpe, deja de contar y se reinicia a 0 el Timer Counter	
  NVIC_EnableIRQ(TIMER1_IRQn);				
}

void genera_muestras(void)	// Genera el seno
{
  int i;
  for(i=0; i < N_MUESTRAS; i++)
    muestras[i]=(uint16_t)1023 * (0.5 + 0.5 * sin(2*PI*i/N_MUESTRAS));		//El DAC es de 10 bits
} 

void TIMER1_IRQHandler(void)
{
  static char indice_muestra;
  LPC_TIM1->IR|=(1<<0);														// Borro el flag de la interrupcion
  LPC_DAC->DACR=muestras[indice_muestra++]<<6;		// 
  if(indice_muestra == N_MUESTRAS -1 )
    indice_muestra = 0;
}

