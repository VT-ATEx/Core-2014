//Philip Neighbour 2/7/2015
// The ITG 3200 gyro
//sudo i2cdetect -r 1
//sudo i2cdump 1 0x69
#define Adress 0x69
#define Bus 1
#define Delay 10000 //in 10^(-6) seconds
#define biastime 3.1 * 1000000 // first termp is in 10^(-6) seconds
#define SampleRate 0.01 * 1000000 //int 10^(-6) seconds
#define Scale 1 //set to !0 for displacement scaling
#define Debug 0
#include "I2C.cpp"
#include "I2C.h"

#include <unistd.h>
#include <ctime>
#include <stdio.h>
#include <ctime>
#include <iostream>
#include <sys/time.h>

using std::cin;
using std::cout;
using std::endl;

// A function for debugging and printing an array of char by having 16 char per row
void FancyPrint(char* data, short start, short end);
//returns an array for correct bias values
void findBias(short* Biases);
void RawADCValuePrint();

// Prepares reading of a register
void prepread(int address) {
	static char* send = new
	char[1];
	send[0] = address - 3;
	usleep(Delay);
	static I2C
	cat(Bus);
	cat.startbus(Adress);
	cat.writebus(send);
}
void SetData(char* data, int StartRegister, int EndRegister) {
	static I2C
	cat(Bus);
	static bool run = 0;
	if (!run) {
		cat.startbus(Adress);
		run = 1;
	}

	prepread(StartRegister);
	usleep(Delay);
	cat.readbus(data + StartRegister, EndRegister - StartRegister);

}

// Sets Time_Seconds to be the Unix time in seconds, and Time_U_seconds to be intermediate us time (between 0 and 999999) 
void SetTime(long& Time_Seconds, long& Time_U_Seconds) {
// Used strut probably defined in #include <sys/time.h> or #include <unistd.h>
	static struct timeval time;
	gettimeofday(&time, NULL);
	Time_Seconds = time.tv_sec;
	Time_U_Seconds = time.tv_usec;
}

