#include "LM73.h"
#include "LM73.cpp"
#include "I2C.h"
#include "I2C.cpp"

#include <iostream>

using namespace std;

int main() {
	LM73 lm73(1, 0x48);

	while(1) {

		cout << "Temperature: " << lm73.getTemperature() << endl;
	}

	return 0;
}
