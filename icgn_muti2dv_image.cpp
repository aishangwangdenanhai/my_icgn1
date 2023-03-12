#include <fstream>

#include "icgn_muti2dv_image.h"

namespace dic
{
	Image2D::Image2D(cv::Mat img)
	{
		width = img.cols;
		height = img.rows;
		eg_mat1.resize(height, width);
		cv::cv2eigen(img, eg_mat1);
	}


	Image2D::Image2D(int width, int height) {
		eg_mat = Eigen::MatrixXf::Zero(height, width);
		this->width = width;
		this->height = height;
	}

	Image2D::Image2D(string file_path) {
		cv_mat = cv::imread(file_path, cv::IMREAD_GRAYSCALE);

		if (!cv_mat.data) {
			throw std::string("Fail to load file: " + file_path);
		}

		this->file_path = file_path;
		width = cv_mat.cols;
		height = cv_mat.rows;
		eg_mat.resize(height, width);

		cv::cv2eigen(cv_mat, eg_mat);
	}

	void Image2D::load(string file_path) {
		cv_mat = cv::imread(file_path, cv::IMREAD_GRAYSCALE);

		if (!cv_mat.data) {
			throw std::string("Fail to load file: " + file_path);
		}

		this->file_path = file_path;
		if (width != cv_mat.cols || height != cv_mat.rows) {
			width = cv_mat.cols;
			height = cv_mat.rows;
			eg_mat.resize(height, width);
		}

		cv::cv2eigen(cv_mat, eg_mat);
	}
	
	Image2D::~Image2D()
	{
	}
	
}//namespace dic