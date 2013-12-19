#ifndef _KEYPOINT_H_
#define _KEYPOINT_H_
#include <iostream>
//#include "kpimage.h"

#define LEN_DISCRIPTOR 256

/*
On peut dire cette classe represente un keypoint avec descripteur
 */
class KeyPoint
{
private:
    float x, y, scale, direction;
    int len_discriptor, data[LEN_DISCRIPTOR];
public:
    KeyPoint();
    float operator[](int index);
    friend std::istream& operator>>(std::istream& is, KeyPoint& kp);    
    float getX();
    float getY();
    void setLenDiscriptor(int key);
    float getDistance(KeyPoint kp);
};

#endif /* _KEYPOINT_H_ */
