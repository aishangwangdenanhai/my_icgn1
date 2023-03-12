#pragma once

#ifndef _POI_H_
#define _POI_H_

#include "icgn_muti2dv_point.h"
#include "icgn_muti2dv_deformation.h"

namespace dic {

	//structures for POI
	union DeformationVector2D {
		struct {
			float u, ux, uy, uxx, uxy, uyy;
			float v, vx, vy, vxx, vxy, vyy;
		};
		//content: u ux uy uxx uxy uyy v vx vy vxx vxy vyy
		float p[12];
	};

	union StrainVector2D {
		struct {
			float exx, eyy, exy;
		};
		//conetent: exx, eyy, exy
		float e[3];
	};

	union Result2D {
		struct {
			float u0, v0, zncc, iteration, convergence, feature;
		};
		float r[6];
	};

	//class for 2D DIC
	class POI2D : public Point2D
	{
	public:
		DeformationVector2D deformation;
		Result2D result;
		StrainVector2D strain;

		POI2D(int x, int y);
		POI2D(float x, float y);
		POI2D(Point2D location);
		~POI2D();

		//reset data except the location
		void clean();
	};

}//namespace opencorr

#endif //_POI_H_
