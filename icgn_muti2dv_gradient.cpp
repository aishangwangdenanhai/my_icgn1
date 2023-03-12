#include <omp.h>
#include "icgn_muti2dv_gradient.h"

namespace dic
{
	Gradient2D4::Gradient2D4(Image2D& image) {
		grad_img = &image;
	}

	Gradient2D4::~Gradient2D4() {
	}

	void Gradient2D4::getGradientX() {
		int height = grad_img->height;
		int width = grad_img->width;

		gradient_x = Eigen::MatrixXf::Zero(height, width);

#pragma omp parallel for
		for (int r = 0; r < height; r++) {
			for (int c = 2; c < width - 2; c++) {
				float result = 0.0f;
				result -= grad_img->eg_mat1(r, c + 2) / 12.f;
				result += grad_img->eg_mat1(r, c + 1) * (2.f / 3.f);
				result -= grad_img->eg_mat1(r, c - 1) * (2.f / 3.f);
				result += grad_img->eg_mat1(r, c - 2) / 12.f;
				gradient_x(r, c) = result;
				//std::cout << result << " ";
			}

			//std::cout << std::endl;
		}
	}

	void Gradient2D4::getGradientY() {
		int height = grad_img->height;
		int width = grad_img->width;

		gradient_y = Eigen::MatrixXf::Zero(height, width);

#pragma omp parallel for
		for (int r = 2; r < height - 2; r++) {
			for (int c = 0; c < width; c++) {
				float result = 0.0f;
				result -= grad_img->eg_mat1(r + 2, c) / 12.f;
				result += grad_img->eg_mat1(r + 1, c) * (2.f / 3.f);
				result -= grad_img->eg_mat1(r - 1, c) * (2.f / 3.f);
				result += grad_img->eg_mat1(r - 2, c) / 12.f;
				gradient_y(r, c) = result;
			}
		}
	}

} //namespcae dic