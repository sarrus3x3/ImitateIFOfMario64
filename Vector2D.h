//補助用の道具として作成したクラスを格納.
//文字処理系に作った関数も、ここに格納してしまっていいものか...

//#include <stdio.h>
#include <string>
#include <string.h>
//#include <iostream>

#pragma once

using namespace std;

class Point2D;
class Vector3D;

// ################### 二次元double型ベクトルクラス ################### 
//実数ベクトルの意
class Vector2D {
public:
	double x, y;

	//コンストラクタ
	Vector2D(){
		x = 0;
		y = 0;
	};
	
	//コンストラクタ(初期値指定)
	Vector2D( double ini_x, double ini_y){
		x = ini_x;
		y = ini_y;
	};

	//値指定(一々成分毎に代入するのがめんどいので)
	void set( double X, double Y )
	{
		x = X;
		y = Y;
	};


	//演算子オーバーライド

	//和
	Vector2D operator+( const Vector2D &vec ) const
	{
		Vector2D temp;
		temp.x = x + vec.x;
		temp.y = y + vec.y;
		return temp;
	};

	//差
	Vector2D operator-( const Vector2D &vec ) const
	{
		Vector2D temp;
		temp.x = x - vec.x;
		temp.y = y - vec.y;
		return temp;
	};

	//内積
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
	//+=でreturn値ってなんなんだ?

	// -=
	Vector2D& operator-=( const Vector2D &vec )
	{
		x -= vec.x;
		y -= vec.y;
		return *this;
	};
	
	// *= (スカラー積)
	Vector2D& operator*=( const double scr )
	{
		x *= scr;
		y *= scr;
		return *this;
	};

	// /=（スカラー除算）
	Vector2D& operator/=( const double scr )
	{
		x /= scr;
		y /= scr;
		return *this;
	};

	//絶対値(ベクトルの長さを返す)
	double len() const
	{
		return sqrt(x*x + y*y);
	};

	//ベクトルの平方長を返す
	double sqlen() const
	{
		return x*x + y*y;
	};

	// 与えられた２つの地点の平方長を返す。
	static double sqlen( const Vector2D &orgvec, const Vector2D &tervec ){
		double dst;
		dst  = (orgvec.x-tervec.x)*(orgvec.x-tervec.x);
		dst += (orgvec.y-tervec.y)*(orgvec.y-tervec.y);
		return dst;
	};

	//初期化(=ゼロベクトルを格納)
	void zero()
	{
		x = 0;
		y = 0;
	};

	//単位ベクトルを返す.
	Vector2D normalize() const
	{
		Vector2D tmp;
		double len = this->len();
		tmp.x = x/len;
		tmp.y = y/len;
		return tmp;
	};

	//(単位ベクトルに対し)直交するベクトルを返す.
	Vector2D side() const
	{
		Vector2D tmp;
		tmp.x = -y;
		tmp.y =  x;
		return tmp;
	};

	// Y軸を反転したベクトルを返す
	Vector2D reversY() const
	{
		Vector2D tmp;
		tmp.x = x;
		tmp.y = -y;
		return tmp;
	};

	// 座標を時計回りに angle（ラジアン） だけ回転させる
	// 回転行列 R
	// |  cos( a ) , -sin( a ) |
	// |  sin( a ) ,  cos( a ) |
	Vector2D rot( double angle ) const
	{
		Vector2D tmp;
		tmp.x =  cos(angle) * x - sin(angle) * y ;
		tmp.y =  sin(angle) * x + cos(angle) * y ;
		return tmp;
	};

	//Vector2D→Pointへのキャストを行う
	Point2D toPoint() const;

	// ################ Vector3D.h のインクルード要 ################
	
	// 2次元座標を X-Z平面上 の3次元座標にに変換
	Vector3D toVector3D() const;
	Vector3D toVector3D( double hight ) const; // Y軸座標値を指定
};

//Vector2Dのスカラー乗算
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

//二次元整数型ベクトルクラス
class Point2D {
public:
	int x , y;

	//コンストラクタ
	Point2D(){
		x = 0;
		y = 0;
	};
	
	//コンストラクタ(初期値指定)
	Point2D( int ini_x, int ini_y){
		x = ini_x;
		y = ini_y;
	};

	//Point→Vector2Dへのキャストを行う
	Vector2D toRealVector() const;
};

// #### ローカル座標への変換 ####
inline Vector2D Vector2DToLocalSpace(const Vector2D &vec,
									const Vector2D &AgentHeading,
									const Vector2D &AgentSide)
{ 
	// ローカル座標 vec' = Aの逆行列 * vec. ただし、A:ローカル座標の基底ベクトルからなる
	// 逆行列の公式とAの行列式が1になることから
	// i.e. AgentHeading, AgentSide は直行規格化されている必要が有ることに注意
	Vector2D tmp;
	tmp.x =  AgentSide.y    * vec.x - AgentSide.x    * vec.y;
	tmp.y = -AgentHeading.y * vec.x + AgentHeading.x * vec.y;
	return tmp;
};

// #### ワールド座標への変換 ####
inline Vector2D Vector2DToWorldSpace(const Vector2D &vec,
									const Vector2D &AgentHeading,
									const Vector2D &AgentSide)
{
	return vec.x * AgentHeading + vec.y * AgentSide;
};

// #### 2x2行列 ####
// DXライブラリの行列の実装に習う

// 行列構造体
struct C2DMATRIX
{
	double					m[2][2] ;
};

// 行列の掛け算
inline Vector2D C2DVTransform( Vector2D vIn, C2DMATRIX InM )
{
	Vector2D vRtn;
	vRtn.x = InM.m[0][0]*vIn.x + InM.m[1][0]*vIn.y;
	vRtn.y = InM.m[0][1]*vIn.x + InM.m[1][1]*vIn.y;
	return vRtn;
};

// 回転行列の取得
inline C2DMATRIX C2DMGetRot( double Rotate )
{
	// 回転行列 R
	// |  cos( a ) , -sin( a ) |
	// |  sin( a ) ,  cos( a ) |
	C2DMATRIX MRtn;
	MRtn.m[0][0] =  cos( Rotate );
	MRtn.m[1][0] = -sin( Rotate );
	MRtn.m[0][1] =  sin( Rotate );
	MRtn.m[1][1] =  cos( Rotate );
	return MRtn;
};

// ２次元ベクトルの線形補間
// bgn と end を線形補間したベクトルを返す。
// ratio = 0 なら bgn と一致。raito = 1 なら end と一致 という具合。
inline Vector2D LerpV2D(Vector2D bgn, Vector2D end, double raito)
{
	return bgn + raito * (end - bgn);
};