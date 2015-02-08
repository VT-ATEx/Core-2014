//Philip Neighbour 2/7/2015
// The ITG 3200 gyro
//sudo i2cdetect -r 1
//sudo i2cdump 1 0x69
#define Adress 0x69
#define Bus 1
#define Delay 10000 //in 10^(-6) seconds
#include "I2C.cpp"
#include "I2C.h"

#include <unistd.h>
#include <ctime>
#include <stdio.h>
#include <iostream>

using std::cin;
using std::cout;
using std::endl;

// A function for debugging and printing an array of char by having 16 char per row
void FancyPrint(char* data, short start, short end);
//returns an array for correct bias values
void findBias(short* Biases);
void RawADCValuePrint();

// Prepares reading of a register
void prepread(int address, I2C* Some_I2C_Class) {
	static char* send = new char[1];
	send[0] = address - 3;
	usleep(Delay);
	Some_I2C_Class->writebus(send);
}

void SetData(char* data, int StartRegister, int EndRegister, I2C* Some_I2C_Class) {
	prepread(StartRegister, Some_I2C_Class);
	usleep(Delay);
	Some_I2C_Class->readbus(data + StartRegister, EndRegister - StartRegister);
}

void Write_1_To_Register(char registerr, char Value_To_Write, I2C* Some_I2C_Class) {
	char Data_To_Write[2];
// The register to write to
	Data_To_Write[0] = registerr;
	Data_To_Write[1] = Value_To_Write;
	Some_I2C_Class->writebus(Data_To_Write);
}

#define biastime 3*1000000 // in 10^(-6) seconds
void Find_ITG_Bias(short* Biases, I2C* Some_I2C_Class) {
	short X, Y, Z;
//is divided later to find average bias
	long Xt, Yt, Zt;
	Xt = 0;
	Yt = 0;
	Zt = 0;

	long TimesToLoop = biastime / Delay / 3;
	cout << "calibrating biases. wait ~" << biastime / 1000000 << " seconds" << endl;
	cout << "sampling " << TimesToLoop << " Times" << endl;

	static char* data = new char[0x40];
	for (int i = 0; i < TimesToLoop; i++) {
		SetData(data, 0x1b, 0x24, Some_I2C_Class);
		X = (data[0x1d] << 8) | data[0x1e];
		Y = (data[0x1f] << 8) | data[0x20];
		Z = (data[0x21] << 8) | data[0x22];

		Xt += X;
		Yt += Y;
		Zt += Z;
		usleep(Delay);
	}
//the x bias
	Biases[0] = Xt / TimesToLoop;
//the y bias
	Biases[1] = Yt / TimesToLoop;
//the z bias
	Biases[2] = Zt / TimesToLoop;
	cout << "Done Sampling Bias" << endl;
}

// Note that there are a ADC value corresponds to 1/14.375 degrees per second 
int main() {

	I2C ITG3200(Bus);
	ITG3200.startbus(Adress);

//set register 0x16 to 0x03
	Write_1_To_Register(0x16, 0x03, &ITG3200);

//0x1000 = 256
	short datasize = 0x100;
// array that will contain the register values read in from the sensor
	char* data = new char[datasize];
//TEMP_OUT_H =  0x1B : TEMP_OUT_L =  0x1C
//GYRO_XOUT_H = 0x1D : GYRO_XOUT_L = 0x1E
//GYRO_YOUT_H = 0x1F : GYRO_YOUT_L = 0x20
//GYRO_ZOUT_H = 0x21 : GYRO_ZOUT_L = 0x22

// Different values that can be read in from the ITG
	short Temperature, X, Y, Z;

	short Biases[3];
	Find_ITG_Bias(Biases, &ITG3200);
	cout << "X bias: " << Biases[0] << "  Y bias: " << Biases[1] << "  Z bias: " << Biases[2] << endl;
	cout << endl;
	cout << "X\t" << "Y\t" << "Z\t" << "Temperature\t" << "Actual Temp?\t" << endl;

	while (1) {
//divide the gyro values by 14.375 to get degrees per second.

		SetData(data, 0x10, 0x30, &ITG3200);
// | == Bitwise or, is same thing as adding. 2 chars make a short
		X = (data[0x1d] << 8) | data[0x1e];
		Y = (data[0x1f] << 8) | data[0x20];
		Z = (data[0x21] << 8) | data[0x22];
		Temperature = (data[0x1B] << 8) | (data[0x1C]);
// Temperature Offset is -13200
// Temperature sensitivity is 280
// Temperature range is -30 to 85 c
// no idea if this temp suff is correct

		X -= Biases[0];
		Y -= Biases[1];
		Z -= Biases[2];

		cout << X << '\t';
		cout << Y << '\t';
		cout << Z << '\t';
//  printf("%x\t",Temperature);
		cout << Temperature << '\t' << '\t';

		Temperature += 13200;
		float RealTemperature = (float) Temperature / 280.0;	//Convert the offset to degree C
		RealTemperature += 35;	//Add 35 degrees C to compensate for the offset

		cout << RealTemperature << '\t';

		cout << endl;
		usleep(1000000);
	}

	return 0;
}

void FancyPrint(char* data, short start, short end) {
	char c = 0;
#ifdef debug
	if(!(end-start)) cout << "opps!" << endl;
#endif
	for (int i = start; i <= end; i++) {
		printf("%02x", data[i]);
		cout << " ";
		c++;
		if (c >= 16) {
			cout << endl;
			c = 0;
		}
	}
}

