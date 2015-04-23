// Compile with g++ -pthread [This file name] -o [name of executable to create]
//NOT NESSESARY compile with g++ -pthread LIS331Test.cpp LIS331.cpp -o run
#include "LIS331.h"

#include <iostream>
#include <unistd.h>

//using namespace std;
using std::cout;
using std::cin;

int main(int argc, char const *argv[])
{

    LIS331 lis331(1, 0x1e);
    lis331.Configure(100, 12);
    lis331.setpollrate(10000);
    lis331.setdatapoints(100);
    lis331.start_update();




    while (true)
    {
usleep(100000);
std::cout << "X: " << lis331.getXvalue() << endl;
std::cout << "Y: " << lis331.getYvalue() << endl;
std::cout << "Z: " << lis331.getZvalue() << endl;

    }





    return 0;
}
