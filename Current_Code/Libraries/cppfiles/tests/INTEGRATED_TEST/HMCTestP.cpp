#include "hmc.h"
#include <iostream>

using namespace std;

int main() {
	HMC hmc(1, 0x1E);
	Magnetics data;
	
	while(1) {
		data = hmc.getData();
		cout << data.x << ", " << data.y << ", " << data.z << endl;
	}
	
	return 0;
}
