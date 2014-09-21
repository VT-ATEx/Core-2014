#include "ChipCap2.h"

#define BUS 2

ChipCap2::ChipCap2(const int addr) : i2c(BUS) {
	i2c.startbus(addr);
}

ChipCap2::ChipCap2(const int bus, const int addr) : i2c(bus) {
	i2c.startbus(addr);
}

float ChipCap2::getHumidity() {
	char* data = new char[5];
	float value;

	data[0] = 0xff;

	while ((data[0] >> 6) != 0) {
		if(i2c.readbus(data, 4) != 4) {
			perror("Read did not return bytes specified");
			delete[] data;
			return -1;
		} else if ((data[0] >> 6) == 0) {
			value = (((float)(data[0]&0x3f)*256 + data[1])/16384)*100;
		}
	}
	delete[] data;

	return value;
}

float ChipCap2::getTemperature() {
	char* data = new char[5];
	float value;

	data[0] = 0xff;

	while ((data[0] >> 6) != 0) {
		if(i2c.readbus(data, 4) != 4) {
			perror("Read did not return bytes specified");
			delete[] data;
			return -1;
		} else if ((data[0] >> 6) == 0) {
			value = (data[2]*64 + (float)(data[3]>>2)/4)/16384*165-40;
		}
	}
	delete[] data;

	return value;
}

void ChipCap2::close() {
	i2c.closebus();
}

ChipCap2::~ChipCap2() {
	close();
}