#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <vector>
using namespace std;
using namespace cv;


struct Candidate
{
    string image_name;
    float score;
};

class Control
{
  private:
    Mat image;
  public:
    Control();
    Control(Mat img);
    void input(string file_name);
    bool output(string file_name);
    void processCommand();

    void deleteKeySmall(string path, int number_key);
    void findKeyPoint(string, string);
    float calScore2Images(string key_img1, string key_img2);
    void drawCorrectImages(string, string, string, string, string);
    void displayClassOfImage(string key_img1,
			     string key_path2, vector<Candidate>);
    int classImage(string key_img1,
		   string key_path2, vector<Candidate>&);
    void classImage(string key_path1, string key_path2);
    
    void displayWelcome();
    void displayMenu();
    Mat getImage();
};

#endif /* _CONTROL_H_ */
