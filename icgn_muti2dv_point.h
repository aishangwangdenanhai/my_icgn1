#pragma once
#pragma once

#ifndef _POINT_H_
#define _POINT_H_

#include <iostream>

namespace dic
{
	//Point in 2D plane
	class Point2D
	{
	public:
		float x, y;

		Point2D();
		Point2D(float x, float y);
		Point2D(int x, int y);
		~Point2D();

		float vectorNorm() const;

		friend std::ostream& operator<<(std::ostream& output, const Point2D& point);
	};

	//reload basic operators
	inline Point2D operator+(Point2D point, Point2D offset) {
		return Point2D(point.x + offset.x, point.y + offset.y);
	}

	inline Point2D operator-(Point2D point, Point2D offset) {
		return point + Point2D(-offset.x, -offset.y);
	}

	inline Point2D operator*(float factor, Point2D point) {
		return Point2D(factor * point.x, factor * point.y);
	}

	inline Point2D operator*(int factor, Point2D point) {
		return float(factor) * point;
	}

	inline Point2D operator*(Point2D point, float factor) {
		return factor * point;
	}

	inline Point2D operator*(Point2D point, int factor) {
		return float(factor) * point;
	}

	//dot product
	inline float operator*(Point2D point1, Point2D point2) {
		return (point1.x * point2.x + point1.y * point2.y);
	}

	inline Point2D operator/(Point2D point, float factor) {
		return Point2D(point.x / factor, point.y / factor);
	}

	inline Point2D operator/(Point2D point, int factor) {
		return point / float(factor);
	}

	//cross product
	inline float operator/(Point2D point1, Point2D point2) {
		return (point1.x * point2.y - point1.y * point2.x);
	}
}//namespace dic

#endif //_POINT_H_