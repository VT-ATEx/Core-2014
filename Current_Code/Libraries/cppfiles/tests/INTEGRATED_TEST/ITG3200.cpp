//Philip Neighbour 2/13/2015
// Code for the ITG3200 gyro (measures angular velovity) for the Beagle Bone Black
// Some commands while on Beagle bone:
//sudo i2cdetect -r 1
//sudo i2cdump 1 0x69

// #define Adress 0x69 // The I2C address that ITG is on
// #define Bus 1 // The I2C bus that the ITG is on
#define ITG_Delay 10000 //Minimum delay for reading sensors. In 10^(-6) seconds
#define biastime 3*1000000 // in 10^(-6) seconds
using std::cin;
using std::cout;
using std::endl;

class ITG3200
{
public:
// Address of ITG on specifieyd bus
	int Adress;
	void RawADCValuePrint();
// Function that prepares the reading of a certian register address. 
	void prepread(int address);
// sets data from the ITG to data[] , an array of type char. Elements in data that are changed correspond to the register values
// void SetData(char* data, int StartRegister, int EndRegister);
//returns an array for correct bias values
	short* Find_ITG_Bias(int SecondDelay);

	I2C* I2C_Object;
	int Bus;

	void StartITG()
	{
		I2C_Object->startbus(Adress);
//set register 0x16 to 0x03. This probably should only be done 1 time
		Write_1_To_Register(0x16, 0x03);
	}
	void CloseITG()
	{
		I2C_Object->closebus();
	}

	ITG3200(int Bus , int Adress)
	{
		this->Bus = Bus;
		this->Adress = Adress;
		I2C_Object = new I2C(Bus);
	}
// returns an array 4*short that contains the raw ADC values. return[0] == x , return [1] == y , return[2] == z , reutrn[3] == temperature. returnd values correspond to rotational motion. Biases should be corrected. 
// Note that there are a ADC value corresponds to 1/14.375 degrees per second 
	short* Get_XYZT();

private:
	void SetData(char* data, int StartRegister, int EndRegister);
// writes 1 byte to a certian register so that values can then be read sequentially from that register
	void Write_1_To_Register(char registerr, char Value_To_Write);

};
short* ITG3200::Get_XYZT()
{
// array that will contain the register values read in from the sensor. I2C device has max 256 registers
	static char* data = new char[0x100];

	SetData(data, 0x10, 0x30);

	static short values[4];
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

void ITG3200::SetData(char* data, int StartRegister, int EndRegister) {
	prepread(StartRegister);
	usleep(ITG_Delay);
	I2C_Object->readbus(data + StartRegister, EndRegister - StartRegister);
}
void ITG3200::Write_1_To_Register(char registerr, char Value_To_Write) {
	char Data_To_Write[2];
// The register to write to
	Data_To_Write[0] = registerr;
	Data_To_Write[1] = Value_To_Write;
	I2C_Object->writebus(Data_To_Write);
}

short* ITG3200::Find_ITG_Bias(int SecondDelay) {
static short* Biases = new short[3];
	short X,Y,Z;
//is divided later to find average bias
	long Xt, Yt, Zt;
	Xt = 0;
	Yt = 0;
	Zt = 0;

	long TimesToLoop = biastime / ITG_Delay / 3;
	cout << "calibrating biases. wait ~" << biastime / 1000000 << " seconds" << endl;
	cout << "sampling " << TimesToLoop << " Times" << endl;

	static char* data = new char[0x40];
	for (int i = 0; i < TimesToLoop; i++) {
		SetData(data, 0x1b, 0x24);
		X = (data[0x1d] << 8) | data[0x1e];
		Y = (data[0x1f] << 8) | data[0x20];
		Z = (data[0x21] << 8) | data[0x22];

		Xt += X;
		Yt += Y;
		Zt += Z;
		usleep(ITG_Delay);
	}
//the x bias
	Biases[0] = Xt / TimesToLoop;
//the y bias
	Biases[1] = Yt / TimesToLoop;
//the z bias
	Biases[2] = Zt / TimesToLoop;
	cout << "Done Sampling Bias" << endl;
	return Biases;
}

// Prepares reading of a register
void ITG3200::prepread(int address) {
	static char* send = new char[1];
	send[0] = address - 3;
	usleep(ITG_Delay);
	I2C_Object->writebus(send);
}

