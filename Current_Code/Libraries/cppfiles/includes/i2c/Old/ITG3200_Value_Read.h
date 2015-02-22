//Philip Neighbour 2/13/2015
// Normal bias values for x,y,z are -38 , 93 , 151

#include "ITG3200_Value_Read.cpp"

// Setup needs to be run before running Get_XYZT
void SetupITG3200();
// returns an array 4*short that contains the raw ADC values. return[0] == x , return [1] == y , return[2] == z , reutrn[3] == temperature. returnd values correspond to rotational motion. Biases should be corrected. 
// Note that there are a ADC value supposidly corresponds to 1/14.375 degrees per second 
short* Get_XYZT();



/*
void SetupITG3200();
short* Get_XYZT();
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
*/


