#pragma once
#ifndef _ICGN_HANDLE_
#define _ICGN_HANDLE_
#include "icgn_muti2dv_fncc.h"
#include "icgn_muti2dv_icgn.h"
#include <fstream>

namespace dic
{
	class ICGN2D1_HANDLE
	{
	public:
		ICGN2D1_HANDLE();
		~ICGN2D1_HANDLE();
		void icgn_muti_apart(std::vector<cv::Rect>& ROI_queue, cv::Mat ref, cv::VideoCapture capture, int cpu_thread_number, int box_radius1, int box_width_height1,
			int subset_radius_x1, int subset_radius_y1, int max_iteration1, float max_deformation_norm,string filePath, int fncc_ROI2_zen_radius);

		std::vector<cv::Rect> process_frame(vector<Mat>fncc_frame1, vector<Mat>fncc_temp, vector<Image2D>ref_img, vector<Image2D>tar_img,
			int& cpu, Mat& frame, vector<Rect>ROI2, vector<Rect>box_tar, int box_radius_, int box_width_height, int subset_radius_x1,
			int subset_radius_y1, int max_iteration1, float max_deformation_norm1,int ii,ofstream& outfile);

		vector<cv::Rect> boxes(vector<cv::Rect> ROI1, int fncc_ROI2_zen_radius_, int fncc_ROI2_width_height);


	public:
		Rect box_tar;//icgnÆ¥ÅäµÄbox
		int fncc_ROI2_zen_radius;
		int fncc_ROI2_width_height;
		int box_radius;
		int box_width_height;

		//set DIC parameters(Ò»´Î£©
		int subset_radius_x;
		int subset_radius_y;
		int max_iteration;
		float max_deformation_norm;
	};
}
#endif // _ICGN_HANDLE_