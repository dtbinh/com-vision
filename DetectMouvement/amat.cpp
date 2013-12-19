#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "amat.h"
#include <algorithm>
#include <iostream>
#include <stdio.h>
using namespace std;

#define SUM_LEVEL 256

int thresh = 100;
int max_thresh = 255;
RNG rng(12345);

AMat::AMat()
  :Mat()
{
}

AMat::AMat(int h, int w, int t)
    :cv::Mat(h, w, t)
{
}


AMat::AMat(int h, int w, int t, Scalar s)
    :cv::Mat(h, w, t, s)
{
}

AMat::AMat(Mat image)
  :Mat(image)
{
}

AMat::AMat(AMat& amat)
  :Mat((Mat&)amat)
{
}

AMat& AMat::operator=(const AMat& amat)
{
  Mat::operator=(amat);
  return *this;
}

AMat AMat::operator-(const AMat &other) const
{
    int M = this->rows;
    int N = this->cols;
    AMat img(M, N, CV_8UC1, Scalar(0));
    int r, g, b;
    int seuil = 50;
    int nb_chanel = this->channels();
    int nb_chanel_other = other.channels();
    if(M != other.rows || N != other.cols || nb_chanel != nb_chanel_other)
	return img;

    if(nb_chanel == 3)
    {
	for (int i = 0; i < M; ++i)
	{
	    for (int j = 0; j < N; ++j)
	    {
		//For chanel 1
		b = this->at<Vec3b>(i, j)[0] - other.at<Vec3b>(i, j)[0];

		//For chanel 2
		g = this->at<Vec3b>(i, j)[1] - other.at<Vec3b>(i, j)[1];

		//For chanel 3
		r = this->at<Vec3b>(i, j)[2] - other.at<Vec3b>(i, j)[2];

		if(b > seuil || g > seuil || r > seuil ||
		   b < -seuil || g < -seuil || r < -seuil)
		    img.at<uchar>(i, j) = b;
		
	    }
	}
    }
    if(nb_chanel == 1)
    {
	for (int i = 0; i < M; ++i)
	{
	    for (int j = 0; j < N; ++j)
	    {
		//For chanel 1
		b = this->at<uchar>(i, j) - other.at<uchar>(i, j);
		if(b > seuil || b < -seuil)
		    img.at<uchar>(i, j) = b;
	    }
	}
    }
    
    return img;
}

// plot points
void AMat::drawCross( Point center, Scalar color, double d, Mat &img, string name )
{
    //int fontFace = cv::FONT_HERSHEY_COMPLEX_SMALL;
    line( img, Point( center.x - d, center.y - d ),
	  Point( center.x + d, center.y + d ), color, 1, CV_AA, 0);
    line( img, Point( center.x + d, center.y - d ),
	  Point( center.x - d, center.y + d ), color, 1, CV_AA, 0 );
    //cv::putText(img, name, Point(center.x - 12, center.y - 5), fontFace, 0.5, color);
}


void AMat::thresh_callback(Mat src_gray, Mat &src_rgb, vector<Point2f> &center,
		     Mat &image_tracking)
{
    Mat threshold_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    
    /// Detect edges using Threshold
    threshold( src_gray, threshold_output, thresh, 255, THRESH_BINARY );
    /// Find contours
    findContours( threshold_output, contours, hierarchy, CV_RETR_TREE,
		  CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

    /// Approximate contours to polygons + get bounding rects and circles
    vector<vector<Point> > contours_poly( contours.size() );
    vector<Rect> boundRect( contours.size() );
    vector<float>radius( contours.size() );
    Point2f c;
    center.clear();

    for( int i = 0; i < (int)contours.size(); i++ )
    {
	approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
	boundRect[i] = boundingRect( Mat(contours_poly[i]) );
    }

    /// Draw bounding rectangs
    for( int i = 0; i< (int)contours.size(); i++ )
    {
	Point v = boundRect[i].tl() - boundRect[i].br();
	double l2 = v.x*v.x + v.y*v.y;
	if(l2 < 255)
	    continue;
	
	rectangle(src_rgb, boundRect[i].tl(), boundRect[i].br(),
		  Scalar(255, 0, 0), 1, 8, 0 );
	cv::minEnclosingCircle((Mat)contours_poly[i], c, radius[i]);
	center.push_back(c);
    }
}




AMat::~AMat()
{
} 
