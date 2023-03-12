#pragma once
#pragma once

#ifndef _ROI_H_
#define _ROI_H_

#include "icgn_muti2dv_array.h"
#include "icgn_muti2dv_image.h"
#include "icgn_muti2dv_poi.h"
#include "icgn_muti2dv_subset.h"

namespace dic
{
	class ROI
	{
	public:
		Image2D* ref_img = nullptr;
		Image2D* tar_img = nullptr;

		int subset_radius_x, subset_radius_y;
		int thread_number; //OpenMP thread number

		ROI();
		virtual ~ROI() = default;

		void setImages(Image2D& ref_img, Image2D& tar_img);
		void setSubsetRadii(int radius_x, int radius_y);

		virtual void prepare();
		virtual void compute(POI2D* poi) = 0;
		virtual void compute(std::vector<POI2D>& poi_queue) = 0;

	};

}//namespace opencorr

#endif //_DIC_H_