// compile with g++ -pthread [filename] -o [executable to create]
// sudo i2cdetect -r -y 1
// sudo i2cdump -y 1 address (0x77 , 0x69)

#define using_BMP180	1
#define using_ITG3200	1
#define using_HMC5883L	1
#define using_LM73		1
#define using_LIS331	1

#define ITG3200Address 0x69
#define BMP180Address 0x77
#define LIS331Address 0x19
#define HMC5883LAddress 0x1E
#define LM73Address 0x48

#include <iostream>
#if using_ITG3200
#include "ITG3200.h"
#endif

#if using_BMP180
#include "BMP180.h"
#endif

#if using_LIS331
#include "LIS331.h"
#endif

#if using_LM73
#include "LM73.h"
#include "LM73.cpp"
#endif

#if using_HMC5883L
#include "hmc.h"
#include "hmc.cpp"
#endif

using std::cout;
using std::endl;

#if using_BMP180
void TakeDataBMP(BMP_Presure_Temp* bmpz)
{BMP_Presure_Temp& bmp = *bmpz;
	bmp.StartBMP();
printf("BMP pressure (pa), temperature (c) %6d , %.1f\n",bmp.GetPressure() , (float) bmp.GetTemp() / 10);
	//		cout << "Temperature is " << (float) bmp.GetTemp() / 10 << " degrees Celsius" << endl;
//		cout << "Pressure is " << bmp.GetPressure() << " Pa" << endl;
//		bmp.CloseBMP();
}
#endif

#if using_ITG3200
void TakeDataITG(ITG3200* itgz)
	{
		ITG3200& itg = *itgz;
		static char ITGBiasesNotFound = 1;
		itg.StartITG();
		if (ITGBiasesNotFound) {
			short* biases = itg.Find_ITG_Bias(3);
			cout << "X \t Y \t Z biases" << endl;
			cout << biases[0] << "\t" << biases[1] << "\t" << biases[2] << endl;
			ITGBiasesNotFound = 0;
		}
		short* values = itg.Get_XYZT();
		//cout << "X \t Y \t Z" << endl;
		//cout << values[0] << "\t" << values[1] << "\t" << values[2] << endl;
		printf("ITG X,Y,Z: %5d %5d %5d\n", values[0],values[1],values[2]);
	//	itg.CloseITG();
	}
#endif


#if using_LIS331
void TakeDataLIS(LIS331* lis331z){
LIS331& lis331 = *lis331z;
usleep(10000);
printf("LIS X: %6f\n", lis331.getXvalue());
usleep(10000);
printf("LIS Y: %6f\n", lis331.getYvalue());
usleep(10000);
printf("LIS Z: %6f\n", lis331.getZvalue());
}
#endif

#if using_LM73
void TakeDataLM73(LM73* lm73z){
LM73& lm73 = *lm73z;
cout << "LM73 Temperature: " << lm73.getTemperature() << endl;
}
#endif 

#if using_HMC5883L
void TakeDataHMC5883L(HMC* hmcz){
HMC& hmc = *hmcz;
Magnetics data = {0,0,0};
data = hmc.getData();
cout <<"HMC x,y,z: "<< data.x << ", " << data.y << ", " << data.z << endl;
}
#endif


int main() {
#if using_ITG3200
		ITG3200	itg(1, ITG3200Address);
#endif

#if using_BMP180
		BMP_Presure_Temp bmp(1, BMP180Address);
#endif 

#if using_LIS331
    LIS331 lis331(1, LIS331Address);
    lis331.Configure(100, 12);
    lis331.setpollrate(10000);
    lis331.setdatapoints(100);
    lis331.start_update();
#endif

#if using_LM73
	LM73 lm73(1, LM73Address);
#endif

#if using_HMC5883L
	HMC hmc(1, HMC5883LAddress);
#endif

	while (1) {

#if using_ITG3200
TakeDataITG(&itg);
#endif
	
#if using_BMP180
TakeDataBMP(&bmp);	
#endif

#if using_LIS331
TakeDataLIS(&lis331);
#endif

#if using_LM73
TakeDataLM73(&lm73);
#endif

#if using_HMC5883L
TakeDataHMC5883L(&hmc);
#endif


// */
		cout << "\n" << endl;
		usleep(500000);
	}
	return 0;
}
