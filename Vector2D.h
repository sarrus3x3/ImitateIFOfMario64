//�⏕�p�̓���Ƃ��č쐬�����N���X���i�[.
//���������n�ɍ�����֐����A�����Ɋi�[���Ă��܂��Ă������̂�...

//#include <stdio.h>
#include <string>
#include <string.h>
//#include <iostream>

#pragma once

using namespace std;

class Point2D;
class Vector3D;

// ################### �񎟌�double�^�x�N�g���N���X ################### 
//�����x�N�g���̈�
class Vector2D {
public:
	double x, y;

	//�R���X�g���N�^
	Vector2D(){
		x = 0;
		y = 0;
	};
	
	//�R���X�g���N�^(�����l�w��)
	Vector2D( double ini_x, double ini_y){
		x = ini_x;
		y = ini_y;
	};

	//�l�w��(��X�������ɑ������̂��߂�ǂ��̂�)
	void set( double X, double Y )
	{
		x = X;
		y = Y;
	};


	//���Z�q�I�[�o�[���C�h

	//�a
	Vector2D operator+( const Vector2D &vec ) const
	{
		Vector2D temp;
		temp.x = x + vec.x;
		temp.y = y + vec.y;
		return temp;
	};

	//��
	Vector2D operator-( const Vector2D &vec ) const
	{
		Vector2D temp;
		temp.x = x - vec.x;
		temp.y = y - vec.y;
		return temp;
	};

	//����
	double operator*( const Vector2D &vec ) const
	{
		return x*vec.x + y*vec.y;
	};

	// +=
	Vector2D& operator+=( const Vector2D &vec )
	{
		x += vec.x;
		y += vec.y;
		return *this;
	};
	//+=��return�l���ĂȂ�Ȃ�?

	// -=
	Vector2D& operator-=( const Vector2D &vec )
	{
		x -= vec.x;
		y -= vec.y;
		return *this;
	};
	
	// *= (�X�J���[��)
	Vector2D& operator*=( const double scr )
	{
		x *= scr;
		y *= scr;
		return *this;
	};

	// /=�i�X�J���[���Z�j
	Vector2D& operator/=( const double scr )
	{
		x /= scr;
		y /= scr;
		return *this;
	};

	//��Βl(�x�N�g���̒�����Ԃ�)
	double len() const
	{
		return sqrt(x*x + y*y);
	};

	//�x�N�g���̕�������Ԃ�
	double sqlen() const
	{
		return x*x + y*y;
	};

	// �^����ꂽ�Q�̒n�_�̕�������Ԃ��B
	static double sqlen( const Vector2D &orgvec, const Vector2D &tervec ){
		double dst;
		dst  = (orgvec.x-tervec.x)*(orgvec.x-tervec.x);
		dst += (orgvec.y-tervec.y)*(orgvec.y-tervec.y);
		return dst;
	};

	//������(=�[���x�N�g�����i�[)
	void zero()
	{
		x = 0;
		y = 0;
	};

	//�P�ʃx�N�g����Ԃ�.
	Vector2D normalize() const
	{
		Vector2D tmp;
		double len = this->len();
		tmp.x = x/len;
		tmp.y = y/len;
		return tmp;
	};

	//(�P�ʃx�N�g���ɑ΂�)��������x�N�g����Ԃ�.
	Vector2D side() const
	{
		Vector2D tmp;
		tmp.x = -y;
		tmp.y =  x;
		return tmp;
	};

	// Y���𔽓]�����x�N�g����Ԃ�
	Vector2D reversY() const
	{
		Vector2D tmp;
		tmp.x = x;
		tmp.y = -y;
		return tmp;
	};

	// ���W�����v���� angle�i���W�A���j ������]������
	// ��]�s�� R
	// |  cos( a ) , -sin( a ) |
	// |  sin( a ) ,  cos( a ) |
	Vector2D rot( double angle ) const
	{
		Vector2D tmp;
		tmp.x =  cos(angle) * x - sin(angle) * y ;
		tmp.y =  sin(angle) * x + cos(angle) * y ;
		return tmp;
	};

