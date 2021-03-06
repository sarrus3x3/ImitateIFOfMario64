#include<vector>

#include "DxLib.h"

#include "Vector2D.h"
#include "Vector3D.h"

#pragma once

// ################## FPS測定用 #######################
class MeasureFPS
{
public:
	static const int SampleRate = 100; // FPS表示で平均をとる数

	std::vector<double> m_FrameTimeHistory;
	int m_iNextUpdateSlot;

	// コンストラクタ
	MeasureFPS():
		m_FrameTimeHistory(SampleRate,0.0),
		m_iNextUpdateSlot(0)
	{}

	// 更新と平均値の計算
	double Update( double NewFrameTime )
	{
		m_FrameTimeHistory[m_iNextUpdateSlot]=NewFrameTime;
		
		m_iNextUpdateSlot = (m_iNextUpdateSlot+1)%SampleRate;

		double sum=0;
		std::vector<double>::iterator it = m_FrameTimeHistory.begin();
		for( it; it!=m_FrameTimeHistory.end(); it++ )
		{
			sum += *it;
		}

		return sum / (double)SampleRate ;
	}

};

// ##### Smoother ##########################################################

template <class T>
class Smoother
{
private:

  //this holds the history
  std::vector<T>  m_History;

  int           m_iNextUpdateSlot;

  //an example of the 'zero' value of the type to be smoothed. This
  //would be something like Vector2D(0,0)
  T             m_ZeroValue;

public:

  //to instantiate a Smoother pass it the number of samples you want
  //to use in the smoothing, and an exampe of a 'zero' type
  Smoother(int SampleSize, T ZeroValue):m_History(SampleSize, ZeroValue),
                                        m_ZeroValue(ZeroValue),
                                        m_iNextUpdateSlot(0)
  {}

  //each time you want to get a new average, feed it the most recent value
  //and this method will return an average over the last SampleSize updates
  T Update(const T& MostRecentValue)
  {  
    //overwrite the oldest value with the newest
    m_History[m_iNextUpdateSlot++] = MostRecentValue;

    //make sure m_iNextUpdateSlot wraps around. 
    if (m_iNextUpdateSlot == m_History.size()) m_iNextUpdateSlot = 0;

    //now to calculate the average of the history list
    T sum = m_ZeroValue;

    std::vector<T>::iterator it = m_History.begin();

    for (it; it != m_History.end(); ++it)
    {
      sum += *it;
    }

    return sum / (double)m_History.size();
  }
};

// ################## 足跡表示機能 ##################
class VisualFootprint
{
public:
	// 軌跡格納
	std::vector<Vector3D> TrajectoryList;
	int CurIndex; // TrajectoryListにおける現在のindex
	unsigned int m_iColor; // 軌跡を描画する色

	// コンストラクタ
	// 軌跡を残すステップ数と、軌跡を描画する色を指定
	VisualFootprint(int MaxQueue, unsigned int Color ) 
		: TrajectoryList(MaxQueue), CurIndex(0), m_iColor(Color) {};

	// 軌跡を記録
	void Update(Vector3D Pos);

	// 軌跡を描画
	void Render();

};

// ################## 描画のための補助関数 #######################

inline int DrawLine2D( Point2D bgn, Point2D end, unsigned int Color, int Thickness=1 )
{
	return DrawLine( bgn.x, bgn.y, end.x, end.y, Color, Thickness );
};

inline int DrawCircle2D( Point2D cnt, int r, unsigned int Color, int FillFlag = TRUE )
{
	return DrawCircle( cnt.x, cnt.y, r, Color, FillFlag );
};

inline int DrawBox2D( Point2D TL, Point2D BR, unsigned int Color, int FillFlag = TRUE )
{
	return DrawBox( TL.x, TL.y, BR.x+1, BR.y+1, Color, FillFlag );
};

inline int DrawTriangle2D( Point2D V1, Point2D V2, Point2D V3, unsigned int Color, int FillFlag = TRUE )
{
	return DrawTriangle( 
		V1.x, V1.y,
		V2.x, V2.y, 
		V3.x, V3.y, 
		Color ,
		FillFlag ) ;
};

// 矢印 - 先端が線のタイプ
void DrawArrow2D( Vector2D bgn, Vector2D end, unsigned int Color, int FillFlag = FALSE, int Thickness=1 );

// 与えられた位置に、（ワールド座標からみ）たローカル座標の基底軸を表示する関数
void DrawCoordi(MATRIX M, double scale);

// 色を決める時にいちいちGetColor関数でRGB指定するストレスから開放するため
class ColorPalette
{
public:
	static const int Red;    // 赤
	static const int Blue;   // 青
	static const int Green;  // 緑
	static const int Cyan;   // シアン
	static const int Yellow; // 黄色

};

// ################## その他 #######################

// カットオフ関数
// 戻り値：カットオフ実施ならtrue
inline bool cutoff( double &val , const double MAX )
{
	// カットオフ処理
	if( fabs(val) > MAX )
	{
		val = (double)( (val>0) - (val<0) ) * MAX;
		return true;
	}

	return false;
};

inline int sgn(double x)
{
	return (x > 0 ? 1 : (x < 0 ? -1 : 0));
};

// Unity の Mathf.MoveTowards に相当
// 最大距離 maxDelta で src → tar へ移動させた位置を返す。
inline double MoveTowards( double src, double tar, double maxDelta )
{
	if (abs(tar - src) < maxDelta)
	{
		return tar;
	}
	else
	{ 
		return sgn(tar - src) * maxDelta + src;
	}

};

// SuperMario64HDの SuperMath.BoundedInterpolation に相当
// オリジナル
// SuperMario64HD - Unity Project\SuperMario64HD\Assets\SuperCharacterController\Core\SuperMath.cs
inline double BoundedInterpolation( vector<double> bounds, vector<double> values, double t )
{
	for (int i = 0; i < bounds.size(); i++)
	{
		if (t < bounds[i])
		{
			return values[i];
		}
	}

	return values[values.size()-1];
};