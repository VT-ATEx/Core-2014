//Philip Neighbour 2/13/2015
// Normal bias values for x,y,z are -38 , 93 , 151
// The ITG tends to enjoy being on i2c address 0x69

#include <unistd.h>
#include <iostream>
#ifndef I2C_H
#include "I2C.h"
#include "I2C.cpp"
#endif
#include "ITG3200.cpp"



class ITG3200;
/* Example Code:
******************************************************
		ITG3200	itg(1, 0x69);
		itg.StartITG();
		if (NotRun) {
			short* biases = itg.Find_ITG_Bias(3);
			cout << "X \t Y \t Z biases" << endl;
			cout << biases[0] << "\t" << biases[1] << "\t" << biases[2] << endl;
			NotRun = 0;
		}
		short* values = itg.Get_XYZT();
		cout << "X \t Y \t Z" << endl;
		cout << values[0] << "\t" << values[1] << "\t" << values[2] << endl;
		itg.CloseITG();
***********************************************************
*/
// #endif


/* class functions to pay attention to:
0. ITG3200(int Bus) 
1. Start(int Bus)
2. Close()
3. short* Get_XYZT()
*/


// returns an array 4*short that contains the raw ADC values. return[0] == x , return [1] == y , return[2] == z , reutrn[3] == temperature. returnd values correspond to rotational motion. Biases should be corrected. 
// Note that there are a ADC value supposidly corresponds to 1/14.375 degrees per second 
// 		short* Get_XYZT();
