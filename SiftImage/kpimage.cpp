#include "kpimage.h"
#include <float.h>

KPImage::KPImage()
    : cv::Mat()
{
}

KPImage::KPImage(cv::Mat img)
    :cv::Mat(img)
{
}

KeyPoint KPImage::operator[](int index)
{
    return this->key_point_list.at(index);
}

KPImage combin2Images(cv::Mat image1, cv::Mat image2)
{
    cv::Mat image(MAX(image1.rows, image2.rows),
		  image1.cols + image2.cols, CV_8UC1);
    for (int i = 0; i < image1.rows; ++i)
    {
	for (int j = 0; j < image1.cols; ++j)
	{
	    image.at<uchar>(i, j) = image1.at<uchar>(i, j);
	}
    }    
    for (int i = 0; i < image2.rows; ++i)
    {
	for (int j = 0; j < image2.cols; ++j)
	{
	    image.at<uchar>(i, image1.cols + j) = image2.at<uchar>(i, j);
	}
    }
    return image;
}

void getRectangle(cv::Point &tl, cv::Point &br,
		  std::vector<PairPoint> pairs, bool start)
{
    int N = pairs.size();
    double x_max = FLT_MIN, x_min = FLT_MAX;
    double y_max = FLT_MIN, y_min = FLT_MAX;
    for (int i = 0; i < N; ++i)
    {
	if(start)
	{
	    x_max = MAX(x_max, pairs[i].getStart().x);
	    x_min = MIN(x_min, pairs[i].getStart().x);
	    y_max = MAX(y_max, pairs[i].getStart().y);
	    y_min = MIN(y_min, pairs[i].getStart().y);
	}
	else{
	    x_max = MAX(x_max, pairs[i].getEnd().x);
	    x_min = MIN(x_min, pairs[i].getEnd().x);
	    y_max = MAX(y_max, pairs[i].getEnd().y);
	    y_min = MIN(y_min, pairs[i].getEnd().y);
	}
    }
    tl = cv::Point(x_min, y_min);
    br = cv::Point(x_max, y_max);
}

bool compare(double a, double b, double e)
{
    double d = a - b;
    return(d < e && d > -e);
}

std::vector<PairPoint> KPImage::getPairPoints(KPImage kpi)
{
    KeyPoint* kp;
    std::vector<PairPoint> pairs;
    int N = this->key_point_list.size();
    for (int i = 0; i < N; ++i)
    {
	kp = kpi.getMatchKey(this->key_point_list.at(i));
	if (kp != NULL)
	{
	    cv::Point p1(this->key_point_list[i].getX(),
			 this->key_point_list[i].getY());
	    cv::Point p2(kp->getX(), kp->getY());
	    PairPoint pair(p1, p2);
	    pairs.push_back(pair);
	    //std::cout<<pair.getAngle() <<std::endl;
	}
    }
    for (int i = 0; i < pairs.size(); ++i)
    {
	std::cout<<pairs[i].getStart() <<" " <<pairs[i].getEnd() <<std::endl;
    }
    //std::cout<<pairs.size() <<std::endl;
    //Filter the pairs
    cv::Point tl_start, br_start;
    getRectangle(tl_start, br_start, pairs, true);
    cv::Point tl_end, br_end;
    getRectangle(tl_end, br_end, pairs, false);
    std::cout<<std::endl <<tl_start <<" " <<br_start <<std::endl;
    std::cout<<std::endl <<tl_end <<" " <<br_end <<std::endl;

    double M_start = br_start.y - tl_start.y;
    double N_start = br_start.x - tl_start.x;
    double M_end = br_end.y - tl_end.y;
    double N_end = br_end.x - tl_end.x;
    double x_scale = N_start/N_end;
    double y_scale = M_start/M_end;
    //std::cout<<x_scale <<" " <<y_scale <<std::endl;
    N = pairs.size();
    std::cout<<N <<std::endl;
    std::vector<PairPoint> pair_filters;
    //cv::Mat correct(M_start, N_start, CV_8UC1);
    //cv::Mat correct_fil(M_start, N_start, CV_8UC1);
    cv::Mat correct, correct_fil;
    this->copyTo(correct);
    this->copyTo(correct_fil);
    for (int i = 0; i < N; ++i)
    {
	cv::Point p1(pairs[i].getStart());
	cv::Point2f p2_scale((double)pairs[i].getEnd().x*x_scale,
			     (double)pairs[i].getEnd().y*y_scale);
	cv::line(correct, p1, p2_scale, cv::Scalar(255, 0, 0), 1, CV_AA);
	//std::cout<<p1 <<";" <<p2_scale <<std::endl;
	if(!compare((double)p1.x, p2_scale.x, 10) ||
	   !compare((double)p1.y, p2_scale.y, 10))
	    continue;
	cv::line(correct_fil, p1, p2_scale, cv::Scalar(255, 0, 0), 1, CV_AA);
	cv::Point2f p2(pairs[i].getEnd().x, pairs[i].getEnd().y);
	pair_filters.push_back(PairPoint(p1, p2));
    }
    cv::imwrite("database/test0/correct.png", correct);
    cv::imwrite("database/test0/correct_fil.png", correct_fil);
    return pair_filters;
}

void KPImage::drawCorrect(KPImage kpi, std::vector<PairPoint> pairs,
			  std::string image_correct)
{
    int N = pairs.size();
    KPImage image = combin2Images(*this, kpi);
    for (int i = 0; i < N; ++i)
    {
	cv::Point p1(pairs[i].getStart());
	cv::Point p2(pairs[i].getEnd().x + this->cols, pairs[i].getEnd().y);
	cv::line(image, p1, p2, cv::Scalar(0), 1, CV_AA);
    }
    cv::imwrite(image_correct, image);
}

std::istream& operator>>(std::istream& is, KPImage& kpi)
{
    int N, len_discriptor;
    is>>N >>len_discriptor;
    KeyPoint kp;
    for (int i = 0; i < N; ++i)
    {
	kp.setLenDiscriptor(len_discriptor);
	is>>kp;
	kpi.key_point_list.push_back(kp);
    }
    return is;
}

/*
Chercher le keypoint qui est bien correct avec kp
Si aucun keypoint satisfait la condiction, elle va retourner NULL
 */
KeyPoint* KPImage::getMatchKey(KeyPoint kp)
{
    float d, dist1 = FLT_MAX, dist2 = FLT_MAX;
    KeyPoint *minkey;
    int N = this->key_point_list.size();
    for (int i = 0; i < N; i++) {
	d = kp.getDistance(this->key_point_list[i]);
	if (d < dist1) {
	    dist2 = dist1;
	    dist1 = d;
	    minkey = &this->key_point_list[i];
	} else if (d < dist2) {
	    dist2 = d;
	}
    }
    if (10*dist1 < 6*dist2)
    {
	return minkey;
    }
    return NULL;
}