//35 c temperature ofset
int main() {
//0x1000 = 256
	short datasize = 0x100;
// array that will contain the register values read in from the sensor
	char* data = new char[datasize];
//TEMP_OUT_H =  0x1B : TEMP_OUT_L =  0x1C
//GYRO_XOUT_H = 0x1D : GYRO_XOUT_L = 0x1E
//GYRO_YOUT_H = 0x1F : GYRO_YOUT_L = 0x20
//GYRO_ZOUT_H = 0x21 : GYRO_ZOUT_L = 0x22
//  data = itgRead(itgAddress, GYRO_XOUT_H)<<8;

// Different values that can be read in from the ITG
	short Temperature, X, Y, Z;

//has x,y,z bias because the sensor is not perfect
// findBiases returns the Average biases of X,Y,Z by sampling them several times in the course of "biastime". biastime is user defined, and customizable  
	short biases[3];
	findBias(biases);

// stores the value of UNIX time (seconds since 1970 or something)

	 long LastTime_Seconds, LastTime_U_Seconds;
	 long Time_Seconds, Time_U_Seconds;
	long U_Time_Difference;

// << 20 is approximation of multiply by 1000000
	long PrintRate = (1 << 20); //the first num represents seconds

//	int SampleRate = 0.01 * (1 << 20); //int 10^(-6) seconds
//Faster sampling rates mean that there will be a greater displacement when added.


// Used later in while(1) loop. A faster sample means that the total integrated values will be greater, which needs to be corrected
long TimeScaler; 
// Checks how many times the while loop loops. used for TimeScaler
long TimesLooped = 0;


// Xs,Ys,Zs represent the total sum of value read in from the registers
	long Xs, Ys, Zs;
	Xs = 0;
	Ys = 0;
	Zs = 0;
	cout << "Xs\t" << "Ys\t" << "Zs\t" << "Temperature\t" << endl;
		// sets Time_Seconds to be Unix time (in seconds) and Time_U_Seconds to be difference between seconds in muicro seconds
		SetTime(Time_Seconds, Time_U_Seconds);

	LastTime_Seconds = Time_Seconds;
	LastTime_U_Seconds = Time_U_Seconds;

	// Used for scaling displacement
	float BaseSamplingPerSecond = 10;
// used in while loop
	float CurSamplesPerSecond;
	long double DisplacementScaler ;/* = CurSamplesPerSecond/BaseSamplingPerSecond;*/
	// Used for when displacement needs to be scaled
long TimeBeginn =Time_Seconds;
TimeBeginn =Time_Seconds;
	while (1) {
		SetData(data, 0x1d, 0x23);
		X = (data[0x1d] << 8) | data[0x1e];
		Y = (data[0x1f] << 8) | data[0x20];
		Z = (data[0x21] << 8) | data[0x22];

		X -= biases[0];
		Y -= biases[1];
		Z -= biases[2];

		Xs += X;
		Ys += Y;
		Zs += Z;

		SetTime(Time_Seconds, Time_U_Seconds);
		U_Time_Difference = ((long) (Time_Seconds - LastTime_Seconds) << 20) + (Time_U_Seconds - LastTime_U_Seconds);

		if (U_Time_Difference > PrintRate) {
			LastTime_Seconds = Time_Seconds;
			LastTime_U_Seconds = Time_U_Seconds;

			CurSamplesPerSecond = (float)TimesLooped/(Time_Seconds - TimeBeginn);

			DisplacementScaler = BaseSamplingPerSecond / CurSamplesPerSecond;
			#if (Debug==1)		
		cout << " Time_Seconds: " <<Time_Seconds << "  TimeBeginn:" << TimeBeginn<< endl;
			cout << "Looped: " << TimesLooped << " Difference: " << (Time_Seconds - TimeBeginn) << endl;
			cout << "samples per sec: " << CurSamplesPerSecond << endl;
			cout << "DisplacementScaler: " << DisplacementScaler << endl;
#endif
		
			# if (Scale)
						cout << (long) ((long double) Xs*DisplacementScaler) << '\t';
			cout << (long) ((long double) Ys*DisplacementScaler) << '\t';
			cout << (long) ((long double) Zs*DisplacementScaler) << '\t';
			#else

			
						cout << Xs << '\t';
			cout <<  Ys << '\t';
			cout <<  Zs << '\t';
			#endif
						cout << endl;

			
		}
		usleep(SampleRate);
		TimesLooped++;
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

void RawADCValuePrint() {
	short datasize = 0x100;
	char* data = new
	char[datasize];
//TEMP_OUT_H =  0x1B : TEMP_OUT_L =  0x1C
//GYRO_XOUT_H = 0x1D : GYRO_XOUT_L = 0x1E
//GYRO_YOUT_H = 0x1F : GYRO_YOUT_L = 0x20
//GYRO_ZOUT_H = 0x21 : GYRO_ZOUT_L = 0x22
//  data = itgRead(itgAddress, GYRO_XOUT_H)<<8;

	short Temperature, X, Y, Z;
	cout << "X\t" << "Y\t" << "Z\t" << "Temperature\t" << endl;

//has x,y,z bias
	short biases[3];
	findBias(biases);

	for (int i = 0; i + 1 /*< 1000*/; i++) {
		SetData(data, 0x10, 0x30);
//|| == or, is same thing as adding. 2 chars make a short
		Temperature = (data[0x1B] << 8) | (data[0x1C]);
		X = (data[0x1d] << 8) | data[0x1e];
		Y = (data[0x1f] << 8) | data[0x20];
		Z = (data[0x21] << 8) | data[0x22];

		X -= biases[0];
		Y -= biases[1];
		Z -= biases[2];

		cout << X << '\t';
		cout << Y << '\t';
		cout << Z << '\t';
//cout << Temperature << '\t';
		cout << endl;
//cout << "Temp is: " << (float)((int)Temp+35000.)/1000. << endl;

		Temperature = 999;
//FancyPrint(data,0x10,0xd0);

		usleep(Delay);
	}

}

void findBias(short* Biases) {
	short X, Y, Z;
//is divided later to find average bias
	long Xt, Yt, Zt;
	Xt = 0;
	Yt = 0;
	Zt = 0;
//	long biastime = 0.1 * 1000000; // in 10^(-6) seconds

	long TimesToLoop = biastime / Delay / 3;
	cout << "calibrating biases. wait ~" << biastime / 1000000 << " seconds" << endl;
	cout << "sampling " << TimesToLoop << " Times" << endl;

	static char* data = new
	char[0x40];
	for (int i = 0; i < TimesToLoop; i++) {
		SetData(data, 0x1b, 0x24);
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

