#include "hmc.h"
using namespace std;

HMC::HMC(const int bus, const int addr) : i2c(bus) {
	i2c.startbus(addr);
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
usleep(10000);
	buffer[0] = 0x02;
	buffer[1] = 0x00;
	if(i2c.writebus(buffer) != 4) {
		perror("Failed to write measurement mode to HMC.");
	}
}

Magnetics HMC::getData() {
	char* data = new char[6];
	Magnetics foo = {0,0,0};
	data[0] = 0xff;
	char baz[3];	
	

	baz[0] = 0x00;
baz[1] = 0x00;	
baz[2] = 0x03;

	i2c.writebus(baz);	
usleep(10000);
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
	delete[] data;
	return foo;
}

void HMC::close() {
	i2c.closebus();
}

HMC::~HMC() {
	close();
}

