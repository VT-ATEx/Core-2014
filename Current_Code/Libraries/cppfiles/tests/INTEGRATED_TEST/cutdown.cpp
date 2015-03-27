#include "cutdown.h"

int activate_cutdown(unsigned int pinNum, double timeVal) {
	sleep(timeVal);
	GPIO gpio(pinNum);
	if (gpio.gpio_export() != 0) {
		perror("Export Failed!");
		return -1;
	}
	if (gpio.gpio_set_dir(1) != 0) {
		perror("Direction Failed!");
		return -1;
	}
	if (gpio.gpio_set_value(1) != 0) {
		perror("Value Set Failed");
		return -1;
	}
	usleep(1000000);
	if (gpio.gpio_set_value(0) != 0) {
		perror("Value not reset!");
		return -1;
	}
	if (gpio.gpio_unexport() != 0) {
		perror("Unexport Failed!");
		return -1;
	}
	return 0;
}
