#include "pairpoint.h"
#include <math.h>

PairPoint::PairPoint()
{
    this->start = cv::Point(0.0, 0.0);
    this->end = cv::Point(0.0, 0.0);
}

PairPoint::PairPoint(cv::Point p1, cv::Point p2)
{
    this->start = p1;
    this->end = p2;
}

cv::Point PairPoint::getStart()
{
    return this->start;
}

cv::Point PairPoint::getEnd()
{
    return this->end;
}

double PairPoint::getAngle()
{
    double angle = 0.0;
    double dx = this->start.x - this->end.x;
    double dy = this->start.y - this->end.y;
    angle = atan(dx/dy);
    return angle;
}
