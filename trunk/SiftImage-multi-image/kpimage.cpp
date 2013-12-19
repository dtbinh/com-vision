#include "kpimage.h"
#include <float.h>

KPImage::KPImage()
    : cv::Mat()
{
    //this->key_point_list.size();
}

KPImage::KPImage(cv::Mat img)
    :cv::Mat(img)
{
}

KeyPoint KPImage::operator[](int index)
{
    return this->key_point_list.at(index);
}

int KPImage::getNBKeyPoint()
{
    return this->key_point_list.size();
}

void KPImage::addKeyPoint(KeyPoint kp)
{
    this->key_point_list.push_back(kp);
}

/*
  Pour calculer le score entre deux objets
*/
float KPImage::getScoreCorrect(KPImage kpi)
{
    KeyPoint* kp;
    int nb_reussi = 0;
    int N = this->getNBKeyPoint();
    for (int i = 0; i < N; ++i)
    {
	kp = kpi.getMatchKey(this->key_point_list.at(i));
	if (kp != NULL)
	{
	    nb_reussi++;
	}
    }
    return ((float)nb_reussi)/N;
}

KPImage combin2Images(cv::Mat image1, cv::Mat image2)
{
    cv::Mat image(image1.rows + image2.rows,
		  MAX(image1.cols , image2.cols), CV_8UC1);
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
	    image.at<uchar>(image1.rows + i, j) = image2.at<uchar>(i, j);
	}
    }
    return image;
}

float KPImage::drawCorrect(KPImage kpi, std::string image_correct)
{
    KeyPoint* kp;
    int nb_reussi = 0;
    int N = this->getNBKeyPoint();
    KPImage image = combin2Images(*this, kpi);
    for (int i = 0; i < N; ++i)
    {
	kp = kpi.getMatchKey(this->key_point_list.at(i));
	if (kp != NULL)
	{
	    nb_reussi++;
	    cv::Point2f p1(this->key_point_list.at(i).getX(),
			   this->key_point_list.at(i).getY());
	    cv::Point2f p2(kp->getX(), kp->getY() + this->rows);
	    cv::line(image, p1, p2, cv::Scalar(255), 1, CV_AA);
	}
    }
    cv::imwrite(image_correct, image);
    return ((float)nb_reussi)/N;
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
	kpi.addKeyPoint(kp);
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
    int N = this->getNBKeyPoint();
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
	return minkey;
    return NULL;
}

