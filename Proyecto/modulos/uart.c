/* uart.c
 * contiene las funciones:

 1  UART0_IRQHandler(void)
 2  tx_cadena_UART0(char *ptr)
 3  uart0_set_baudrate(unsigned int baudrate) 
 4  uart0_init(int baudrate) 
  
 */
#include <LPC17xx.h>
#include "uart.h"

char *ptr_tx;			// puntero de transmisi�n
char tx_completa;		// Flag de transmisi�n de cadena completa
char buffer[30];	    // Buffer de recepci�n

/*
 * UART0 interrupt handler
 */
 void tx_cadena_UART0(char *cadena)
{
   ptr_tx=cadena;
   tx_completa=0;
   LPC_UART0->THR=*ptr_tx++;	 // IMPORTANTE: Introducir un car�cter al comienzo para iniciar TX o
}							     // activar flag interrupci�n por registro transmisor vacio



void analize_msg(void)
    {
        static char first_time = 1;
        // Esta variable es algo cutre, cambiar para la versión final,
				// aunque sea más rollo analizar los numeros trabajar 
				// con chars.
				int aux = 0; 
        
				if (first_time)
				{
            tx_cadena_UART0("You're in automatic mode. \n - To set resolution in degrees enter xxg (where xx are possible resolutions in degrees: 05, 10, 15, 20).\n-To set the period of each servomotor movement enter xs(where x are possible periods in seconds: 1 \n (for 0.5s), 2 (for 1s), 3 (for 2s). \n-To show this help message again press h ");
					  first_time = 0;
        }    
        else if(buffer[0] == 'h')
            tx_cadena_UART0("You're in automatic mode. \n - To set resolution in degrees enter xxg (where xx are possible resolutions in degrees: 05, 10, 15, 20).\n-To set the period of each servomotor movement enter xs(where x are possible periods in seconds: 1 \n (for 0.5s), 2 (for 1s), 3 (for 2s). \n-To show this help message again press h ");
        else if(buffer[1] == 's')
        {
					if (buffer[0] > '0' && buffer[0] <= '3')
                sonar.servo_period = buffer[0] - 48;
				}
        else if(buffer[2] == 'g')
        {   
            buffer[2] = 0;
            aux = atoi(buffer);
            if(aux== 5 || aux== 10 || aux == 15|| aux == 20)
              sonar.servo_resolution = aux;
        }
        else
				{
           tx_cadena_UART0("Mensaje no esperado"); 
				}
}

void UART0_IRQHandler(void) {
	
  switch(LPC_UART0->IIR&0x0E) {
		static int index = 0;
		case 0x04:								 	    /* RBR, Receiver Buffer Ready */
			buffer[index] = LPC_UART0->RBR; 	            /* lee el dato recibido y lo almacena */
			if (buffer[index] == 13) 					    // Caracter return --> Cadena completa
			{
        analize_msg();
				//buffer[index] = 0;		  					/* A�adimos el caracter null para tratar los datos recibidos como una cadena*/ 
				index = 0;
			}
			else 
				index++;
			
		break;
	
   case 0x02:								            /* THRE, Transmit Holding Register empty */
		if (*ptr_tx!=0) LPC_UART0->THR=*ptr_tx++;	    /* carga un nuevo dato para ser transmitido */
		else tx_completa=1;
		break;

    }
}

// Funci�n para enviar una cadena de texto
// El argumento de entrada es la direcci�n de la cadena, o
// directamente la cadena de texto entre comillas


static int uart0_set_baudrate(unsigned int baudrate) {
    int errorStatus = -1; //< Failure

    // UART clock (FCCO / PCLK_UART0)
   // unsigned int uClk = SystemCoreClock / 4;
    unsigned int uClk =SystemCoreClock/4;
    unsigned int calcBaudrate = 0;
    unsigned int temp = 0;

    unsigned int mulFracDiv, dividerAddFracDiv;
    unsigned int divider = 0;
    unsigned int mulFracDivOptimal = 1;
    unsigned int dividerAddOptimal = 0;
    unsigned int dividerOptimal = 0;

    unsigned int relativeError = 0;
    unsigned int relativeOptimalError = 100000;

    uClk = uClk >> 4; /* div by 16 */

    /*
     *  The formula is :
     * BaudRate= uClk * (mulFracDiv/(mulFracDiv+dividerAddFracDiv) / (16 * DLL)
     *
     * The value of mulFracDiv and dividerAddFracDiv should comply to the following expressions:
     * 0 < mulFracDiv <= 15, 0 <= dividerAddFracDiv <= 15
     */
    for (mulFracDiv = 1; mulFracDiv <= 15; mulFracDiv++) {
        for (dividerAddFracDiv = 0; dividerAddFracDiv <= 15; dividerAddFracDiv++) {
            temp = (mulFracDiv * uClk) / (mulFracDiv + dividerAddFracDiv);

            divider = temp / baudrate;
            if ((temp % baudrate) > (baudrate / 2))
                divider++;

            if (divider > 2 && divider < 65536) {
                calcBaudrate = temp / divider;

                if (calcBaudrate <= baudrate) {
                    relativeError = baudrate - calcBaudrate;
                } else {
                    relativeError = calcBaudrate - baudrate;
                }

                if (relativeError < relativeOptimalError) {
                    mulFracDivOptimal = mulFracDiv;
                    dividerAddOptimal = dividerAddFracDiv;
                    dividerOptimal = divider;
                    relativeOptimalError = relativeError;
                    if (relativeError == 0)
                        break;
                }
            }
        }

        if (relativeError == 0)
            break;
    }

    if (relativeOptimalError < ((baudrate * UART_ACCEPTED_BAUDRATE_ERROR) / 100)) {

        LPC_UART0->LCR |= DLAB_ENABLE; 	// importante poner a 1
        LPC_UART0->DLM = (unsigned char) ((dividerOptimal >> 8) & 0xFF);
        LPC_UART0->DLL = (unsigned char) dividerOptimal;
        LPC_UART0->LCR &= ~DLAB_ENABLE;	// importante poner a 0

        LPC_UART0->FDR = ((mulFracDivOptimal << 4) & 0xF0) | (dividerAddOptimal & 0x0F);

        errorStatus = 0; //< Success
    }

    return errorStatus;
}
 					   					  
void uart0_init(int baudrate) {
    
    LPC_PINCON->PINSEL0|=(1<<4)|(1<<6);// Change P0.2 and P0.3 mode to TXD0 and RXD0
  
    LPC_UART0->LCR &= ~STOP_1_BIT & ~PARITY_NONE; // Set 8N1 mode (8 bits/dato, sin pariad, y 1 bit de stop)
    LPC_UART0->LCR |= CHAR_8_BIT;

    uart0_set_baudrate(baudrate);// Set the baud rate
    
     
    LPC_UART0->IER = THRE_IRQ_ENABLE|RBR_IRQ_ENABLE;// Enable UART TX and RX interrupt (for LPC17xx UART)   
    NVIC_EnableIRQ(UART0_IRQn);// Enable the UART interrupt (for Cortex-CM3 NVIC)
    NVIC_SetPriority(UART0_IRQn, 1);  
    
}

