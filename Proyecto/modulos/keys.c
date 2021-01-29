// Link this source code with his .h file.
#include "keys.h"

void config_keys(void)
{
  /*
    config_keys :: void -> void

    Enables the buttons 
    and their interruptions.
  */
	
  LPC_PINCON->PINSEL4|=(1<<20)|(1<<22)|(1<<24);		    // Configuration of pin functionality.
  NVIC_EnableIRQ(EINT0_IRQn);											    // Enable the interruption of ISP.
  NVIC_EnableIRQ(EINT1_IRQn);											    // Enable the interruption of KEY1.
  NVIC_EnableIRQ(EINT2_IRQn);										      // Enable the interruption of KEY2.
}


void EINT0_IRQHandler()
{
  /*
  EINT0_IRQHandler :: void -> void

  Handles the interruption that is 
  generated when the ISP button
  is pressed.

  If the sonar is in automatic
  mode the flag f_block_move toggles,
  When this flag is high the servo 
  stops moving.

  Instead, if the sonar is in manual
  mode the flag f_block_measure is 
  the flag that toggles. When this
  flag is rised the UTS stops 
  measuring. 

  If the sonar is in another mode
  this function does not do 
  anything.
  */

  LPC_SC->EXTINT |= (1<<0);												    // Clear the interruption flag.
  if(!sonar.f_block_keys)															// Is the firt time that interrupts this cycle?																				
  {																								    
    switch(sonar.state)                               
    {
      case(ST_AUTOMATICO):                            // If we are in aumatic mode:
        sonar.f_block_move ^= 1;                      //  Toggle the f_block_move flag.
        break;
      case(ST_MANUAL):                                // If we are in manual mode:
        sonar.f_block_measure ^= 1;			              //  Toggle the f_block_measure flag.
        break;
    }
    sonar.f_block_keys = 1;                           // Raise the flag to indicate that we have alredy 
  }                                                   // interrupted this cycle.
}

void EINT1_IRQHandler()
{
   /*
    EINT1_IRQHandler :: void -> void

    Handles the interruption that is 
    generated when the KEY1 button
    is pressed.

    If the sonar is in Setup mode
    the mode is changed to automatic,
    and the UART is configured with
    a baudrate of 9600 bauds.

    Instead, if the sonar is in manual
    mode the servo moves 10 degrees
    in positive direction as long as
    it not exceeds the maximun angle,
    in our case 180 degrees.

    If the sonar is in another mode
    this function does not do 
    anything.
  */

  LPC_SC->EXTINT |= (1<<1);												    // Clear the interruption flag.
  switch(sonar.state)
  {
    case(ST_SETUP):                                   // If we are in Setup mode:
      sonar.state = ST_AUTOMATICO;                    //  Change the mode to autatic mode.
			uart0_init(UART_BAUDRATE);                      //  Configure the UART protocol.
      break;

    case(ST_MANUAL):                                  // If we are in Manual mode:
      if( 
        (!((sonar.servo_pose + 10) > 180))            //  If the sonar does not exceed the maximun angle   
        &&                                            //  in the next move AND is the first time that
        !(sonar.f_block_keys))                        //  interrupts this cycle?	
      {	
        set_servo(sonar.servo_pose += 10);            //    Increase the servo pose by 10 degrees.
        sonar.f_block_keys = 1;	                      //    Raise the flag to indicate that we have alredy
      }                                               //    interrupted this cycle.
      break;
  }
}

void EINT2_IRQHandler()
{
  /*
    EINT2_IRQHandler :: void -> void

    Handles the interruption that is 
    generated when the KEY1 button
    is pressed.

    If the sonar is in manual
    mode the servo moves 10 degrees
    in negative direction as long as
    it not exceeds the minimun angle,
    in our case 0 degrees.

    If the sonar is in another mode
    this function does not do 
    anything.
  */

  LPC_SC->EXTINT |= (1<<2);												    // Clear the interruption flag.
  if(
		(sonar.state == ST_MANUAL)                        // If we are in Manual mode
		&&                                                // AND
		(!((sonar.servo_pose - 10) < 0))                  // the sonar does not exceed the minimun angle in the next move 
		&&                                                // AND
	  !(sonar.f_block_keys)                             // is the first time that interrupts this cycle
	)
	{	
    set_servo(sonar.servo_pose -= 10);                //  Decrease the servo pose by 10 degrees.
    sonar.f_block_keys = 1;                           //  Raise the flag to indicate that we have alredy
  }                                                   //  interrupted this cycle.
}
