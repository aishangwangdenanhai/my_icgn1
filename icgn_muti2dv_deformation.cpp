#include "icgn_muti2dv_deformation.h"

namespace dic
{
	//deformation2d 1st order
	Deformation2D1::Deformation2D1() {
		u = 0.f;
		ux = 0.f;
		uy = 0.f;
		v = 0.f;
		vx = 0.f;
		vy = 0.f;

		setWarp();
	}

	Deformation2D1::Deformation2D1(float u, float ux, float uy, float v, float vx, float vy) {
		this->u = u;
		this->ux = ux;
		this->uy = uy;
		this->v = v;
		this->vx = vx;
		this->vy = vy;

		setWarp();
	}

	Deformation2D1::Deformation2D1(float p[6]) {
		u = p[0];
		ux = p[1];
		uy = p[2];
		v = p[3];
		vx = p[4];
		vy = p[5];

		setWarp();
	}

	Deformation2D1::~Deformation2D1() {
	}

	void Deformation2D1::setDeformation(float u, float ux, float uy, float v, float vx, float vy) {
		this->u = u;
		this->ux = ux;
		this->uy = uy;
		this->v = v;
		this->vx = vx;
		this->vy = vy;

		setWarp();
	}

	void Deformation2D1::setDeformation(float p[6]) {
		u = p[0];
		ux = p[1];
		uy = p[2];
		v = p[3];
		vx = p[4];
		vy = p[5];

		setWarp();
	}

	void Deformation2D1::setDeformation(Deformation2D1& another_deformation) {
		u = another_deformation.u;
		ux = another_deformation.ux;
		uy = another_deformation.uy;
		v = another_deformation.v;
		vx = another_deformation.vx;
		vy = another_deformation.vy;

		setWarp();
	}

	Point2D Deformation2D1::warp(Point2D& location) {
		Eigen::Vector3f point_vector;
		point_vector(0) = location.x;
		point_vector(1) = location.y;
		point_vector(2) = 1.f;

		Eigen::Vector3f warped_vector = warp_matrix * point_vector;

		Point2D new_location(warped_vector(0), warped_vector(1));
		return new_location;
	}

	void Deformation2D1::setDeformation() {
		u = warp_matrix(0, 2);
		ux = warp_matrix(0, 0) - 1;
		uy = warp_matrix(0, 1);
		v = warp_matrix(1, 2);
		vx = warp_matrix(1, 0);
		vy = warp_matrix(1, 1) - 1;
	}

	void Deformation2D1::setWarp() {
		warp_matrix(0, 0) = 1 + ux;
		warp_matrix(0, 1) = uy;
		warp_matrix(0, 2) = u;
		warp_matrix(1, 0) = vx;
		warp_matrix(1, 1) = 1 + vy;
		warp_matrix(1, 2) = v;
		warp_matrix(2, 0) = 0.f;
		warp_matrix(2, 1) = 0.f;
		warp_matrix(2, 2) = 1.f;
	}

}//namespace dic