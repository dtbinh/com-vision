#include <iostream>
#include <stdio.h>
#include <vector>
#include "control.h"
#include <string>
#include<dirent.h>
#include <fstream>
#include <stdlib.h>

using namespace std;
using namespace cv;


#define GRAY_LEVEL 256
#define CMD_EXT "exit"
#define CMD_HELP "help"
#define CMD_SKIN "skin"
#define CMD_TRAIN "train"
#define CMD_COMP "comp"
#define EVALUATION_PATH "database/evaluation.OUT"
#define MASK_PATH "database/mask.INP"
#define HIST_SKIN_PATH "database/histogram/hist_skin.png"
#define HIST_NO_SKIN_PATH "database/histogram/hist_no_skin.png"
#define HIST_SKIN_FILE_PATH "database/histogram/hist_skin.OUT"
#define HIST_NO_SKIN_FILE_PATH "database/histogram/hist_no_skin.OUT"

Control::Control()
{
}

Control::Control(Mat img)
{
    image = img;
    
}

Mat Control::getImage()
{
    return image;
}


void Control::displayWelcome()
{
    cout<<"Bienvenue d'utiliser ctimage - l'auteur: NGUYEN Quoc Khai\n";
    cout<<"==================================================\n";
}

/*
  Permet d'afficher le menu de l'application
 */
void Control::displayMenu()
{
    cout<<"Vous pourriez taper:\n";
    cout<<"1) train repertoire_entrainner";
    cout<<"repertoire_entrainner niveau_gris - "
	<<"\n\tPour entrainer l'application\n";
    cout<<"2) skin [0] image_test niveau_gris - "
	<<"\n\tPour detection de peau par la methode simple\n";
    cout<<"3) skin 1 image_test p_seul niveau_gris - "
	<<"\n\tPour detection de peau par la methode Bayes\n";
    cout<<"4) skin 2 image_test niveau_gris - "
	<<"\n\tPour detection de peau par la methode RGB\n";
    cout<<"5) comp image_peau image_test - "
	<<"\n\tPour comparaision l'image de test avec l'image de peau\n";
    cout<<"6) help - pour aider\n";
    cout<<"7) exit - pour quitter\n";
    cout<<"=================================================\n";
}

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


void convolution(Mat& img, Mask mask)
{
    Mat image;
    img.copyTo(image);
    int cols = image.cols;
    int rows = image.rows;
    int size = mask.getSize();
    Mask mask_temp;
    if(cols < size || rows < size)
	return;
    for (int i = size/2; i < rows-size/2; i++)
    {
	for (int j = size/2; j < cols-size/2; j++)
	{
	    for (int k = 0; k < 3; ++k)
	    {
		mask_temp.extractMaskFromImage(image, i, j, size, k);
		float convolution = mask_temp*mask;
		img.at<Vec3b>(i, j)[k] = convolution;
	    }
	}
    }
}


