// Preprocessor Directives to include the library only once
#ifndef _SCREEN
#define _SCREEN

// Necessary libraries:
#include <stdio.h>
#include "state.h"
#include "GLCD/GLCD.h"

// Avaible functions that can be called:
void update_screen(struct sonar_status *sonar);       // Update the screen with the information of the sonar state.

#endif


