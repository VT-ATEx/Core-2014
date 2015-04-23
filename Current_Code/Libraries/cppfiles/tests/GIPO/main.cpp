// Outputs to GPIO pin 30, pin 11 on the beagle bone
// Turns the GPIO pin on and off every 500 ms
// Philip Neighbour 4-12-2015
#include <iostream>
#include <unistd.h>
#include "SimpleGPIO.h"
#include "SimpleGPIO.cpp"

using std::cin;
using std::cout;
using std::endl;


int main(){
int pin = 30;
gpio_export(30);
//int gpio_set_dir(unsigned int gpio, PIN_DIRECTION out_flag)
gpio_set_dir(30, OUTPUT_PIN);
//int gpio_set_value(unsigned int gpio, PIN_VALUE value);

int value = 0;
while(1){

gpio_set_value(30, (PIN_VALUE)value);

if(value == 1)
value = 0;
else
value = 1;
usleep(500000);
}

return 0;
}










