#pragma once
#ifndef _DEFORMATION_H_
#define _DEFORMATION_H_

#include "icgn_muti2dv_array.h"
#include "icgn_muti2dv_point.h"

namespace dic
{
	//2D 1st order
	class Deformation2D1
	{
	public:
		float u, ux, uy;
		float v, vx, vy;
		Eigen::Matrix3f warp_matrix;

		Deformation2D1();
		Deformation2D1(float u, float ux, float uy, float v, float vx, float vy);
		Deformation2D1(float p[6]);
		~Deformation2D1();

		void setDeformation(); // set deformation according to warp_matrix
		void setDeformation(float u, float ux, float uy, float v, float vx, float vy);
		void setDeformation(float p[6]);
		void setDeformation(Deformation2D1& another_deformation);

		void setWarp(); //update warp_matrix according to deformation
		Point2D warp(Point2D& location);
	};

}//namespace dic

#endif // _DEFORMATION_H_