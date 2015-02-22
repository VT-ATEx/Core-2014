
//Philip N 2/13/2015

#define Adress 0x77 // The I2C address that the BMP is on
// #define Bus 1 // The I2C bus the BMP is on
#define Delay 12500 // Min delay between reading temperature and pressure values. should be > than 4500 (4.5 ms)

#include <iostream>
#include <math.h>
#include "I2C.cpp"
#include "I2C.h"
#include <unistd.h>

using std::cin;
using std::cout;
using std::endl;

// Prototypes. Functions are called by the class

class BMP_Presure_Temp {
public:
void RegWrite(char NewValue, short LocationToWriteTo);
void read(char* data, short location, short BytesToRead);
void SetupBMP180();

int Bus;
I2C* I2C_Object;

//registers 0xab to 0xbf need to be read before using the class
//class functions handle the reading of registers 0xf6 to 0xf8
	BMP_Presure_Temp(int Bus);
	int GetTemp();
	int GetPressure();
	long p; //The atmospheric pressure in pa
	long T; //The temperature in Celsius * 10. 150 = 15 Celsius, 200 = 20, ect...
private:
char* data;

	short AC1, AC2, AC3, B1, B2, MB, MC, MD;
	long x1, x2, B5,  UT;
	unsigned short AC4, AC5, AC6;
	short MSB, LSB;

//pressure specific variables
	long UP, x3, B3, B6;
	unsigned long B4, B7;
// this variable, oversampling setting? is customizable
	char oss;
	char XLSB;
//end of pressure specific variables

void StartBMP(){
I2C_Object ->startbus(Adress);
}

void CloseBMP()
{
I2C_Object ->closebus();
}

};
BMP_Presure_Temp::BMP_Presure_Temp(int Bus) {
data = new char[0x100];
this->Bus = Bus;
I2C_Object = new I2C(Bus);

//AC1(0xAA)(0XAB), AC2(0XAC)(0XAD), AC3(0XAE)(0XAF), AC4(0XB0)(0XB1), AC5(0XB2)(0XB3),
//AC6(0XB4)(0XB5), B1(0XB6)(0XB7) , B2(0XB8)(0XB9) , MB(0XBA)(0XBB) , MC(0XBC)(0XBD) ,
//MD(0XBE)(0XBF)
	AC1 = (data[0xaa] << 8) + data[0xab];
	AC2 = (data[0xac] << 8) + data[0xad];
	AC3 = (data[0xae] << 8) + data[0xaf];
	AC4 = (data[0xb0] << 8) + data[0xb1];
	AC5 = (data[0xb2] << 8) + data[0xb3];
	AC6 = (data[0xb4] << 8) + data[0xb5];
	B1 = (data[0xb6] << 8) + data[0xb7];
	B2 = (data[0xb8] << 8) + data[0xb9];
	MB = (data[0xba] << 8) + data[0xbb];
	MC = (data[0xbc] << 8) + data[0xbd];
	MD = (data[0xbe] << 8) + data[0xbf];
//this is a customizable variable
	oss =2;
}

int BMP_Presure_Temp::GetTemp() {
//AC1=408;AC2=-72;AC3=-14383;AC4=32741;AC5=32757;AC6=23153; B1=6190;B2=4; MB =-32768;MC=-8711;MD=2868;
//printf("%02x",data[0xac]);printf("%02x\n",data[0xad]);
//The following algorithm is outlined in the BMP 180 data sheet
	RegWrite(0x2e, 0xf4);
	read(data + 0xF6, 0xF6, 2);

	MSB = data[0xF6];
	LSB = data[0xF7];
	UT = (MSB << 8) + LSB;

	x1 = ((UT - AC6) * AC5 >> 15);
	x2 = (MC << 11) / (x1 + MD);
	B5 = x1 + x2;
	T = (B5 + 8) / 16;
return T;
}

// Returns an int that represents that directly corelates to atmospheric temp in pa
int BMP_Presure_Temp::GetPressure() {
// The following algorithm is outlined in the BMP 180 data sheet
//RegWrite and read pause at the beginning when they are called
//they pause for ~4.5 ms
	RegWrite(0x34 + (oss << 6), 0xf4);
	read(data + 0xF6, 0xF6, 3);
	MSB = data[0xf6];
	LSB = data[0xf7];
	XLSB = data[0xf8];
	UP = ((MSB << 16) + (LSB << 8) + XLSB) >> (8 - oss);
	B6 = B5 - 4000;
	x1 = (B2 * ((B6 * B6) >> 12)) >> 11;
	x2 = (AC2 * B6) >> 11;
	x3 = x1 + x2;
	B3 = (((AC1 * 4 + x3) << oss) + 2) / 4;

	x1 = (AC3 * B6) >> 13;
	x2 = (B1 * (B6 * B6)) >> (12 + 16);
	x3 = ((x1 + x2) + 2) / 4;
	B4 = (AC4 * (unsigned long) (x3 + 32768)) >> 15;
	B7 = ((unsigned long) UP - B3) * (50000 >> oss);
	if (B7 < 0X80000000) {
		p = (B7 * 2) / B4;
	}
	else {
		p = (B7 / B4) * 2;
	}

	x1 = (p >> 8) * (p >> 8);
	x1 = (x1 * 3038) >> 16;
	x2 = (-7357 * p) >> 16;
	p = p + (x1 + x2 + 3791) / 16;
return p;
// a = 44330*(1 - (p/p0)^(1/5.255)) . //p = output pressure. //p0 = 101325 (sea level)
}

void BMP_Presure_Temp::SetupBMP180()
{
I2C_Object->startbus(Adress);;
}

void BMP_Presure_Temp::read(char* data, short location, short BytesToRead) {
/*
	static I2C cat(Bus);
	static bool run = 0;
	if (!run) {
		cat.startbus(Adress);
		run = 1;
	}
// */
	static char* WriteLocation = new char[3];
	WriteLocation[2] = location;
	usleep(Delay);
	I2C_Object->writebus(WriteLocation);
	usleep(Delay);
	I2C_Object->readbus(data, BytesToRead);
}


void BMP_Presure_Temp::RegWrite(char NewValue, short LocationToWriteTo) {
//	static I2C cat(Bus);
//	cat.startbus(Adress);

	static char* temp = new char[2];
	temp[0] = LocationToWriteTo;
	temp[1] = NewValue;
	usleep(Delay);
	I2C_Object->writebus(temp);
}
// */

