#include <math.h>

#include "icgn_muti2dv_point.h"

namespace dic
{
	//Point2D
	Point2D::Point2D() {
		x = 0.f;
		y = 0.f;
	}

	Point2D::Point2D(float x, float y) {
		this->x = x;
		this->y = y;
	}

	Point2D::Point2D(int x, int y) {
		this->x = (float)x;
		this->y = (float)y;
	}

	Point2D::~Point2D() {
	}

	float Point2D::vectorNorm() const {
		return sqrtf(x * x + y * y);
	}

	std::ostream& operator<<(std::ostream& output, const Point2D& point) {
		output << point.x << "," << point.y;
		return output;
	}
}//namespace dic