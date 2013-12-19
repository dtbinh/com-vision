#include <iostream>
#include <stdio.h>
#include "control.h"
#include <string>
#include <stdlib.h>
#include <dirent.h>
#include "kalman.h"
using namespace cv;
using namespace std;

#define GRAY_LEVEL 256
#define CMD_FOND "fond"
#define CMD_DETECT "detect"

Control::Control(){}

/*
  Permet de séparer le nom des fichiers appliqué et leur file
 */
int Split(vector<string>& vecteur, string chaine, char separateur)
{
    vecteur.clear();
    string::size_type stTemp = chaine.find(separateur);
    while(stTemp != string::npos)
    {
	vecteur.push_back(chaine.substr(0, stTemp));
	chaine = chaine.substr(stTemp + 1);
	stTemp = chaine.find(separateur);
    }
    vecteur.push_back(chaine);
    return vecteur.size();
}

/*
Pour attendre l'utilisateur tapper des commandes
Cette methode va traitter des commandes entrées pour
réaliser des besoin en rappant des autres méthodes correspondantes
*/
void Control::processCommand(int argc, char* argv[])
{
    int time_per_image = 10;
    string video_path, fond_name;
    string video_src, video_des;
    
    if(argc < 2)
	return;
    if(strcmp(argv[1], CMD_FOND) == 0)
    {
	if(argc < 4)
	    return;
        video_path = std::string(argv[2]);
	fond_name = std::string(argv[3]);
        this->getImageDeFond(video_path, fond_name);
    }
    if(strcmp(argv[1], CMD_DETECT) == 0)
    {
	if(argc < 5)
	    return;
	video_src = std::string(argv[2]);
        fond_name = std::string(argv[3]);
	time_per_image = atoi(argv[4]);
	this->detectVideo(video_src, fond_name, time_per_image);
    }
    return;
}

void Control::detectVideo(string video_src, string fond_path, int time_per_image)
{
    AMat mouvement, frame, frame_gray, fond;
    //vector<AMat> fonds;
    int time = 0;
    ostringstream num_str;
    string video_show = video_src + " - detect";
    Mat image_tracking;
    Mat kernel(3, 3, CV_8UC1, Scalar(255));
    //Parametter for kalman filtter
    vector<Point2f> center;
    string video_tracking = video_src + " - tracking";
    Kalman kalman;
    
    fond = cv::imread(fond_path);
    // open the video file for reading
    cv::VideoCapture cap(video_src);
    fond.copyTo(image_tracking);
    cv::cvtColor(fond, fond, CV_RGB2GRAY);
    cv::blur(fond, fond, Size(3, 3));

    if ( !cap.isOpened() )  // if not success, exit program
    {
	cout << "Cannot open the video file" << endl;
	return;
    }
    
    //get the frames per seconds of the video
    double fps = cap.get(CV_CAP_PROP_FPS);
    cout << "Frame per seconds : " << fps << endl;
    namedWindow(video_src, CV_WINDOW_AUTOSIZE);
    namedWindow(video_show, CV_WINDOW_AUTOSIZE);

    while(1)
    {
        bool bSuccess = cap.read(frame); // read a new frame from video
	if (!bSuccess) //if not success, break loop
	{
	    cout << "Cannot read the frame from video file" << endl;
	    break;
	}
	time++;	
	if(time % time_per_image == 0)
	{
	    num_str.str("");
	    num_str<<time;
	    cv::cvtColor(frame, frame_gray, CV_RGB2GRAY);
	    blur(frame_gray, frame_gray, Size(3, 3));
	    mouvement = frame_gray - fond;
	    cv::erode(mouvement, mouvement, kernel);
	    cv::dilate(mouvement, mouvement, kernel);
	    imshow(video_show, mouvement);
	    cv::imwrite("image/test3/2/" + num_str.str() + ".png", mouvement);
	    AMat::thresh_callback(mouvement, frame, center, image_tracking);
	    kalman.filters(center, image_tracking);
	    imshow(video_tracking, image_tracking);
	    cv::imwrite("image/test3/3/" + num_str.str() + ".png", image_tracking);
	}
	//blur(frame, frame, Size(3, 3));
        imshow(video_src, frame); //show the frame in "video_name" window
	cv::imwrite("image/test3/1/" + num_str.str() + ".png", frame);
	//wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
        if(waitKey(30) == 27)
	{
	    cout << "esc key is pressed by user" << endl;
	    destroyWindow(video_src);
	    destroyWindow(video_show);
	    break; 
	}
    }
    return;
}

vector<AMat> Control::capImageFromVideo(string video_name, int time_per_image)
{
    int time = 0;
    // open the video file for reading
    cv::VideoCapture cap(video_name);
    std::vector<AMat> images;
    cv::Mat frame_gray;
    if ( !cap.isOpened() )  // if not success, exit program
    {
	cout << "Cannot open the video file" << endl;
	return images;
    }    
    //get the frames per seconds of the video
    double fps = cap.get(CV_CAP_PROP_FPS);
    cout << "Frame per seconds : " << fps << endl;
    namedWindow(video_name,CV_WINDOW_AUTOSIZE);
    while(1)
    {
        Mat frame;
        bool bSuccess = cap.read(frame); // read a new frame from video
	if (!bSuccess) //if not success, break loop
	{
	    cout << "Cannot read the frame from video file" << endl;
	    break;
	}	
        imshow(video_name, frame); //show the frame in "video_name" window
	time++;
	if(time % time_per_image == 0)
	{
	    cv::cvtColor(frame, frame_gray, CV_RGB2GRAY);
	    images.push_back(frame_gray);
	}
	
	//wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
        if(waitKey(30) == 27)
	{
	    cout << "esc key is pressed by user" << endl;
	    destroyWindow(video_name);
	    break;
	}
    }
    return images;
}

bool isEqualeSize(vector<AMat> &images)
{
    int N = images.size();
    for (int i = 0; i < N-1; ++i)
    {
	if(images[i].cols != images[i+1].cols ||
	   images[i].rows != images[i+1].rows)
	{
	    return false;
	}
    }
    return true;
}

template <typename T>
T getMedian(vector<T> L)
{
    std::nth_element(L.begin(), L.begin() + L.size()/2, L.end());
    return L[L.size()/2];
}

AMat Control::getImageDeFond(vector<AMat> images) const
{
    AMat fond;
    int M, N, SIZE = images.size();
    vector<uchar> grays;
    uchar value;
    if(!isEqualeSize(images))
	return fond;
    if(SIZE > 0)
    {
        M = images[0].rows;
	N = images[0].cols;
    }
    fond = AMat(M, N, CV_8UC1);
    for (int i = 0; i < M; ++i)
    {
	for (int j = 0; j < N; ++j)
	{
	    grays.clear();
	    for (int k = 0; k < SIZE; k++)
	    {
		grays.push_back(images[k].at<uchar>(i, j));
	    }
	    value = getMedian(grays);
	    fond.at<uchar>(i, j) = value;
	}
    }
    return fond;
}

void Control::getImageDeFond(string video_path, string image_fond)
{
    vector<AMat> image_models = this->capImageFromVideo(video_path, 20);
    AMat fond = this->getImageDeFond(image_models);
    imwrite(image_fond, fond);
}