void compareImage(string img_skin_name, string img_test_name)
{
    Mat img_skin = imread(img_skin_name, -1);
    Mat img_test = imread(img_test_name, -1);
    int M1 = img_skin.rows;
    int N1 = img_skin.cols;
    int M2 = img_test.rows;
    int N2 = img_test.cols;
    Vec3b value_skin, value_test;
    double nb_correct = 0;
    double nb_detect_no_skin = 0;
    double nb_skin_no_detect = 0;
    double nb_skin = 0;
    int m_min = M1 < M2 ? M1 : M2;
    int n_min = N1 < N2 ? N1 : N2;

    for (int i = 0; i < M1; ++i)
    {
	for (int j = 0; j < N1; ++j)
	{
	    value_skin = img_skin.at<Vec3b>(i, j);
	    if(value_skin[0] > 0 || value_skin[1] > 0 || value_skin[2] > 0)
		nb_skin++;
	}
    }
    
    for (int i = 0; i < m_min; ++i)
    {
	for (int j = 0; j < n_min; ++j)
	{
	    value_skin = img_skin.at<Vec3b>(i, j);
	    value_test = img_test.at<Vec3b>(i, j);
	    
	    if((value_skin[0] > 0 || value_skin[1] > 0||value_skin[2] > 0) &&
	       (value_test[0] > 0 || value_test[1] > 0 || value_test[2] > 0))
		nb_correct++;
	    if((value_skin[0] > 0 || value_skin[1] > 0||value_skin[2] > 0) &&
	       (value_test[0] == 0 && value_test[1] == 0 && value_test[2] == 0))
		nb_skin_no_detect++;
	    if((value_skin[0] == 0 && value_skin[1] == 0 && value_skin[2]== 0) &&
	       (value_test[0] > 0 || value_test[1] > 0 || value_test[2] > 0))
		nb_detect_no_skin++;
	}
    }

    ofstream ofs(EVALUATION_PATH);
    ofs<<"Correct: "
       <<nb_correct/(nb_skin/100) <<"%" <<endl;
    ofs<<"Detect non peau: "
       <<nb_detect_no_skin/(nb_skin/100) <<"%" <<endl;
    ofs<<"Peau non detect: "
       <<nb_skin_no_detect/(nb_skin/100) <<"%" <<endl;
    cout<<"Correct: "
	<<nb_correct/(nb_skin/100) <<"%" <<endl;
    cout<<"Detect non peau: "
	<<nb_detect_no_skin/(nb_skin/100) <<"%" <<endl;
    cout<<"Peau non detect: "
	<<nb_skin_no_detect/(nb_skin/100) <<"%" <<endl;
}

/*
  Permet de lire toutes les images d'entrainner le programme
  Les images sont mises dans un répertoire
  Les images sont mises dans un seul répertoire
  data[][] est pour enregistrer l'histogramme
  data_train_path est le chemin du répertoire
  nb_pixel est le nombre de pixel total (peau et non peau)
  nb_pixel_skin_no_skin est le nombre de pixel peau ou non peau,
  ça depend ce qu'on veut prendre
 */
void minimizeScaleImageLab(Mat& image, int level);
void readDataTrainning(double data[][GRAY_LEVEL],
		       string data_train_path, int level,
		       long& nb_pixel, long& nb_pixel_skin_no_skin)
{
    Mat img_temp;
    int M;
    int N;
    DIR *pDIR;
    int l = 0;
    int a = 0;
    int b = 0;
    nb_pixel_skin_no_skin = 0;
    Mask mask;
    ifstream ifs(MASK_PATH);
    ifs>>mask;
    struct dirent *entry;
    pDIR=opendir(data_train_path.c_str());
    if(pDIR){
	while((entry = readdir(pDIR))){ //quand pas encore lire tous les fichiers
	    //. et .. est le répertoire actuel et le père
	    if( strcmp(entry->d_name, ".") != 0 && 
		strcmp(entry->d_name, "..") != 0 )
	    {
		img_temp = imread(data_train_path + "/" + entry->d_name, 1);
		convolution(img_temp, mask);
		cvtColor(img_temp, img_temp, CV_RGB2Lab);
		minimizeScaleImageLab(img_temp, level);
		M = img_temp.rows;
		N = img_temp.cols;

		for (int i = 0; i < M; ++i)
		{
		    for (int j = 0; j < N; ++j)
		    {
			l = img_temp.at<Vec3b>(i, j)[0];
			a = img_temp.at<Vec3b>(i, j)[1];
			b = img_temp.at<Vec3b>(i, j)[2];
			if(l > 0 && a != 128 && b != 128)
			{
			    nb_pixel_skin_no_skin++;
			    nb_pixel++;
			    data[a][b]++;
			}
		    }
		}
	    }
	}
	closedir(pDIR);
    }
}

