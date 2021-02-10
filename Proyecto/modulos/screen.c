// Link this source code with his .h file.
#include "screen.h"

void update_screen(struct sonar_status *sonar)
{
  /*
    update_screen :: *sonar -> void

    Update the screen with the information
    of the sonar state. 
  
  */
  char msg [50];                                      // Variable that will contains the string with the information of the sonar.
 
  switch(sonar->state)
  {
    case(ST_SETUP):
      sprintf(msg, "Sonar mode = Setup");             
      GUI_Text(20,70,(uint8_t *)msg,White,Black);     // Puts the info in the screen.
      break;
    case(ST_MANUAL):
      sprintf(msg, "Sonar mode = Manual");            
      GUI_Text(20,70,(uint8_t *)msg,White,Black);     // Puts the info in the screen.
      break;
    case(ST_AUTOMATIC):
      sprintf(msg, "Sonar mode = Automatic");         
      GUI_Text(20,70,(uint8_t *)msg,White,Black);     // Puts the info in the screen.
      break;
    
  }

  sprintf(msg, "Sonar pose = %d ",                    // Format the string with the servo pose.
    sonar->servo_pose);
  GUI_Text(20,100,(uint8_t *)msg,White,Black);        // Puts the info in the screen.

  sprintf(msg, "Measured distance = %3.2f ",          // Format the string with the measured distance.
    sonar->distance);
  GUI_Text(20,130,(uint8_t *)msg,White,Black);        // Puts the info in the screen.

}
