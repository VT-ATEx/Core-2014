#include "RTC.h"

#define BUS 2

RTC::RTC(const int addr) : i2c(BUS) {
	i2c.startbus(addr);
}

RTC::RTC(const int bus, const int addr) : i2c(bus) {
	i2c.startbus(addr);
}
int RTC::getSeconds()
{
	char* address = "0x00";
	
	char* data = new char[0];
	int value;

	i2c.writebus(address);

	data[0] = 0xff;

	while ((data[0] >> 6) != 0) {
		if(i2c.readbus(data, 1) != 1) {
			perror("Read did not return bytes specified");
			delete[] data;
			return -1;
		} else if ((data[0] >> 6) == 0) {
			value = data[0];
		}
	}
	delete[] data;

	return value;

}

int RTC::getMinutes()
{

	char* address = "0x01";
	char* data = new char[0];
	int value;

	i2c.writebus(address);

	data[0] = 0xff;

	while ((data[0] >> 6) != 0) {
		if(i2c.readbus(data, 1) != 1) {
			perror("Read did not return bytes specified");
			delete[] data;
			return -1;
		} else if ((data[0] >> 6) == 0) {
			value = data[0];
		}
	}
	delete[] data;

	return value;


}

int RTC::getHours()
{
	char* address = "0x03";


	char* data = new char[0];
	int value;

	i2c.writebus(address);

	data[0] = 0xff;

	while ((data[0] >> 6) != 0) {
		if(i2c.readbus(data, 1) != 1) {
			perror("Read did not return bytes specified");
			delete[] data;
			return -1;
		} else if ((data[0] >> 6) == 0) {
			value = data[0];
		}
	}
	delete[] data;

	return value;


}

char* RTC::getTime ()
{
	char* address = "0x00";

	char* data = new char[10];
	char value[8];

	i2c.writebus(address);

	data[0] = 0xff;

	while ((data[0] >> 6) != 0) {
		if(i2c.readbus(data, 3) != 1) {
			perror("Read did not return bytes specified");
			delete[] data;
			return "-1";
		} else if ((data[0] >> 6) == 0) {
			sprintf(value, "%d:%d:%d", data[2], data[1], data[0]);
		}
	}
	delete[] data;

	return value;


}


void RTC::close() {
	i2c.closebus();
}

RTC::~RTC() {
	close();
}