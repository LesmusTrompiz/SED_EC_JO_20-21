#include <LPC17xx.H>



int main(){
	


	///// Comenzamos con la configuración de PWM 1.1
	//// PCONP (Alimentación) por defecto a 1
	//// Clock, no hace falta cambiar la configuración.
	//   Pinsel:
	LPC_PINCON->PINSEL3|= (1<<9);

	
}
