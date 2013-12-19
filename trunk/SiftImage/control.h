#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <vector>
using namespace std;
using namespace cv;

class Control
{
public:
    Control();
    void processCommand();
    void findKeyPoint(string, string);
    void drawCorrectImages(string, string, string, string, string);
    void displayWelcome();
    void displayMenu();
};

#endif /* _CONTROL_H_ */
