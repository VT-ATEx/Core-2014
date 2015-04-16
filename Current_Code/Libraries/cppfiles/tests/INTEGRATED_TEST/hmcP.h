#ifndef HMC_H_
#define HMC_H_

#include "I2C.h"

typedef struct Magnetics{
	char x;
	char y;
	char z;
} Magnetics;

class HMC {
private:
	I2C i2c;
public:
	HMC(const int bus, const int addr);
	~HMC();
	Magnetics getData();
	void close();
	int selfTest();
};

#endif
