#include "PrecisionTimer.h"

//---------------------- default constructor ------------------------------
//
//-------------------------------------------------------------------------
PrecisionTimer::PrecisionTimer():
					m_LastTime(0),
					m_TimeElapsed(0.0),
					m_bStarted(false),
					m_bSmoothUpdates(false){}

//------------------------Start()-----------------------------------------
//
//  call this immediately prior to game loop. Starts the timer (obviously!)
//
//--------------------------------------------------------------------------
void PrecisionTimer::Start()
{
	m_LastTime = GetNowHiPerformanceCount();
	m_bStarted = true;
  
	return;
}

