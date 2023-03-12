#pragma once

#ifndef _GRADIENT_H_
#define _GRADIENT_H_

#include "icgn_muti2dv_array.h"
#include "icgn_muti2dv_image.h"

namespace dic
{
	class Gradient2D4
	{
	protected:
		Image2D* grad_img = nullptr;

	public:
		Eigen::MatrixXf gradient_x;
		Eigen::MatrixXf gradient_y;
		Eigen::MatrixXf gradient_xy;

		Gradient2D4(Image2D& image);
		~Gradient2D4();

		void getGradientX(); //create an array of gradient_x
		void getGradientY(); //create an array of gradient_y
		void getGradientXY(); //create an array of gradient_xy
	};

}//namespace dic

#endif //_GRADIENT_H_