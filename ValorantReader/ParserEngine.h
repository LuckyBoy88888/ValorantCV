#pragma once

#include "Common.h"
#include "TimelineData.h"

class CParserEngine
{
public:
	CParserEngine();

	bool						Run(uint8_t* buffer, int width, int height, int8_t lastWeapon, CTimelineData &result);

private:
	uint8_t*					m_curBuffer;
	int16_t						m_width;
	int16_t						m_height;

	WhitePattern				m_scorePattern[MAX_DIGITS];
	WhitePattern				m_timePattern[MAX_DIGITS];
	WhitePattern				m_ammoPattern[MAX_DIGITS];
	WhitePattern				m_armorPattern[MAX_DIGITS];
	WhitePattern				m_creditsPattern[MAX_DIGITS];
	ColorPattern				m_agentsPattern[N_AGENTS];
	WhitePattern				m_weaponChangePattern[1];
	WhitePattern				m_spikePattern[1];
	ColorPattern				m_headshotPattern[2];
	WhitePattern				m_otherPlayerPattern[1];
	ColorPattern				m_mapAgentsPattern[N_MAPAGENTS];
	WhitePattern				m_mapRedCirclePattern[1];
	WhitePattern				m_weaponPattern[MAX_WEAPONS];

	int							m_allyScoreY;
	int							m_allyScoreX1;
	int							m_allyScoreX10;
	int							m_allyScoreX01;

	int							m_enemyScoreY;
	int							m_enemyScoreX1;
	int							m_enemyScoreX10;
	int							m_enemyScoreX01;

	int							m_timeY;
	int							m_timeX100;
	int							m_timeX010;
	int							m_timeX001;

	int							m_ammoY;
	int							m_ammoX100;
	int							m_ammoX010;
	int							m_ammoX001;

	int							m_healthY;
	int							m_healthX100;
	int							m_healthX010;
	int							m_healthX001;
	int							m_healthX10;
	int							m_healthX01;
	int							m_healthX1;

	int							m_armorY;
	int							m_armorX10;
	int							m_armorX01;
	int							m_armorX1;

	int							m_ammoleftY;
	int							m_ammoleftX100;
	int							m_ammoleftX010;
	int							m_ammoleftX001;

	int							m_creditsY;
	int							m_creditsX1;
	int							m_creditsX10;
	int							m_creditsX100;
	int							m_creditsX1000;
	int							m_creditsX10000;
	int							m_creditsX100000;

	int							m_agentsY;
	int							m_agentsXAlly;
	int							m_agentsXEnemy;
	int							m_agentsXStep;

	int							m_weaponchangeY, m_weaponchangeX;
	int							m_weaponchangeY1, m_weaponchangeX1;
	int							m_weaponchangeY2, m_weaponchangeX2;
	int							m_weaponchangeY3, m_weaponchangeX3;

	int							m_spikeY, m_spikeX;
	int							m_hitheadshotY, m_hitheadshotX;
	int							m_otherY, m_otherX;
	int							m_skillY;
	int							m_skillStep;
	int							m_skillWidth;

	int							m_weaponX;
	int							m_weaponY1;
	int							m_weaponY2;
	int							m_weaponY3;

private:
	int							MatchWhitePattern(int x, int y, WhitePattern* pPattern, int patternSize);
	int							MatchWhitePattern_(int x, int y, WhitePattern* pPattern, int patternSize);
	int							MatchColorPattern(int x, int y, ColorPattern* pPattern, int patternSize, bool flip = false);
	int							MatchColorPattern_(int x, int y, ColorPattern* pPattern, int patternSize, bool flip = false);
	int							MatchMapPattern(int x, int y, ColorPattern* pPattern, int patternSize);

	bool 						LoadDB(int width, int height);

	int8_t						GetYourTeamScore();
	int8_t						GetEnemyTeamScore();
	int8_t						GetRoundTime();
	int8_t						GetHealth();
	int8_t						GetArmor();
	int8_t						GetHasSpike();
	int8_t						GetWeaponId();
	int8_t						GetHitHeadShot();
	int16_t						GetAmmo();
	int16_t						GetAmmoLeft();
	int16_t						GetCredits();
	int							GetIsOtherPlayer();
	void						GetAbilityLeft(int8_t* pAbilityInfo);
	std::string					GetAgentNameFromNo(int number);
	std::vector<std::string>	GetAllyAgents();
	std::vector<std::string>	GetEnemyAgents();
	std::vector<EnemyRect>		GetEnemy(int otherPlayer, int8_t health);
	std::vector<EnemyRect>		GetEnemyOnScreen();
	std::vector<EnemyRect>		GetEnemyOnMap();
};

