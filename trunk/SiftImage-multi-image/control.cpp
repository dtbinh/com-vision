#include <iostream>
#include <stdio.h>
#include <vector>
#include "control.h"
#include <string>
#include <dirent.h>
#include <fstream>
#include <stdlib.h>
#include "kpimage.h"
#include "keypoint.h"

using namespace std;
using namespace cv;


#define NUMBER_CLASS 100
#define GRAY_LEVEL 256
#define CMD_DELET "rm"
#define CMD_EXT "exit"
#define CMD_HELP "help"
#define CMD_FKEY "fkey"
#define CMD_SCORE "corr"
#define CMD_CONVERT "conv"
#define MATRIX_CONF_PATH "database/matrix_confusion.OUT"
#define IMG_CONF_PATH "database/matrix_confusion.png"
#define IMG_CONF_VISIBLE_PATH "database/matrix_confusion_visible.png"


Control::Control()
{    
}

Control::Control(Mat img)
{
    image = img;
    
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
    cout<<"0) rm repertoire_key number_key - "
	<<"\nPour supprimer des fichiers ayant le nombre de point < number_key\n";
    cout<<"1) fkey repertoire_image repertoire_key - "
	<<"Pour chercher des points intérêts\n";

    cout<<"2) corr 0 img1 key1 img2 key2 img_correct- "
	<<"Pour correct entre 2 images\n";
    cout<<"3) corr 1 key repertoire_des_keys_test - \n"
	<<"Pour calculer le score entre l'image de"
	<<" test avec d'images d'apprentassge\n";
    cout<<"4) corr 2 repertoire_des_keys repertoire_des_keys_test - \n"
	<<"Pour calculer le score entre 2 bases d'images\n";

    cout<<"5) help - pour aider\n";
    cout<<"6) exit - pour quitter\n";
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

//Pour attendre l'utilisateur tapper des commandes
//Cette methode va traitter des commandes entrées pour
//réaliser des besoin en rappant des autres méthodes correspondantes
void Control::processCommand()
{
    string cmd_line;
    vector<string> cmd_elem;
    int size;
    int number_key;
    string cmd;
    string data_train_path, data_key_path, path;
    string key1, key2;
    int option;
    string img1, img2, img_correct;
    vector<Candidate> candidate_list;
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
	
	if(cmd.compare(CMD_DELET) == 0)
	{
	    if(size < 3)
		continue;
	    path = cmd_elem.at(1);
	    number_key = atoi(cmd_elem.at(2).c_str());
	    deleteKeySmall(path, number_key);
	}
	if(cmd.compare(CMD_FKEY) == 0)
	{
	    if(size < 3)
		continue;
	    data_train_path = cmd_elem.at(1);
	    data_key_path = cmd_elem.at(2);
	    findKeyPoint(data_train_path, data_key_path);
	}
	if(cmd.compare(CMD_SCORE) == 0)
	{
	    if(size < 4)
		continue;
	    sscanf(cmd_elem.at(1).c_str(), "%d", &option);
	    if(option == 0)
	    {
		if(size < 7)
		    continue;
		img1 = cmd_elem.at(2);
		key1 = cmd_elem.at(3);
		img2 = cmd_elem.at(4);
		key2 = cmd_elem.at(5);
		img_correct = cmd_elem.at(6);
		drawCorrectImages(img1, key1, img2, key2, img_correct);
	    }
	    else if(option == 1)
	    {
		key1 = cmd_elem.at(2);
		key2 = cmd_elem.at(3);
	        displayClassOfImage(key1, key2, candidate_list);
	    }
	    else{
		key1 = cmd_elem.at(2);
		key2 = cmd_elem.at(3);
	        classImage(key1, key2);
	    }
	}
	candidate_list.clear();
    }
    return;
}

