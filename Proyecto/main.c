#include <LPC17xx.H>
#define Fpclk 25e6	// Fcpu/4 (defecto después del reset)
#define Tpwm 15e-3	// Perido de la señal PWM (15ms)

#define ST_SETUP      0
#define ST_MANUAL     1
#define ST_AUTOMATICO 2

#define POSITIVO      0
#define NEGATIVO      1


// La lista de la verguenza 
int  posicion = 0;
char flag_bloqueante = 0;
char estado = ST_SETUP; // Variable de estado

	void config_servo(void)
	{
		LPC_PINCON->PINSEL3|=(2<<8); // P1.20 salida PWM (PWM1.2)
		LPC_SC->PCONP|=(1<<6);
		LPC_PWM1->MR0=Fpclk*Tpwm-1;
		LPC_PWM1->PCR|=(1<<10); //configurado el ENA2 (1.2)
		LPC_PWM1->MCR|=(1<<1);
		LPC_PWM1->TCR|=(1<<0)|(1<<3);
		LPC_PWM1->LER|= (1<<0);
	}
	
	void set_servo(float grados)
	{
			LPC_PWM1->MR2=(Fpclk * 0.4e-3 + Fpclk *2e-3* grados/180); // TH
			LPC_PWM1->LER|=(1<<2);
	}

	void config_botones(void)
	{
		LPC_PINCON->PINSEL4|=(1<<20)|(1<<22)|(1<<24);
		NVIC_EnableIRQ(EINT0_IRQn);
    NVIC_EnableIRQ(EINT1_IRQn);
    NVIC_EnableIRQ(EINT2_IRQn);
	}
	
	void config_prioridades(void)
	{
		NVIC_SetPriority(EINT0_IRQn,8);
    NVIC_SetPriority(EINT1_IRQn,8);
    NVIC_SetPriority(EINT2_IRQn,8);
		NVIC_SetPriority(TIMER0_IRQn,4);		
	}
	
	void EINT1_IRQHandler()
	{
		LPC_SC->EXTINT |= (1<<1);		// Borramos flag interrupción
		
		switch(estado)
		{
			case(ST_SETUP):
				estado = ST_AUTOMATICO;
	
			case(ST_MANUAL):
				if((!((posicion + 10) > 180)) && !(flag_bloqueante))
				{	
					set_servo(posicion += 10);
					flag_bloqueante = 1;	
				}
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
			Configuro un timer para que salte cada 0.5 S para que interrumpa
			y salte el flag.
    */
    LPC_SC->PCONP |=(1<<1);  						// Alimento Timer 0 
		LPC_TIM0->PR   = 0;	  							// Ponemos el prescaler a 0 
		LPC_TIM0->MR0  = (Fpclk*0.5-1);
		LPC_TIM0->MCR  = 3; 								// El match interrumpe, deja de contar y se reinicia a 0 el Timer Counter	
		LPC_TIM0->TCR |= (1<<0);				  	// Habilitamos timer 0
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
			
			case(ST_AUTOMATICO):	
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
	}


	int main(void)
	{
		char init = 1;
		// Configuraciones:
		config_timer05();
		config_botones();
		config_servo();
		config_prioridades();
		set_servo(0);
		
		
		while(1)
		{
			switch (estado)
			{
				case(ST_MANUAL):			
					// Hacemos cosas manuales
					init = 0;
					flag_bloqueante = 0;
				
				case(ST_AUTOMATICO):
					// Hacemos cosas automaticas
					init = 0;
					
			}
		}
	}	