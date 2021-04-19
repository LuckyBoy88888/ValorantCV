#pragma once

#include "TimelineData.h"

#include <string>

class CEventBuilder
{
public:
	CEventBuilder();
	virtual ~CEventBuilder();

	CTimelineData Build(CTimelineData* pCurrSlice, std::vector<CTimelineData>& aryTimelineData);

private:
	void BuildAbiltyCastEvent(CTimelineData* pCurrSlice, CTimelineData* pPrevSlice);
	void BuildAmmoReloadEvent(CTimelineData* pCurrSlice, CTimelineData* pPrevSlice);
	void BuildKillEvent(CTimelineData* pCurrSlice, CTimelineData* pPrevSlice);
	void BuildDeathEvent(CTimelineData* pCurrSlice, CTimelineData* pPrevSlice);
	void BuildEnemyAppearedEvent(CTimelineData* pCurrSlice, CTimelineData* pPrevSlice);
	void BuildShootEventFromAmmo(CTimelineData* pCurrSlice, CTimelineData* pPrevSlice);
	void BuildShootEventFromEtc(CTimelineData* pCurrSlice, CTimelineData* pPrevSlice);

private:
	int32_t		m_startTickCount;
	int32_t		m_tickCount;
	int32_t		m_shoot;
	int32_t		m_enemyAppear;
	int32_t		m_changeTargetShootDely;
	std::string m_killedEnemy;
};

