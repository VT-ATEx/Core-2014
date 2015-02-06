#include "hmc.h"

//Peter Morrissey
//2015.02.05
//Contact pfm1995[at]vt[dot]edu with questions

//Stars up I2C
int* hmcInit(int addr, int bus) {
	int*  i2cfd;
	i2cfd = I2COpenBus(bus);
	//If I2C fails to open, return error
	if (*i2cfd < 0) return (int*)-1;
	//If I2C fails to contact the sensor, return error
	if (I2CStartSlave(i2cfd, addr) < 0) return (int*)-1;

	return i2cfd;
}

//This is where I pull the data.
Magnetics hmcGetData(int* i2cfd) {
	//Create an array of six bytes to hold the data
	char data[6];
	//Create my struct
	struct Magnetics foo = {0,0,0};

	//If I2C returns the wrong number of bytes, quit with error
	if(I2CRead(i2cfd, 6, data) != 6) {
		perror("Read did not return the bytes specified");	
	} else {
		//Joins the two separate bytes into one intiger
		//for each axis
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
	return foo;
}

//Closes the I2C connection
void hmcClose(int* i2cfd) {
	I2CCloseBus(i2cfd);
}
