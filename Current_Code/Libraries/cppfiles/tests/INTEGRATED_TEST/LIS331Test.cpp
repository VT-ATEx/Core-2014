#include "LIS331.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;
using std::cout;

int main(int argc, char const *argv[])
{

    LIS331 lis331(2, 19);
    lis331.Configure(100, 12);
    lis331.setpollrate(10000);
    lis331.setdatapoints(100);
    lis331.start_update();




    while (true)
    {
        std::cout << "X: " << lis331.getXvalue() << endl;
        std::cout << "Y: " << lis331.getYvalue() << endl;
        std::cout << "Z: " << lis331.getZvalue() << endl;

    }





    return 0;
}
