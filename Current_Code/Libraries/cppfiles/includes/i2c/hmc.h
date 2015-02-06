#ifndef HMC_H_
#define HMC_H_

#include "I2C.h"

typedef struct Magnetics{
	int x;
	int y;
	int z;
} Magnetics;

class HMC {
private:
	I2C i2c;
public:
	HMC(const int addr, const int bus);
	~HMC();
	Magnetics getData();
	void close();
};

#endif