//Permet de lire l'image et séparer et récupérer le nom et la file
void readImage(Mat& image, string image_name,
	       string& name, string& extra)
{
    image = imread(image_name, -1);
    if(!image.data)
    {
	cout<<"Impossible de acceder l'image";
	return;
    }
    vector<string> name_elem;
    Split(name_elem, image_name, '.');
    name = name_elem.at(0);
    extra = name_elem.at(1);
}


//Pour attendre l'utilisateur tapper des commandes
//Cette methode va traitter des commandes entrées pour
//réaliser des besoin en rappant des autres méthodes correspondantes
void Control::processCommand()
{
    string cmd_line;
    vector<string> cmd_elem;
    int size;
    string cmd;

    string img_train_dir_skin_name;
    string img_train_dir_no_skin_name;
    string img_test_name;
    string img_skin_name;
    int option = 0;
    int level = 0;
    float p = 0.0;

    while(true)
    {
	cout<<"Tapper: ";
	getline(cin, cmd_line);
	size = Split(cmd_elem, cmd_line, ' ');
	if(size <= 0)
	    continue;
	cmd = cmd_elem.at(0);
	if (cmd.compare(CMD_EXT) == 0)
	    return;
	if(cmd.compare(CMD_HELP) == 0)
	    displayMenu();
	if(cmd.compare(CMD_TRAIN) == 0)
	{
	    if(size < 4)
		continue;
	    img_train_dir_skin_name = cmd_elem.at(1);
	    img_train_dir_no_skin_name = cmd_elem.at(2);
	    sscanf(cmd_elem.at(3).c_str(), "%d", &level);
	    trainning(img_train_dir_skin_name,
		      img_train_dir_no_skin_name, level);
	}
	if(cmd.compare(CMD_SKIN) == 0){
	    if(size < 3)
		continue;
	    if(size < 4)
	    {
		img_test_name = cmd_elem.at(1);
		sscanf(cmd_elem.at(2).c_str(), "%d", &level);		
		detectSkinSimple(img_test_name, level);
	    }
	    else
	    {
		sscanf(cmd_elem.at(1).c_str(), "%d", &option);
		if(option == 0)
		{
		    img_test_name = cmd_elem.at(2);
		    sscanf(cmd_elem.at(3).c_str(), "%d", &level);
		    detectSkinSimple(img_test_name, level);
		}
		else if(option == 1){
		    img_test_name = cmd_elem.at(2);
		    sscanf(cmd_elem.at(3).c_str(), "%f", &p);
		    sscanf(cmd_elem.at(4).c_str(), "%d", &level);
		    detectSkinBayes(img_test_name, p, level);
		}
		else{
		    img_test_name = cmd_elem.at(2);
		    sscanf(cmd_elem.at(3).c_str(), "%d", &level);
		    detectSkinRGB(img_test_name, level);
		}
	    }
	}
	if(cmd.compare(CMD_COMP) == 0)
	{
	    if(size < 3)
		continue;
	    img_skin_name = cmd_elem.at(1);
	    img_test_name = cmd_elem.at(2);
	    compareImage(img_skin_name, img_test_name);
	}
    }
    return;
}

//Pour initialiser les données
void initHistData(double data[][GRAY_LEVEL], int N)
{
    for (int i = 0; i < N; i++)
    {
	for (int j = 0; j < N; ++j)
	{
	    data[i][j] = 0.0;
	}
    }
}

//Prendre la valeur maximum
double getMaxValueHistData(double data[][GRAY_LEVEL], int N)
{
    double value_max = 0;
    for (int i = 0; i < N; i++)
    {
	for (int j = 0; j < N; ++j)
	{
	    if(data[i][j] > value_max)
		value_max = data[i][j];
	}
    }
    return value_max;
}

