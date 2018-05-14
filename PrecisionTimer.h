//-----------------------------------------------------------------------
//
//  Name: PrecisionTimer.h
//
//  Author: Mat Buckland 2002
//
//  Desc: Windows timer class.
//
//        nb. this only uses the high performance timer. There is no
//        support for ancient computers. I know, I know, I should add
//        support, but hey, I have shares in AMD and Intel... Go upgrade ;o)
//
//-----------------------------------------------------------------------
#include <cassert>

#include "DxLib.h"

class PrecisionTimer
{

private:
	// 以下はマイクロ秒単位であることに注意 
	LONGLONG  m_LastTime;
	double    m_TimeElapsed;
	bool      m_bStarted;

	bool      m_bSmoothUpdates;

public:

    //ctors
	PrecisionTimer();

    //whatdayaknow, this starts the timer
	void    Start();

	inline double  TimeElapsed();

	bool    Started()const{return m_bStarted;}
	void    SmoothUpdatesOn(){m_bSmoothUpdates = true;}
	void    SmoothUpdatesOff(){m_bSmoothUpdates = false;}

};

//--------------------------- TimeElapsed --------------------------------
//
//  returns time elapsed since last call to this function.
//-------------------------------------------------------------------------
inline double PrecisionTimer::TimeElapsed()
{
	LONGLONG m_CurrentTime = GetNowHiPerformanceCount();
	int interval = (int)( m_CurrentTime - m_LastTime );
	m_TimeElapsed = interval * 0.000001 ; // マイクロ秒単位を秒単位に変換
	m_LastTime = m_CurrentTime;

	return m_TimeElapsed;
	
	//return 1.0 / 60.0;
	//return 1.0/20.0;

	//return 0.004;
}

  
