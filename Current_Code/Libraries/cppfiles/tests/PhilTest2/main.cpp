// compile with g++ -pthread [filename] -o [executable to create]
// sudo i2cdetect -r -y 1
// sudo i2cdump -y 1 address (0x77 , 0x69)

#define Debug 0
// Max time untill cutdown
#define Cutdown_Time 15
// Prevents cutdown from activating if it appears that the ballon has already poped. does this by seeing if the ballon has reatched an altitude where it is likly that the ballon may have poped
#define UsingPressureDoNotCutdown 0
// Balllon must reatch this pressure in order for the cutdown to NOT activate when it is at ground pressure (aka in a tree) 
#define MinReachPressure 50000
// if the MinReachPressure has been deceeded, and the pressure is greater than NoCutdownAtPressure and  UsingPressureDoNotCutdown is 1, the cutodwn will NOT activate
#define NoCutdownAtPressure 90000
// the minimum expected time in seconds for ballon to reach altitude
#define MinTime 3600
// GPIO pin numbers are different from pin numbers
#define CutDownPin 31
// time that gpio pin is on in 10^(-6) seconds
#define CutDownTime (30*1000000)
// set to !0 if device is to be used
#define using_BMP180	1 // Temperature and pressure sensor
#define using_ITG3200	1 // Angular Velocity Sensor
#define using_HMC5883L	1 // Magnetometer (orientation wrt magnetic filed and magnetic filed strength.)
#define using_LM73		1 // Low quality temperature sensor
#define using_LIS331	1 // Accelerometer

//Makes each file print a hex time since the flight started above each line
#define using_TimeStamps 1
#define WaitForThreadsClose 0
// Various i2c addresses of the sensors
#define ITG3200Address 0x69
#define BMP180Address 0x77
#define LIS331Address 0x19
#define HMC5883LAddress 0x1E
#define LM73Address 0x48

// Period untill thread retries acessing bus in micro seconds (10^(-6))
// if bus not currently avaliable, tries again in Retry_Period micro seconds
#define Retry_Period 10000

// Sample Periods for each sensor in micro seconds (10^(-6))
#define ITG_Sample_Period 100000
#define BMP_Sample_Period 100000
#define LIS331_Sample_Period 100000
#define HMC_Sample_Period 100000
#define LM73_Sample_Period 100000

// how many data samples untill file is closed and new file is created
#define ITG_Data_Untill_New_File 40
#define BMP_Data_Untill_New_File 40
#define LIS331_Data_Untill_New_File 40
#define HMC_Data_Untill_New_File 40
#define LM73_Data_Untill_New_File 40

#define ITG_Data_Directory "itg/"
#define BMP_Pressure_Data_Directory "bmpp/"
#define BMP_Temperature_Data_Directory "bmpt/"
#define LIS331_Data_Directory "lis/"
#define HMC_Data_Directory "hmc/"
#define LM73_Data_Directory "lm73/"
#define GeneralDir "generic/"

#include <iostream>
#include <pthread.h>
#include <unistd.h>
// #include <stdlib.h>
#include <stdio.h>
#include <ctime>
#include <fstream>
#include <math.h>

// for cutdown
//#include "GPIO.h"
#include "GPIO.cpp"

// Only devices that are being used need their stuff included
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

// used to determine when to activate cutdown
volatile long OutSidePressure = 10000000;
// gets set to the unix time at start of flight. used to decrease excessive stuff
int StartTime;
// when this is 0, all threads will eventually end
bool ContinueThreads=0x0f;
// different bits in it signify which thread(s) are finnished
volatile int ThreadFinnished=0;
// made smaller each time there is a new MinPressure
volatile int MinPressure = 100000;
#include "SensorThreads.cpp"
#include "ManageThreads.cpp"
using std::cout;
using std::endl;
using std::ofstream;

