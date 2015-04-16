// compile with g++ -pthread [filename] -o [executable to create]
// sudo i2cdetect -r -y 1
// sudo i2cdump -y 1 address (0x77 , 0x69)

#define OnBeagleBone 0
#define Debug 0
// Max time untill cutdown
#define Cutdown_Time 20
// derired cutdown altitude in meters
#define Cutdown_Altitude 28000
// Initial launch start altitude ~600 meters
#define Start_Altitude 600

// the minimum expected time in seconds for ballon to reach altitude
#define MinTime 3600

// set to !0 if device is to be used
#define using_BMP180	0
#define using_ITG3200	0
#define using_HMC5883L	0
#define using_LM73		0
#define using_LIS331	0

// Various i2c addresses of the sensors
#define ITG3200Address 0x69
#define BMP180Address 0x77
#define LIS331Address 0x19
#define HMC5883LAddress 0x1E
#define LM73Address 0x48

// Period untill thread retries acessing bus in micro seconds (10^(-6))
#define Retry_Period 10000

// Sample Periods for each sensor in micro seconds (10^(-6))
#define ITG_Sample_Period 100000
#define BMP_Sample_Period 100000
#define LIS331_Sample_Period 100000
#define HMC_Sample_Period 100000
#define LM73_Sample_Period 100000

// how many data samples untill file is closed and new file is created
#define ITG_Data_Untill_New_File 20
#define BMP_Data_Untill_New_File 20
#define LIS331_Data_Untill_New_File 20
#define HMC_Data_Untill_New_File 20
#define LM73_Data_Untill_New_File 20

#define ITG_Data_Directory "itg/"
#define BMP_Pressure_Data_Directory "bmpp/"
#define BMP_Temperature_Data_Directory "bmpt/"
#define LIS331_Data_Directory "lis/"
#define HMC_Data_Directory "hmc/"
#define LM73_Data_Directory "lm73/"

#include <iostream>
#include <pthread.h>
#include <unistd.h>
// #include <stdlib.h>
#include <stdio.h>
#include <ctime>
#include <fstream>
#include <math.h>

#if OnBeagleBone
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
#endif

using std::cout;
using std::endl;
using std:: ofstream;

// A struct that is passed to each sensor thread
struct ThreadPass {
	// An integer that tells which sensors are using the bus currently
	volatile int* BusUsage;
	// A mask that determines which part of BusUsage^^ the function is to mask with 1 when its using the bus
	int BusUseMask;
	// A passed class
	void* SomeClass;
	volatile long *i2c_bus_call_count;
};
// used to determine when to activate cutdown
volatile long OutSidePressure = 10000000;

