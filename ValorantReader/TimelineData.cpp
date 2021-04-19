#include "TimelineData.h"

CTimelineData::CTimelineData()
{
}

CTimelineData::~CTimelineData()
{
	std::vector<IEvent*>::iterator iter;

	for (iter = m_aryEvents.begin(); iter != m_aryEvents.end(); iter++)
	{
		if ((*iter))
		{
			delete (*iter);
		}
	}
}

void CTimelineData::Export()
{
	GLogA("{timestamp:%d", m_timestamp);
	
	GLogA(",gameState:");
	m_gameState.Export();
	
	GLogA(",playerState:");
	m_playerState.Export();


	GLogA(", events:[");
	for (int i = 0; i < m_aryEvents.size(); i++)
	{
		if (i != 0)
		{
			GLogA(",");
		}
		m_aryEvents[i]->Export();
	}
	GLogA("]");
	
	GLogA("}\n");
}