int main() {
FileWriteTest();
generic();
	GPIO TheCutDownPin(CutDownPin);
	TheCutDownPin.gpio_export();
	TheCutDownPin.gpio_set_dir(1);
	// have it output low
	TheCutDownPin.gpio_set_value(0);
enum SensorNum {
		ITG3200_Int, BMP180_Int, LIS331_Int, LM73_Int, HMC5883L_Int
	};
	// The integer which says which sensors are currently using the bus. Should only be one at a time.
	volatile int BusUsage = 0;
	volatile long i2c_bus_call_count = 0;
	ThreadPass PassToThreadStuff[5];
	pthread_t Sensors[5];


#if using_ITG3200
	ITG3200 itg(1, ITG3200Address);
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
	
	int Thread = 0;
	// pthread_create ( &pthread_t SomePthread, NULL, *FunctionToCall, &PointerReference);

#if using_ITG3200
	PassToThreadStuff[ITG3200_Int].BusUsage = &BusUsage;
	PassToThreadStuff[ITG3200_Int].BusUseMask = (1 << ITG3200_Int);
	PassToThreadStuff[ITG3200_Int].SomeClass = &itg;
	PassToThreadStuff[ITG3200_Int].i2c_bus_call_count = &i2c_bus_call_count;
	pthread_create(&Sensors[Thread++], NULL, *Manage_ITG3200, (void*) &PassToThreadStuff[ITG3200_Int]);
#endif
#if using_BMP180
	PassToThreadStuff[BMP180_Int].BusUsage = &BusUsage;
	PassToThreadStuff[BMP180_Int].BusUseMask = (1 << BMP180_Int);
	PassToThreadStuff[BMP180_Int].SomeClass = &bmp;
	PassToThreadStuff[BMP180_Int].i2c_bus_call_count = &i2c_bus_call_count;
	pthread_create(&Sensors[Thread++], NULL, *Manage_BMP180, (void*) &PassToThreadStuff[BMP180_Int]);
#endif
#if using_LIS331
	PassToThreadStuff[LIS331_Int].BusUsage = &BusUsage;
	PassToThreadStuff[LIS331_Int].BusUseMask = (1 << LIS331_Int);
	PassToThreadStuff[LIS331_Int].SomeClass = &lis331;
	PassToThreadStuff[LIS331_Int].i2c_bus_call_count = &i2c_bus_call_count;
	pthread_create(&Sensors[Thread++], NULL, *Manage_LIS331, (void*) &PassToThreadStuff[LIS331_Int]);
#endif
#if using_LM73
	PassToThreadStuff[LM73_Int].BusUsage = &BusUsage;
	PassToThreadStuff[LM73_Int].BusUseMask = (1 << LM73_Int);
	PassToThreadStuff[LM73_Int].SomeClass = &lm73;
	PassToThreadStuff[LM73_Int].i2c_bus_call_count = &i2c_bus_call_count;
	pthread_create(&Sensors[Thread++], NULL, *Manage_LM73, (void*) &PassToThreadStuff[LM73_Int]);
#endif
#if using_HMC5883L
	PassToThreadStuff[HMC5883L_Int].BusUsage = &BusUsage;
	PassToThreadStuff[HMC5883L_Int].BusUseMask = (1 << HMC5883L_Int);
	PassToThreadStuff[HMC5883L_Int].SomeClass = &hmc;
	PassToThreadStuff[HMC5883L_Int].i2c_bus_call_count = &i2c_bus_call_count;
	pthread_create(&Sensors[Thread++], NULL, *Manage_HMC, (void*) &PassToThreadStuff[HMC5883L_Int]);
#endif

	long StartTime = std::time(0);

	
		
		long Lasti2cCall = i2c_bus_call_count;
	long MinCutDownTime = MinTime + std::time(0);
	while (1) {
		if (((std::time(0)) > Cutdown_Time+StartTime) ){ 
	// Does not cut down if it is apparent that the ballon has reatched a high altitude and is now at a much lower altitude
	  			if ((UsingPressureDoNotCutdown)&&(MinPressure > MinReachPressure) && (NoCutdownAtPressure > OutSidePressure)) {
			cout << "Not cutting down due to pressure not met!" << endl;
			cout << "NoCutdownAtPressure: " << NoCutdownAtPressure << " > " <<  OutSidePressure << " :OutSidePressure"<< endl;
cout << "Min Pressure: " << MinPressure << " > " << MinReachPressure << endl;
			break;
			}
			else
				cout << "Cutdown Activating!" << endl;
				cout << "Outside: " << OutSidePressure<<endl;
				TheCutDownPin.gpio_set_value(1);
	usleep(CutDownTime);
	TheCutDownPin.gpio_set_value(0);
break;
		}


		// the buss is "frozen" for whatever reason nothing has happened in over a second
// attemt to unfreze the bus
		if (Lasti2cCall >= i2c_bus_call_count + 3 ) {
				cout << "Error Occored  Count: " << i2c_bus_call_count << endl;
				BusUsage = 0;
						Lasti2cCall = i2c_bus_call_count;
			
		}
// if there is no error yet, makes stuff get close to having an error
		else{	// used for finding when error has occored
		if(i2c_bus_call_count > Lasti2cCall){
		Lasti2cCall = i2c_bus_call_count;}
		else{
		// continues to increment the variable. The bus should be reading in data much faster than this can increment
		Lasti2cCall++;
		}}

		// */
	//	printf("\n\r-------\n\r %x \t %x\t %d\n\r---------", (int)std::time(0), (int)i2c_bus_call_count,BusUsage);
		usleep(1000000);
	}
	#if WaitForThreadsClose
	ContinueThreads = 0;
	cout << "ending..." << endl;
		usleep(1000000);
		const int ThreadFinnishedMask = 0 
		#if using_ITG3200
		| (1 << ITG3200_Int) 
		#endif
		#if using_BMP180
		| (1 << BMP180_Int) 
		#endif
		#if using_LIS331
		| (1<<LIS331_Int) 
		#endif
		#if using_LM73
		| (1 << LM73_Int) 
		#endif
		#if using_HMC5883L
		| (1<<HMC5883L_Int)
		#endif
		;

		cout << "Waiting for threads to close!" << endl;
while(		ThreadFinnished != ThreadFinnishedMask);//does nothing
		cout << "Okie Dokie!" << endl;
			void *status;
	#if using_ITG3200
	pthread_join(Sensors[ITG3200_Int], &status);
	#endif
	#if using_BMP180
	pthread_join(Sensors[BMP180_Int], &status);
		#endif
	#if using_LIS331
	pthread_join(Sensors[LIS331_Int], &status);
	#endif
	#if using_LM73
	pthread_join(Sensors[LM73_Int], &status);
	#endif
	#if using_HMC5883L
	pthread_join(Sensors[HMC5883L_Int], &status);
	#endif
	#endif
cout << "Program done!" << endl;
			
			
	return 0;
}
