//#include <stdio.h>
#include <string>
#include <string.h>
//#include <iostream>

#include <cassert>

#include "DxLib.h"
#include "Vector2D.h"

#pragma once

using namespace std;


// �� Dxlib��VECTOR �� Vector3D �ւ̃L���X�g���`����B
// �L���X�g���Z�q�H�݂����̂��I�[�o�[���C�h���ă_�C���N�g�ɃL���X�g�ł���ƃX�}�[�g���Ǝv�������A
// �Ȃ񂩊댯�ȍ��肪����̂ł悵�Ă����B
// �R���X�g���N�^�ɏ��������B��������������ł��邵�B

// �R�����x�N�g���N���X
// double �^
class Vector3D {
public:
	double x, y, z;

	// �R���X�g���N�^
	Vector3D(){
		x = 0;
		y = 0;
		z = 0;
	};
	
	// �R���X�g���N�^(�����l�w��)
	Vector3D( double X, double Y, double Z )
	{
		x = X;
		y = Y;
		z = Z;
	};

	// DxLib �� VECTOR �^ ���� Vector3D �^ �փL���X�g����
	Vector3D( VECTOR vec )
	{
		x = (double)vec.x;
		y = (double)vec.y;
		z = (double)vec.z;
	};

	//�l�w��(��X�������ɑ������̂��߂�ǂ��̂�)
	void set( double X, double Y, double Z )
	{
		x = X;
		y = Y;
		z = Z;
	};

	//===���Z�q�I�[�o�[���C�h===

	//�a
	Vector3D operator+( const Vector3D &vec ) const
	{
		Vector3D temp;
		temp.x = x + vec.x;
		temp.y = y + vec.y;
		temp.z = z + vec.z;
		return temp;
	};

	//��
	Vector3D operator-( const Vector3D &vec ) const
	{
		Vector3D temp;
		temp.x = x - vec.x;
		temp.y = y - vec.y;
		temp.z = z - vec.z;
		return temp;
	};

	//����
	double operator*( const Vector3D &vec ) const
	{
		return x*vec.x + y*vec.y + z*vec.z;
	};

	// +=
	Vector3D& operator+=( const Vector3D &vec )
	{
		x += vec.x;
		y += vec.y;
		z += vec.z;
		return *this;
	};
	//+=��return�l���ĂȂ�Ȃ�?

	// -=
	Vector3D& operator-=( const Vector3D &vec )
	{
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;
		return *this;
	};
	
	// *= (�X�J���[��)
	Vector3D& operator*=( const double scr )
	{
		x *= scr;
		y *= scr;
		z *= scr;
		return *this;
	};

	// /=�i�X�J���[���Z�j
	Vector3D& operator/=( const double scr )
	{
		x /= scr;
		y /= scr;
		z /= scr;
		return *this;
	};

	//��Βl(�x�N�g���̒�����Ԃ�)
	double len() const
	{
		return sqrt( x*x + y*y + z*z );
	};

	//�x�N�g���̕�������Ԃ�
	double sqlen() const
	{
		return x*x + y*y + z*z;
	};

	// �^����ꂽ�Q�̒n�_�̕�������Ԃ��B
	static double sqlen( const Vector3D &orgvec, const Vector3D &tervec )
	{
		double dst=0;
		dst += (orgvec.x-tervec.x)*(orgvec.x-tervec.x);
		dst += (orgvec.y-tervec.y)*(orgvec.y-tervec.y);
		dst += (orgvec.z-tervec.z)*(orgvec.z-tervec.z);
		return dst;
	};

	//������(=�[���x�N�g�����i�[)
	void zero()
	{
		x = 0;
		y = 0;
		z = 0;
	};

	//�P�ʃx�N�g����Ԃ�.
	Vector3D normalize() const
	{
		Vector3D tmp;
		double len = this->len();
		if( len > 0 )
		{
			tmp.x = x/len;
			tmp.y = y/len;
			tmp.z = z/len;
		}
		return tmp;
	};

	// 0�x�N�g��������
	bool isZero() const
	{
		if (x == 0.0 && y == 0.0 && z == 0.0)
		{
			return true;
		}
		return false;
	};
	
	// �x�N�g�� vec ���^����ꂽ�Ƃ��Athis �� vec �̍�镽�ʂɂ����� 
	// vec ���̌������������i i.e. vec*this > 0 �ƂȂ� �jthis �ɒ��s����P�ʃx�N�g����Ԃ��B
	Vector3D getOrthoVec( const Vector3D &vec );
	
	// ################ DxLib.h �̃C���N���[�h�v ################

	// Vector3D �� DxLib::VECTOR �`���ɕϊ�
	VECTOR toVECTOR() const
	{
		VECTOR tmp;
		tmp.x = (float) x;
		tmp.y = (float) y;
		tmp.z = (float) z;
		return tmp;
	}

	// ################ Vector2D.h �̃C���N���[�h�v ################
	
	// x-z���ʂɓ��e����2D�x�N�g��
	Vector2D toVector2D() const
	{
		Vector2D tmp;
		tmp.x = x;
		tmp.y = z;
		return tmp;
	}

};