#if using_BMP180
void TakeDataBMP(BMP_Presure_Temp* bmpz)
{	BMP_Presure_Temp& bmp = *bmpz;
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
void TakeDataLIS(LIS331* lis331z) {
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
void TakeDataLM73(LM73* lm73z) {
	LM73& lm73 = *lm73z;
	cout << "LM73 Temperature: " << lm73.getTemperature() << endl;
}
#endif 

#if using_HMC5883L
void TakeDataHMC5883L(HMC* hmcz) {
	HMC& hmc = *hmcz;
	Magnetics data = {0,0,0};
	data = hmc.getData();
	cout <<"HMC x,y,z: "<< data.x << ", " << data.y << ", " << data.z << endl;
}
#endif

inline void WriteIntsToFile(int ArraySize, char* Name, int* Array) {
	//	cout << "Writing!" << endl;
	//	cout << endl << endl;
	// cout << "New File Name: "<< Name << endl;
	std
	:: ofstream Writer;
	Writer.open(Name);
	for (int i = 0; i < ArraySize; i++) {
		char buffer[50];
		sprintf(buffer, "%d \n", Array[i]);
		Writer << buffer;
	}

	Writer.close();
}

void TakeDataBMP() {
	static int temperatures[BMP_Data_Untill_New_File + 20];
	static int pressures[BMP_Data_Untill_New_File + 20];

	static int FileNumber = 1; // For naming of files
	static int CurPointer = 0; 	// which part of the array is being looked at
	// Gets data
	static int temperature, pressure;
#if !OnBeagleBone
	temperatures[CurPointer] = temperature++;
	pressures[CurPointer] = pressure++;
#endif
	// Important: Used to determine when to cut down ballon!
	OutSidePressure = pressures[CurPointer];
	CurPointer++;
	// when there is enough data, it is written to a file and a new file is created
	if (CurPointer >= BMP_Data_Untill_New_File) {
		char TemperatureFileName[50];
		char PressureFileName[50];
		//	char BMP_TemperatureDir[] ="bmp/";

		char BMP_TemperatureDir[] = BMP_Temperature_Data_Directory;
		char BMP_Pressure_Dir[] = BMP_Pressure_Data_Directory;

		sprintf(TemperatureFileName, "%stemp%d.txt", BMP_TemperatureDir, FileNumber);
		sprintf(PressureFileName, "%spres%d.txt", BMP_Pressure_Dir, FileNumber);
		WriteIntsToFile(BMP_Data_Untill_New_File, TemperatureFileName, &temperatures[0]);
		WriteIntsToFile(BMP_Data_Untill_New_File, PressureFileName, &pressures[0]);

		CurPointer = 0;
		FileNumber++;
	}
}

void TakeDataITG() {
	static int x, y, z;
	#if !OnBeagleBone
	x++;
	y++;
	z++;
#endif
	static int AgularVelocity[3 * (ITG_Data_Untill_New_File + 20)];
	static int FileNumber = 1; // For naming of files
	static int CurPointer = 0; 	// which part of the array is being looked at
	char FileToCreateName[50];
	char dir[] = ITG_Data_Directory;
	AgularVelocity[CurPointer++] = x;
	AgularVelocity[CurPointer++] = y;
	AgularVelocity[CurPointer++] = z;
	if (CurPointer >= 3 * ITG_Data_Untill_New_File) {
		sprintf(FileToCreateName, "%sitg%d.txt", dir, FileNumber);
		WriteIntsToFile(3 * ITG_Data_Untill_New_File, FileToCreateName, &AgularVelocity[0]);

		CurPointer = 0;
		FileNumber++;
	}
}
void TakeDataLIS() {
	static int x, y, z;
	#if !OnBeagleBone
	x++;
	y++;
	z++;
#endif
	static int AccelerometerValues[3 * (LIS331_Data_Untill_New_File + 20)];
	char dir[] = LIS331_Data_Directory;
	char FileToCreateName[50];
	static int FileNumber = 1; // For naming of files
	static int CurPointer = 0; 	// which part of the array is being looked at

	AccelerometerValues[CurPointer++] = x;
	AccelerometerValues[CurPointer++] = y;
	AccelerometerValues[CurPointer++] = z;
	if (CurPointer >= 3 * LIS331_Data_Untill_New_File) {
		sprintf(FileToCreateName, "%slis%d.txt", dir, FileNumber);
		WriteIntsToFile(3 * LIS331_Data_Untill_New_File, FileToCreateName, &AccelerometerValues[0]);

		CurPointer = 0;
		FileNumber++;
	}

}
void TakeDataLM73() {
	static int temp = 0;
	temp++;
	static int Temperature[LM73_Data_Untill_New_File + 20];
	char dir[] = LM73_Data_Directory;
	char FileToCreateName[50];
	static int FileNumber = 1; // For naming of files
	static int CurPointer = 0; 	// which part of the array is being looked at

	Temperature[CurPointer++] = temp;

	if (CurPointer >= (LM73_Data_Untill_New_File)) {
		sprintf(FileToCreateName, "%slm%d.txt", dir, FileNumber);
		WriteIntsToFile(LM73_Data_Untill_New_File, FileToCreateName, &Temperature[0]);

		CurPointer = 0;
		FileNumber++;
	}
}
void TakeDataHMC5883L() {
	static int x, y, z;
	#if !OnBeagleBone
	x++;
	y++;
	z++;
#endif
	static int Magnetometer[3 * (LIS331_Data_Untill_New_File + 20)];
	char dir[] = HMC_Data_Directory;
	char FileToCreateName[50];
	static int FileNumber = 1; // For naming of files
	static int CurPointer = 0; 	// which part of the array is being looked at

	Magnetometer[CurPointer++] = x;
	Magnetometer[CurPointer++] = y;
	Magnetometer[CurPointer++] = z;

	if (CurPointer >= 3 * HMC_Data_Untill_New_File) {
		sprintf(FileToCreateName, "%shmc%d.txt", dir, FileNumber);
		WriteIntsToFile(3 * LIS331_Data_Untill_New_File, FileToCreateName, &Magnetometer[0]);

		CurPointer = 0;
		FileNumber++;
	}
}

/*
 // A struct that is passed to each sensor thread
 struct ThreadPass{
 // An integer that tells which sensors are using the bus currently
 int* BusUsage;
 // A mask that determines which part of BusUsage^^ the function is to mask with 1 when its using the bus
 int BusUseMask;
 // A passed class
 void* SomeClass;
 };
 */
// Handles each manage functions waiting around for the bus
inline void WaitForBus(int SampleRate, volatile int* BusUsage, int BusUseMask, int ReTryRate) {
	goto_SleepAgain: usleep(SampleRate);
	// it could be that the BusUsage for some reason might still have BusUseMask
	if (*BusUsage & BusUseMask)
		*BusUsage ^= BusUseMask;
	// while the I2C bus is being used, delays and sees if it's being used
	while (*BusUsage)
		usleep(ReTryRate);
	// tells everybody it's using the bus
	*BusUsage |= BusUseMask;
	// Checks to make sure nothing else is using the Bus
	if (BusUseMask ^ *BusUsage) { // if the bus is already in use, resets it, and tries all over again (this should be very unlikly)
		*BusUsage = BusUseMask ^ *BusUsage;
		goto goto_SleepAgain;
	}
	// Now the bus belongs to it
	usleep(100000);
}

	
	
void *Manage_ITG3200(void* Something) {
	ThreadPass & stuff = *(ThreadPass*) Something;
	ITG3200& itg = *(ITG3200*)stuff.SomeClass;
	int SampleRate = ITG_Sample_Period;
	int ReTryRate = Retry_Period;
	while (1) {
		WaitForBus(SampleRate, stuff.BusUsage, stuff.BusUseMask, ReTryRate);
		*stuff.i2c_bus_call_count += 1;
#if Debug
		cout << "ITG3200 In action!" << endl;
#endif
		TakeDataITG();

#if using_ITG3200
		TakeDataITG(&itg);
#endif
		usleep(1000);
		// finnished with i2cbus, resets its bit
		*stuff.BusUsage = stuff.BusUseMask ^ *stuff.BusUsage;
	}
	return NULL;
}

void *Manage_BMP180(void* Something) {
	ThreadPass & stuff = *(ThreadPass*) Something;
		BMP_Presure_Temp& bmp = *(BMP_Presure_Temp*)stuff.SomeClass;

	int SampleRate = BMP_Sample_Period;
	int ReTryRate = Retry_Period;
	while (1) {
		WaitForBus(SampleRate, stuff.BusUsage, stuff.BusUseMask, ReTryRate);
		*stuff.i2c_bus_call_count += 1;
#if Debug
		cout << "BMP180 In action!" << endl;
#endif
		TakeDataBMP();
#if using_BMP180
		TakeDataBMP(&bmp);
#endif
		usleep(1000);
		// finnished with i2cbus, resets its bit
		*stuff.BusUsage = stuff.BusUseMask ^ *stuff.BusUsage;
	}
}

void *Manage_LIS331(void* Something) {
	ThreadPass & stuff = *(ThreadPass*) Something;
			LIS331& lis331 = *(LIS331*)stuff.SomeClass;
	int SampleRate = LIS331_Sample_Period;
	int ReTryRate = Retry_Period;
	while (1) {
		WaitForBus(SampleRate, stuff.BusUsage, stuff.BusUseMask, ReTryRate);
		*stuff.i2c_bus_call_count += 1;
#if Debug
		cout << "LIS331 In action!" << endl;
#endif
		TakeDataLIS();
#if using_LIS331
		TakeDataLIS(&lis331);
#endif
		usleep(1000);
		// finnished with i2cbus, resets its bit
		*stuff.BusUsage = stuff.BusUseMask ^ *stuff.BusUsage;
		//	cout << "New Mask: " << *stuff.BusUsage << endl;
	}
	return NULL;
}

	
	
void *Manage_LM73(void* Something) {

	ThreadPass & stuff = *(ThreadPass*) Something;
LM73& lm73 = *(LM73*)stuff.SomeClass;
	int SampleRate = LM73_Sample_Period;
	int ReTryRate = Retry_Period;
	while (1) {
		WaitForBus(SampleRate, stuff.BusUsage, stuff.BusUseMask, ReTryRate);
#if Debug
		cout << "LM73 In action!" << endl;
#endif
		*stuff.i2c_bus_call_count += 1;

		TakeDataLM73();
#if using_LM73
		TakeDataLM73(&lm73);
#endif
		usleep(1000);
		// finnished with i2cbus, resets its bit
		*stuff.BusUsage = stuff.BusUseMask ^ *stuff.BusUsage;
	}
	return NULL;
}

void *Manage_HMC(void* Something) {
	ThreadPass & stuff = *(ThreadPass*) Something;
	HMC& hmc = *(HMC*)stuff.SomeClass;
	int SampleRate = HMC_Sample_Period;
	int ReTryRate = Retry_Period;
	while (1) {
		WaitForBus(SampleRate, stuff.BusUsage, stuff.BusUseMask, ReTryRate);
		*stuff.i2c_bus_call_count += 1;
#if Debug
		cout << "HMC In action!" << endl;
#endif
		TakeDataHMC5883L();
#if using_HMC5883L
		TakeDataHMC5883L(&hmc);
#endif
		usleep(1000);
		// finnished with i2cbus, resets its bit
		*stuff.BusUsage = stuff.BusUseMask ^ *stuff.BusUsage;
	}
	return NULL;
}

int main() {
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
	enum SensorNum {
		ITG3200_Int, BMP180_Int, LIS331_Int, LM73_Int, HMC5883L_Int
	};
	// The integer which says which sensors are currently using the bus. Should only be one at a time.
	volatile int BusUsage = 0;
	volatile long i2c_bus_call_count = 0;
	ThreadPass PassToThreadStuff[5];
	pthread_t Sensors[5];
	/*
	 // A struct that is passed to each sensor thread
	 struct ThreadPass{
	 // An integer that tells which sensors are using the bus currently
	 int* BusUsage;
	 // A mask that determines which part of BusUsage^^ the function is to mask with 1 when its using the bus
	 int BusUseMask;
	 // A passed class
	 void* SomeClass;
	 };
	 */

	int itg, bmp, lis331, lm73, hmc;

	PassToThreadStuff[ITG3200_Int].BusUsage = &BusUsage;
	PassToThreadStuff[ITG3200_Int].BusUseMask = (1 << ITG3200_Int);
	PassToThreadStuff[ITG3200_Int].SomeClass = &itg;
	PassToThreadStuff[ITG3200_Int].i2c_bus_call_count = &i2c_bus_call_count;

	PassToThreadStuff[BMP180_Int].BusUsage = &BusUsage;
	PassToThreadStuff[BMP180_Int].BusUseMask = (1 << BMP180_Int);
	PassToThreadStuff[BMP180_Int].SomeClass = &bmp;
	PassToThreadStuff[BMP180_Int].i2c_bus_call_count = &i2c_bus_call_count;

	PassToThreadStuff[LIS331_Int].BusUsage = &BusUsage;
	PassToThreadStuff[LIS331_Int].BusUseMask = (1 << LIS331_Int);
	PassToThreadStuff[LIS331_Int].SomeClass = &lis331;
	PassToThreadStuff[LIS331_Int].i2c_bus_call_count = &i2c_bus_call_count;

	PassToThreadStuff[LM73_Int].BusUsage = &BusUsage;
	PassToThreadStuff[LM73_Int].BusUseMask = (1 << LM73_Int);
	PassToThreadStuff[LM73_Int].SomeClass = &lm73;
	PassToThreadStuff[LM73_Int].i2c_bus_call_count = &i2c_bus_call_count;

	PassToThreadStuff[HMC5883L_Int].BusUsage = &BusUsage;
	PassToThreadStuff[HMC5883L_Int].BusUseMask = (1 << HMC5883L_Int);
	PassToThreadStuff[HMC5883L_Int].SomeClass = &hmc;
	PassToThreadStuff[HMC5883L_Int].i2c_bus_call_count = &i2c_bus_call_count;

	int Thread = 0;
	// pthread_create ( &pthread_t SomePthread, NULL, *FunctionToCall, &PointerReference); 
	pthread_create(&Sensors[Thread++], NULL, *Manage_ITG3200, (void*) &PassToThreadStuff[ITG3200_Int]);
	pthread_create(&Sensors[Thread++], NULL, *Manage_BMP180, (void*) &PassToThreadStuff[BMP180_Int]);
	pthread_create(&Sensors[Thread++], NULL, *Manage_LIS331, (void*) &PassToThreadStuff[LIS331_Int]);
	pthread_create(&Sensors[Thread++], NULL, *Manage_LM73, (void*) &PassToThreadStuff[LM73_Int]);
	pthread_create(&Sensors[Thread++], NULL, *Manage_HMC, (void*) &PassToThreadStuff[HMC5883L_Int]);

	long StartTime = std::time(0);
	// waits untill there are somewhat accurate pressure measurements
	while(OutSidePressure > 130000)
				usleep(1000000);
// $p = p_o \exp\left(-frac{h}{h_o} \right) $
			// $p_c = p_o \exp \left(-h_c \log \left( \frac{p_o}{p_B} \right) / h_b \right)$
			// mathematicall calculates the desired pressure that the cutdown should activate at based on current atmospheric contidion and launch location
			OutSidePressure = 94000;
	long CutdownPressure = 101000 * exp(-(double)Cutdown_Altitude * log((double)101000/(double)OutSidePressure)/Start_Altitude);
	cout << "Cutdown Pressure at " << CutdownPressure << " pa" << endl;
		long Lasti2cCall = i2c_bus_call_count;
// raised to prevent cutdown if pressure is unreasonable
	char NoPressureError = 0x0f;
	long MinCutDownTime = MinTime + std
	::time(0);
	while (1) {
		if (((std::time(0)) > Cutdown_Time+StartTime) || (NoPressureError &&(CutdownPressure > OutSidePressure))) { // activates cutdown
			if (std::time(0) < MinCutDownTime && (CutdownPressure > OutSidePressure))
				NoPressureError = 0;
			if (std::time(0) > StartTime+MinCutDownTime) {
				cout << "Cutdown Activating!" << endl;
				break;
			}
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
		printf("\n\r-------\n\r %x \t %x\t %d\n\r---------", std::time(0), i2c_bus_call_count,BusUsage);
		usleep(1000000);
	}
		
	return 0;
}
