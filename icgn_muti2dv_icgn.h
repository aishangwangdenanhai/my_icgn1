#pragma once

#ifndef _ICGN_H_
#define _ICGN_H_

#include "icgn_muti2dv_interpolation.h"
#include "icgn_muti2dv_roi.h"
#include "icgn_muti2dv_gradient.h"
#include "icgn_muti2dv_image.h"
#include "icgn_muti2dv_poi.h"
#include "icgn_muti2dv_point.h"
#include "icgn_muti2dv_subset.h"

namespace dic
{
	//the 1st shape function of 2D case
	class ICGN2D1_
	{
	public:
		Subset2D* ref_subset;
		Subset2D* tar_subset;
		Eigen::MatrixXf error_img;
		Matrix6f hessian, inv_hessian;
		float*** sd_img; //steepest descent image

		static ICGN2D1_* allocate(int subset_radius_x, int subset_radius_y);
		static void release(ICGN2D1_* instance);
	};

	class ICGN2D1 : public ROI
	{
	private:
		BicubicBspline* tar_interp;//计算插值的指针
		Gradient2D4* ref_gradient;//计算参考图像梯度的指针

		float conv_criterion; //convergence criterion: norm of maximum deformation increment in subset
		float stop_condition; //stop condition: max iteration

		std::vector<ICGN2D1_*> instance_pool;
		ICGN2D1_* getInstance(int tid);

	public:

		ICGN2D1(int subset_radius_x, int subset_radius_y, float conv_criterion, float stop_condition,int cpu_thread_number);
		~ICGN2D1();

		void prepareRef(); //calculate gradient maps of ref image
		void prepareTar(); //calculate interpolation coefficient look_up table of tar image
		void prepare(); //calculate gradient maps of ref image and interpolation coefficient look_up table of tar image

		void compute(POI2D* poi);
		void compute(std::vector<POI2D>& poi_queue);

		void icgn_muti_apart(std::vector<cv::Rect>& ROI_queue, cv::Mat ref, cv::Mat tar,int cpu_thread_number);
		
		void setIteration(float conv_criterion, float stop_condition);
		void setIteration(POI2D* poi);
	};

}
#endif