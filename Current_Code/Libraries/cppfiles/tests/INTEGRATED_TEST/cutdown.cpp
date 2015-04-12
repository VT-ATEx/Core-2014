#include "cutdown.h"

#define LED0_PATH "/sys/class/leds/beaglebone:green:usr0"

using namespace std;

int on_led() {
	std::fstream fs;

	fs.open(LED0_PATH "/trigger", std::fstream::out);
	fs << "none";
	fs.close();

	fs.open(LED0_PATH "/brightness", std::fstream::out);
	fs << "1";
	fs.close();
	}

int activate_cutdown(unsigned int pinNum) {
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
	on_led();
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
