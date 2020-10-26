#include <LPC17xx.H>
#define Fpclk 25e6	// Fcpu/4 (defecto después del reset)
#define Tpwm 15e-3	// Perido de la señal PWM (15ms)

#define ST_MANUAL     0
#define ST_AUTOMATICO 1
#define POSITIVO      0
#define NEGATIVO      1
#define PRESIONAR			0
#define DESPRESIONAR	1


char estado = ST_MANUAL; // Variable de estado
int posicion = 0;
char flag = 0;
char bloqueante =0;

	void config_servo(void)
	{
		LPC_PINCON->PINSEL3|=(2<<8); // P1.20 salida PWM (PWM1.2)
		LPC_SC->PCONP|=(1<<6);
		LPC_PWM1->MR0=Fpclk*Tpwm-1;
		LPC_PWM1->PCR|=(1<<10); //configurado el ENA2 (1.2)
		LPC_PWM1->MCR|=(1<<1);
		LPC_PWM1->TCR|=(1<<0)|(1<<3);
	}
	
	void set_servo(uint8_t grados)		// Mueve el servo
	{
		
			LPC_PWM1->MR2	 = (Fpclk*0.4e-3 + Fpclk*2e-3*grados/180); // Tiempo en alto
			LPC_PWM1->LER	|= (1<<2)|(1<<0);														// Load Register
	}

	void config_botones(void)
	{
		LPC_PINCON->PINSEL4|=(1<<20)|(1<<22)|(1<<24);
		//NVIC_EnableIRQ(EINT0_IRQn);
    NVIC_EnableIRQ(EINT1_IRQn);
    NVIC_EnableIRQ(EINT2_IRQn);
	}
	
	void EINT1_IRQHandler()
	{
		int t;
		
		if(estado == ST_MANUAL)
		{
			LPC_TIM0->TCR  |= (1<<0);		// Habilitamos timer 0
			LPC_SC->EXTINT |= (1<<1);		// Borramos flag interrupción
			
			
			if((posicion < 180)&(!bloqueante))
			{
				set_servo(posicion += 10);
				bloqueante = 1;
			}
			
		}
	}
		
	void EINT2_IRQHandler()
	{
		LPC_SC->EXTINT |= (1<<2);		// Borramos flag interrupción
		if(estado == ST_MANUAL)
		{	
			if((posicion > 0)&(!bloqueante))
			{
				set_servo(posicion -= 10);
				bloqueante = 1;
			}
		}
		
	}
	
	void config_timer_05()
	{
    /*
			Configuro un timer para que salte cada 0.5 S para que interrumpa
			y salte el flag.
    */
		
    LPC_SC->PCONP|= (1<<1);  					// Alimento Timer 0 
		LPC_TIM0->PR  = 0;	  						// Ponemos el prescaler a 0 
		LPC_TIM0->MCR|= (3<<0); 					// El match interrumpe y se reinicia a 0 el Timer Counter	
		LPC_TIM0->MR0 = (Fpclk*0.5 -1);
		NVIC_EnableIRQ(TIMER0_IRQn);	
	}
	
	void TIMER0_IRQHandler()
	{
		static char sentido = POSITIVO;
		
		LPC_TIM0->IR=(1<<0);
		
		if(estado == ST_AUTOMATICO)
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
		else
			estado = ST_AUTOMATICO;
	}
	
	void config_prioridades(void)
	{
		NVIC_SetPriorityGrouping(0);
		//NVIC_SetPriority(EINT0_IRQn,8);
    NVIC_SetPriority(EINT1_IRQn,8);
    NVIC_SetPriority(EINT2_IRQn,8);
		NVIC_SetPriority(TIMER0_IRQn,4);		
	}
	
	void rutina_inicio(void)
	{
		int i,t;
		int grados1 = 0;
		for(i=0;i<18;i++)
		{
			for(t=0;t<10000000;t++); // variar el límite en simulación para ver más rápido la variación de la señal PWM									
			set_servo(grados1+=10);	 // incrementamos la posición del servo de 10 en 10º	
		}
	}
	

	int main(void)
	{
		
		int init = 1;
		// Configuraciones:
		config_servo();
		
		rutina_inicio();
		config_botones();
		
		config_prioridades();
		set_servo(0);
		config_timer_05();
		
		while(1)
		{
			if(estado == ST_MANUAL)
			{
				init = 1;
				LPC_TIM0->TCR &=~ (1<<0);		// Habilitamos timer 0
			}
			else
				if(init)
				{
					init = 0;
					LPC_TIM0->TCR   |= (1<<0);				// Habilitamos timer 0
				}
				// Hacemos cosas automaticas
			bloqueante =0;
		
		}
		
	}
		