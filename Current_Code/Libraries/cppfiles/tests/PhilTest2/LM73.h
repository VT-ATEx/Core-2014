#ifndef LM73_H_
#define LM73_H_

#include "I2C.h"
#include "I2C.cpp"

class LM73 {
private:
        I2C i2c;
public:
        LM73(const int bus, const int addr);
        ~LM73();
        float getTemperature();
        void close();
};


#endif