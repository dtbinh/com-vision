#include "keypoint.h"
#include <math.h>
#include <limits.h>
#include <float.h>



KeyPoint::KeyPoint()
{
    this->len_discriptor = 0;
}


float KeyPoint::operator[](int index)
{
    return this->data[index];
}

std::istream& operator>>(std::istream& is, KeyPoint& kp)
{
    is>>kp.y >>kp.x >>kp.scale >>kp.direction;
    for (int i = 0; i < kp.len_discriptor; ++i)
    {
	is>>kp.data[i];
    }
    return is;
}

float KeyPoint::getX()
{
    return this->x;
}

float KeyPoint::getY()
{
    return this->y;
}

void KeyPoint::setLenDiscriptor(int key)
{
    this->len_discriptor = key;
}

float KeyPoint::getDistance(KeyPoint kp)
{
    int N = this->len_discriptor;
    float distance = 0.0;
    if(kp.len_discriptor != N)
	return FLT_MAX;
    for (int i = 0; i < N; ++i)
    {
	distance += (data[i] - kp[i])*(data[i] - kp[i]);
    }
    return sqrt(distance);
}
