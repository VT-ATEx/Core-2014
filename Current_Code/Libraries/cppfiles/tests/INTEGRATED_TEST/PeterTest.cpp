/*Peter Morrissey
 *2015.2.15
 *e-mail pfm1995[at]vt[dot]edu with questions
*/

//Please comment out/add any header files necessary
#include "BMP180.h"
//#include "ChipCap2.h"
#include "hmcP.h"
#include "ITG3200.h"
#include "LIS331.h"
#include "LM73.h"
//#include "RTC.h"
#include "cutdown.h"
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <ctime>

//Defines the various addresses and busses the sensors are on
//Configuration-dependant, change as needed
//TODO: Check busses and adresses, change as necessary
#define NUMTHRDS 6
#define LM73_BUS 2
#define LM73_ADDRESS 0x49
#define HMC5883L_BUS 2
#define HMC5883L_ADDRESS 0x3C
//#define CHIPCAP2_BUS 2
//#define CHIPCAP2_ADDRESS 0x28
#define LIS331_BUS 2
#define LIS331_ADDRESS 0x19
#define BMP180_BUS 2
#define BMP180_ADDRESS 0x77
#define ITG3200_ADDRESS 0x69
#define ITG3200_BUS 2
#define CUTDOWN_PIN 13
#define TIME_TIL_CUTDOWN 9000

using namespace std;

//Thread control variables
//pthread is a way of stopping two sensors from trying to access the 
//same i2c bus at the same time.
pthread_t callThd[NUMTHRDS];
pthread_mutex_t mutexI2C1;
pthread_mutex_t mutexI2C2;
pthread_mutex_t mutexI2C3;

time_t launchtime;

//Functions to create threads and retrieve data
//TODO: Please comment out any sensors we are not using

void *LM73_retrieve(void *arg) {
	//Creates output stream
	ofstream lm73_data;
	//Creates sensor object
	LM73 lm73(LM73_BUS, LM73_ADDRESS);
	//Runs forever
	while(1) {
		//Opens or creates file for data output
		lm73_data.open ("lm73data.csv");

		//Locks bus so only this thread can access it
		pthread_mutex_lock (&mutexI2C2);
		//Retrieves data
		lm73_data << lm73.getTemperature();
		//Closes bus
		lm73.close();
		//Unlocks bus
		pthread_mutex_unlock (&mutexI2C2);

		lm73_data << "\n";
		//CLoses data file
		lm73_data.close();
	}
	return 0;
}

void *LIS331_retrieve(void *arg) {
	ofstream lis331_data;
	LIS331  lis331(LIS331_BUS, LIS331_ADDRESS);

	pthread_mutex_lock (&mutexI2C2);
	lis331.Configure(100, 12);
	lis331.setpollrate(1000);
	lis331.setdatapoints(100);
	pthread_mutex_unlock (&mutexI2C2);

	while(1) {
		lis331_data.open ("lis331data.csv");
		pthread_mutex_lock (&mutexI2C2);
		lis331.start_update();
		lis331_data << lis331.getXvalue();
		lis331_data << ",";
		lis331_data << lis331.getYvalue();
		lis331_data << ",";
		lis331_data << lis331.getZvalue();
		lis331_data << ",";
		lis331_data << "\n";
		lis331.closer();
		pthread_mutex_unlock(&mutexI2C2);
		lis331_data.close();
	}
	return 0;
}

void *ITG3200_retrieve(void *arg) {
	ofstream itg3200_data;
	short *itg3200_values;

	while(1) {
		itg3200_data.open ("itg3200data.csv");
	
		pthread_mutex_lock (&mutexI2C2);
		ITG3200 itg(ITG3200_BUS, ITG3200_ADDRESS);
		itg3200_values = itg.Get_XYZT();
		for (int i = 0; i < 3; i++) {
			itg3200_data << itg3200_values[i];
			itg3200_data << ",";
		}
		itg.CloseITG();
		pthread_mutex_unlock (&mutexI2C2);
	
		itg3200_data << "\n";
		itg3200_data.close();
	}
	return 0;
}