	//Vector2D��Point�ւ̃L���X�g���s��
	Point2D toPoint() const;

	// ################ Vector3D.h �̃C���N���[�h�v ################
	
	// 2�������W�� X-Z���ʏ� ��3�������W�ɂɕϊ�
	Vector3D toVector3D() const;
	Vector3D toVector3D( double hight ) const; // Y�����W�l���w��
};

//Vector2D�̃X�J���[��Z
inline Vector2D operator*( const double scr , const Vector2D &vec )
{
	Vector2D tmp(vec);
	tmp *= scr;
	return tmp;
};

inline Vector2D operator*( const Vector2D &vec , const double scr )
{
	Vector2D tmp(vec);
	tmp *= scr;
	return tmp;
};

inline Vector2D operator/( const Vector2D &vec , const double scr )
{
	Vector2D tmp(vec);
	tmp /= scr;
	return tmp;	
};

//�񎟌������^�x�N�g���N���X
class Point2D {
public:
	int x , y;

	//�R���X�g���N�^
	Point2D(){
		x = 0;
		y = 0;
	};
	
	//�R���X�g���N�^(�����l�w��)
	Point2D( int ini_x, int ini_y){
		x = ini_x;
		y = ini_y;
	};

	//Point��Vector2D�ւ̃L���X�g���s��
	Vector2D toRealVector() const;
};

// #### ���[�J�����W�ւ̕ϊ� ####
inline Vector2D Vector2DToLocalSpace(const Vector2D &vec,
									const Vector2D &AgentHeading,
									const Vector2D &AgentSide)
{ 
	// ���[�J�����W vec' = A�̋t�s�� * vec. �������AA:���[�J�����W�̊��x�N�g������Ȃ�
	// �t�s��̌�����A�̍s�񎮂�1�ɂȂ邱�Ƃ���
	// i.e. AgentHeading, AgentSide �͒��s�K�i������Ă���K�v���L�邱�Ƃɒ���
	Vector2D tmp;
	tmp.x =  AgentSide.y    * vec.x - AgentSide.x    * vec.y;
	tmp.y = -AgentHeading.y * vec.x + AgentHeading.x * vec.y;
	return tmp;
};

// #### ���[���h���W�ւ̕ϊ� ####
inline Vector2D Vector2DToWorldSpace(const Vector2D &vec,
									const Vector2D &AgentHeading,
									const Vector2D &AgentSide)
{
	return vec.x * AgentHeading + vec.y * AgentSide;
};

// #### 2x2�s�� ####
// DX���C�u�����̍s��̎����ɏK��

// �s��\����
struct C2DMATRIX
{
	double					m[2][2] ;
};

// �s��̊|���Z
inline Vector2D C2DVTransform( Vector2D vIn, C2DMATRIX InM )
{
	Vector2D vRtn;
	vRtn.x = InM.m[0][0]*vIn.x + InM.m[1][0]*vIn.y;
	vRtn.y = InM.m[0][1]*vIn.x + InM.m[1][1]*vIn.y;
	return vRtn;
};

// ��]�s��̎擾
inline C2DMATRIX C2DMGetRot( double Rotate )
{
	// ��]�s�� R
	// |  cos( a ) , -sin( a ) |
	// |  sin( a ) ,  cos( a ) |
	C2DMATRIX MRtn;
	MRtn.m[0][0] =  cos( Rotate );
	MRtn.m[1][0] = -sin( Rotate );
	MRtn.m[0][1] =  sin( Rotate );
	MRtn.m[1][1] =  cos( Rotate );
	return MRtn;
};

// �Q�����x�N�g���̐��`���
// bgn �� end ����`��Ԃ����x�N�g����Ԃ��B
// ratio = 0 �Ȃ� bgn �ƈ�v�Braito = 1 �Ȃ� end �ƈ�v �Ƃ�����B
inline Vector2D LerpV2D(Vector2D bgn, Vector2D end, double raito)
{
	return bgn + raito * (end - bgn);
};