void minimizeScaleImageRGB(Mat& image, int level)
{
    int size = GRAY_LEVEL/level;
    int k;
    int M = image.rows;
    int N = image.cols;
    int r, g, b;
    for (int i = 0; i < M; i++)
    {
	for (int j = 0; j < N; j++)
	{
	    b = image.at<Vec3b>(i, j)[0];
	    g = image.at<Vec3b>(i, j)[1];
	    r = image.at<Vec3b>(i, j)[2];
	    k = b/size;
	    image.at<Vec3b>(i, j)[0] = k*size;
	    k = g/size;
	    image.at<Vec3b>(i, j)[1] = k*size;
	    k = r/size;
	    image.at<Vec3b>(i, j)[2] = k*size;
	}
    }
}

void minimizeScaleImageLab(Mat& image, int level)
{
    int size = GRAY_LEVEL/level;
    int k;
    int M = image.rows;
    int N = image.cols;
    int a, b;
    for (int i = 0; i < M; i++)
    {
	for (int j = 0; j < N; j++)
	{
	    a = image.at<Vec3b>(i, j)[1];
	    b = image.at<Vec3b>(i, j)[2];
	    k = a/size;
	    image.at<Vec3b>(i, j)[1] = k*size;
	    k = b/size;
	    image.at<Vec3b>(i, j)[2] = k*size;
	}
    }
}

void normalizeHistData(double data[][GRAY_LEVEL], int N, long nb_pixel)
{
    for (int i = 0; i < N; i++)
    {
	for (int j = 0; j < N; ++j)
	{
	    data[i][j] = data[i][j]/nb_pixel;
	}
    }
}


//Les histogramme enregistreraient pour que l'on ne doit pas recalculer
//Si on lire directement dans image de histogram
//C'est pas exact, car dans image, on enregistre des entiers
void writeHistogramToFile(double data[][GRAY_LEVEL],
			  string file_name, int N,
			  long nb_pixel, long& nb_pixel_skin_no_skin)
{
    ofstream ofs(file_name.c_str());
    ofs<<nb_pixel <<" " <<nb_pixel_skin_no_skin <<endl;
    for (int i = 0; i < N; i++)
    {
	for (int j = 0; j < N; ++j)
	{
	    if(data[i][j] > 255)
		data[i][j] = 255;
	    ofs<<data[i][j] <<" ";
	}
    }
}

void readHistogramFromFile(double data[][GRAY_LEVEL],
			   string file_name, int N)
{
    ifstream ifs(file_name.c_str());
    ifs>>data[0][0] >>data[0][0];
    for (int i = 0; i < N; i++)
    {
	for (int j = 0; j < N; ++j)
	{
	    ifs>>data[i][j];
	}
    }
}


void readHistogramFromFile(double data[][GRAY_LEVEL],
			   string file_name, int N,
			   long& nb_pixel,
			   long& nb_pixel_skin_no_skin)
{
    ifstream ifs(file_name.c_str());
    ifs>>nb_pixel >>nb_pixel_skin_no_skin;
    for (int i = 0; i < N; i++)
    {
	for (int j = 0; j < N; ++j)
	{
	    ifs>>data[i][j];
	}
    }
}

//affichier image de histogram dans le disque
void dessinHistogram(double data[][GRAY_LEVEL], int N,
		     long nb_pixel, string hist_name)
{
    Mat img_hist(N, N, CV_8UC3);
    cvtColor(img_hist, img_hist, CV_RGB2GRAY);
    double value_max = getMaxValueHistData(data, N);
    for (int i = 0; i < N; i++)
    {
	for (int j = 0; j < N; ++j)
	{
	    img_hist.at<uchar>(i, j) = (uchar)(data[i][j] * 255/value_max);
	}
    }
    imwrite(hist_name, img_hist);
}

