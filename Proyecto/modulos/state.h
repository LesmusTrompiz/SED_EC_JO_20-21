// Preprocessor Directives to include the library only once
#ifndef _SONARSTATUS
#define _SONARSTATUS


// New defines:
#define ST_SETUP       0			                        // Number associated with the setup mode.	
#define ST_MANUAL      1			                        // Number associated with the manual mode.
#define ST_AUTOMATIC   2			                        // Number associated with the automatic mode.
#define POSITIVO       0			                        // Number associated with the positive direction of the servo.
#define NEGATIVO       1			                        // Number associated with the negative direction of the servo.
#define N_SAMPLES     32                              // Total number of samples of the sinusoidal signal.
#define Fpclk 25e6	                                  // Frecuency of the peripherals by default.

// New struct:
struct sonar_status               
{
  /*
    Structure that contains all
    the information of the Sonar
    and allows us to handle it
    in a simple way.
  */
  char  state;                       							    // Contains the mode of the servo, modes allowed: Setup, Manual and Automatic.
  float distance;           											    // The distance measure by the UTS in cm.
  int   servo_pose;                                   // The position of the servo in degrees.
  int   servo_period;                                 // How many 0.5 cycles are equal to the period of the servo, this parameter can be configured via UART.
  int   servo_resolution;                             // Servo motion resolution,this parameter can be configured via UART.
  char  f_block_keys;       											    // Flag to prevent the Eints handlers being executed multiple times when the button is pressed.
  char  f_block_move;      										        // Flag that enables the movement of the servo in automatic mode, it can be modified via ISP.
  char  f_block_measure;											        // Flag that allows the distance measure in manual mode, it can be modified via ISP.
};

#endif
