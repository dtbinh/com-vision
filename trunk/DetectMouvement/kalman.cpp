#include "kalman.h"
#include "amat.h"
#include <iostream>

Kalman::Kalman()
{
    //For Kalman filtter
    kman.KF = KalmanFilter(4, 2, 0);
    kman.state = Mat_<float> (4, 1);
    kman.KF.transitionMatrix = *(cv::Mat_<float>(4, 4) << 1,0,1,0,
			    0,1,0,1,  0,0,1,0,  0,0,0,1);

    setIdentity(kman.KF.measurementMatrix, cv::Scalar::all(1));
    setIdentity(kman.KF.processNoiseCov, cv::Scalar::all(1e-0));
    setIdentity(kman.KF.measurementNoiseCov, cv::Scalar::all(0.1));
    setIdentity(kman.KF.errorCovPost, cv::Scalar::all(.1));
    randn(kman.KF.statePost, cv::Scalar::all(0), cv::Scalar::all(0.1));
    //End
}

void copy(FilKalman &k1, FilKalman k2)
{
    k1.KF = k2.KF;
    k2.state.copyTo(k1.state);
}

void Kalman::filters(std::vector<cv::Point2f> center, cv::Mat &image_tracking)
{
    std::vector<FilKalman> _kalmans;

    for (int i = 0; i < (int)center.size(); ++i)
    {
	filter(center[i], image_tracking);
	FilKalman _kalman;
	copy(_kalman, kman);
        _kalmans.push_back(_kalman);
    }
    kman_prevs = _kalmans;
}

void Kalman::filter(cv::Point2f center, cv::Mat &image_tracking)
{
    std::ostringstream num_str;
    cv::Mat_<float> measurement(2,1);
    cv::Mat processNoise(4, 1, CV_32F);

    //Kalman
    int state_near_id = getStateIDNearest(center);
    if(state_near_id <= -1)
    {
        return;
    }

    copy(kman, kman_prevs[state_near_id]);
    kman.state(0) = center.x;
    kman.state(1) = center.y;
    
    //Prediction
    cv::Mat prediction = kman.KF.predict();
    //std::cout<<prediction <<" ";
    cv::Point predictPt(prediction.at<float>(0),prediction.at<float>(1));

    //generate measurement
    randn( measurement, cv::Scalar::all(0),
	   cv::Scalar::all(kman.KF.measurementNoiseCov.at<float>(0)));
    measurement += kman.KF.measurementMatrix*kman.state;
    cv::Point measPt(measurement(0), measurement(1));

    //Correct
    cv::Mat estimated = kman.KF.correct(measurement);
    cv::Point correctPt(estimated.at<float>(0),estimated.at<float>(1));

    //num_str<< i <<"-p";
    AMat::drawCross(predictPt, cv::Scalar(255, 0, 0), 1, image_tracking, num_str.str());
    //num_str.str("");
    //num_str<< i <<"-m";
    AMat::drawCross(measPt, cv::Scalar(0, 255, 0), 1, image_tracking, num_str.str());
    //num_str.str("");
    //num_str<< i <<"-c";
    AMat::drawCross(correctPt, cv::Scalar(0, 0, 255), 1, image_tracking, num_str.str());
    //num_str.str("");
/*
  for (int i = 0; i < (int)predicts.size()-1; i++) {
  line(image_tracking, predicts[i], predicts[i+1], Scalar(255, 0, 0), 1);
  }
  for (int i = 0; i < (int)measures.size()-1; i++) {
  line(image_tracking, measures[i], measures[i+1], Scalar(0,255,0), 1);
  }
  for (int i = 0; i < (int)corrects.size()-1; i++) {
  line(image_tracking, corrects[i], corrects[i+1], Scalar(0, 0, 255), 1);
  }
*/
    //randn( processNoise, cv::Scalar(0),
//	   cv::Scalar::all(sqrt(kman.KF.processNoiseCov.at<float>(0, 0))));
    //   kman.state = kman.KF.transitionMatrix*kman.state + processNoise;
}

int Kalman::getStateIDNearest(cv::Point2f p)
{
    int SIZE = kman_prevs.size();
    if(SIZE <= 0)
	return -1;
    double distance = 0.0;
    double min = 9999999.0;
    double min_index = 0;
    for (int i = 0; i < SIZE; ++i)
    {
	distance = sqrt((p.x - kman_prevs[i].state(0))*(p.x - kman_prevs[i].state(0)) +
			(p.y - kman_prevs[i].state(1))*(p.y - kman_prevs[i].state(1)));
	
	if(min > distance)
	{
	    min = distance;
	    min_index = i;
	}
    }
    return min_index;
}
