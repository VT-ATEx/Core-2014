#include "hmc.h"
#include <unistd.h>
#include <stdlib.h>

using namespace std;

int HMC::selfTest() {
	//NB: Assumes bus has been started
	char buffer[2];
	Magnetics foo;
	buffer[0] = 0x00;
	buffer[1] = 0x71;
	usleep(10000);
	if (i2c.writebus(buffer) != 4) {
		perror("Failed to write self-test settings to HMC.");
	}

	buffer[0] = 0x01;
	buffer[1] = 0xA0;
	usleep(10000);
	if (i2c.writebus(buffer) != 4) {
		perror("Failed to write self-test gain to HMC.");
	}

	buffer[0] = 0x02;
	buffer[1] = 0x00;
	usleep(10000);
	if (i2c.writebus(buffer) != 4) {
		perror("Failed to write self-test measurement mode.");
	}

	foo = HMC::getData();

	//TODO: Write automatic gain adjustment
	if (243 > foo.x && foo.y && foo.z) {
		perror("Self test failed, gain too low");
		return 1;
	}
	if (243 > foo.x && foo.y && foo.z) {
		perror("Self test failed, gain too low");
		return 1;
	}

	buffer[0] = 0x00;
	buffer[1] = 0x70;
	if (i2c.writebus(buffer) != 4) {
		perror("Failed to exit test mode");
		return 1;
	}

	return 0;
}
	
HMC::HMC(const int bus, const int addr) : i2c(bus) {
	i2c.startbus(addr);

	if (HMC::selfTest() != 0) {
		perror("Failed self test. Aborting.");
		abort();
	}

	//Sets sensor to single measurement, 8-average, 15 Hz
	char buffer[2];
	buffer[0] = 0x00;
	buffer[1] = 0x70;
	usleep(10000);
	if (i2c.writebus(buffer) != 4) {
		perror("Failed to write settings to HMC.");
	}

	buffer[0] = 0x01;
	buffer[1] = 0xA0;
	usleep(10000);
	if(i2c.writebus(buffer) != 4) {
		perror("Failed to write gain settings to HMC.");
	}

	buffer[0] = 0x02;
	buffer[1] = 0x00;
	usleep(10000);
	if(i2c.writebus(buffer) != 4) {
		perror("Failed to write measurement mode to HMC.");
	}
	usleep(60);
}

Magnetics HMC::getData() {
	char* data = new char[6];
	Magnetics foo = {0,0,0};		
	char bar[1];
	bar[0] = 0x03;	
	
	i2c.writebus(bar);
	usleep(60);
	if(i2c.readbus(data, 6) != 6) {
		perror("Read did not return bytes specified");
		delete[] data;
		return foo;
	} else {
		foo.x = data[0];
		foo.y = data[2];
		foo.z = data[4];
		foo.x << 8;
		foo.y << 8;
		foo.z << 8;
		foo.x |= data[1];
		foo.y |= data[3];
		foo.z |= data[5];
	}
	i2c.writebus(bar);
	delete[] data;
	usleep(70);
	return foo;
}

void HMC::close() {
	i2c.closebus();
}

HMC::~HMC() {
	close();
}