//Pour entrainer le programme
//Apres avoir construit l'histogramme, on va enregistrer
void Control::trainning(string img_train_dir_skin_name,
			string img_train_dir_no_skin_name,
			int level)
{
    Mat img_test;
    string img_name;
    string img_extra;
    long nb_pixel = 0;
    long nb_pixel_skin;
    long nb_pixel_no_skin;
    double histogram_skin[GRAY_LEVEL][GRAY_LEVEL];
    double histogram_no_skin[GRAY_LEVEL][GRAY_LEVEL];
    initHistData(histogram_skin, GRAY_LEVEL);
    initHistData(histogram_no_skin, GRAY_LEVEL);
    
    readDataTrainning(histogram_skin,
		      img_train_dir_skin_name, level,
		      nb_pixel, nb_pixel_skin);
    readDataTrainning(histogram_no_skin,
		      img_train_dir_no_skin_name, level,
		      nb_pixel, nb_pixel_no_skin);
    normalizeHistData(histogram_skin, GRAY_LEVEL, nb_pixel);
    normalizeHistData(histogram_no_skin, GRAY_LEVEL, nb_pixel);

    writeHistogramToFile(histogram_skin,
			 HIST_SKIN_FILE_PATH,
			 GRAY_LEVEL, nb_pixel, nb_pixel_skin);
    writeHistogramToFile(histogram_no_skin,
			 HIST_NO_SKIN_FILE_PATH, GRAY_LEVEL,
			 nb_pixel, nb_pixel_no_skin);
    dessinHistogram(histogram_skin, GRAY_LEVEL,
		    nb_pixel, HIST_SKIN_PATH);
    dessinHistogram(histogram_no_skin, GRAY_LEVEL,
		    nb_pixel, HIST_NO_SKIN_PATH);
}

//Sert a detecter la peau dans une image
//en appliquant l'équation simple dans le tp
void Control::detectSkinSimple(string img_test_name, int level)
{
    Mat img_test;
    string img_name;
    string img_extra;
    double histogram_skin[GRAY_LEVEL][GRAY_LEVEL];
    double histogram_no_skin[GRAY_LEVEL][GRAY_LEVEL];
    ifstream ifs(MASK_PATH);
    Mask mask;
    ifs>>mask;
    initHistData(histogram_skin, GRAY_LEVEL);
    initHistData(histogram_no_skin, GRAY_LEVEL);
    readImage(img_test, img_test_name, img_name, img_extra);

    readHistogramFromFile(histogram_skin,
			  HIST_SKIN_FILE_PATH, GRAY_LEVEL);
    readHistogramFromFile(histogram_no_skin,
			 HIST_NO_SKIN_FILE_PATH, GRAY_LEVEL);
    convolution(img_test, mask);
    cvtColor(img_test, img_test, CV_RGB2Lab);
    minimizeScaleImageLab(img_test, level);
    int M = img_test.rows;
    int N = img_test.cols;
    int a = 0;
    int b = 0;
    double p_c_skin, p_c_no_skin;
    for (int i = 0; i < M; ++i)
    {
	for (int j = 0; j < N; ++j)
	{
	    a = img_test.at<Vec3b>(i, j)[1];
	    b = img_test.at<Vec3b>(i, j)[2];
	    p_c_skin = histogram_skin[a][b];
	    p_c_no_skin = histogram_no_skin[a][b];
	    if(p_c_skin <= p_c_no_skin)
	    {
		img_test.at<Vec3b>(i, j)[0] = 0;
		img_test.at<Vec3b>(i, j)[1] = 128;
		img_test.at<Vec3b>(i, j)[2] = 128;
	    }
	}
    }
    cvtColor(img_test, img_test, CV_Lab2RGB);
    imwrite(img_name + "_detect." + img_extra, img_test);
}