//RealVector�̃X�J���[��Z
inline Vector3D operator*( const double scr , const Vector3D &vec )
{
	Vector3D tmp(vec);
	tmp *= scr;
	return tmp;
};

inline Vector3D operator*( const Vector3D &vec , const double scr )
{
	Vector3D tmp(vec);
	tmp *= scr;
	return tmp;
};

inline Vector3D operator/( const Vector3D &vec , const double scr )
{
	Vector3D tmp(vec);
	tmp /= scr;
	return tmp;
};

// �Q�̃x�N�g���̊O�ς��v�Z���� = vec1 �~ vec2
inline Vector3D operator%( const Vector3D &vec1, const Vector3D &vec2 )
{
	Vector3D tmp;
	tmp.x = vec1.y*vec2.z - vec1.z*vec2.y;
	tmp.y = vec1.z*vec2.x - vec1.x*vec2.z;
	tmp.z = vec1.x*vec2.y - vec1.y*vec2.x;
	return tmp;
};

// �Q�̃x�N�g���Ԃ̊p�x���v�Z����B�P�ʂ̓��W�A���B�߂�l�� 0 �` ��(180��) �̊ԁB
inline double Angle3D(const Vector3D &vec1, const Vector3D &vec2)
{
	double cosval = (vec1*vec2) / (vec1.len()*vec2.len());
	return acos(cosval);
};


// �^����ꂽ������xz���ʂ̌�_�����߂�
inline int calcCrossPointWithXZPlane( Vector3D bgn, Vector3D end, Vector3D &rslt )
{
	double t = bgn.y / ( bgn.y - end.y );
	// ���^�[���l������
	rslt = bgn + t*( end - bgn ); 
	
	if( t<0 ) return -1;
	else      return  0;
	
};

// �ő� maxRadiansDelta �̉�]�p�i���W�A���j�ŁA�x�N�g��src �� tar�����։�]�������x�N�g����Ԃ��B
// �������A src �� tar �Ԃ̊p�� maxRadiansDelta �ɖ����Ȃ��ꍇ�́Atar ��Ԃ������B
// Unity�̊֐������l�^
// �y�O������z
// �x�N�g�� src, tar �͋K�i������Ă��邱�ƁB
inline Vector3D RotateTowards3D(Vector3D src, Vector3D tar, double maxRadiansDelta)
{
	// �y�l�����z
	// y���̐������̉�]��x����z�������̉�]
	// �܂��A���x�N�g���̊O�ςł�
	//   ez �~ ex = ey
	// ���������āACur��Tar�𐳕����Ƃ����]���x�N�g���́A
	//   tar �~ src �œ�����B
	// �� ������W�n�ōl���Ă��邱�Ƃɒ���

	// src �� tar �̐����p���v�Z
	double prd = src*tar;

	// �ۂ߁H�ɂ��Asrc �� tar ���ς� 1.0 ���傫���Ȃ��Ă��܂��ꍇ������ �� ���̏ꍇ�� acos �̓��삪�ۏ؂���Ȃ�
	if( fabs(prd)>1.0 ) prd /= fabs(prd);
	// ������Ⴞ�߂��B

	// �����Asrc �� tar �����S�ɔ��Ε����������Ă���ꍇ�́A��]�������Ȃ��B�isrc�����̂܂ܕԋp�j
	// �� ����A�������A-1.0�����傫������
	// �� ������Ɣj�]����B�i�Ō�Ƀx�N�g���Ɛ��������Ȃ��Ȃ�̂ŁB�j
	//if (prd == -1.0) prd += 0.0001;

	double angle = acos(prd);

	// ���ꂪ maxRadiansDelta �ȉ��̏ꍇ�́Atar ��Ԃ��ďI��
	if (angle <= maxRadiansDelta) return tar;

	/*
	// ��]�����v�Z
	//Vector3D RotAxis = (tar%src).normalize();
	Vector3D RotAxis = (src%tar).normalize();

	// ��]���ɁAmaxRadiansDelta ��]�������]�s����v�Z�iDXlib�g�ݍ��݂��g�p�j
	MATRIX RotMat = MGetRotAxis(RotAxis.toVECTOR(), maxRadiansDelta);

	// src �ɉ�]�s�����p�����ĕԋp
	return VTransformSR(src.toVECTOR(), RotMat);
	*/

	// �Q�x�N�g���̐����p���������Ȃ�ƕs����Ȃ���ɑΏ����邽�߁A
	// �A���S���Y���̉��P�B

	double beta = sin(maxRadiansDelta) / sin(angle);
	double alpa = cos(maxRadiansDelta) - cos(angle)*beta;

	Vector3D tmp = alpa * src + beta * tar;
	assert(tmp.len() < 1000.0);
	return tmp;

	//return alpa * src + beta * tar;

	// ���̃A���S���Y���̓��o���@�́A�ł���΃h�L�������g�Ɏc�������B
	// ���Ƃ��΁Agithub��wiki�Ƃ���...

};