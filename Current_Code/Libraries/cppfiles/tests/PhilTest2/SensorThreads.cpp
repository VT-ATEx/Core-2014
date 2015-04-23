// A struct that is passed to each sensor thread
struct ThreadPass {
	// An integer that tells which sensors are using the bus currently
	volatile int* BusUsage;
	// A mask that determines which part of BusUsage^^ the function is to mask with 1 when its using the bus
	int BusUseMask;
	// A passed class
	void* SomeClass;
	volatile long* i2c_bus_call_count;
};

inline void WriteIntsToFile(int ArraySize, char* Name, int* Array) {
	//	cout << "Writing!" << endl;
	//	cout << endl << endl;
	 cout << "New File Name: "<< Name << endl;
	std::ofstream Writer;
	Writer.open(Name);
	for (int i = 0; i < ArraySize; i++) {
		char buffer[50];
		sprintf(buffer, "%d \n", Array[i]);
		Writer << buffer;
	}

	Writer.close();
}

inline void WriteFloatsToFile(int ArraySize, char* Name, float* Array) {
	//	cout << "Writing!" << endl;
	//	cout << endl << endl;
	 cout << "New File Name: "<< Name << endl;
	std::ofstream Writer;
	Writer.open(Name);
	for (int i = 0; i < ArraySize; i++) {
		char buffer[50];
		sprintf(buffer, "%f \n", Array[i]);
		Writer << buffer;
	}
	Writer.close();
}
//for the LM73
inline void WriteFloatsToFile2DecimalOnly(int ArraySize, char* Name, float* Array) {
	//	cout << "Writing!" << endl;
	//	cout << endl << endl;
	 cout << "New File Name: "<< Name << endl;
	std::ofstream Writer;
	Writer.open(Name);
	for (int i = 0; i < ArraySize; i++) {
		char buffer[50];
		sprintf(buffer, "%2f \n", Array[i]);
		int loc=0;
		while(loc < 14){
		if(buffer[loc++] == '.')
		break;
		}
		buffer[loc+2] = '\n';
				buffer[loc+3] = '\0';
//	*/
	Writer << buffer;
	}
	Writer.close();
}


inline int TimeSinceStart(){
return std::time(0)-StartTime;
}

#if using_BMP180
void TakeDataBMP(BMP_Presure_Temp* bmpz, volatile int& BusUsage, const int& BusUseMask) {
	static int temperatures[BMP_Data_Untill_New_File + 20];
	static int pressures[BMP_Data_Untill_New_File + 20];
	int Times[BMP_Data_Untill_New_File + 20];
	
	static int FileNumber = 1; // For naming of files
	static int CurPointer = 0; 	// which part of the array is being looked at

	BMP_Presure_Temp& bmp = *bmpz;
	bmp.StartBMP();

	temperatures[CurPointer] = bmp.GetTemp();
	pressures[CurPointer] = bmp.GetPressure();
// Done using i2C bus, frees up bus
				BusUsage = (~(BusUseMask) &  BusUsage);
	if(pressures[CurPointer] < MinPressure)
MinPressure = 	pressures[CurPointer];

Times[CurPointer] = TimeSinceStart();
	// Important: Used to determine when to not cutdown ballon!
	OutSidePressure = pressures[CurPointer];
	CurPointer++;
	// when there is enough data, it is written to a file and a new file is created
	if (CurPointer >= BMP_Data_Untill_New_File) {
		char TemperatureFileName[50];
		char PressureFileName[50];
		char TimeTemperatureFileName[50];
		char TimePressureFileName[50];
		//	char BMP_TemperatureDir[] ="bmp/";

		char BMP_TemperatureDir[] = BMP_Temperature_Data_Directory;
		char BMP_Pressure_Dir[] = BMP_Pressure_Data_Directory;
		char BMP_TimePressureDir[] = BMP_Pressure_Data_Directory;
		char BMP_TimeTemeratureDir[] = BMP_Temperature_Data_Directory;
				
		sprintf(TemperatureFileName, "%stemp%d.txt", BMP_TemperatureDir, FileNumber);
		sprintf(PressureFileName, "%spres%d.txt", BMP_Pressure_Dir, FileNumber);
		
		sprintf(TimeTemperatureFileName, "%stempTime%d.txt", BMP_TemperatureDir, FileNumber);
		sprintf(TimePressureFileName, "%spresTime%d.txt", BMP_Pressure_Dir, FileNumber);
		
		WriteIntsToFile(BMP_Data_Untill_New_File, TemperatureFileName, &temperatures[0]);
		WriteIntsToFile(BMP_Data_Untill_New_File, PressureFileName, &pressures[0]);

		WriteIntsToFile(BMP_Data_Untill_New_File, TimeTemperatureFileName, &Times[0]);
		WriteIntsToFile(BMP_Data_Untill_New_File, TimePressureFileName, &Times[0]);

		CurPointer = 0;
		FileNumber++;
	}
}
#endif

