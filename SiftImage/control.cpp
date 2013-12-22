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

#define GRAY_LEVEL 256
#define CMD_EXT "exit"
#define CMD_HELP "help"
#define CMD_FKEY "fkey"
#define CMD_CORRECT "corr"

Control::Control()
{
}

void Control::displayWelcome()
{
    cout<<"Bienvenue d'utiliser siftimage - l'auteur: NGUYEN Quoc Khai\n";
    cout<<"==================================================\n";
}

/*
  Permet d'afficher le menu de l'application
 */
void Control::displayMenu()
{
    cout<<"Vous pourriez taper:\n";
    cout<<"1) fkey image_name key_name - "
	<<"Pour chercher des points intérêts\n";
    cout<<"2) corr img1 key1 img2 key2 img_correct- "
	<<"Pour correct entre 2 images\n";
    cout<<"3) help - pour aider\n";
    cout<<"4) exit - pour quitter\n";
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
    string cmd;
    string image_name, key_name;
    string key1, key2;
    string img1, img2, img_correct;
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
	if(cmd.compare(CMD_FKEY) == 0)
	{
	    if(size < 3)
		continue;
	    image_name = cmd_elem.at(1);
	    key_name = cmd_elem.at(2);
	    findKeyPoint(image_name, key_name);
	}
	if(cmd.compare(CMD_CORRECT) == 0)
	{
	    if(size < 6)
		continue;
	    img1 = cmd_elem.at(1);
	    key1 = cmd_elem.at(2);
	    img2 = cmd_elem.at(3);
	    key2 = cmd_elem.at(4);
	    img_correct = cmd_elem.at(5);
	    drawCorrectImages(img1, key1, img2, key2, img_correct);
	}
    }
    return;
}

/*
Cette fonction permet de detecter des keypoints des images
Elle rappelle la commande sift de l'auteur
 */
void Control::findKeyPoint(string image_name, string key_name)
{
    string cmd = "./siftDemoV4/sift<" + image_name + "> " + key_name;
    std::system(cmd.c_str());
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
    if(kp_img1.channels() > 1)
	cv::cvtColor(kp_img1, kp_img1, CV_RGB2GRAY);
    if(kp_img2.channels() > 1)
	cv::cvtColor(kp_img2, kp_img2, CV_RGB2GRAY);
    std::vector<PairPoint> pairs = kp_img2.getPairPoints(kp_img1);
    kp_img2.drawCorrect(kp_img1, pairs, img_correct);
}
