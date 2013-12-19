#ifndef _KPIMAGE_H_
#define _KPIMAGE_H_

#include <vector>
#include <iostream>
#include "keypoint.h"
#include "pairpoint.h"

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
    friend std::istream& operator>>(std::istream& is, KPImage& kpi);
    KeyPoint operator[](int index);
    KeyPoint* getMatchKey(KeyPoint kp);
    std::vector<PairPoint> getPairPoints(KPImage kpi);
    void drawCorrect(KPImage kpi, std::vector<PairPoint> pairs,
		     std::string image_corr);
};

#endif /* _KPIMAGE_H_ */
