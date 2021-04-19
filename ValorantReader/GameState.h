#pragma once
#include <vector>
#include <string>

#include "Common.h"
#include "structs.h"

struct CGameState
{
	void						Export();

	int8_t						m_round;
	int8_t						m_roundTime;
	int8_t						m_yourTeamScore;
	int8_t						m_enemyTeamScore;

	std::vector<std::string>	m_aryYourAgents;
	std::vector<std::string>	m_aryEnemyAgents;
	std::vector<EnemyRect>		m_enemyList;
};

