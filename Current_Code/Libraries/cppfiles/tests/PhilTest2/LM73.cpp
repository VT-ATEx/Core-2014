#include "LM73.h"

LM73::LM73(const int bus, const int addr) : i2c(bus) {
        i2c.startbus(addr);
}

float LM73::getTemperature() {
    char data[5];
    float value;

    i2c.writebus(0x00);

    if (i2c.readbus(data, 2) != 2) {
        perror("Read did not return bytes specified");
        return -1;
    } else {
		
		if (data[0] & 128) {
			data[0] = (data[0]^0xff)+1;
			data[1] = (data[1]^0xff)+1;
			value = -1*((data[0] << 8) + data[1])/128.0;
		} else {
			value = ((data[0] << 8) + data[1])/128.0;
		}
    }
    return value;
} 

void LM73::close() {
        i2c.closebus();
}

LM73::~LM73() {
        close();
}