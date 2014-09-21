#include "LIS331.h"
#include <iostream>

using namespace std;
using std::cout;


void *update(void *ptr);

LIS331::LIS331(const int bus, const int address)
{

    char *device = new char[50];
    sprintf(device, "/dev/i2c-%d", bus);

    if ((i2cfdacc = open(device, O_RDWR)) < 0)
    {
        sprintf(device, "Failed to open %s", device);
        perror(device);
    }
    delete[] device;
    char *addr = new char[50];

    sprintf(addr, "0x%x", address);

    if (ioctl(i2cfdacc, I2C_SLAVE, address) < 0)
    {
        sprintf(addr, "Start error with address %s", addr);
        perror(addr);
        delete[] addr;

    }
    delete[] addr;


    //runs self test to ensure sensor functionality

    char tx[2];

    tx[0] = 0x20;
    tx[1] = 0x37;

    clients = 42;


    write(i2cfdacc, tx, 2);
    usleep (100);

    // tx[0] = 0x21;
    // tx[1] = 0x10;

    // write(i2cfdacc, tx, 2);
    // usleep (100);

    float valuex, valuey, valuez, valuext, valueyt, valuezt;

    tx[0] = 0xA8;
    //tx[1]=0x27;


    write(i2cfdacc, tx, 1);

    int tmp;
    char *data = new char[2];
    //gets x value
    tmp = read(i2cfdacc, data, 2);

    if (tmp != 2)
        perror("Read did not return bytes specified");

    valuex = ((data[0] | (data[1] << 8))) * .002929688;

    tx[0] = 0xAA;
    //tx[1]=0x27;


    write(i2cfdacc, tx, 1);


    tmp = read(i2cfdacc, data, 2);

    if (tmp != 2)
        perror("Read did not return bytes specified");

    valuey = ((data[0] | (data[1] << 8))) * .002929688;

    tx[0] = 0xAC;
    //tx[1]=0x27;


    write(i2cfdacc, tx, 1);
    ;

    tmp = read(i2cfdacc, data, 2);

    if (tmp != 2)
        perror("Read did not return bytes specified");

    valuez = ((data[0] | (data[1] << 8))) * .002929688;




    tx[0] = 0x23;
    tx[1] = 0x02;


    write(i2cfdacc, tx, 2);

    usleep (100);


    tmp = read(i2cfdacc, data, 2);

    if (tmp != 2)
        perror("Read did not return bytes specified");

    valuext = ((data[0] | (data[1] << 8))) * .002929688;

    tx[0] = 0xAA;
    //tx[1]=0x27;


    write(i2cfdacc, tx, 1);


    tmp = read(i2cfdacc, data, 2);

    if (tmp != 2)
        perror("Read did not return bytes specified");

    valueyt = ((data[0] | (data[1] << 8))) * .002929688;

    tx[0] = 0xAC;
    //tx[1]=0x27;


    write(i2cfdacc, tx, 1);


    tmp = read(i2cfdacc, data, 2);

    if (tmp != 2)
        perror("Read did not return bytes specified");

    valuezt = ((data[0] | (data[1] << 8))) * .002929688;

    tx[0] = 0x23;
    tx[1] = 0x00;


    write(i2cfdacc, tx, 2);

    tx[0] = 0x20;
    tx[1] = 0x07;


    write(i2cfdacc, tx, 2);
    usleep (100);


    if ((valuext - valuex < 180) | (valuext - valuex > 50))
    {

    }
    else
    {
        perror("X axis out of range!!!!");
    }

    if ((valueyt - valuey < -50) | (valueyt - valuey > -180))
    {

    }
    else
    {
        perror("Y axis out of range!!!!");
    }

    if ((valuezt - valuez < 370) | (valuezt - valuez > 220))
    {

    }
    else
    {
        perror("Z axis out of range!!!!");
    }


    pthread_mutex_init(&mutex, NULL);
    status = INIT;
    join = false;




}




void LIS331::Configure(int datarate, int g)
{

    char tx[2];
    tx[0] = 0x20;

    switch (datarate)
    {
    case 50:
    {
        tx[1] = 0x27;
        break;
    }

    case 100:
    {
        tx[1] = 0x2F;
        break;
    }

    case 400:
    {
        tx[1] = 0x37;
        break;
    }

    case 1000:
    {
        tx[1] = 0x3F;
        break;
    }

    default:
    {
        tx[1] = 0x2F;
        break;
    }
    }



    write(i2cfdacc, tx, 2);

    tx[0] = 0x23;
    switch (g) //0x23
    {

    case 6: //0x00
    {
        tx[1] = 0x00;
        scale = .000183105;
        break;
    }

    case 12: //0x10
    {
        tx[1] = 0x10;
        scale = .000366211;
        break;
    }

    case 24: //0x30
    {
        tx[1] = 0x30;
        scale = .000732422;
        break;
    }


    default:
    {
        tx[1] = 0x30;
        scale = .000732422;
        break;
    }
    }

    write(i2cfdacc, tx, 2);

    maxX = 0;
    maxY = 0;
    maxZ = 0;
    minX = 0;
    minY = 0;
    minZ = 0;
    X = 0;
    Y = 0;
    Z = 0;

}



