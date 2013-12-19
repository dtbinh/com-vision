#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "mask.h"
//using namespace std;
using namespace cv;

class Control
{
private:
    Mat image;
public:
    Control();
    Control(Mat img);
    // virtual ~Control();
    void input(string file_name);
    bool output(string file_name);
    void processCommand();

    void trainning(string img_train_dir_skin_name,
		   string img_train_dir_no_skin_nam, int level);
    void detectSkinSimple(string img_test_name, int level);
    void detectSkinBayes(string img_test_name, float p, int level);
    void detectSkinRGB(string img_test_name, int level);
    friend void minimizeScaleImageRGB(Mat& image, int level);
    friend void minimizeScaleImageLab(Mat& image, int level);
    friend void convolution(Mat& img, Mask mask);
    friend void compareImage(Mat image1, Mat image2);
    void displayWelcome();
    void displayMenu();
    Mat getImage();
};

#endif /* _CONTROL_H_ */
