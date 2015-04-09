#include "I2C.h"
#include "I2C.cpp"
#include "hmc.h"
#include "hmc.cpp"

#include <iostream>

using namespace std;

int main() {
	HMC hmc(1, 0x1E);
	
	while(1) {
		Magnetics data = {0,0,0};
		data = hmc.getData();
	cout << data.x << ", " << data.y << ", " << data.z << endl;
usleep(500000);
	}
	
	return 0;
}
