#include "mask.h"
#include <iostream>
#include <stdio.h>
#include <vector>
#include "control.h"
#include <string>
#include<dirent.h>
#include <fstream>
#include <stdlib.h>


Mask::Mask()
{
    size = 0;
}

Mask::Mask(int s, float a[][LEN])
{
    size = s;
    for (int i = 0; i < s; ++i)
    {
	for (int j = 0; j < s; ++j)
	{
	    A[i][j] = a[i][j];
	}
    }
}

Mask::Mask(Mask& m)
{
    size = m.size;
    for (int i = 0; i < size; ++i)
    {
	for (int j = 0; j < size; ++j)
	{
	    A[i][j] = m.A[i][j];
	}
    }
}


Mask& Mask::operator=(const Mask& m)
{
    size = m.size;
    for (int i = 0; i < size; ++i)
    {
	for (int j = 0; j < size; ++j)
	{
	    A[i][j] = m.A[i][j];
	}
    }
    return *this;
}


void Mask::extractMaskFromImage(cv::Mat img, int row, int col,
				int size_mask, uchar chanel)
{
    size = size_mask;
    if(row - size/2 < 0 || col - size/2 < 0)
	return;
    if(row + size/2 > img.rows-1 || col + size/2 > img.cols-1)
	return;

    for (int i = 0; i < size; ++i)
    {
	for (int j = 0; j <= size; ++j)
	{
	    A[i][j] = img.at<Vec3b>(row - size/2 + i,
				    col - size/2 + j)[chanel];
	}
    }

    return;
}

float operator*(const Mask& mask1, const Mask& mask2)
{
    if(mask1.getSize() != mask2.getSize())
	return 0.0;
    int size = mask1.getSize();
    double sum = 0.0;
    for(int i = 0; i < size; i++)
    {
	for (int j = 0; j < size; j++)
	{
	    sum += mask1.get(i, j)*mask2.get(size - i - 1, size - j - 1);
	}
    }
    sum /= size*size;
    return sum;
}

istream& operator>>(istream& is, Mask& m)
{
    is>>m.size;
    for (int i = 0; i < m.size; ++i)
    {
	for (int j = 0; j < m.size; ++j)
	{
	    is>>m.A[i][j];
	}
    }
    return is;
}

ostream& operator<<(ostream& os, const Mask& m)
{
    os<<m.size;
    for (int i = 0; i < m.size; ++i)
    {
	os<<"\n";
	for (int j = 0; j < m.size; ++j)
	{
	    os<<m.A[i][j] <<"\t";
	}
    }
    return os;
}
