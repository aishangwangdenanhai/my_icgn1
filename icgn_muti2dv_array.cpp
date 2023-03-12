#include "icgn_muti2dv_array.h"

namespace dic
{
	float** new2D(int dimension1, int dimension2) {
		float** ptr = nullptr;
		hCreatePtr(ptr, dimension1, dimension2);
		return ptr;
	}

	void delete2D(float**& ptr) {
		if (ptr == nullptr) return;
		hDestroyPtr(ptr);
	}

	float*** new3D(int dimension1, int dimension2, int dimension3) {
		float*** ptr = nullptr;
		hCreatePtr(ptr, dimension1, dimension2, dimension3);
		return ptr;
	}

	void delete3D(float***& ptr) {
		if (ptr == nullptr) return;
		hDestroyPtr(ptr);
	}

	float**** new4D(int dimension1, int dimension2, int dimension3, int dimension4) {
		float**** ptr = nullptr;
		hCreatePtr(ptr, dimension1, dimension2, dimension3, dimension4);
		return ptr;
	}

	void delete4D(float****& ptr) {
		if (ptr == nullptr) return;
		hDestroyPtr(ptr);
	}

}//namespace dic