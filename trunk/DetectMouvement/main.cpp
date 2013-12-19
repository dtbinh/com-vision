#include "control.h"
#include <stdio.h>
#include <string>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <stdlib.h>


using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
    Control control;
    control.processCommand(argc, argv);
    return 0;
}
