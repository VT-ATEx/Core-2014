#include "LIS331.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

int main(int argc, char const *argv[])
{

    LIS331 lis331(2, 0x19);
    lis331.Configure(100, 12);
    lis331.setpollrate(10000);
    lis331.setdatapoints(100);
    lis331.start_update();




    while (true)
    {
        cout << "X: " << lis331.getXvalue() << endl;
        cout << "Y: " << lis331.getYvalue() << endl;
        cout << "Z: " << lis331.getZvalue() << endl;

    }





    return 0;
}
