//#define path "/sys/class/leds/beaglebone:green:usrx"

#include<iostream>
#include<fstream>
#include<string>
#include <unistd.h>
using namespace std;

#define path "/sys/class/leds/beaglebone:green:usrx/brightness"

void light(short led, bool on) {
	static fstream fs;
	static char thing[] = path;
	thing[36] = led + '0';
	cout <<"Filepath: " << thing << endl;
	fs.open(thing);
	if (!fs)
		cout << "fs does not exist for " << led << endl;
	fs << on;
	fs.close();
}

int main() {
	bool state;
	cout << "Beginning LED flash program" << endl;
	long delay = 1000000;
	cout << "delay is " << delay / 1000000. << " Seconds" << endl;
	cout << "lighting all leds:" << endl;

	//Turns the LEDs on
	state = 1;
	for (int i = 0; i <= 3; i++) {
		light(i, state);
	}
	cout << "Delay1 begining" << endl;
	usleep(delay);
	
	//Turns the lEDs off
	state = 0;
	for (int i = 0; i <= 3; i++) {
		light(i, state);
	}
	cout << "Delay2 begining" << endl;
	usleep(delay);
	
		//Turns the LEDs on
	state = 1;
	for (int i = 0; i <= 3; i++) {
		light(i, state);
	}
	cout << "Delay1 begining" << endl;
	usleep(delay);
	
	//Turns the lEDs off
	state = 0;
	for (int i = 0; i <= 3; i++) {
		light(i, state);
	}
	cout << "Delay2 begining" << endl;
	usleep(delay);
		
	cout << "Finished the LED flash program" << endl;
	return 0;
}
