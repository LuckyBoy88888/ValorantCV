#pragma once

#include <list>
#include <string>

#include "Logs.h"
#include "TimelineData.h"

class CEventBuilder
{
public:
	CEventBuilder();
	virtual ~CEventBuilder();

	bool Build(CTimelineData& currSlice, std::list<CTimelineData>& aryTimelineData, int32_t timestamp);

private:
	void BuildAbiltyCastEvent(CTimelineData& currSlice, CTimelineData& prevSlice);
	void BuildAmmoReloadEvent(CTimelineData& currSlice, CTimelineData& prevSlice);
	void BuildKillEvent(CTimelineData& currSlice, CTimelineData& prevSlice);
	void BuildDeathEvent(CTimelineData& currSlice, CTimelineData& prevSlice);
	void BuildEnemyAppearedEvent(CTimelineData& currSlice, CTimelineData& prevSlice);
	void BuildShootEventFromAmmo(CTimelineData& currSlice, CTimelineData& prevSlice);
	void BuildShootEventFromEtc(CTimelineData& currSlice, CTimelineData& prevSlice);

private:
	int32_t		m_timestamp;
	int32_t		m_shoot;
	int32_t		m_enemyAppear;
	int32_t		m_changeTargetShootDely;
	std::string m_killedEnemy;
};

