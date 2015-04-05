// sudo i2cdetect -r -y 1
// sudo i2cdump -y 1 address (0x77 , 0x69)

#include <iostream>
//#include "ITG3200.h"
#include "BMP180.h"

using std::cout;
using std::endl;

int main() {
	char NotRun = 1;
	while (1) {
 /*
		ITG3200	itg(1, 0x69);
		itg.StartITG();
		if (NotRun) {
			short* biases = itg.Find_ITG_Bias(3);
			cout << "X \t Y \t Z biases" << endl;
			cout << biases[0] << "\t" << biases[1] << "\t" << biases[2] << endl;
			NotRun = 0;
		}
		short* values = itg.Get_XYZT();
		cout << "X \t Y \t Z" << endl;
		cout << values[0] << "\t" << values[1] << "\t" << values[2] << endl;
		itg.CloseITG();
// */
// /*

		BMP_Presure_Temp bmp(1, 0x77);
		bmp.StartBMP();
		cout << "Temperature is " << (float) bmp.GetTemp() / 10 << " degrees Celsius" << endl;
		cout << "Pressure is " << bmp.GetPressure() << " Pa" << endl;
		bmp.CloseBMP();

// */
		cout << "\n" << endl;
		usleep(500000);

	}
	return 0;
}
