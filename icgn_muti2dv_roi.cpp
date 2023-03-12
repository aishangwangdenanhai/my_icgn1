#include "icgn_muti2dv_roi.h"

namespace dic
{
	ROI::ROI() {
	}

	void ROI::setImages(Image2D& ref_img, Image2D& tar_img) {
		this->ref_img = &ref_img;
		this->tar_img = &tar_img;
	}

	void ROI::setSubsetRadii(int subset_radius_x, int subset_radius_y) {
		this->subset_radius_x = subset_radius_x;
		this->subset_radius_y = subset_radius_y;
	}

	void ROI::prepare() {
	}

}//namespace dic