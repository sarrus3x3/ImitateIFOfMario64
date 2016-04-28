#include <stdio.h>
#include <time.h>   // time()�֐��p

#include "Vector3D.h"

// �x�N�g�� vec ���^����ꂽ�Ƃ��Athis �� vec �̍�镽�ʂɂ����� 
// vec �����������i i.e. vec*this > 0 �ƂȂ� �jthis �ɒ��s����P�ʃx�N�g����Ԃ��B
Vector3D Vector3D::getOrthoVec( const Vector3D &vec )
{
	double t = ( (*this) * vec ) / sqlen();
	return ( t * (*this) + vec ).normalize();
};

