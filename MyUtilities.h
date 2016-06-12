#include<vector>


#pragma once

// ################## FPS����p #######################
class MeasureFPS
{
public:
	static const int SampleRate = 100; // FPS�\���ŕ��ς��Ƃ鐔

	std::vector<double> m_FrameTimeHistory;
	int m_iNextUpdateSlot;

	// �R���X�g���N�^
	MeasureFPS():
		m_FrameTimeHistory(SampleRate,0.0),
		m_iNextUpdateSlot(0)
	{}

	// �X�V�ƕ��ϒl�̌v�Z
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