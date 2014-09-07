#ifndef RTC_H
#define RTC_H

#include "I2C.h"

class RTC {
private:
	I2C i2c;
public:
	int G;

   //!Creates connection to Chipcap sensor on bus 2
	/*!	Creates and intializes a ChipCap sensor at the given i2c address on i2c bus 2
     \param addr is the address of the sensor
     \return none
	 */
	RTC(const int addr);

	 //!Creates connection to ChipCap sensor on any bus
	/*!	Creates and intializes a ChipCap sensor at the given i2c address on any i2c bus
	 \param bus is the i2c bus number
     \param addr is the address of the sensor 
     \return none
	 */
	RTC(const int bus, const int addr);

	//!Deconstructor for the ChipCap class
	/*!	When called, this remove all control of the sensor
	\param gnone
	\return none
	 */
	~RTC();

	 //!Receives current seconds from clock
	/*!Receives data from sensor at the given address and i2c bus (defaults to bus 2)
	 \param none
     \return int which is seconds
	 */
	int getSeconds();

	 //!Receives current minute from clock
	/*!Receives data from sensor at the given address and i2c bus (defaults to bus 2)
	 \param none
     \return int which is minutes
	 */

	int getMinutes();


	//!Receives current hours from clock
	/*!Receives data from sensor at the given address and i2c bus (defaults to bus 2)
	 \param none
     \return int which is hours
	 */

	int getHours ();


	 //!Receives current minute from clock
	/*!Receives data from sensor at the given address and i2c bus (defaults to bus 2)
	 \param none
     \return string that is the current time in HH:MM:SS
	 */
	char* getTime ();



   //!Closes i2c bus
	/*!	Closes connection to previously defined i2c bus
     \param none
     \return none
	 */
	void close();
};

#endif