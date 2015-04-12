#ifndef CUTDOWN_H_
#define CUTDOWN_H_

#include "GPIO.h"
#include <fstream>
#include <unistd.h>

//timeVal is in seconds
int activate_cutdown(unsigned int pinNum, double timeVal);

#endif


