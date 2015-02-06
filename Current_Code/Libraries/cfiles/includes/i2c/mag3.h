#ifndef MAG3_H_
#define MAG3_H_
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

int* mag3Init(int addr, int bus);
Magnetics mag3GetData(int* i2cfd);
void mag3Close(int* i2cfd);

#endif