float LIS331::getXvalue()
{


    char tx[2];
    float value = 0;

    tx[0] = 0xA8;
    //tx[1]=0x27;


    write(i2cfdacc, tx, 1);

    int tmp;
    char *data = new char[2];

    tmp = read(i2cfdacc, data, 2);

    if (tmp != 2)
        perror("Read did not return bytes specified");

    value = ((int16_t)(data[0] | (data[1] << 8))) * scale;


    delete[] data;

    return value;



}

float LIS331::getYvalue()
{
    char tx[2];
    float value = 0;

    tx[0] = 0xAA;
    //tx[1]=0x27;


    write(i2cfdacc, &tx[0], 1);

    int tmp;
    char *data = new char[2];
    //data[0]=0;
    //data[1]=0;
    tmp = read(i2cfdacc, data, 2);

    if (tmp != 2)
        perror("Read did not return bytes specified");

    value = ((int16_t)(data[0] | (data[1] << 8))) * scale;
    //      printf ("%x, %x\n", data[0], data [1]);

    delete[] data;

    return value;



}

float LIS331::getZvalue()
{
    char tx[2];
    float value = 0;

    tx[0] = 0xAC;
    //tx[1]=0x27;


    write(i2cfdacc, &tx[0], 1);

    int tmp;
    char *data = new char[2];

    tmp = read(i2cfdacc, data, 2);

    if (tmp != 2)
        perror("Read did not return bytes specified");

    value = ((int16_t)(data[0] | (data[1] << 8))) * scale;


    delete[] data;

    return value;



}


float LIS331::getX()
{
    float x = 0;
    pthread_mutex_lock (&mutex);
    x = X;
    pthread_mutex_unlock (&mutex);
    return x;

}
float LIS331::getY()
{
    float y = 0;
    pthread_mutex_lock (&mutex);
    y =  Y;
    pthread_mutex_unlock (&mutex);
    return y;

}
float LIS331::getZ()
{
    float z = 0;
    pthread_mutex_lock (&mutex);
    z = Z;
    pthread_mutex_unlock (&mutex);
    return z;
}






bool  LIS331::start_update()
{


    int status_local;
    pthread_mutex_lock (&mutex);
    status_local = status;
    pthread_mutex_unlock (&mutex);
    if (status_local == INIT)
    {
        pthread_mutex_lock (&mutex);
        status_local = status;
        runningaverageX = 0;
        runningaverageY = 0;
        runningaverageZ = 0;
        X = 0;
        Y = 0;
        Z = 0;
        pthread_mutex_unlock (&mutex);
        pthread_create (&pt, NULL, update, (void *)this);
        return true;
    }
    return false;
}
bool  LIS331::stop_update()
{
    pthread_mutex_lock (&mutex);
    join = true;
    pthread_mutex_unlock (&mutex);
    //pthread_join (pt,NULL);
    pthread_exit (&pt);
    status = INIT;
    join = false;
    pt = NULL;
    return connected;
}


float LIS331::getXaverage()
{
    float localx = 0;
    pthread_mutex_lock (&mutex);
    localx = runningaverageX;
    pthread_mutex_unlock (&mutex);

    return localx;

}
float LIS331::getYaverage()
{
    float localy = 0;
    pthread_mutex_lock (&mutex);
    localy = runningaverageY;
    pthread_mutex_unlock (&mutex);

    return localy;

}
float LIS331::getZaverage()
{
    float localz = 0;
    pthread_mutex_lock (&mutex);
    localz = runningaverageZ;
    pthread_mutex_unlock (&mutex);

    return localz;

}

void LIS331::setpollrate(int rate)
{
    pthread_mutex_lock (&mutex);
    pollrate = rate;
    pthread_mutex_unlock (&mutex);


}
void LIS331::setdatapoints(int points)
{

    pthread_mutex_lock (&mutex);
    datapoints = points;
    pthread_mutex_unlock (&mutex);
}

