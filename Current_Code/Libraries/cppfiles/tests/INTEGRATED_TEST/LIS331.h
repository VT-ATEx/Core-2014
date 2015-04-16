#ifndef LIS331_H_
#define LIS331_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <linux/i2c-dev.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <vector>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <fstream>
#include <math.h>
#include <limits>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdint.h>

//int* lm73Init(int addr, int bus);
//float lm73GetTemperature(int* i2cfd);
//void lm73Close(int* i2cfd);

class LIS331
{

public:

    //status: ready for fill, processing or ready for write
    enum
    {
        READY_WAITING = 0, //this state is set external to the thread execution when the thread is waiting for new data
        WAITING = 1, //this state is set internal to the thread execution should be used when the thread is done and waiting for data to be removed
        PROCESSING = 2, //set externally to tell the thread to set new data, must trigger the ready_to_process condition signal as well
        DONE = 3, //set internally to indicate done processing, should wait for waiting before removing data
        INIT = 4 //set on thread creation so that the thread goes straight to the READY_WAITING state when it is ready
    } Status;


public:

    bool stop_update();
    bool start_update();
    float getXaverage();
    float getYaverage();
    float getZaverage();
    float getXmax();
    float getXmin();
    float getYmax();
    float getYmin();
    float getZmax();
    float getZmin();

    float getX();
    float getY();
    float getZ();

    void setpollrate(int rate);
    void setdatapoints(int points);
    void setclient (int c);

public:

    LIS331(const int bus, const int address);
    ~LIS331();
    void Configure(int datarate, int g);
    float getXvalue();
    float getYvalue();
    float getZvalue();
    void closer();

public:

    float scale;
    int i2cfdacc;

    bool connected;
    float runningaverageX;
    float runningaverageY;
    float runningaverageZ;
    float X;
    float Y;
    float Z;
    float maxX;
    float maxY;
    float maxZ;
    float minX;
    float minY;
    float minZ;
    int pollrate;
    unsigned int datapoints;



    //Thread variables
    int status;
    bool join;
    int clients;

    //Mutex for locking access to status
    pthread_mutex_t mutex;

    //Thread object
    pthread_t pt;
};


#endif
