//#include <stdio.h>
#include <string>
#include <string.h>
//#include <iostream>

#include "DxLib.h"
#include "Vector2D.h"

#pragma once

using namespace std;


// ★ DxlibのVECTOR → Vector3D へのキャストを定義する。
// キャスト演算子？みたいのをオーバーライドしてダイレクトにキャストできるとスマートだと思ったが、
// なんか危険な香りがするのでよしておく。
// コンストラクタに書こうか。そうすりゃ代入もできるし。

// ３次元ベクトルクラス
// double 型
class Vector3D {
public:
	double x, y, z;

	// コンストラクタ
	Vector3D(){
		x = 0;
		y = 0;
		z = 0;
	};
	
	// コンストラクタ(初期値指定)
	Vector3D( double X, double Y, double Z )
	{
		x = X;
		y = Y;
		z = Z;
	};

	// DxLib の VECTOR 型 から Vector3D 型 へキャストする
	Vector3D( VECTOR vec )
	{
		x = (double)vec.x;
		y = (double)vec.y;
		z = (double)vec.z;
	};

	//値指定(一々成分毎に代入するのがめんどいので)
	void set( double X, double Y, double Z )
	{
		x = X;
		y = Y;
		z = Z;
	};

	//===演算子オーバーライド===

	//和
	Vector3D operator+( const Vector3D &vec )
	{
		Vector3D temp;
		temp.x = x + vec.x;
		temp.y = y + vec.y;
		temp.z = z + vec.z;
		return temp;
	};

	//差
	Vector3D operator-( const Vector3D &vec )
	{
		Vector3D temp;
		temp.x = x - vec.x;
		temp.y = y - vec.y;
		temp.z = z - vec.z;
		return temp;
	};

	//内積
	double operator*( const Vector3D &vec )
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
	//+=でreturn値ってなんなんだ?

	// -=
	Vector3D& operator-=( const Vector3D &vec )
	{
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;
		return *this;
	};
	
	// *= (スカラー積)
	Vector3D& operator*=( const double scr )
	{
		x *= scr;
		y *= scr;
		z *= scr;
		return *this;
	};

	// /=（スカラー除算）
	Vector3D& operator/=( const double scr )
	{
		x /= scr;
		y /= scr;
		z /= scr;
		return *this;
	};

	//絶対値(ベクトルの長さを返す)
	double len() const
	{
		return sqrt( x*x + y*y + z*z );
	};

	//ベクトルの平方長を返す
	double sqlen() const
	{
		return x*x + y*y + z*z;
	};

	// 与えられた２つの地点の平方長を返す。
	static double sqlen( const Vector3D &orgvec, const Vector3D &tervec )
	{
		double dst=0;
		dst += (orgvec.x-tervec.x)*(orgvec.x-tervec.x);
		dst += (orgvec.y-tervec.y)*(orgvec.y-tervec.y);
		dst += (orgvec.z-tervec.z)*(orgvec.z-tervec.z);
		return dst;
	};

	//初期化(=ゼロベクトルを格納)
	void zero()
	{
		x = 0;
		y = 0;
		z = 0;
	};

	//単位ベクトルを返す.
	Vector3D normalize()
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
	
	// ベクトル vec が与えられたとき、this と vec の作る平面において 
	// vec 側の向きを向いた（ i.e. vec*this > 0 となる ）this に直行する単位ベクトルを返す。
	Vector3D getOrthoVec( const Vector3D &vec );
	
	// ################ DxLib.h のインクルード要 ################

	// Vector3D → DxLib::VECTOR 形式に変換
	VECTOR toVECTOR() const
	{
		VECTOR tmp;
		tmp.x = (float) x;
		tmp.y = (float) y;
		tmp.z = (float) z;
		return tmp;
	}

	// ################ Vector2D.h のインクルード要 ################
	
	// x-z平面に投影した2Dベクトル
	Vector2D toVector2D() const
	{
		Vector2D tmp;
		tmp.x = x;
		tmp.y = z;
		return tmp;
	}

};

//RealVectorのスカラー乗算
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

// ２つのベクトルの外積を計算する = vec1 × vec2
inline Vector3D operator%( const Vector3D &vec1, const Vector3D &vec2 )
{
	Vector3D tmp;
	tmp.x = vec1.y*vec2.z - vec1.z*vec2.y;
	tmp.y = vec1.z*vec2.x - vec1.x*vec2.z;
	tmp.z = vec1.x*vec2.y - vec1.y*vec2.x;
	return tmp;
};

// 与えられた線分とxz平面の交点を求める
inline int calcCrossPointWithXZPlane( Vector3D bgn, Vector3D end, Vector3D &rslt )
{
	double t = bgn.y / ( bgn.y - end.y );
	// リターン値を決定
	rslt = bgn + t*( end - bgn ); 
	
	if( t<0 ) return -1;
	else      return  0;
	
};