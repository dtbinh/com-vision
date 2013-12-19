#ifndef _MASK_H_
#define _MASK_H_

#define LEN 10
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
using namespace std;


class Mask
{
private:
    int size;
    float A[LEN][LEN];
public:
    Mask();
    Mask(int s, float a[][LEN]);
    Mask(Mask& m);
    int getSize() const{
	return size;
    }
    float get(int i, int j) const{
	return A[i][j];
    }
    Mask& operator=(const Mask& m);
    void extractMaskFromImage(cv::Mat img, int row, int col,
			      int size_mask, uchar chanel);
    friend float operator*(const Mask& mask1, const Mask& mask2);
    
    friend istream& operator>>(istream& is, Mask& m);
    friend ostream& operator<<(ostream& os, const Mask& m);
};

#endif /* _MASK_H_ */