//Sert a detecter la peau dans une image
//en appliquant l'équation Bayes dans l'article référence
void Control::detectSkinBayes(string img_test_name, float p, int level)
{
    Mat img_test;
    string img_name;
    string img_extra;
    long nb_pixel = 0;
    long nb_pixel_skin;
    long nb_pixel_no_skin;
    double histogram_skin[GRAY_LEVEL][GRAY_LEVEL];
    double histogram_no_skin[GRAY_LEVEL][GRAY_LEVEL];
    ifstream ifs(MASK_PATH);
    Mask mask;
    ifs>>mask;
    initHistData(histogram_skin, GRAY_LEVEL);
    initHistData(histogram_no_skin, GRAY_LEVEL);
    readImage(img_test, img_test_name, img_name, img_extra);

    readHistogramFromFile(histogram_skin,
			  HIST_SKIN_FILE_PATH, GRAY_LEVEL,
			  nb_pixel, nb_pixel_skin);
    readHistogramFromFile(histogram_no_skin,
			  HIST_NO_SKIN_FILE_PATH, GRAY_LEVEL,
			  nb_pixel, nb_pixel_no_skin);
    convolution(img_test, mask);
    cvtColor(img_test, img_test, CV_RGB2Lab);
    minimizeScaleImageLab(img_test, level);
    int M = img_test.rows;
    int N = img_test.cols;
    int a = 0;
    int b = 0;
    double p_a_b_skin;
    double p_c_skin, p_c_no_skin;
    double p_skin = (double)nb_pixel_skin/(double)nb_pixel;
    double p_no_skin = (double)nb_pixel_no_skin/(double)nb_pixel;
    for (int i = 0; i < M; ++i)
    {
	for (int j = 0; j < N; ++j)
	{
	    a = img_test.at<Vec3b>(i, j)[1];
	    b = img_test.at<Vec3b>(i, j)[2];
	    p_c_skin = histogram_skin[a][b];
	    p_c_no_skin = histogram_no_skin[a][b];
	    p_a_b_skin = 0.0;
	    if(p_c_skin*p_skin + p_c_no_skin*p_no_skin != 0)
		p_a_b_skin =
		    p_c_skin*p_skin/(p_c_skin*p_skin + p_c_no_skin*p_no_skin);
	    if(p_a_b_skin < p)
	    {
		img_test.at<Vec3b>(i, j)[0] = 0;
		img_test.at<Vec3b>(i, j)[1] = 128;
		img_test.at<Vec3b>(i, j)[2] = 128;
	    }
	}
    }
    cvtColor(img_test, img_test, CV_Lab2RGB);
    imwrite(img_name + "_bayes_detect." + img_extra, img_test);
}


int getMaxFromArray(int data[], int N)
{
    if(N <= 0)
	return -1;
    int max = data[0];
    for (int i = 1; i < N; ++i)
    {
	if(data[i] > max)
	    max = data[i];
    }
    return max;
}

int getMinFromArray(int data[], int N)
{
    if(N <= 0)
	return -1;
    int min = data[0];
    for (int i = 1; i < N; ++i)
    {
	if(data[i] < min)
	    min = data[i];
    }
    return min;
}

void Control::detectSkinRGB(string img_test_name, int level)
{
    Mat img_test;
    string img_name;
    string img_extra;
    Mask mask;
    readImage(img_test, img_test_name, img_name, img_extra);
    minimizeScaleImageRGB(img_test, level);
    ifstream ifs(MASK_PATH);
    ifs>>mask;
    convolution(img_test, mask);
    int M = img_test.rows;
    int N = img_test.cols;
    uchar r, g, b;
    Vec3b color;
    int max, min;
    for (int i = 0; i < M; ++i)
    {
	for (int j = 0; j < N; ++j)
	{
	    color = img_test.at<Vec3b>(i, j);
	    b = color[0];
	    g = color[1];
	    r = color[2];
	    int data[] = {b, g, r};
	    max = getMaxFromArray(data, 3);
	    min = getMinFromArray(data, 3);
	    if (!(r > 95 && g > 40 && b > 20 && (max - min) > 15 &&
		  abs(r-g) > 15 && r > g && r > b))
	    {
		img_test.at<Vec3b>(i, j)[0] = 0;
		img_test.at<Vec3b>(i, j)[1] = 0;
		img_test.at<Vec3b>(i, j)[2] = 0;
	    }
	}
    }
    imwrite(img_name + "_rgb_detect." + img_extra, img_test);
}
