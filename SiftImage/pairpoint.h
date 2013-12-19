#ifndef _PAIR_H_
#define _PAIR_H_
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

class PairPoint
{
public:
    PairPoint();
    PairPoint(cv::Point p1, cv::Point p2);
    cv::Point getStart();
    cv::Point getEnd();
    double getAngle();

  private:
    cv::Point start;
    cv::Point end;
};

#endif /* _PAIR_H_ */
