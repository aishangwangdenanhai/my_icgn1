#include <cmath>
#include <omp.h>
#include "icgn_muti2dv_interpolation.h"

namespace dic
{
	BicubicBspline::BicubicBspline(Image2D& image) :lookup_table(nullptr) {
		interp_img = &image;
	}

	BicubicBspline::~BicubicBspline() {
		if (lookup_table != nullptr)
			delete4D(lookup_table);
	}

	void BicubicBspline::prepare() {
		if (lookup_table != nullptr) {
			delete4D(lookup_table);
		}
		int height = interp_img->height;
		int width = interp_img->width;
		
		//默认构造函数
		//std::cout << height << std::endl;
		//std::cout << width << std::endl;

		if (height < 5 || width < 5) {
			std::cerr << "Too small image:" << width << ", " << height << std::endl;
		}
		lookup_table = new4D(height, width, 4, 4);

#pragma omp parallel for
		for (int r = 1; r < height - 2; r++) {
			for (int c = 1; c < width - 2; c++) {
				float matrix_g[4][4] = { 0 };
				float matrix_b[4][4] = { 0 };
				for (int i = 0; i < 4; i++) {
					for (int j = 0; j < 4; j++) {
						matrix_g[i][j] = interp_img->eg_mat1(r - 1 + i, c - 1 + j);//▼▼：这里涉及坐标转换
					}
				}

				for (int k = 0; k < 4; k++) {
					for (int l = 0; l < 4; l++) {
						for (int m = 0; m < 4; m++) {
							for (int n = 0; n < 4; n++) {
								matrix_b[k][l] += CONTROL_MATRIX[k][m] * CONTROL_MATRIX[l][n] * matrix_g[n][m];
							}
						}
					}
				}

				for (int k = 0; k < 4; k++) {
					for (int l = 0; l < 4; l++) {
						lookup_table[r][c][k][l] = 0;
						for (int m = 0; m < 4; m++) {
							for (int n = 0; n < 4; n++) {
								lookup_table[r][c][k][l] += FUNCTION_MATRIX[k][m] * FUNCTION_MATRIX[l][n] * matrix_b[n][m];
							}
						}
					}
				}

				for (int k = 0; k < 2; k++) {
					for (int l = 0; l < 4; l++) {
						float buffer = lookup_table[r][c][k][l];
						lookup_table[r][c][k][l] = lookup_table[r][c][3 - k][3 - l];
						lookup_table[r][c][3 - k][3 - l] = buffer;
					}
				}
			}
		}
	}

	float BicubicBspline::compute(Point2D& location) {
		//边界保护
		if (location.x < 0 || location.y < 0 || location.x >= interp_img->width || location.y >= interp_img->height) {
			return 0.f;
		}

		//15*15
		int y_integral = (int)floorf(location.y);
		int x_integral = (int)floorf(location.x);

		float x_decimal = location.x - x_integral;//▲x
		float y_decimal = location.y - y_integral;//▲y

		float x2_decimal = x_decimal * x_decimal;
		float y2_decimal = y_decimal * y_decimal;

		float x3_decimal = x2_decimal * x_decimal;
		float y3_decimal = y2_decimal * y_decimal;

		float value = 0.f;
		float**& coefficient = lookup_table[y_integral][x_integral];

		value += coefficient[0][0];
		value += coefficient[0][1] * x_decimal;
		value += coefficient[0][2] * x2_decimal;
		value += coefficient[0][3] * x3_decimal;

		value += coefficient[1][0] * y_decimal;
		value += coefficient[1][1] * y_decimal * x_decimal;
		value += coefficient[1][2] * y_decimal * x2_decimal;
		value += coefficient[1][3] * y_decimal * x3_decimal;

		value += coefficient[2][0] * y2_decimal;
		value += coefficient[2][1] * y2_decimal * x_decimal;
		value += coefficient[2][2] * y2_decimal * x2_decimal;
		value += coefficient[2][3] * y2_decimal * x3_decimal;

		value += coefficient[3][0] * y3_decimal;
		value += coefficient[3][1] * y3_decimal * x_decimal;
		value += coefficient[3][2] * y3_decimal * x2_decimal;
		value += coefficient[3][3] * y3_decimal * x3_decimal;

		return value;//得到亚像素插值
	}

}//namespace dic