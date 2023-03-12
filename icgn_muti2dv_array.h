#pragma once
#ifndef _ARRAY_H_
#define _ARRAY_H_

#include <string>
#include <iostream>
#include <Eigen/Core>
#include <Eigen/Dense>

typedef Eigen::Matrix<float, 6, 6> Matrix6f;
typedef Eigen::Matrix<float, 12, 12> Matrix12f;
typedef Eigen::Matrix<float, 6, 1> Vector6f;
typedef Eigen::Matrix<float, 4, 1> Vector4f;

namespace dic
{
	//new and delete 2d array
	float** new2D(int dimension1, int dimension2);
	void delete2D(float**& ptr);

	//new and delete 3d array
	float*** new3D(int dimension1, int dimension2, int dimension3);
	void delete3D(float***& ptr);

	//new and delete 4d array
	float**** new4D(int dimension1, int dimension2, int dimension3, int dimension4);
	void delete4D(float****& ptr);

	//allocate memory for 2d, 3d, and 4d arrays
	template <class Real>
	void hCreatePtr(Real*& ptr, int dimension1) {
		ptr = (Real*)calloc(dimension1, sizeof(Real)); //allocate the memory and initialize all the elements with zero
	}

	template <class Real>
	void hCreatePtr(Real**& ptr, int dimension1, int dimension2) {
		Real* ptr1d = (Real*)calloc(dimension1 * dimension2, sizeof(Real));
		ptr = (Real**)malloc(dimension1 * sizeof(Real*));

		for (int i = 0; i < dimension1; i++) {
			ptr[i] = ptr1d + i * dimension2;
		}
	}

	template <class Real>
	void hCreatePtr(Real***& ptr, int dimension1, int dimension2, int dimension3) {
		Real* ptr1d = (Real*)calloc(dimension1 * dimension2 * dimension3, sizeof(Real));
		Real** ptr2d = (Real**)malloc(dimension1 * dimension2 * sizeof(Real*));
		ptr = (Real***)malloc(dimension1 * sizeof(Real**));

		for (int i = 0; i < dimension1; i++) {
			for (int j = 0; j < dimension2; j++) {
				ptr2d[i * dimension2 + j] = ptr1d + (i * dimension2 + j) * dimension3;
			}
			ptr[i] = ptr2d + i * dimension2;
		}
	}

	template <class Real>
	void hCreatePtr(Real****& ptr, int dimension1, int dimension2, int dimension3, int dimension4) {
		Real* ptr1d = (Real*)calloc(dimension1 * dimension2 * dimension3 * dimension4, sizeof(Real));
		Real** ptr2d = (Real**)malloc(dimension1 * dimension2 * dimension3 * sizeof(Real*));
		Real*** ptr3d = (Real***)malloc(dimension1 * dimension2 * sizeof(Real**));
		ptr = (Real****)malloc(dimension1 * sizeof(Real***));

		for (int i = 0; i < dimension1; i++) {
			for (int j = 0; j < dimension2; j++) {
				for (int k = 0; k < dimension3; k++) {
					ptr2d[(i * dimension2 + j) * dimension3 + k] = ptr1d + ((i * dimension2 + j) * dimension3 + k) * dimension4;
				}
				ptr3d[i * dimension2 + j] = ptr2d + (i * dimension2 + j) * dimension3;
			}
			ptr[i] = ptr3d + i * dimension2;
		}
	}

	//release memory of 2d, 3d, and 4d arrays
	template <class Real>
	void hDestroyPtr(Real*& ptr) {
		free(ptr);
		ptr = nullptr;
	}

	template <class Real>
	void hDestroyPtr(Real**& ptr) {
		free(ptr[0]);
		free(ptr);
		ptr = nullptr;
	}

	template<class Real>
	void hDestroyPtr(Real***& ptr) {
		free(ptr[0][0]);
		free(ptr[0]);
		free(ptr);
		ptr = nullptr;
	}

	template <class Real>
	void hDestroyPtr(Real****& ptr) {
		free(ptr[0][0][0]);
		free(ptr[0][0]);
		free(ptr[0]);
		free(ptr);
		ptr = nullptr;
	}

}//namespace dic

#endif //_ARRAY_H_