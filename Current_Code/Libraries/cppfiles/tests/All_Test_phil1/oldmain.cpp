// compile with g++ -pthread [filename] -o [executable to create]
// sudo i2cdetect -r -y 1
// sudo i2cdump -y 1 address (0x77 , 0x69)

#include <iostream>
#include "ITG3200.h"
#include "BMP180.h"
#include "LIS331.h"


using std::cout;
using std::endl;

void TakeDataBMP(BMP_Presure_Temp* bmpz)
{BMP_Presure_Temp& bmp = *bmpz;
	bmp.StartBMP();
		cout << "Temperature is " << (float) bmp.GetTemp() / 10 << " degrees Celsius" << endl;
		cout << "Pressure is " << bmp.GetPressure() << " Pa" << endl;
		bmp.CloseBMP();
}
void TakeDataITG(ITG3200* itgz)
{ITG3200& itg = *itgz;
		static char ITGBiasesNotFound = 1;
		itg.StartITG();
		if (ITGBiasesNotFound) {
			short* biases = itg.Find_ITG_Bias(3);
			cout << "X \t Y \t Z biases" << endl;
			cout << biases[0] << "\t" << biases[1] << "\t" << biases[2] << endl;
			ITGBiasesNotFound = 0;
		}
		short* values = itg.Get_XYZT();
		cout << "X \t Y \t Z" << endl;
		cout << values[0] << "\t" << values[1] << "\t" << values[2] << endl;
		itg.CloseITG();
		

}

void TakeDataLIS(LIS331* lis331z){
LIS331& lis331 = *lis331z;
std::cout << "X: " << lis331.getXvalue() << endl;
std::cout << "Y: " << lis331.getYvalue() << endl;
std::cout << "Z: " << lis331.getZvalue() << endl;

}

int main() {
		ITG3200	itg(1, 0x69);
		
		BMP_Presure_Temp bmp(1, 0x77);


    LIS331 lis331(1, 0x19);
    lis331.Configure(100, 12);
    lis331.setpollrate(10000);
    lis331.setdatapoints(100);
    lis331.start_update();


	while (1) {
// TakeDataITG(&itg);

// TakeDataBMP(&bmp);	

TakeDataLIS(&lis331);

		cout << "\n" << endl;
		usleep(500000);

	}
	return 0;
}
