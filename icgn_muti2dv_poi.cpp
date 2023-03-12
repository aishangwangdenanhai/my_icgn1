#include <algorithm>

#include "icgn_muti2dv_poi.h"

namespace dic
{
	//POI2D
	POI2D::POI2D(int x, int y) :Point2D(x, y) {
		clean();
	}

	POI2D::POI2D(float x, float y) : Point2D(x, y) {
		clean();
	}

	POI2D::POI2D(Point2D location) : Point2D(location) {
		clean();
	}

	POI2D::~POI2D() {
	}

	void POI2D::clean() {
		std::fill(std::begin(deformation.p), std::end(deformation.p), 0.f);
		std::fill(std::begin(result.r), std::end(result.r), 0.f);
		std::fill(std::begin(strain.e), std::end(strain.e), 0.f);
	}
}//namespace dic