void LIS331::setclient (int c)
{
    pthread_mutex_lock (&mutex);
    clients = c;
    pthread_mutex_unlock (&mutex);


}

float LIS331::getXmax()
{
    float localx = 0;
    pthread_mutex_lock (&mutex);
    localx = maxX;
    pthread_mutex_unlock (&mutex);

    return localx;

}
float LIS331::getXmin()
{
    float localx = 0;
    pthread_mutex_lock (&mutex);
    localx = minX;
    pthread_mutex_unlock (&mutex);

    return localx;


}
float LIS331::getYmax()
{
    float localy = 0;
    pthread_mutex_lock (&mutex);
    localy = maxY;
    pthread_mutex_unlock (&mutex);

    return localy;


}
float LIS331::getYmin()
{
    float localy = 0;
    pthread_mutex_lock (&mutex);
    localy = minY;
    pthread_mutex_unlock (&mutex);

    return localy;

}
float LIS331::getZmax()
{
    float localz = 0;
    pthread_mutex_lock (&mutex);
    localy = maxZ;
    pthread_mutex_unlock (&mutex);

    return localz;


}
float LIS331::getZmin()
{

    float localz = 0;
    pthread_mutex_lock (&mutex);
    localy = minZ;
    pthread_mutex_unlock (&mutex);

    return localz;


}


void  LIS331::closer()
{
    close(i2cfdacc);
}

LIS331::~LIS331()
{
    closer();
}



