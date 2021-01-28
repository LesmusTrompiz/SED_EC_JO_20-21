#ifndef _SONARSTATUS

#define _SONARSTATUS


#define ST_SETUP      0			                    // sonar.state inicial		
#define ST_MANUAL     1			                      // sonar.state manual
#define ST_AUTOMATICO 2			// sonar.state automático
#define ST_TEST 			3			// Modo para probar nuevas funcionalidades
#define N_MUESTRAS    32    // Numero de muestras en la señal sinusoidal
#define POSITIVO      0			// sonar.f_block_move del servo en sentido horario.
#define NEGATIVO      1			// sonar.f_block_move del servo en sentido antihorario.

struct sonar_status
{
  char  state;                       							// Variable que indica el estado.
  float distance;           											// Distancia medida por el ultrasonidos en cm.
  int   servo_pose; 
  int   servo_period;
  int   servo_resolution;
  char  f_block_keys;       											// Flag para evitat.
  char  f_block_move;      										    // Flag que habilita/deshabilita el servo, se modifica con el ISP.
  char  f_block_measure;											    // Flag que habilita/deshabilita el UTS, se modifica con el ISP.
};

#endif
