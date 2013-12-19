#ifndef _KPIMAGE_H_
#define _KPIMAGE_H_

#include <vector>
#include <iostream>
#include "keypoint.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

/*
  On peut dire cette classe represente tous les keypoints d'un objet
*/
class KPImage: public cv::Mat
{
  private:
    std::vector<KeyPoint> key_point_list;
    
  public:
    KPImage();
    KPImage(cv::Mat image);
    void addKeyPoint(KeyPoint kp);
    friend std::istream& operator>>(std::istream& is, KPImage& kpi);
    KeyPoint operator[](int index);
    int getNBKeyPoint();
    KeyPoint* getMatchKey(KeyPoint kp);
    float getScoreCorrect(KPImage kpi);
    float drawCorrect(KPImage kpi, std::string);
};

#endif /* _KPIMAGE_H_ */