#if using_ITG3200		
void TakeDataITG(ITG3200* itgz, volatile int& BusUsage, const int& BusUseMask) {

	int AgularVelocity[3 * (ITG_Data_Untill_New_File + 20)];
	int Times[(ITG_Data_Untill_New_File + 20)];
	
	static int FileNumber = 1; // For naming of files
	static int CurPointer = 0; 	// which part of the array is being looked at
	char FileToCreateName[50];
	char dir[] = ITG_Data_Directory;

	ITG3200& itg = *itgz;

	static char ITGBiasesNotFound = 0;
	itg.StartITG();
	if (ITGBiasesNotFound) {
		short* biases = itg.Find_ITG_Bias(3);
		cout << "X \t Y \t Z biases" << endl;
		cout << biases[0] << "\t" << biases[1] << "\t" << biases[2] << endl;
		ITGBiasesNotFound = 0;
	}
	short* values = itg.Get_XYZT();
	// Done using i2C bus, frees up bus
				BusUsage = (~(BusUseMask) &  BusUsage);
	//cout << "X \t Y \t Z" << endl;
//	printf("ITG X,Y,Z: %5d %5d %5d\n", values[0],values[1],values[2]);

	Times[CurPointer/3] = TimeSinceStart();
	AgularVelocity[CurPointer++] = values[0];
	AgularVelocity[CurPointer++] = values[1];
	AgularVelocity[CurPointer++] = values[2];
	
	if (CurPointer >= 3 * ITG_Data_Untill_New_File) {
		sprintf(FileToCreateName, "%sitg%d.txt", dir, FileNumber);
		WriteIntsToFile(3 * ITG_Data_Untill_New_File, FileToCreateName, &AgularVelocity[0]);
	sprintf(FileToCreateName, "%sitgTime%d.txt", dir, FileNumber);
		WriteIntsToFile(ITG_Data_Untill_New_File,FileToCreateName ,&Times[0]);
		CurPointer = 0;
		FileNumber++;
	}
}
#endif

#if using_LIS331
void TakeDataLIS(LIS331* lis331z, volatile int& BusUsage, const int& BusUseMask) {
	static float x, y, z;
	LIS331& lis331 = *lis331z;
	usleep(10000);
	x= lis331.getXvalue();
	usleep(10000);
	y = lis331.getYvalue();
	usleep(10000);
	z=lis331.getZvalue();
// Done using i2C bus, frees up bus
				BusUsage = (~(BusUseMask) &  BusUsage);
float AccelerometerValues[3 * (LIS331_Data_Untill_New_File + 20)];
int Times[(LIS331_Data_Untill_New_File + 20)];

	char dir[] = LIS331_Data_Directory;
	char FileToCreateName[50];
	static int FileNumber = 1; // For naming of files
	static int CurPointer = 0; 	// which part of the array is being looked at
 // printf("X: %f\n\rY: %f\n\rZ: %f\n\r",*(int*)&x,*(int*)&y,*(int*)&z);
Times[CurPointer/3] = TimeSinceStart();
	AccelerometerValues[CurPointer++] = x;
	AccelerometerValues[CurPointer++] = y;
	AccelerometerValues[CurPointer++] = z;
	if (CurPointer >= 3 * LIS331_Data_Untill_New_File) {
		sprintf(FileToCreateName, "%slis%d.txt", dir, FileNumber);
		WriteFloatsToFile(3 * LIS331_Data_Untill_New_File, FileToCreateName, &AccelerometerValues[0]);
				sprintf(FileToCreateName, "%slisTimes%d.txt", dir, FileNumber);
				WriteIntsToFile(LIS331_Data_Untill_New_File,FileToCreateName ,&Times[0]);
		CurPointer = 0;
		FileNumber++;
	}
}
#endif

	#if using_LM73
