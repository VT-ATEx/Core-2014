#ifndef CUTDOWN_H_
#define CUTDOWN_H_

#include "GPIO.h"
#include <fstream>
#include <unistd.h>

void on_led(); 
int activate_cutdown(unsigned int pinNum);

#endif