/*
  Cette fonction permet de supprimer des objets qui a moins de
  number_key descripteurs
*/
void Control::deleteKeySmall(string key_path, int number_key)
{
    DIR *pDIR;
    struct dirent *entry;
    string key_name;
    string cmd;
    int nb_key;
    ifstream ifs;
    pDIR=opendir(key_path.c_str());
    if(pDIR){
	while((entry = readdir(pDIR))){ //quand pas encore lire tous les fichiers
	    //. et .. est le répertoire actuel et le père
	    if( strcmp(entry->d_name, ".") != 0 && 
		strcmp(entry->d_name, "..") != 0 )
	    {
		key_name = key_path + "/" + entry->d_name;
		ifs.open(key_name.c_str());
		ifs>>nb_key;
		cmd = "rm " + key_name;
		ifs.close();
		if(nb_key < number_key)
		{
		    cout<<cmd <<endl;
		    std::system(cmd.c_str());
		}
	    }
	}
	closedir(pDIR);
    }
}

/*
  Cette fonction permet de detecter des keypoints des images
  Elle rappelle la commande sift de l'auteur
  data_train_path est le repertoire de base d'image
  data_key_path est le repertoire de keypoint ou on veut enregistrer
*/
void Control::findKeyPoint(string data_train_path, string data_key_path)
{
    DIR *pDIR;
    struct dirent *entry;
    string img_path;
    string key_path;
    string key_name;
    string cmd;
    vector<string> path_elem, name_elem;
    Split(path_elem, data_train_path, '/');
    pDIR=opendir(data_train_path.c_str());
    if(pDIR){
	while((entry = readdir(pDIR))){ //quand pas encore lire tous les fichiers
	    //. et .. est le répertoire actuel et le père
	    if( strcmp(entry->d_name, ".") != 0 && 
		strcmp(entry->d_name, "..") != 0 )
	    {
		Split(name_elem, entry->d_name, '.');
		key_name = name_elem.at(0) + ".key";
		img_path = data_train_path + "/" + entry->d_name;
		key_path = data_key_path + "/" + key_name;
		cmd = "./siftDemoV4/sift<" + img_path + "> " + key_path;
		std::system(cmd.c_str());
	    }
	}
	closedir(pDIR);
    }
}

float Control::calScore2Images(string key_img1, string key_img2)
{    
    ifstream ifs1(key_img1.c_str());
    ifstream ifs2(key_img2.c_str());
    KPImage kp_img1, kp_img2;
    ifs1>>kp_img1;
    ifs2>>kp_img2;
    float score = kp_img2.getScoreCorrect(kp_img1);
    return score;
}

void Control::drawCorrectImages(string img1, string key_img1,
				string img2, string key_img2,
				string img_correct)
{   
    ifstream ifs1(key_img1.c_str());
    ifstream ifs2(key_img2.c_str());
    KPImage kp_img1(imread(img1)), kp_img2(imread(img2));
    ifs1>>kp_img1;
    ifs2>>kp_img2;
    cv::cvtColor(kp_img1, kp_img1, CV_RGB2GRAY);
    cv::cvtColor(kp_img2, kp_img2, CV_RGB2GRAY);
    float score = kp_img2.drawCorrect(kp_img1, img_correct);
    cout<<score*100 <<"%" <<endl;
}

//Prendre le nom de l'objet qui est le plus correct
//de l'objet de test
string getCandidateMax(vector<Candidate> &candidates)
{
    int N = candidates.size();
    string image_name = "";
    float score_max = 0;
    for (int i = 0; i < N; ++i)
    {
	if(score_max < candidates[i].score)
	{
	    score_max = candidates[i].score;
	    image_name = candidates[i].image_name;
	}
    }
    return image_name;
}

/*
  Par rapport le nom de l'objet, on connait la classe de l'objet
  grace a cette fonction
*/
int classImageFromName(string image_name)
{
    if(image_name.compare("") == 0)
	return -1;
    string::size_type _pos = 0;
    _pos = image_name.find('_');
    image_name = image_name.substr(0, _pos);
    return atoi(image_name.substr(3, _pos).c_str());
}

