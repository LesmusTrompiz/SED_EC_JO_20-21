#include "keys.h"
 
void config_keys(void)
{
  /*
    config_keys :: void -> void

    Esta función habilita los botones 
    y sus interrupciones.
  */
  LPC_PINCON->PINSEL4|=(1<<20)|(1<<22)|(1<<24);		
  NVIC_EnableIRQ(EINT0_IRQn);
  NVIC_EnableIRQ(EINT1_IRQn);
  NVIC_EnableIRQ(EINT2_IRQn);
}



void EINT0_IRQHandler()
{
  /*
    EITN0_IRQHandler :: void -> void

    Esta función atiende la interrupción 
    que se genera cuando pulsa el botón 
    ISP.

    Si se pulsa este botón cuando estamos
    en modo automático se alterna el 
    estado del flag block move, este flag 
    impide que el servo se mueva cuando 
    se encuentra a nivel alto. 
    
    En caso de que el sonar este en modo
    manual alterna el flag block measure 
    evitando que se tomen medidas con el 
    ultrasonidos cuando el flag se 
    encuentra a nivel alto. 

    No hace nada cuando nos encontramos 
    en modo Setup.
  */
  LPC_SC->EXTINT |= (1<<0);												// Borramos flag interrupción
  if(!sonar.f_block_keys)
  {
    switch(sonar.state)
    {
      case(ST_AUTOMATICO):
        sonar.f_block_move ^= (1<<0);
        break;
      case(ST_MANUAL):
        sonar.f_block_measure ^= (1<<0);			
        break;
    }
    sonar.f_block_keys = 1;
  }
}

void EINT1_IRQHandler()
{
  /*
    EITN1_IRQHandler :: void -> void

    Esta función atiende la interrupción 
    que se genera cuando pulsa el botón 
    KEY1.

    Si se pulsa este botón cuando estamos
    en modo Setup cambiamos de modo a 
    automático e iniciamos la UART
    a 9600 baudios.

    En caso de que el sonar este en modo
    manual, desplaza el servo en sentido
    positivo 10 grados siempre que no 
    se superen los 180 grados.

    No hace nada cuando nos encontramos 
    en modo Automático.
  */
  LPC_SC->EXTINT |= (1<<1);												// Borramos flag interrupción
  
  switch(sonar.state)
  {
    case(ST_SETUP):
      sonar.state = ST_AUTOMATICO;
			uart0_init(UART_BAUDRATE);
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
    /*
    EITN2_IRQHandler :: void -> void

    Esta función atiende la interrupción 
    que se genera cuando pulsa el botón 
    KEY2.


    Si se pulsa este botón cuando estamos
    en modo manual el servo se desplaza
    en sentido negativo 10 grados siempre
    que no baje por debajo de los 10 grados.

    No hace nada cuando nos encontramos 
    en modo Automático ni en modo Setup.
  */
  LPC_SC->EXTINT |= (1<<2);												// Borramos flag interrupción
  if((sonar.state == ST_MANUAL) && (!((sonar.servo_pose - 10) < 0)) && !(sonar.f_block_keys))
  {	
    set_servo(sonar.servo_pose -= 10);
    sonar.f_block_keys = 1;
  }
}
