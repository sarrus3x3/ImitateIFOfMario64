#include <stdio.h>

#include "Vector2D.h"
#include "Vector3D.h"

//Vector2D��Point�ւ̃L���X�g���s��
Point2D Vector2D::toPoint() const
{
	Point2D pt;
	pt.x = (int)x;
	pt.y = (int)y;

	return pt;
};

//Point��Vector2D�ւ̃L���X�g���s��
Vector2D Point2D::toRealVector() const
{
		Vector2D vec;
		vec.x = (double)x;
		vec.y = (double)y;
		return vec;
};

// 2�������W�� X-Z���ʏ� ��3�������W�ɂɕϊ�
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