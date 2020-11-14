																																																																																																																																																			#include <LPC17xx.H>
#include "ultrasonidos.h"

void config_UTS(void)
{
	LPC_SC->PCONP|=(1<<23);  //Alimento Timer3 
	LPC_TIM3->PR=0;	  //Ponemos el prescaler a 0 
	LPC_TIM3->MR0=(Fpclk*TH_UTS-1);
	LPC_TIM3->MCR=7; //El match interrumpe, deja de contar y se reinicia a 0 el Timer Counter	
	LPC_SC->PCONP       |= (1<<23); 		// Alimentamos el TIMER3
	LPC_PINCON->PINSEL1 |= (3<<14);			// Selecciono CAP3.0	P0.23
	LPC_PINCON->PINSEL1 |= (3<<16);			// Selecciono CAP3.1 P0.24
	NVIC_EnableIRQ(TIMER3_IRQn);	
}

void UTS_trigger(void)
{
	LPC_GPIO1->FIODIR|=1<<25;
  LPC_TIM3->TCR=1<<0;             
  LPC_GPIO1->FIOSET|=1<<25;       // Pongo el pin a 1   
	
}

void TIMER3_IRQHandler(void)
{
	static uint32_t t = 0;
	char msg [20];
	if(((LPC_TIM3->IR>>0)&1))   //El MR0 interrumpe y reconfiguramos para que CAP2.0 interrumpa a flanco de subida
	{
		LPC_TIM3->IR=1<<0;
		LPC_GPIO1->FIOCLR|=1<<25; 		// Bajo el pin del trigger al pasar los 10us
		t = 0;
		LPC_TIM3->MCR=0;
		LPC_TIM3->CCR=(1<<2)|(1<<0);
		LPC_TIM3->TCR=1<<0;
	}
	else if(((LPC_TIM3->IR>>4)&1)==1)  //El CAP3.0 interrumpe y guarda el valor en CR0 automaticamente pasandlo a temp y reconfiguramos para que interrumpa a flanco de bajada
	{
		LPC_TIM3->IR=1<<4;
		
		if((LPC_TIM3->CCR>>0)&1)
		{
			t = LPC_TIM3->CR0;
			LPC_TIM3->CCR=(1<<2)|(1<<1);
		}
	
		else //El CAP2.0 interrumpe y guarda el valor en CR0 automaticamente y reconfiguramos para que sea MR0 de nuevo
		{	
			distancia = (LPC_TIM3->CR0-t)*(1/Fpclk)*0.5*340*100;  //Cálculo de distancia medida en cm sacando el numero de los CR y aplicando la frecuencia, el tiempo de vuelo y la velocidad del sonid (resumida en el factor 0.01715) 
			LPC_TIM3->CCR = 0; //Desactivo el modo captura para poder pasarlo a modo match
			LPC_TIM3->MR0 = (Fpclk*TH_UTS-1); //Tiempo en alto del sónar (10us)
			LPC_TIM3->TCR = 2; // Lo deshabilito y reseteo hasta que trigger me diga
			LPC_TIM3->MCR = 7; //El match interrumpe, deja de contar y se reinicia a 0 el Timer Counter
			sprintf(msg, "Distancia medida =  %3.2f",distancia);
			GUI_Text(20,40,(uint8_t *)msg,White,Black);
		}	
	}
}