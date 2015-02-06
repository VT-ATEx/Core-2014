#ifndef HMC_H_
#define HMC_H_
//The above defines the header

#include "i2c.h"

/*This is a little wierd, and I don't know if you'll have to do this.
*My sensor return each axis individually and with it's data split into
*two bytes, but makes me pull all three axes down at once. The struct
*creates a new data type with three floats.
*/
typedef struct Magnetics{
	int x;
	int y;
	int z;
} Magnetics;

int* hmcInit(int addr, int bus);
Magnetics hmcGetData(int* i2cfd);
void hmcClose(int* i2cfd);

#endif
