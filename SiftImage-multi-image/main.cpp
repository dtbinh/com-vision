#include "control.h"
#include <stdio.h>
#include <string>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <stdlib.h>


using namespace std;

int main(int argc, char* argv[])
{
    Control control;
    control.displayWelcome();
    control.displayMenu();
    control.processCommand();

    return 0;
}
