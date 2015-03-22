#include "cutdown.h"

int main (int argc, char *argv[]) {
	unsigned int pinNumber;
	double timeValue;

	pinNumber = 1;
	timeValue = 30000000;

	if(activate_cutdown(pinNumber, timeValue) != 0) {
		perror("Error, Aborted!");
		return -1;
	}

	return 0;
} 
