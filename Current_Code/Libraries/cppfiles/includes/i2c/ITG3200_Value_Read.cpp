//Philip Neighbour 2/13/2015
// Code for the ITG3200 gyro (measures angular velovity) for the Beagle Bone Black
// Some commands while on Beagle bone:
//sudo i2cdetect -r 1
//sudo i2cdump 1 0x69

#define Adress 0x69 // The I2C address that ITG is on
#define Bus 1 // The I2C bus that the ITG is on
#define Delay 10000 //Minimum delay for reading sensors. In 10^(-6) seconds
#include "I2C.cpp"
#include "I2C.h"

#include <unistd.h>
#include <iostream>

using std::cin;
using std::cout;
using std::endl;


// A function for debugging and printing an array of char by having 16 char per row
void FancyPrint(char* data, short start, short end);
void RawADCValuePrint();
// Function that prepares the reading of a certian register address. 
void prepread(int address, I2C* Some_I2C_Class);
// sets data from the ITG to data[] , an array of type char. Elements in data that are changed correspond to the register values
void SetData(char* data, int StartRegister, int EndRegister, I2C* Some_I2C_Class);
//returns an array for correct bias values
void Find_ITG_Bias(short* Biases, I2C* Some_I2C_Class);
// writes 1 byte to a certian register so that values can then be read sequentially from that register
void Write_1_To_Register(char registerr, char Value_To_Write, I2C* Some_I2C_Class);


I2C* ITG3200;

void SetupITG3200() 
{
//	I2C ITG3200(Bus);
//	ITG3200.startbus(Adress);

ITG3200 = new I2C(Bus);
ITG3200->startbus(Adress);

//set register 0x16 to 0x03
	Write_1_To_Register(0x16, 0x03, ITG3200);
}


// returns an array 4*short that contains the raw ADC values. return[0] == x , return [1] == y , return[2] == z , reutrn[3] == temperature. returnd values correspond to rotational motion. Biases should be corrected. 
// Note that there are a ADC value corresponds to 1/14.375 degrees per second 
short* Get_XYZT()
{
// array that will contain the register values read in from the sensor. I2C device has max 256 registers
static char* data = new char[0x100];

SetData(data, 0x10, 0x30, ITG3200);

static short *values[4];
short& X = values[0];
short& Y = values[1];
short& Z = values[2];
short& Temperature = values[3];

// | == Bitwise or, is same thing as adding. 2 chars make a short
X = (data[0x1d] << 8) | data[0x1e];
Y = (data[0x1f] << 8) | data[0x20];
Z = (data[0x21] << 8) | data[0x22];
Temperature = (data[0x1B] << 8) | (data[0x1C]);
return values;
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


