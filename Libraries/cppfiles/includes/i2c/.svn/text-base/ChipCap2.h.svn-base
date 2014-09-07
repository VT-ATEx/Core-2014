#ifndef CHIPCAP2_H
#define CHIPCAP2_H

#include "I2C.h"

class ChipCap2 {
private:
	I2C i2c;
public:
	int G;

   //!Creates connection to Chipcap sensor on bus 2
	/*!	Creates and intializes a ChipCap sensor at the given i2c address on i2c bus 2
     \param addr is the address of the sensor
     \return none
	 */
	ChipCap2(const int addr);

	 //!Creates connection to ChipCap sensor on any bus
	/*!	Creates and intializes a ChipCap sensor at the given i2c address on any i2c bus
	 \param bus is the i2c bus number
     \param addr is the address of the sensor 
     \return none
	 */
	ChipCap2(const int bus, const int addr);

	//!Deconstructor for the ChipCap class
	/*!	When called, this remove all control of the sensor
	\param gnone
	\return none
	 */
	~ChipCap2();

	 //!Receives humdity data from ChipCap sensor
	/*!Receives data from sensor at the given address and i2c bus (defaults to bus 2)
	 \param none
     \return float which is %RH
	 */
	float getHumidity();

	 //!Receives temperature data from ChipCap sensor
	/*!Receives data from sensor at the given address and i2c bus (defaults to bus 2)
	 \param none
     \return float which is temperature in degrees C
	 */

	float getTemperature();



   //!Closes i2c buss
	/*!	Closes connection to previously defined i2c bus
     \param none
     \return none
	 */
	void close();
};

#endif