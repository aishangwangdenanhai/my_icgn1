#include <cmath>
#include <iostream>

#include "icgn_muti2dv_subset.h"

namespace dic
{
	//2D susbet
	Subset2D::Subset2D(Point2D center, int radius_x, int radius_y) {
		if (radius_x < 1 || radius_y < 1) {
			std::cerr << "Too small radius:" << radius_x << ", " << radius_y << std::endl;
		}

		this->center = center;
		this->radius_x = radius_x;
		this->radius_y = radius_y;
		width = radius_x * 2 + 1;
		height = radius_y * 2 + 1;

		eg_mat = Eigen::MatrixXf::Zero(height, width);
	}

	void Subset2D::fill(Image2D* image) {
		Point2D topleft_point(center.x - radius_x, center.y - radius_y);
		eg_mat << image->eg_mat1.block(topleft_point.y, topleft_point.x, height, width);
	}

	float Subset2D::zeroMeanNorm() {
		float subset_mean = eg_mat.mean();
		eg_mat.array() -= subset_mean;
		float subset_sum = eg_mat.squaredNorm();

		return sqrtf(subset_sum);
	}
}//namespace dic