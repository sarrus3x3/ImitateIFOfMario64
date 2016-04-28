#include <stdio.h>

#include "Vector2D.h"
#include "Vector3D.h"

//Vector2D→Pointへのキャストを行う
Point2D Vector2D::toPoint() const
{
	Point2D pt;
	pt.x = (int)x;
	pt.y = (int)y;

	return pt;
};

//Point→Vector2Dへのキャストを行う
Vector2D Point2D::toRealVector() const
{
		Vector2D vec;
		vec.x = (double)x;
		vec.y = (double)y;
		return vec;
};

// 2次元座標を X-Z平面上 の3次元座標にに変換
Vector3D Vector2D::toVector3D() const
{
	Vector3D vec3D;
	vec3D.x = x;
	vec3D.y = 0;
	vec3D.z = y;
	return vec3D;
};

Vector3D Vector2D::toVector3D( double hight ) const
{
	Vector3D vec3D;
	vec3D.x = x;
	vec3D.y = hight;
	vec3D.z = y;
	return vec3D;
};