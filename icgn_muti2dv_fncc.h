#pragma once

#ifndef FNCC251_H_
#define FNCC251_H_

#include"icgn_muti2dv_image.h"
using namespace cv;
using namespace std;

vector<double> TemplateMatch(const Mat* const src, Mat* temp);
void f(const Mat* const temp,int i);
void sum(const Mat* const cur_image);
vector<float> g(const Mat* cur_image, const Mat* temp, Mat sum1, Mat sum2,int ii,int iii,int n_size);
void integral(Mat src, Mat& integral_out, Mat& integral_out1);

//���궨


//��ǰ���������
static vector<double> fm_;
static double f_;
static double f2_ = f_ * f_;
static double f_fm;
static double f_fm2 = f_fm * f_fm;
static double f_A1;

static vector<double> f_C1;
static double g_;
static int searchLength_x;
static int searchLength_y;

static vector<vector<float>>result;//��ά����

//���ֱ�
static Mat sum1;
static Mat sum2;


//��������
static Point2i* pLastLastPoint;
static Point2i* pStartPoint;
//static vector<vector<Point2f>>coordinates;//���е�
static vector<Point2i>coordinate;//һά�㼯
static vector<Point2i>coordinate_search;//һά�㼯

//���궨

#endif