void *HMC5883L_retrieve(void *arg) {
	ofstream hmc5883l_data;
	Magnetics hmc5883l_values;

	while(1) {
		hmc5883l_data.open("hmc5883ldata.csv");

		pthread_mutex_lock (&mutexI2C2);
		HMC hmc(HMC5883L_BUS, HMC5883L_ADDRESS);
		hmc5883l_values = hmc.getData();
		hmc.close();
		pthread_mutex_unlock (&mutexI2C2);
	
		hmc5883l_data << hmc5883l_values.x;
		hmc5883l_data << ",";
		hmc5883l_data << hmc5883l_values.y;
		hmc5883l_data << ",";
		hmc5883l_data << hmc5883l_values.z;
		hmc5883l_data << ",\n";
		hmc5883l_data.close();
	}
	return 0;
}
/*
void *ChipCap2_retrieve(void *arg) {
	ofstream chipcap2_data;
	
	while(1) {
		chipcap2_data.open("chipcap2data.csv");

		pthread_mutex_lock (&mutexI2C2);
		ChipCap2 cc2(CHIPCAP2_BUS, CHIPCAP2_ADDRESS);
		chipcap2_data << cc2.getHumidity() + "," + 
			cc2.getTemperature + "\n";
		cc2.close();
		pthread_mutex_unlock (&mutexI2C2);

		chipcap2_data.close();
	}
	return 0;
}
*/
void *BMP180_retrieve(void *arg) {
	ofstream bmp180_data;
	time_t ctime;
	int foo;
	
	while(1) {
		bmp180_data.open("bmp180data.csv");

		pthread_mutex_lock (&mutexI2C2);
		BMP_Presure_Temp bmp(BMP180_BUS, BMP180_ADDRESS);
		foo = bmp.GetPressure();
		if (foo >= 100000 || foo <= 0) {
			bmp180_data << "BMP has failed, aborting!";
			return 0;
		}
		if (foo <= 8000 && (time(&ctime) -
			launchtime) >= TIME_TIL_CUTDOWN) {
			activate_cutdown(CUTDOWN_PIN);
		}
		bmp180_data << foo;
		bmp180_data << ",";
		bmp180_data << bmp.GetTemp();
		bmp180_data << ",\n";
		bmp.CloseBMP();
		pthread_mutex_unlock (&mutexI2C2);
		
		bmp180_data.close();
	}
	return 0;
}

int main (int argc, char *argv[]) {
	//Creates a thread attribute
	pthread_attr_t attr;
	void *status;
	time(&launchtime);

	//Initializes thread blockers. These stop the threads from
	//trying to access the same bus
	pthread_mutex_init(&mutexI2C1, NULL);
	pthread_mutex_init(&mutexI2C2, NULL);
	pthread_mutex_init(&mutexI2C3, NULL);

	//Creates the threads
	//TODO: Add/comment out any sensors we add or remove. Remember 
	//to change the index numbers (the ones in []).
	pthread_create(&callThd[0], NULL, LM73_retrieve, (void *)NULL);	
	pthread_create(&callThd[1], NULL, LIS331_retrieve, (void *)NULL);
	pthread_create(&callThd[2], NULL, ITG3200_retrieve, (void *)NULL);
	pthread_create(&callThd[3], NULL, HMC5883L_retrieve, (void *)NULL);
	//pthread_create(&callThd[4], &attr, ChipCap2_retrieve, (void *)i);
	pthread_create(&callThd[4], NULL, BMP180_retrieve, (void *)NULL);

	//Collects the thread end condition if the thread has ended
	//Otherwise, program pauses here until thread has ended
	//NB: Currently threads never stop running, this is easy to change if we
	//need to change it. Just change the while loops.
	pthread_join(callThd[0], &status);
	pthread_join(callThd[1], &status);
	pthread_join(callThd[2], &status);
	pthread_join(callThd[3], &status);
	pthread_join(callThd[4], &status);
	//pthread_join(callThd[5], &status);

	//Destroys the thread blockers
	pthread_mutex_destroy(&mutexI2C1);
	pthread_mutex_destroy(&mutexI2C2);
	pthread_mutex_destroy(&mutexI2C3);
	//Closes any remaining threads
	pthread_exit(NULL);
}
