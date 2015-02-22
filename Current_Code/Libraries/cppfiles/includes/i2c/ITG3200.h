//Philip Neighbour 2/13/2015
// Normal bias values for x,y,z are -38 , 93 , 151

#include "ITG3200.cpp"

class ITG3200;
/* class functions to pay attention to:
0. ITG3200(int Bus) 
1. Start(int Bus)
2. Close()
3. short* Get_XYZT()
*/


// returns an array 4*short that contains the raw ADC values. return[0] == x , return [1] == y , return[2] == z , reutrn[3] == temperature. returnd values correspond to rotational motion. Biases should be corrected. 
// Note that there are a ADC value supposidly corresponds to 1/14.375 degrees per second 
// 		short* Get_XYZT();


