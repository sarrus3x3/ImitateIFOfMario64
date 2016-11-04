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
