#pragma once
#ifndef _INTERPOLATION_H_
#define _INTERPOLATION_H_

#include "icgn_muti2dv_array.h"
#include "icgn_muti2dv_image.h"
#include "icgn_muti2dv_point.h"

namespace dic
{
	class BicubicBspline
	{
	protected:
		Image2D* interp_img = nullptr;

	public:
		BicubicBspline(Image2D& image);
		~BicubicBspline();

		void prepare();
		float compute(Point2D& location);

	private:
		float**** lookup_table = nullptr;

		const float CONTROL_MATRIX[4][4] = {
			{ 71.0f / 56.0f, -19.0f / 56.0f, 5 / 56.0f, -1.0f / 56.0f},
			{ -19.0f / 56.0f, 95.0f / 56.0f, -25 / 56.0f, 5.0f / 56.0f},
			{ 5.0f / 56.0f, -25.0f / 56.0f, 95 / 56.0f, -19.0f / 56.0f},
			{ -1.0f / 56.0f, 5.0f / 56.0f, -19 / 56.0f, 71.0f / 56.0f}
		};

		const float FUNCTION_MATRIX[4][4] = {
			{ -1.0f / 6.0f, 3.0f / 6.0f, -3.0f / 6.0f, 1.0f / 6.0f },
			{ 3.0f / 6.0f, -6.0f / 6.0f, 3.0f / 6.0f, 0.0f },
			{ -3.0f / 6.0f, 0.0f, 3.0f / 6.0f, 0.0f },
			{ 1.0f / 6.0f, 4.0f / 6.0f, 1.0f / 6.0f, 0.0f }
		};
	};

}//namespace dic

#endif //_INTERPOLATION_H_