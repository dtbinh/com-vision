#ifndef _KALMAN_H_
#define _KALMAN_H_
#include <vector>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/video/tracking.hpp"

struct FilKalman
{
    cv::Mat_<float> state;
    cv::KalmanFilter KF;
};

class Kalman
{
public:
    Kalman();

    /*
      Fonctionnalite de la classe
     */
    void filters(std::vector<cv::Point2f> center, cv::Mat &image_tracking);
    void filter(cv::Point2f center, cv::Mat &image_tracking);
    int getStateIDNearest(cv::Point2f p);
    friend void copy(FilKalman k1, FilKalman k2);

private:
    FilKalman kman;
    std::vector<FilKalman> kman_prevs;
};


#endif /* _KALMAN_H_ */
