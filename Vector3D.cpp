#include <stdio.h>
#include <time.h>   // time()関数用

#include "Vector3D.h"

// ベクトル vec が与えられたとき、this と vec の作る平面において 
// vec 側を向いた（ i.e. vec*this > 0 となる ）this に直行する単位ベクトルを返す。
Vector3D Vector3D::getOrthoVec( const Vector3D &vec )
{
	double t = ( (*this) * vec ) / sqlen();
	return ( t * (*this) + vec ).normalize();
};

