/*
 * uart_.h
 *
 *  Created on: 1-Oct-2011
 *  Author: J.M.V.C.
 *  Modified: 12_Dec-2020
 *  Authotr E.C.R and J.O.P-J
 */

/////////////////////////////////
//////////// Original ///////////
/////////////////////////////////
// Preprocessor Directives to include the library only once:
#ifndef UART_H_
#define UART_H_

// Accepted Error baud rate value (in percent unit)
#define UART_ACCEPTED_BAUDRATE_ERROR    3

#define CHAR_8_BIT                      (3 << 0)
#define STOP_1_BIT                      (0 << 2)
#define PARITY_NONE                     (0 << 3)
#define DLAB_ENABLE                     (1 << 7)
#define FIFO_ENABLE                     (1 << 0)
#define RBR_IRQ_ENABLE                  (1 << 0)
#define THRE_IRQ_ENABLE                 (1 << 1)
#define UART_LSR_THRE   				        (1 << 5)
#define RDA_INTERRUPT                   (2 << 1)
#define CTI_INTERRUPT                   (6 << 1)

extern void uart0_init(int baudrate);
extern void tx_cadena_UART0(char *ptr);

/////////////////////////////////
// Modified for the proyect:	///
/////////////////////////////////

// Necessary libraries:
#include <LPC17xx.h>
#include <stdlib.h>
#include "state.h"


// New defines
#define UART_BAUDRATE  9600                           // Selected baudrate

// Necessary global variable:
extern struct sonar_status sonar;                     // Sonar state is modified by timer 0.5 handler.


#endif /* UART_H_ */
