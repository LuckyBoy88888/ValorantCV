#pragma once

#include <vector>

#include "Common.h"
#include "structs.h"
#include "GameState.h"
#include "PlayerState.h"

class CTimelineData
{
public:
	CTimelineData();
	virtual ~CTimelineData();

	void					Export();

public:
	int16_t					m_width;
	int16_t					m_height;
	uint64_t				m_timestamp;

	CGameState				m_gameState;
	CPlayerState			m_playerState;
	std::vector<IEvent*>	m_aryEvents;
};