void *update(void *ptr) //inside here need to poll and update sensors, compute running average
{
    LIS331 *thread = ( LIS331 *) ptr;
    int status;
    int join;
    //create vectors for variables, add length

    vector<float> valuesX;
    vector<float> valuesY;
    vector<float> valuesZ;

    vector<float> maxx;
    vector<float> maxy;
    vector<float> maxz;

    vector<float> minx;
    vector<float> miny;
    vector<float> minz;

    float localx = 0;
    float localy = 0;
    float localz = 0;

    float localaveragex = 0;
    float localaveragey = 0;
    float localaveragez = 0;

    unsigned int i = 0;
    unsigned int y = 0;

    pthread_mutex_lock (&thread->mutex);
    thread->X = thread->getXvalue();
    thread->Y = thread->getYvalue();
    thread->Z = thread->getZvalue();

    localx = thread->X;
    localy = thread->Y;
    localz = thread->Z;

    pthread_mutex_unlock (&thread->mutex);

    usleep (400);

    pthread_mutex_lock (&thread->mutex);
    thread->X = thread->getXvalue();
    thread->Y = thread->getYvalue();
    thread->Z = thread->getZvalue();

    localx = thread->X;
    localy = thread->Y;
    localz = thread->Z;

    pthread_mutex_unlock (&thread->mutex);

    // printf ("X(in thread): %f\n", localx);
    // printf ("Y(in thread): %f\n", localy);
    // printf ("Z(in thread): %f\n", localz);

    while (i < 10)
    {
        maxx.push_back(localx);
        i++;
    }
    i = 0;

    while (i < 10)
    {
        minx.push_back(localx);
        i++;
    }
    i = 0;

    while (i < 10)
    {
        maxy.push_back(localx);
        i++;
    }
    i = 0;

    while (i < 10)
    {
        miny.push_back(localy);
        i++;
    }
    i = 0;

    while (i < 10)
    {
        maxz.push_back(localz);
        i++;
    }
    i = 0;

    while (i < 10)
    {
        minz.push_back(localz);
        i++;
    }
    i = 0;





    while (true)
    {
        //get the status variables
        pthread_mutex_lock (&thread->mutex);
        thread->status =  LIS331::PROCESSING;
        status = thread->status;
        join = thread->join;
        pthread_mutex_unlock (&thread->mutex);
        //EXIT if ASKED
        if (join)
            break;

        //if the status is processing, then run
        if (status ==  LIS331::PROCESSING)
        {
            pthread_mutex_lock (&thread->mutex);
            thread->X = thread->getXvalue();
            thread->Y = thread->getYvalue();
            thread->Z = thread->getZvalue();

            localx = thread->X;
            localy = thread->Y;
            localz = thread->Z;

            pthread_mutex_unlock (&thread->mutex);

            if (valuesX.size() < thread->datapoints && valuesY.size() < thread->datapoints && valuesZ.size() < thread->datapoints)
            {
                if (valuesX.size() < 1 && valuesY.size() < 1 && valuesZ.size() < 1)
                {
                    valuesX.push_back(localx);
                    valuesY.push_back(localy);
                    valuesZ.push_back(localz);
                    localaveragex = valuesX[0];
                    localaveragey = valuesY[0];
                    localaveragez = valuesZ[0];
                   // printf ("Inside of first if statement\n");

                }
                else
                {
                    localaveragex = ((localaveragex * valuesX.size()) + localx) / (valuesX.size() + 1);
                    valuesX.push_back(localx);
                    localaveragey = ((localaveragey * valuesY.size()) + localy) / (valuesY.size() + 1);
                    valuesY.push_back(localy);
                    localaveragez = ((localaveragez * valuesZ.size()) + localz) / (valuesZ.size() + 1);
                    valuesZ.push_back(localz);
                }
            }

            else
            {
                if (y < thread->datapoints)
                {
                    localaveragex = ((localaveragex * thread->datapoints) - valuesX[y] + localx) / thread->datapoints;
                    valuesX[y] = localx;
                    localaveragey = ((localaveragey * thread->datapoints) - valuesY[y] + localy) / thread->datapoints;
                    valuesY[y] = localy;
                    localaveragez = ((localaveragez * thread->datapoints) - valuesZ[y] + localz) / thread->datapoints;
                    valuesZ[y] = localz;
                    y++;
                }

                else
                {
                    y = 0;
                    localaveragex = ((localaveragex * thread->datapoints) - valuesX[y] + localx) / thread->datapoints;
                    valuesX[y] = localx;
                    localaveragey = ((localaveragey * thread->datapoints) - valuesY[y] + localy) / thread->datapoints;
                    valuesY[y] = localy;
                    localaveragez = ((localaveragez * thread->datapoints) - valuesZ[y] + localz) / thread->datapoints;
                    valuesZ[y] = localz;
                    y++;
                }

            }

            pthread_mutex_lock (&thread->mutex);
            thread->runningaverageX = localaveragex;
            thread->runningaverageY = localaveragey;
            thread->runningaverageZ = localaveragez;
            pthread_mutex_unlock (&thread->mutex);


            // //min max stuff

            while (i < 10)
            {
                if (localx > maxx[i])
                {
                    maxx[i] = localx;
                    // printf ("X:%f\n", maxx[i]);
                }
                i++;
            }
            i = 0;

            while (i < 10)
            {
                if (localx < minx[i])
                {
                    minx[i] = (localx);
                }
                i++;
            }
            i = 0;

            while (i < 10)
            {
                if (localy > maxy[i])
                {
                    maxy[i] = localy;
                    //printf ("Y:%f\n", maxy[i]);
                }
                i++;
            }
            i = 0;

            while (i < 10)
            {
                if (localy < miny[i])
                {
                    miny [i] = localy;
                }
                i++;
            }
            i = 0;

            while (i < 10)
            {
                if (localz > maxz[i])
                {
                    maxz[i] = localz;
                    //printf ("Z:%f\n", maxz[i]);
                }
                i++;
            }
            i = 0;

            while (i < 10)
            {
                if (localy < minz[i])
                {
                    minz[i] = localz;
                }
                i++;
            }
            i = 0;

            pthread_mutex_lock (&thread->mutex);
            int client = thread->clients;
            pthread_mutex_unlock (&thread->mutex);

            if (client < 42)
            {
                cout << maxx[client] << endl;
                cout << maxy[client] << endl;
                cout << maxz[client] << endl;
                cout << minx[client] << endl;
                cout << miny[client] << endl;
                cout << minz[client] << endl;

                pthread_mutex_lock (&thread->mutex);
                thread->maxX =  maxx[client];
                thread->maxY =  maxy[client];
                thread->maxZ =  maxz[client];
                thread->minX =  minx[client];
                thread->minY =  miny[client];
                thread->minZ =  minz[client];
                pthread_mutex_unlock (&thread->mutex);

                maxx[client] = thread->getX();
                maxy[client] = thread->getY();
                maxz[client] = thread->getZ();
                minx[client] = thread->getX();
                miny[client] = thread->getY();
                minz[client] = thread->getZ();




                pthread_mutex_lock (&thread->mutex);
                thread->clients = 42;
                pthread_mutex_unlock (&thread->mutex);
                client = 42;

            }

            // printf ("X(value at end): %f\n", localx);
            // printf ("Y(value at end): %f\n", localy);
            // printf ("Z(value at end): %f\n", localz);










            pthread_mutex_lock (&thread->mutex);
            thread->status = LIS331::WAITING;
            pthread_mutex_unlock (&thread->mutex);
            usleep(thread->pollrate);
            pthread_mutex_lock (&thread->mutex);
            thread->status = LIS331::DONE;
            pthread_mutex_unlock (&thread->mutex);
        }
    }
    return (void *) NULL;
}