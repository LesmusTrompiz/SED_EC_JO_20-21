// Link this source code with his .h file.
#include "screen.h"

void update_screen(struct sonar_status *sonar)
{
  /*
    config_UTS :: *sonar -> void

    Update the screen with the information
    of the sonar state. 
  
  */

  char msg [50];                                      // Variable that will contains the string with the information of the sonar state.
  sprintf(msg, "Measured distance =  %3.2f",          // Format the string with the measured distance.
    sonar->distance);
  GUI_Text(20,40,(uint8_t *)msg,White,Black);         // Puts the info in the screen.
}