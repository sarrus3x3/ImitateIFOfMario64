//#include <stdio.h>
#include <string>
#include <string.h>
//#include <iostream>

#include <cassert>

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
	Vector3D operator+( const Vector3D &vec ) const
	{
		Vector3D temp;
		temp.x = x + vec.x;
		temp.y = y + vec.y;
		temp.z = z + vec.z;
		return temp;
	};

	//差
	Vector3D operator-( const Vector3D &vec ) const
	{
		Vector3D temp;
		temp.x = x - vec.x;
		temp.y = y - vec.y;
		temp.z = z - vec.z;
		return temp;
	};

	//内積
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

	// 0ベクトルか判定
	bool isZero() const
	{
		if (x == 0.0 && y == 0.0 && z == 0.0)
		{
			return true;
		}
		return false;
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

// ２つのベクトル間の角度を計算する。単位はラジアン。戻り値は 0 ～ π(180°) の間。
inline double Angle3D(const Vector3D &vec1, const Vector3D &vec2)
{
	double cosval = (vec1*vec2) / (vec1.len()*vec2.len());
	return acos(cosval);
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

// 最大 maxRadiansDelta の回転角（ラジアン）で、ベクトルsrc を tar方向へ回転させたベクトルを返す。
// ただし、 src と tar 間の角が maxRadiansDelta に満たない場合は、tar を返す動き。
// Unityの関数が元ネタ
// 【前提条件】
// ベクトル src, tar は規格化されていること。
inline Vector3D RotateTowards3D(Vector3D src, Vector3D tar, double maxRadiansDelta)
{
	// 【考え方】
	// y軸の正方向の回転＝x軸→z軸方向の回転
	// また、基底ベクトルの外積では
	//   ez × ex = ey
	// したがって、Cur→Tarを正方向とする回転軸ベクトルは、
	//   tar × src で得られる。
	// ※ 左手座標系で考えていることに注意

	// src と tar の成す角を計算
	double prd = src*tar;

	// 丸め？により、src と tar 内積が 1.0 より大きくなってしまう場合がある → その場合は acos の動作が保証されない
	if( fabs(prd)>1.0 ) prd /= fabs(prd);
	// ↑こりゃだめだ。

	// もし、src と tar が完全に反対方向を向いている場合は、回転をさせない。（srcをそのまま返却）
	// → いや、少し削り、-1.0よりも大きくする
	// → これやると破綻する。（最後にベクトルと整合が取れなくなるので。）
	//if (prd == -1.0) prd += 0.0001;

	double angle = acos(prd);

	// これが maxRadiansDelta 以下の場合は、tar を返して終了
	if (angle <= maxRadiansDelta) return tar;

	/*
	// 回転軸を計算
	//Vector3D RotAxis = (tar%src).normalize();
	Vector3D RotAxis = (src%tar).normalize();

	// 回転軸に、maxRadiansDelta 回転させる回転行列を計算（DXlib組み込みを使用）
	MATRIX RotMat = MGetRotAxis(RotAxis.toVECTOR(), maxRadiansDelta);

	// src に回転行列を作用させて返却
	return VTransformSR(src.toVECTOR(), RotMat);
	*/

	// ２ベクトルの成す角が小さくなると不安定なる問題に対処するため、
	// アルゴリズムの改善。

	double beta = sin(maxRadiansDelta) / sin(angle);
	double alpa = cos(maxRadiansDelta) - cos(angle)*beta;

	Vector3D tmp = alpa * src + beta * tar;
	assert(tmp.len() < 1000.0);
	return tmp;

	//return alpa * src + beta * tar;

	// このアルゴリズムの導出方法は、できればドキュメントに残したい。
	// たとえば、githubのwikiとかに...

};