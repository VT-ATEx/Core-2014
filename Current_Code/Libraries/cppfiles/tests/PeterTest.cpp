/*Peter Morrissey
 *2015.2.15
 *e-mail pfm1995[at]vt[dot]edu with questions
*/

#include "BMP180.h"
#include "ChipCap2.h"
#include "hmc.h"
#include "ITG3200_Value_Read.h"
#include "LIS331.h"
#include "LM73.h"
#include "RTC.h"
#include <pthread.h>
#include <iostream>

//Defines the various addresses and busses the sensors are on
// COnfiguration-dependant, change as needed
#define NUMTHRDS 6
#define LM73_BUS 1
#define LM73_ADDRESS 0x49
#define HMC5883L_BUS 3
#define HMC5993L_ADDRESS NULL
#define CHIPCAP2_BUS 2
#define CHIPCAP2_ADDRESS 0x28
#define LIS331_BUS 2
#define LIS331_ADDRESS 0x19

//Thread control variables
pthread_t callThd[NUMTHRDS];
pthread_mutex_t mutexI2C1;
pthread_mutex_t mutexI2C2;
pthread_mutex_t mutexI2C3;

//Functions to create threads and retrieve data
void *LM73_retrieve(void *arg) {
	//Creates output stream
	ostream lm73_data;
	//Creates sensor object
	LM73 lm73(LM73_BUS, LM73_ADDRESS);

	while(1) {
		//Opens or creates file for data output
		lm73_data.open ("lm73data.csv");

		//Locks bus so only this thread can access it
		pthread_mutex_lock (&mutexI2C1);
		//Retrieves data
		lm73_data << lm73.getTemperature();
		//Closes bus
		lm73.close();
		//Unlocks bus
		pthread_mutex_unlock (&mutexI2C1);

		lm73_data << "\n";
		//CLoses data file
		lm73_data.close();
	}
	return 0;
}

void *LIS331_retrieve(void *arg) {
	ostream lis331_data;
	LIS331  lis331(LIS331_BUS, LIS331_ADDRESS);

	pthread_mutex_lock (&mutexI2C2);
	lis331.Configure(100, 12)
	lis331.setpollrate(1000);
	lis331.setdatapoints(100);
	pthread_mutex_unlock (&mutexI2C2);

	while(1) {
		lis331_data.open ("lis331data.csv");
		pthread_mutex_lock (&mutexI2C2);
		lis331.start_update();
		lis331_data << lis331.getXvalue() + "," +
			lis331.getYvalue + "," + lis331.getZvalue +
			"\n";
		lis331.closer();
		pthread_mutex_unlock();
		lis331_data.close();
	} 
}

void *ITG3200_retrieve(void *arg) {
	ostream itg3200_data;
	short *itg3200_values;

	while(1) {
		itg3200_data.open ("itg3200data.csv");
	
		pthread_mutex_lock (&mutexI2C2);
		//TODO: Re-write ITG3200 code to include classes and not
		// auto-define bus and address
		SetupITG3200();	
		itg3200_values = Get_XYZT();
		for (int i = 0; i < 3; i++) {
			itg3200_data << *(itg3200_values + i) + ",";
		}
		//TODO: ITG3200 Code needs closing function
		pthread_mutex_unlock (&mutexI2C2);
	
		itg3200_data << "\n";
		itg3200_data.close();
	}
	return 0;
}

void *HMC5883L_retrieve(void *arg) {
	ostream hmc5883l_data;
	Magnetics hmc5883l_values;

	while(1) {
		hmc5883l_data.open("hmc5883ldata.csv");

		pthread_mutex_lock (&mutexI2C3);
		HMC hmc(HMC5883L_BUS, HMC_5883L_ADDRESS);
		hmc5883l_values = hmc.getData();
		hmc.close();
		pthread_mutex_unlock (&mutexI2C3);
	
		hmc5883l_data << hmc5883l_values.x + "," + 
			hmc5883l_values.y + "," + hmc5883l_values.z + 
			"\n";
		hmc5883l_data.close();
	}
	return 0;
}

void *ChipCap2_retrieve(void *arg) {
	ostream chipcap2_data;
	
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

void *BMP180_retrieve(void *arg) {
	ostream bmp180_data;
	bmp180_data.open("bmp180data.csv");

	pthread_mutex_lock (&mutexI2C1);
	//TODO: Talk to Phillip about code
}

//TODO: Write loop into sub-threads, not main
int main (int argc, char *argv[]) {
	//Creates a thread attribute
	pthread_attr_t attr;
	int i = 0;
	void *status;

	//Initializes thread blockers. These stop the threads from
	//trying to access the same bus
	pthread_mutex_init(&mutexI2C1, NULL);
	pthread_mutex_init(&mutexI2C2, NULL);
	pthread_mutex_init(&mutexI2C3, NULL);

	//Initializes the attribute and defines it as 'joinable'
	//This allows me to read the threads' end states
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHRED_CREATE_JOINABLE);

	//Creates the threads
	pthread_create(&callThd[0], &attr, LM73_retrieve, (void *)i);	
	pthread_create(&callThd[1], &attr, LIS331_retrieve, (void *)i);
	pthread_create(&callThd[2], &attr, ITG3200_retrieve, (void *)i);
	pthread_create(&callThd[3], &attr, HMC5883L_retrieve, (void *)i);
	pthread_create(&callThd[4], &attr, ChipCap2_retrieve, (void *)i);
	pthread_create(&callThd[5], &attr, BMP180_retrieve, (void *)i);

	//Destroys the thread attribute
	pthread_attr_destroy(&attr);
	
	//Calls the thread end condition if the thread has ended
	//Otherwise, program pauses here until thread has ended
	pthread_join(callThd[0], &status);
	pthread_join(callThd[1], &status);
	pthread_join(callThd[2], &status);
	pthread_join(callThd[3], &status);
	pthread_join(callThd[4], &status);
	pthread_join(callThd[5], &status);

	//Destroys the thread blockers
	pthread_mutex_destroy(&mutexI2C1);
	pthread_mutex_destroy(&mutexI2C2);
	pthread_mutex_destroy(&mutexI2C3);
	//Closes any remaining threads
	pthread_exit(NULL);
}