/*
  Pour classifier un objet vers la base de modele
  elle retourne id de classe et la liste des candidat
  c'est a dire des objet de modele avec des scores
*/
int Control::classImage(string key_img1,
			string key_path2, vector<Candidate> &candidates)
{
    Candidate candidate;
    DIR *pDIR;
    vector<string> elem;
    string key_img2;
    string image_name = "";
    struct dirent *entry;
    pDIR=opendir(key_path2.c_str());
    if(pDIR){
	while((entry = readdir(pDIR))){ //quand pas encore lire tous les fichiers
	    //. et .. est le répertoire actuel et le père
	    if( strcmp(entry->d_name, ".") != 0 && 
		strcmp(entry->d_name, "..") != 0 )
	    {
		key_img2 = key_path2 + "/" + entry->d_name;
		candidate.score = calScore2Images(key_img1, key_img2);
		candidate.image_name = entry->d_name;
		candidates.push_back(candidate);
	    }
	}
	closedir(pDIR);
    }
    image_name = getCandidateMax(candidates);
    return classImageFromName(image_name);
}

void sort(vector<Candidate> &candidates)
{
    int N = candidates.size();
    for (int i = N-1; i > 0; --i)
    {
	for (int j = 0; j < i; ++j)
	{
	    if(candidates[j].score < candidates[j+1].score)
		std::swap(candidates[j], candidates[j+1]);
	}
    }
}

void Control::displayClassOfImage(string key_img1, string key_path2,
				  vector<Candidate> candidates)
{
    vector<Candidate> _candidates = candidates;
    cout<<"Classe de l'image: " <<endl;
    cout<<this->classImage(key_img1, key_path2, candidates) <<endl;
    sort(candidates);
    for (int i = 0; i < 5; ++i)
    {
	cout<<candidates[i].image_name <<"("
	    <<candidates[i].score <<")"<<" "
	    <<endl;
    }
}


void init(int element[][NUMBER_CLASS], int M, int N)
{
    for (int i = 0; i < M; ++i)
    {
	for (int j = 0; j < N; ++j)
	{
	    element[i][j] = 0;
	}
    }
}

int getMax(int confusion_element[][NUMBER_CLASS])
{
    int max = 0;
    for (int i = 0; i < NUMBER_CLASS; ++i)
    {
	for (int j = 0; j < NUMBER_CLASS; ++j)
	{
	    if(max < confusion_element[i][j])
		max = confusion_element[i][j];
	}	
    }
    return max;
}

/*
  Permet de classifier toutes les images dans la base de test
  vers toutes les images dans la base d'apprentissage
*/
void Control::classImage(string key_path1, string key_path2)
{
    vector<Candidate> candidates;
    int confusion_element[NUMBER_CLASS][NUMBER_CLASS];
    DIR *pDIR;
    int max_value;
    int class_correct, class_test;
    vector<string> elem;
    vector<string> image_name;
    string key_img1;
    struct dirent *entry;
    init(confusion_element, NUMBER_CLASS, NUMBER_CLASS);
    pDIR = opendir(key_path1.c_str());
    if(pDIR)
    {
	while((entry = readdir(pDIR)))
	{
	    if( strcmp(entry->d_name, ".") != 0 && 
		strcmp(entry->d_name, "..") != 0 )
	    {
	        key_img1 = key_path1 + "/" + entry->d_name;
		string _s(entry->d_name);
		class_correct = classImageFromName(_s);
		class_test = classImage(key_img1, key_path2, candidates);
		if(class_correct == -1 || class_test == -1)
		    continue;
		confusion_element[class_correct - 1][class_test - 1]++;
		candidates.clear();
	    }
	}
	closedir(pDIR);
    }
    ofstream ofs(MATRIX_CONF_PATH);
    cv::Mat image_confusion(NUMBER_CLASS, NUMBER_CLASS, CV_8UC1);
    cv::Mat image_confusion_visible(NUMBER_CLASS, NUMBER_CLASS, CV_8UC1);
    max_value = getMax(confusion_element);
    vector<Candidate> list_temp;
    for (int i = 0; i < NUMBER_CLASS; ++i)
    {
	for (int j = 0; j < NUMBER_CLASS; ++j)
	{
	    ofs<<confusion_element[i][j] <<" ";
	    image_confusion.at<uchar>(i, j) = confusion_element[i][j];
	    image_confusion_visible.at<uchar>(i, j) =
		confusion_element[i][j]*255/max_value;
	}
	ofs<<endl;	
    }
    imwrite(IMG_CONF_PATH, image_confusion);
    imwrite(IMG_CONF_VISIBLE_PATH, image_confusion_visible);
    
    return;
}
