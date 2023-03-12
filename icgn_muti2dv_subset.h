#pragma once

#ifndef _SUBSET_H_
#define _SUBSET_H_

#include "icgn_muti2dv_array.h"
#include "icgn_muti2dv_image.h"
#include "icgn_muti2dv_point.h"

namespace dic
{
	class Subset2D
	{
	public:
		Point2D center;
		int radius_x, radius_y;
		int height, width;

		Eigen::MatrixXf eg_mat;

		Subset2D(Point2D center, int radius_x, int radius_y);
		~Subset2D() = default;

		void fill(Image2D* image);
		float zeroMeanNorm();
	};

}//namespace dic

#endif //_SUBSET_H_