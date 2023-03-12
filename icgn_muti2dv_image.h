#pragma once
#ifndef _IMAGE_H_
#define _IMAGE_H_

#include "icgn_muti2dv_array.h"
#include <opencv2/core/eigen.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/eigen.hpp>



using std::string;

namespace dic
{
	class Image2D
	{
	public:
		int height, width;
		string file_path;
		
		cv::Mat cv_mat;
		Eigen::MatrixXf eg_mat;
		cv::Mat cv_mat1;
		Eigen::MatrixXf eg_mat1;

		Image2D(cv::Mat img);
		Image2D(int width, int height);
		Image2D(string file_path);
		~Image2D();//= default;

		void load(string file_path);
	};

}//namespace dic

#endif //_IMAGE_H_