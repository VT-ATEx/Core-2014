//Philip N 2/13/2015

// Contains a class the is initialized by specifying bus

// Returns an int that represents that directly corelates to atmospheric temp in pa
// int BMP_Presure_Temp::GetPressure();

// class BMP_Presure_Temp;

#include <iostream>
#include <math.h>
#ifndef I2C_H
#include "I2C.h"
#include "I2C.cpp"
#endif
#include <unistd.h>
class BMP_Presure_Temp;
#include "BMP180.cpp"

 /* Example Code
 ******************************************
 		BMP_Presure_Temp bmp(1, 0x77);
		bmp.StartBMP();
		cout << "Temperature is " << (float) bmp.GetTemp() / 10 << " degrees Celsius" << endl;
		cout << "Pressure is " << bmp.GetPressure() << " Pa" << endl;
		bmp.CloseBMP();
*******************************************

/*
	void CloseBMP();
	void StartBMP();
	BMP_Presure_Temp(int bus);
	int GetTemp();
	int GetPressure();
*/
/*
Class functions to pay attention to:
1. StartBMP{)
2. CloseBMP()
3. BMP_Presure_Temp(Bus)
4. int BMP_Presure_Temp::GetPressure()
5. int BMP_Presure_Temp::GetTemp()
*/