void TakeDataLM73(LM73* lm73z, volatile int& BusUsage, const int& BusUseMask) {
	float temp = 0;

	LM73& lm73 = *lm73z;
	// Max precision is 0.25 c. multiply by 4 for integer
	temp = lm73.getTemperature() ;
// Done using i2C bus, frees up bus
				BusUsage = (~(BusUseMask) &  BusUsage);
	float Temperature[LM73_Data_Untill_New_File + 20];
	int Times[LM73_Data_Untill_New_File + 20];
	
	char dir[] = LM73_Data_Directory;
	char FileToCreateName[50];
	static int FileNumber = 1; // For naming of files
	static int CurPointer = 0; 	// which part of the array is being looked at
Times[CurPointer] = TimeSinceStart();
	Temperature[CurPointer++] = temp;

	if (CurPointer >= (LM73_Data_Untill_New_File)) {
		sprintf(FileToCreateName, "%slm%d.txt", dir, FileNumber);
		WriteFloatsToFile2DecimalOnly(LM73_Data_Untill_New_File, FileToCreateName, &Temperature[0]);
		sprintf(FileToCreateName, "%slm%dTimes.txt", dir, FileNumber);
		WriteIntsToFile(LM73_Data_Untill_New_File,FileToCreateName ,&Times[0]);
		CurPointer = 0;
		FileNumber++;
	}
}
#endif

#if using_HMC5883L
void TakeDataHMC5883L(HMC* hmcz, volatile int& BusUsage, const int& BusUseMask) {
	static int x, y, z;

HMC& hmc = *hmcz;
	Magnetics data = {0,0,0};
	data = hmc.getData();
	// Done using i2C bus, frees up bus
				BusUsage = (~(BusUseMask) &  BusUsage);
x = data.x;
y = data.y;
z = data.z;

	int Magnetometer[3 * (LIS331_Data_Untill_New_File + 20)];
	int Times[(LIS331_Data_Untill_New_File + 20)];
	
	char dir[] = HMC_Data_Directory;
	char FileToCreateName[50];
	static int FileNumber = 1; // For naming of files
	static int CurPointer = 0; 	// which part of the array is being looked at

	Times[CurPointer/3] = TimeSinceStart();
	Magnetometer[CurPointer++] = x;
	Magnetometer[CurPointer++] = y;
	Magnetometer[CurPointer++] = z;

	if (CurPointer >= 3 * HMC_Data_Untill_New_File) {
		sprintf(FileToCreateName, "%shmc%d.txt", dir, FileNumber);
		WriteIntsToFile(3 * LIS331_Data_Untill_New_File, FileToCreateName, &Magnetometer[0]);
	sprintf(FileToCreateName, "%shmcTimes%d.txt", dir, FileNumber);
		WriteIntsToFile(LIS331_Data_Untill_New_File, FileToCreateName, &Times[0]);

		
		
		CurPointer = 0;
		FileNumber++;
	}
}
#endif
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





void FileWriteTest(){
ofstream tester;
tester.open(ITG_Data_Directory "cat.txt");
if(!tester)
cout << "Error with ITG file" << endl;
else
tester.close();

tester.open(BMP_Pressure_Data_Directory "cat.txt");
if(!tester)
cout << "Error with BMP file" << endl;
else
tester.close();

tester.open(BMP_Temperature_Data_Directory "cat.txt");
if(!tester)
cout << "Error with BMP file" << endl;
else
tester.close();

tester.open(LIS331_Data_Directory "cat.txt");
if(!tester)
cout << "Error with LIS file" << endl;
else
tester.close();

tester.open(HMC_Data_Directory "cat.txt");
if(!tester)
cout << "Error with HMC file" << endl;
else
tester.close();

tester.open(LM73_Data_Directory "cat.txt");
if(!tester)
cout << "Error with LM73 file" << endl;
else
tester.close();


tester.open(GeneralDir "FlightInfo.txt");
if(!tester)
cout << "Error with FlightInfo file" << endl;
else
tester.close();
}

// records generic stuff such as start time. sets variables
void generic(){
StartTime = std::time(0);
ofstream writer;
char buffer[100];

writer.open(GeneralDir "FlightInfo.txt");
sprintf(buffer, "ATEX launch\n");
writer << buffer;
sprintf(buffer,"Decimal Unix Time at start of launch: %d \n" , StartTime);
cout << buffer << endl;
writer << buffer;
writer.close();
}
