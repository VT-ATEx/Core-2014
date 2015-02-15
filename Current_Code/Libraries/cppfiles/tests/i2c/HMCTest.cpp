#include "hmc.h"
#include <iostream>

using namespace std;

int main() {
	HMC hmc(2, 0x3C);
	Magnetics data;
	
	while(1) {
		data = hmc.getData();
		cout << data.x + ", " + data.y + ", " + data.z << endl;
	}
	
	return 0;
}
