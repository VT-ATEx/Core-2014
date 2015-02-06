#include "hmc.h"

#define BUS 2

HMC::HMC(const int addr) : i2c(BUS) {
	i2c.startbus(addr);
}

HMC::HMC(const int bus, const int addr) : i2c(bus) {
	i2c.startbus(addr);
}

Magnetics HMC::getData() {
	char* data = new char[6];
	Magnetics foo = {0,0,0};
	data[0] = 0xff;
	
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
