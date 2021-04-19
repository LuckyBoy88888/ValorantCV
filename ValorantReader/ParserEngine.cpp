#include "pch.h"
#include "structs.h"
#include "ParserEngine.h"


CParserEngine::CParserEngine()
	: m_width (0)
	, m_height (0)
	, m_curBuffer (nullptr)
{
	ZeroMemory(m_scorePattern, sizeof(WhitePattern) * MAX_DIGITS);
	ZeroMemory(m_timePattern, sizeof(WhitePattern) * MAX_DIGITS);
	ZeroMemory(m_ammoPattern, sizeof(WhitePattern) * MAX_DIGITS);
	ZeroMemory(m_armorPattern, sizeof(WhitePattern) * MAX_DIGITS);
	ZeroMemory(m_creditsPattern, sizeof(WhitePattern) * MAX_DIGITS);
	ZeroMemory(m_agentsPattern, sizeof(ColorPattern) * N_AGENTS);
	ZeroMemory(m_weaponChangePattern, sizeof(WhitePattern) * 1);
	ZeroMemory(m_spikePattern, sizeof(WhitePattern) * 1);
	ZeroMemory(m_headshotPattern, sizeof(ColorPattern) * 2);
	ZeroMemory(m_otherPlayerPattern, sizeof(WhitePattern) * 1);
	ZeroMemory(m_mapAgentsPattern, sizeof(ColorPattern) * N_MAPAGENTS);
	ZeroMemory(m_mapRedCirclePattern, sizeof(WhitePattern) * 1);
	ZeroMemory(m_weaponPattern, sizeof(WhitePattern) * MAX_WEAPONS);

	m_allyScoreY = 0; 
	m_allyScoreX1 = 0; 
	m_allyScoreX10 = 0; 
	m_allyScoreX01 = 0;
	
	m_enemyScoreY = 0; 
	m_enemyScoreX1 = 0; 
	m_enemyScoreX10 = 0; 
	m_enemyScoreX01 = 0;

	m_timeY = 0; 
	m_timeX100 = 0; 
	m_timeX010 = 0; 
	m_timeX001 = 0;

	m_ammoY = 0; 
	m_ammoX100 = 0; 
	m_ammoX010 = 0; 
	m_ammoX001 = 0;

	m_healthY = 0; 
	m_healthX100 = 0; 
	m_healthX010 = 0; 
	m_healthX001 = 0; 
	m_healthX10 = 0; 
	m_healthX01 = 0; 
	m_healthX1 = 0;

	m_armorY = 0; 
	m_armorX10 = 0; 
	m_armorX01 = 0; 
	m_armorX1 = 0;

	m_ammoleftY = 0; 
	m_ammoleftX100 = 0; 
	m_ammoleftX010 = 0; 
	m_ammoleftX001 = 0;

	m_creditsY = 0; 
	m_creditsX1 = 0; 
	m_creditsX10 = 0; 
	m_creditsX100 = 0; 
	m_creditsX1000 = 0; 
	m_creditsX10000 = 0; 
	m_creditsX100000 = 0;

	m_agentsY = 0; 
	m_agentsXAlly = 0; 
	m_agentsXEnemy = 0; 
	m_agentsXStep = 0;

	m_weaponchangeY = 0; 
	m_weaponchangeX = 0; 
	m_weaponchangeY1 = 0; 
	m_weaponchangeX1 = 0; 
	m_weaponchangeY2 = 0; 
	m_weaponchangeX2 = 0; 
	m_weaponchangeY3 = 0; 
	m_weaponchangeX3 = 0;

	m_spikeY = 0; 
	m_spikeX = 0; 
	m_hitheadshotY = 0; 
	m_hitheadshotX = 0; 
	m_otherY = 0; 
	m_otherX = 0;

	m_skillY = 0; 
	m_skillStep = 0; 
	m_skillWidth = 0;

	m_weaponX = 0; 
	m_weaponY1 = 0; 
	m_weaponY2 = 0; 
	m_weaponY3 = 0;
}

bool CParserEngine::Run(uint8_t* buffer, int width, int height, int8_t lastWeapon, CTimelineData &result)
{
	result.m_width = width;
	result.m_height = height;

	if (!LoadDB(width, height))
	{
		result.m_height = 0;
		return false;
	}


	m_curBuffer = buffer;

	// GameState
	result.m_gameState.m_yourTeamScore	= GetYourTeamScore();
	result.m_gameState.m_enemyTeamScore = GetEnemyTeamScore();
	result.m_gameState.m_aryYourAgents	= GetAllyAgents();
	result.m_gameState.m_aryEnemyAgents = GetEnemyAgents();
	result.m_gameState.m_roundTime		= GetRoundTime();
	result.m_gameState.m_round			= result.m_gameState.m_yourTeamScore + result.m_gameState.m_enemyTeamScore + 1;

	// PlayerState
	int otherPlayers					= GetIsOtherPlayer();
	int8_t updatedWeaponId				= GetWeaponId();
	result.m_playerState.m_health		= GetHealth();
	result.m_playerState.m_armor		= GetArmor();
	result.m_playerState.m_ammo			= GetAmmo();
	result.m_playerState.m_ammoLeft		= GetAmmoLeft();
	result.m_playerState.m_hasSpike		= GetHasSpike();
	result.m_playerState.m_credits		= GetCredits();
	result.m_playerState.m_hitHeadShot	= GetHitHeadShot();
	result.m_gameState.m_enemyList		= GetEnemy(otherPlayers, result.m_playerState.m_health);
	result.m_playerState.m_weaponId		= updatedWeaponId == -1 ? lastWeapon:updatedWeaponId;
	if (otherPlayers != 0 &&
		result.m_playerState.m_health > 0)
	{
		result.m_playerState.m_isAlive = true;
	}


	GetAbilityLeft(result.m_playerState.m_aryAbilityLeft);

	return true;
}

int8_t CParserEngine::GetYourTeamScore()
{
	int8_t score = 0;
	int ally, ally1, ally2;

	ally = MatchWhitePattern(m_width / 2 + m_allyScoreX1, m_allyScoreY, m_scorePattern, 10);
	ally1 = MatchWhitePattern(m_width / 2 + m_allyScoreX10, m_allyScoreY, m_scorePattern, 10);
	ally2 = MatchWhitePattern(m_width / 2 + m_allyScoreX01, m_allyScoreY, m_scorePattern, 10);

	if ((ally == INVALID) && (ally1 != INVALID) && (ally2 != INVALID))
	{
		score  = ally1 * 10 + ally2;
	}
	else if ((ally != INVALID) && (ally1 == INVALID) && (ally2 == INVALID))
	{
		score = ally;
	}
	else if ((ally == INVALID) && (ally1 == INVALID) && (ally2 == INVALID))
	{
		score = INVALID;
	}
	else
	{
		score = MATCH_ERROR;
	}

	return score;
}

int8_t CParserEngine::GetEnemyTeamScore()
{
	int8_t score = 0;
	int enemy, enemy1, enemy2;
	enemy = MatchWhitePattern(m_width / 2 + m_enemyScoreX1, m_enemyScoreY, m_scorePattern, 10);
	enemy1 = MatchWhitePattern(m_width / 2 + m_enemyScoreX10, m_enemyScoreY, m_scorePattern, 10);
	enemy2 = MatchWhitePattern(m_width / 2 + m_enemyScoreX01, m_enemyScoreY, m_scorePattern, 10);

	if ((enemy == INVALID) && (enemy1 != INVALID) && (enemy2 != INVALID))
	{
		score = enemy1 * 10 + enemy2;
	}
	else if ((enemy != INVALID) && (enemy1 == INVALID) && (enemy2 == INVALID))
	{
		score = enemy;
	}
	else if ((enemy == INVALID) && (enemy1 == INVALID) && (enemy == INVALID))
	{
		score = INVALID;
	}
	else
	{
		score = MATCH_ERROR;
	}

	return score;
}

int8_t CParserEngine::GetRoundTime()
{
	int8_t roundTime = 0;
	int time1, time2, time3;

	time1 = MatchWhitePattern(m_width / 2 + m_timeX100, m_timeY, m_timePattern, 2);
	time2 = MatchWhitePattern(m_width / 2 + m_timeX010, m_timeY, m_timePattern, 6);
	time3 = MatchWhitePattern(m_width / 2 + m_timeX001, m_timeY, m_timePattern, 10);

	if ((time1 != INVALID) && (time2 != INVALID) && (time3 != INVALID))
	{
		roundTime = time1 * 60 + time2 * 10 + time3;
	}
	else if ((time1 == INVALID) && (time2 == INVALID) && (time3 == INVALID))
	{
		roundTime = INVALID;
	}
	else
	{
		roundTime = MATCH_ERROR;
	}

	return roundTime;
}

int8_t CParserEngine::GetHealth()
{
	int8_t health = 0;
	int health1, health2, health3, health4, health5, health6;

	health1 = MatchWhitePattern(m_width / 2 + m_healthX100, m_healthY, m_ammoPattern, 2);
	health2 = MatchWhitePattern(m_width / 2 + m_healthX010, m_healthY, m_ammoPattern, 10);
	health3 = MatchWhitePattern(m_width / 2 + m_healthX001, m_healthY, m_ammoPattern, 10);

	health4 = MatchWhitePattern(m_width / 2 + m_healthX10, m_healthY, m_ammoPattern, 10);
	health5 = MatchWhitePattern(m_width / 2 + m_healthX01, m_healthY, m_ammoPattern, 10);

	health6 = MatchWhitePattern(m_width / 2 + m_healthX1, m_healthY, m_ammoPattern, 10);

	if ((health1 != INVALID) && (health2 != INVALID) && (health3 != INVALID) && (health4 == INVALID) && (health5 == INVALID) && (health6 == INVALID))
	{
		health = health1 * 100 + health2 * 10 + health3;
	}
	else if ((health1 == INVALID) && (health2 == INVALID) && (health3 == INVALID) && (health4 != INVALID) && (health5 != INVALID) && (health6 == INVALID))
	{
		health = health4 * 10 + health5;
	}
	else if ((health1 == INVALID) && (health2 == INVALID) && (health3 == INVALID) && (health4 == INVALID) && (health5 == INVALID) && (health6 != INVALID))
	{
		health = health6;
	}
	else if ((health1 == INVALID) && (health2 == INVALID) && (health3 == INVALID) && (health4 == INVALID) && (health5 == INVALID) && (health6 == INVALID))
	{
		health = INVALID;
	}
	else
	{
		health = MATCH_ERROR;
	}

	return health;
}

int CParserEngine::GetIsOtherPlayer()
{
	int u8IsOtherPlayer = 0;
	u8IsOtherPlayer = MatchWhitePattern(m_otherX, m_otherY, m_otherPlayerPattern, 1);

	return u8IsOtherPlayer;
}

int8_t CParserEngine::GetArmor()
{
	int8_t armor = 0;
	int armor1, armor2, armor3;

	armor1 = MatchWhitePattern(m_width / 2 + m_armorX10, m_armorY, m_armorPattern, 10);
	armor2 = MatchWhitePattern(m_width / 2 + m_armorX01, m_armorY, m_armorPattern, 10);
	armor3 = MatchWhitePattern(m_width / 2 + m_armorX1, m_armorY, m_armorPattern, 10);

	if ((armor1 != INVALID) && (armor2 != INVALID) && (armor3 == INVALID))
	{
		armor = armor1 * 10 + armor2;
	}
	else if ((armor1 == INVALID) && (armor2 == INVALID) && (armor3 != INVALID))
	{
		armor = armor3;
	}
	else if ((armor1 == INVALID) && (armor2 == INVALID) && (armor3 == INVALID))
	{
		armor = INVALID;
	}
	else
	{
		armor = MATCH_ERROR;
	}

	return armor;
}

int16_t CParserEngine::GetAmmo()
{
	int16_t ammo = 0;
	int ammo0, ammo1, ammo2;

	ammo0 = MatchWhitePattern(m_width / 2 + m_ammoX100, m_ammoY, m_ammoPattern, 3);
	ammo1 = MatchWhitePattern(m_width / 2 + m_ammoX010, m_ammoY, m_ammoPattern, 10);
	ammo2 = MatchWhitePattern(m_width / 2 + m_ammoX001, m_ammoY, m_ammoPattern, 10);

	if ((ammo0 != INVALID) && (ammo1 != INVALID) && (ammo2 != INVALID))
	{
		ammo = ammo0 * 100 + ammo1 * 10 + ammo2;
	}
	else if ((ammo0 == INVALID) && (ammo1 != INVALID) && (ammo2 != INVALID))
	{
		ammo = ammo1 * 10 + ammo2;
	}
	else if ((ammo0 == INVALID) && (ammo1 == INVALID) && (ammo2 != INVALID))
	{
		ammo = ammo2;
	}
	else if ((ammo0 == INVALID) && (ammo1 == INVALID) && (ammo2 == INVALID))
	{
		ammo = INVALID;
	}
	else
	{
		ammo = MATCH_ERROR;
	}

	return ammo;
}

int16_t CParserEngine::GetAmmoLeft()
{
	int16_t ammoLeft = 0;
	int ammoleft1, ammoleft2, ammoleft3;

	ammoleft1 = MatchWhitePattern(m_width / 2 + m_ammoleftX100, m_ammoleftY, m_armorPattern, 10);
	ammoleft2 = MatchWhitePattern(m_width / 2 + m_ammoleftX010, m_ammoleftY, m_armorPattern, 10);
	ammoleft3 = MatchWhitePattern(m_width / 2 + m_ammoleftX001, m_ammoleftY, m_armorPattern, 10);

	if ((ammoleft1 != INVALID) && (ammoleft2 == INVALID) && (ammoleft3 == INVALID))
	{
		ammoLeft = ammoleft1;
	}
	else if ((ammoleft1 != INVALID) && (ammoleft2 != INVALID) && (ammoleft3 == INVALID))
	{
		ammoLeft = ammoleft1 * 10 + ammoleft2;
	}
	else if ((ammoleft1 != INVALID) && (ammoleft2 != INVALID) && (ammoleft3 != INVALID))
	{
		ammoLeft = ammoleft1 * 100 + ammoleft2 * 10 + ammoleft3;
	}
	else if ((ammoleft1 == INVALID) && (ammoleft2 == INVALID) && (ammoleft3 == INVALID))
	{
		ammoLeft = INVALID;
	}
	else
	{
		ammoLeft = MATCH_ERROR;
	}

	return ammoLeft;
}

int16_t CParserEngine::GetCredits()
{
	int16_t credits = 0;
	int credit;

	credits = INVALID;

	credit = MatchWhitePattern(m_width + m_creditsX1, m_creditsY, m_creditsPattern, 10);
	if (credit == INVALID)
	{
		return credits;
	}

	credits = credit;
	credit = MatchWhitePattern(m_width + m_creditsX10, m_creditsY, m_creditsPattern, 10);
	if (credit == INVALID)
	{
		return credits;
	}

	credits += credit * 10;
	credit = MatchWhitePattern(m_width + m_creditsX100, m_creditsY, m_creditsPattern, 10);
	if (credit == INVALID)
	{
		return credits;
	}

	credits += credit * 100;
	credit = MatchWhitePattern(m_width + m_creditsX1000, m_creditsY, m_creditsPattern, 10);
	if (credit == INVALID)
	{
		return credits;
	}

	credits += credit * 1000;
	credit = MatchWhitePattern(m_width + m_creditsX10000, m_creditsY, m_creditsPattern, 10);

	if (credit == INVALID)
		return credits;

	credits += credit * 10000;

	return credits;
}

int8_t CParserEngine::GetHasSpike()
{
	return MatchWhitePattern(m_width + m_spikeX, m_spikeY, m_spikePattern, 1);
}

int8_t CParserEngine::GetWeaponId()
{
	int8_t weaponId = -1;
	int nWeaponChange;

	nWeaponChange = MatchWhitePattern(m_width + m_weaponchangeX, m_weaponchangeY, m_weaponChangePattern, 1);

	if (nWeaponChange != INVALID)
	{
		uint8_t* p = nullptr;
		int c1, c2, c3;

		p = m_curBuffer + (m_width * m_weaponchangeY1 + m_width + m_weaponchangeX1) * 3;
		c1 = (int)(*p) + *(p + 1) + *(p + 2);
		p = m_curBuffer + (m_width * m_weaponchangeY2 + m_width + m_weaponchangeX2) * 3;
		c2 = (int)(*p) + *(p + 1) + *(p + 2);
		p = m_curBuffer + (m_width * m_weaponchangeY3 + m_width + m_weaponchangeX3) * 3;
		c3 = (int)(*p) + *(p + 1) + *(p + 2);

		if (c1 < 255 * 3 - 150 && c2 < 255 * 3 - 80 && c3 < 255 * 3 - 150)
		{
			//skill
			weaponId = -1;
		}
		else if (c1 >= c2 && c1 >= c3)
		{
			//automatic rifle
			weaponId = MatchWhitePattern(m_width + m_weaponX, m_weaponY1, &(m_weaponPattern[6]), 12);

			if (weaponId != INVALID)
			{
				weaponId += 6;
			}
		}
		else if (c2 >= c1 && c2 >= c3)
		{
			//pistol
			weaponId = MatchWhitePattern(m_width + m_weaponX, m_weaponY2, &(m_weaponPattern[1]), 5);

			if (weaponId != INVALID)
			{
				weaponId += 1;
			}
		}
		else if (c3 >= c1 && c3 >= c2)
		{
			//knife
			weaponId = 0;
		}
	}

	return weaponId;
}

int8_t CParserEngine::GetHitHeadShot()
{
	return MatchColorPattern(m_width / 2 + m_hitheadshotX, m_hitheadshotY, m_headshotPattern, 2);
}

void CParserEngine::GetAbilityLeft(int8_t* pAbilityInfo)
{
	int x = m_width / 2 - 1 - m_skillStep - (m_skillStep + m_skillWidth) / 2;
	int R = 95, G = 238, B = 184, diff = 35;
	uint8_t* p = nullptr;

	for (int j = 0; j < MAX_ABILITY; j++)
	{
		pAbilityInfo[j] = 0;

		int f = 0;
		for (int i = 0; i < m_skillWidth; i++)
		{
			p = m_curBuffer + (m_skillY * m_width + x + i) * 3;

			if ((abs(B - *p) < diff) && (abs(G - *(++p)) < diff) && (abs(R - *(++p)) < diff))
			{
				if (f == 0)
				{
					pAbilityInfo[j]++;
				}

				f = 1;
			}
			else
			{
				f = 0;
			}
		}

		x += m_skillStep;
	}
}

std::vector<std::string> CParserEngine::GetAllyAgents()
{
	std::vector<std::string> agentList;

	for (int i = 0; i < 5; i++)
	{
		int n = MatchColorPattern(m_width / 2 + m_agentsXAlly + i * m_agentsXStep, m_agentsY, m_agentsPattern, N_AGENTS);

		if (n == 19)
			n = 8;

		if (n != INVALID)
		{
			char szAliveAgent[MAX_PATH] = { 0 };

			sprintf_s(szAliveAgent, "agent%d", n);
			agentList.push_back(szAliveAgent);
		}
	}

	return agentList;
}

std::vector<std::string> CParserEngine::GetEnemyAgents()
{
	std::vector<std::string> agentList;
	for (int i = 0; i < MAX_ENEMIES; i++)
	{
		int n = MatchColorPattern(m_width / 2 + m_agentsXEnemy + i * m_agentsXStep, m_agentsY, m_agentsPattern, N_AGENTS, true);

		if (n == 19)
		{
			n = 8;
		}

		if (n != INVALID)
		{
			char szAliveAgent[MAX_PATH] = { 0 };

			sprintf_s(szAliveAgent, "agent%d", n);
			agentList.push_back(szAliveAgent);
		}
	}

	return agentList;
}

bool CParserEngine::LoadDB(int width, int height)
{
	FILE* fp = nullptr;
	char fn[MAX_PATH];

	m_width = width;
	if (m_height == height)
	{
		return true;
	}

	m_height = height;

	sprintf_s(fn, "ptn%d.dat", m_height);
	if (fopen_s(&fp, fn, "rb"))
	{
		m_height = 0;
		return false;
	}

	if (!fp)
	{
		return false;
	}
	
	fread(m_scorePattern, MAX_DIGITS, sizeof(WhitePattern), fp);
	fread(m_timePattern, MAX_DIGITS, sizeof(WhitePattern), fp);
	fread(m_ammoPattern, MAX_DIGITS, sizeof(WhitePattern), fp);
	fread(m_armorPattern, MAX_DIGITS, sizeof(WhitePattern), fp);
	fread(m_creditsPattern, MAX_DIGITS, sizeof(WhitePattern), fp);
	fread(m_agentsPattern, N_AGENTS, sizeof(ColorPattern), fp);
	fread(m_weaponChangePattern, 1, sizeof(WhitePattern), fp);
	fread(m_spikePattern, 1, sizeof(WhitePattern), fp);
	fread(m_headshotPattern, 2, sizeof(ColorPattern), fp);
	fread(m_otherPlayerPattern, 1, sizeof(WhitePattern), fp);
	fread(m_mapAgentsPattern, N_MAPAGENTS, sizeof(ColorPattern), fp);
	fread(m_mapRedCirclePattern, 1, sizeof(WhitePattern), fp);
	fread(m_weaponPattern, MAX_WEAPONS, sizeof(WhitePattern), fp);

	fclose(fp);

	char strApp[MAX_PATH];

	sprintf_s(strApp, "%d", m_height);

	m_allyScoreY	= GetPrivateProfileIntA(strApp, "allyScoreY", 0, POS_INI);
	m_allyScoreX1	= GetPrivateProfileIntA(strApp, "allyScoreX1", 0, POS_INI);
	m_allyScoreX10	= GetPrivateProfileIntA(strApp, "allyScoreX10", 0, POS_INI);
	m_allyScoreX01	= GetPrivateProfileIntA(strApp, "allyScoreX01", 0, POS_INI);
	m_enemyScoreY	= GetPrivateProfileIntA(strApp, "enemyScoreY", 0, POS_INI);
	m_enemyScoreX1	= GetPrivateProfileIntA(strApp, "enemyScoreX1", 0, POS_INI);
	m_enemyScoreX10 = GetPrivateProfileIntA(strApp, "enemyScoreX10", 0, POS_INI);
	m_enemyScoreX01 = GetPrivateProfileIntA(strApp, "enemyScoreX01", 0, POS_INI);

	m_timeY			= GetPrivateProfileIntA(strApp, "timeY", 0, POS_INI);
	m_timeX100		= GetPrivateProfileIntA(strApp, "timeX100", 0, POS_INI);
	m_timeX010		= GetPrivateProfileIntA(strApp, "timeX010", 0, POS_INI);
	m_timeX001		= GetPrivateProfileIntA(strApp, "timeX001", 0, POS_INI);

	m_ammoY			= GetPrivateProfileIntA(strApp, "ammoY", 0, POS_INI);
	m_ammoX100		= GetPrivateProfileIntA(strApp, "ammoX100", 0, POS_INI);
	m_ammoX010		= GetPrivateProfileIntA(strApp, "ammoX010", 0, POS_INI);
	m_ammoX001		= GetPrivateProfileIntA(strApp, "ammoX001", 0, POS_INI);

	m_healthY		= GetPrivateProfileIntA(strApp, "healthY", 0, POS_INI);
	m_healthX100	= GetPrivateProfileIntA(strApp, "healthX100", 0, POS_INI);
	m_healthX010	= GetPrivateProfileIntA(strApp, "healthX010", 0, POS_INI);
	m_healthX001	= GetPrivateProfileIntA(strApp, "healthX001", 0, POS_INI);
	m_healthX10		= GetPrivateProfileIntA(strApp, "healthX10", 0, POS_INI);
	m_healthX01		= GetPrivateProfileIntA(strApp, "healthX01", 0, POS_INI);
	m_healthX1		= GetPrivateProfileIntA(strApp, "healthX1", 0, POS_INI);

	m_armorY		= GetPrivateProfileIntA(strApp, "armorY", 0, POS_INI);
	m_armorX10		= GetPrivateProfileIntA(strApp, "armorX10", 0, POS_INI);
	m_armorX01		= GetPrivateProfileIntA(strApp, "armorX01", 0, POS_INI);
	m_armorX1		= GetPrivateProfileIntA(strApp, "armorX1", 0, POS_INI);

	m_ammoleftY		= GetPrivateProfileIntA(strApp, "ammoleftY", 0, POS_INI);
	m_ammoleftX100	= GetPrivateProfileIntA(strApp, "ammoleftX100", 0, POS_INI);
	m_ammoleftX010	= GetPrivateProfileIntA(strApp, "ammoleftX010", 0, POS_INI);
	m_ammoleftX001	= GetPrivateProfileIntA(strApp, "ammoleftX001", 0, POS_INI);

	m_creditsY		= GetPrivateProfileIntA(strApp, "creditsY", 0, POS_INI);
	m_creditsX1		= GetPrivateProfileIntA(strApp, "creditsX1", 0, POS_INI);
	m_creditsX10	= GetPrivateProfileIntA(strApp, "creditsX10", 0, POS_INI);
	m_creditsX100	= GetPrivateProfileIntA(strApp, "creditsX100", 0, POS_INI);
	m_creditsX1000	= GetPrivateProfileIntA(strApp, "creditsX1000", 0, POS_INI);
	m_creditsX10000 = GetPrivateProfileIntA(strApp, "creditsX10000", 0, POS_INI);

	m_agentsY		= GetPrivateProfileIntA(strApp, "agentsY", 0, POS_INI);
	m_agentsXAlly	= GetPrivateProfileIntA(strApp, "agentsXAlly", 0, POS_INI);
	m_agentsXEnemy	= GetPrivateProfileIntA(strApp, "agentsXEnemy", 0, POS_INI);
	m_agentsXStep	= GetPrivateProfileIntA(strApp, "agentsXStep", 0, POS_INI);

	m_weaponchangeY = GetPrivateProfileIntA(strApp, "weaponchangeY", 0, POS_INI);
	m_weaponchangeX = GetPrivateProfileIntA(strApp, "weaponchangeX", 0, POS_INI);
	m_weaponchangeY1 = GetPrivateProfileIntA(strApp, "weaponchangeY1", 0, POS_INI);
	m_weaponchangeX1 = GetPrivateProfileIntA(strApp, "weaponchangeX1", 0, POS_INI);
	m_weaponchangeY2 = GetPrivateProfileIntA(strApp, "weaponchangeY2", 0, POS_INI);
	m_weaponchangeX2 = GetPrivateProfileIntA(strApp, "weaponchangeX2", 0, POS_INI);
	m_weaponchangeY3 = GetPrivateProfileIntA(strApp, "weaponchangeY3", 0, POS_INI);
	m_weaponchangeX3 = GetPrivateProfileIntA(strApp, "weaponchangeX3", 0, POS_INI);
	m_weaponX		= GetPrivateProfileIntA(strApp, "weaponX", 0, POS_INI);
	m_weaponY1		= GetPrivateProfileIntA(strApp, "weaponY1", 0, POS_INI);
	m_weaponY2		= GetPrivateProfileIntA(strApp, "weaponY2", 0, POS_INI);
	m_weaponY3		= GetPrivateProfileIntA(strApp, "weaponY3", 0, POS_INI);

	m_spikeY		= GetPrivateProfileIntA(strApp, "spikeY", 0, POS_INI);
	m_spikeX		= GetPrivateProfileIntA(strApp, "spikeX", 0, POS_INI);
	m_hitheadshotY	= GetPrivateProfileIntA(strApp, "hitheadshotY", 0, POS_INI);
	m_hitheadshotX	= GetPrivateProfileIntA(strApp, "hitheadshotX", 0, POS_INI);
	m_otherY		= GetPrivateProfileIntA(strApp, "otherY", 0, POS_INI);
	m_otherX		= GetPrivateProfileIntA(strApp, "otherX", 0, POS_INI);

	m_skillY		= GetPrivateProfileIntA(strApp, "skillY", 0, POS_INI);
	m_skillStep		= GetPrivateProfileIntA(strApp, "skillStep", 0, POS_INI);
	m_skillWidth	= GetPrivateProfileIntA(strApp, "skillWidth", 0, POS_INI);

	return TRUE;
}

int CParserEngine::MatchWhitePattern(int x, int y, WhitePattern* pPattern, int patternSize)
{
	int ret = INVALID;

	ret = MatchWhitePattern_(x, y, pPattern, patternSize);

	if (ret == INVALID)
		ret = MatchWhitePattern_(x - 1, y, pPattern, patternSize);

	if (ret == INVALID)
		ret = MatchWhitePattern_(x + 1, y, pPattern, patternSize);

	return ret;
}

int CParserEngine::MatchWhitePattern_(int x, int y, WhitePattern* pPattern, int patternSize)
{
	int w = pPattern[0].width;
	int h = pPattern[0].height;
	
	uint8_t maxRGB = 0;
	uint64_t avgBlackColor = 0;

	uint32_t* blackImg = new uint32_t[w * h];
	uint32_t* pi = blackImg;

	int i, j, n;
	uint8_t* p = nullptr;
	for (j = 0; j < h; j++)
	{
		if (y + j >= m_height)
		{
			SAFE_DELETEA(blackImg);
			return INVALID;
		}

		for (i = 0; i < w; i++)
		{
			if (x + i >= m_width)
			{
				SAFE_DELETEA(blackImg);
				return INVALID;
			}

			p = m_curBuffer + (m_width * (y + j) + (x + i)) * 3;
			
			avgBlackColor += *p;
			*pi = *p;
			p++;

			avgBlackColor += *p;
			*pi += *p;
			p++;

			avgBlackColor += *p;
			*pi += *p;

			if (maxRGB < *pi)
			{
				maxRGB = *pi;
			}

			pi++;
		}
	}

	avgBlackColor /= (w * h);
	avgBlackColor += 9;

	if (avgBlackColor >= 253 * 3)
	{
		avgBlackColor = 253 * 3 - 1;
	}

	for (i = 0; i < w*h; i++)
	{
		if (blackImg[i] > avgBlackColor)
			blackImg[i] = 1;
		else
			blackImg[i] = 0;
	}
	
	for (n = patternSize-1; n >= 0; n--)
	{
		for (i = 0; i < w * h; i++)
		{
			if (pPattern[n].mask[i] == 0)
				continue;

			if (blackImg[i] == 0)
				break;
		}

		if (i == w * h)
		{
			SAFE_DELETEA(blackImg);
			return n;
		}
	}

	SAFE_DELETEA(blackImg);

	return INVALID;
}

int CParserEngine::MatchColorPattern(int x, int y, ColorPattern* pPattern, int patternSize, bool flip)
{
	int ret = INVALID;

	ret = MatchColorPattern_(x, y, pPattern, patternSize, flip);

	if (ret == INVALID)
		ret = MatchColorPattern_(x - 1, y, pPattern, patternSize, flip);

	if (ret == INVALID)
		ret = MatchColorPattern_(x + 1, y, pPattern, patternSize, flip);

	return ret;
}

int CParserEngine::MatchColorPattern_(int x, int y, ColorPattern* pPattern, int patternSize, bool flip)
{
	int w = pPattern[0].width;
	int h = pPattern[0].height;
	int diff = h * w * 3 * 300;
	uint8_t *p = nullptr;
	int i, j, n, ii, jj;
	int idx;
	int64_t total, t, tt;

	for (n = 0; n < patternSize; n++)
	{
		if ((pPattern[n].B[0] == 255) && (pPattern[n].G[0] == 255) && (pPattern[n].R[0] == 255))
		{
			continue;
		}

		total = 0;

		for (j = 0; j < h; j++)
		{
			for (i = 0; i < w; i++)
			{
				if (flip)
					idx = (j + 1) * w - 1 - i;
				else
					idx = j * w + i;

				t = 0xFFFFFFF;

				for (jj = -1; jj <= 1; jj++)
				{
					for (ii = -1; ii <= 1; ii++)
					{
						if (y + j + jj >= m_height)
							return INVALID;

						if (y + j + jj < 0)
							return INVALID;

						if (x + i + ii >= m_width)
							return INVALID;

						if (x + i + ii < 0)
							return INVALID;

						p = m_curBuffer + (m_width * (y + j + jj) + (x + i + ii)) * 3;
						tt = ((pPattern[n].B[idx] > (*p)) ? pPattern[n].B[idx] - (*p) : (*p) - pPattern[n].B[idx]) *
							((pPattern[n].B[idx] > (*p)) ? pPattern[n].B[idx] - (*p) : (*p) - pPattern[n].B[idx]);
						p++;
						tt += ((pPattern[n].G[idx] > (*p)) ? pPattern[n].G[idx] - (*p) : (*p) - pPattern[n].G[idx]) *
							((pPattern[n].G[idx] > (*p)) ? pPattern[n].G[idx] - (*p) : (*p) - pPattern[n].G[idx]);
						p++;
						tt += ((pPattern[n].R[idx] > (*p)) ? pPattern[n].R[idx] - (*p) : (*p) - pPattern[n].R[idx]) *
							((pPattern[n].R[idx] > (*p)) ? pPattern[n].R[idx] - (*p) : (*p) - pPattern[n].R[idx]);

						if (tt < t)
							t = tt;
					}
				}

				total += t;

				if (total >= diff)
				{
					break;
				}
			}

			if (i < w)
			{
				break;
			}
		}

		if (j == h)
		{
			return n;
		}
	}

	return INVALID;
}

int CParserEngine::MatchMapPattern(int x, int y, ColorPattern* pPattern, int patternSize)
{
	int w = pPattern[0].width, h = pPattern[0].height;
	uint8_t *p = nullptr;
	int i, j, n, d, dd, diff = 350 * h * w * 3;
	int idx;

	for (n = 0; n < patternSize; n++)
	{
		if ((pPattern[n].B[0] == 255) && (pPattern[n].G[0] == 255) && (pPattern[n].R[0] == 255))
		{
			continue;
		}

		d = 0;
		for (j = 0; j < h; j++)
		{
			for (i = 0; i < w; i++)
			{
				p = m_curBuffer + (m_width * (y + j) + (x + i)) * 3;
				idx = j * w + i;

				dd = abs((int)(pPattern[n].B[idx]) - (*p));
				dd = min(dd, abs((int)(pPattern[n].B[idx]) - (*(p + 3))));
				dd = min(dd, abs((int)(pPattern[n].B[idx]) - (*(p + m_width* 3))));
				dd = min(dd, abs((int)(pPattern[n].B[idx]) - (*(p + m_width * 3 + 3))));
				d += dd * dd;

				if (d > diff)
				{
					break;
				}

				p++;
				dd = abs((int)(pPattern[n].G[idx]) - (*p));
				dd = min(dd, abs((int)(pPattern[n].G[idx]) - (*(p + 3))));
				dd = min(dd, abs((int)(pPattern[n].G[idx]) - (*(p + m_width * 3))));
				dd = min(dd, abs((int)(pPattern[n].G[idx]) - (*(p + m_width * 3 + 3))));
				d += dd * dd;

				if (d > diff)
				{
					break;
				}

				p++;
				dd = abs((int)(pPattern[n].R[idx]) - (*p));
				dd = min(dd, abs((int)(pPattern[n].R[idx]) - (*(p + 3))));
				dd = min(dd, abs((int)(pPattern[n].R[idx]) - (*(p + m_width * 3))));
				dd = min(dd, abs((int)(pPattern[n].R[idx]) - (*(p + m_width * 3 + 3))));
				d += dd * dd;

				if (d > diff)
				{
					break;
				}
			}

			if (i < w)
			{
				break;
			}
		}

		if (j == h)
		{
			return n;
		}
	}

	return INVALID;
}

std::vector<EnemyRect> CParserEngine::GetEnemyOnMap()
{
	std::vector<EnemyRect> aryMapEnemyRects;
	int dxy = 50 * m_height / 1080, oxy = 227 * m_height / 1080;
	int i, j, m;
	int ii, jj;
	int wid = oxy * 2 - dxy * 2;
	int hei = oxy - dxy + 15;
	uint8_t* pMap = new uint8_t[wid * hei], *p;

	//detect red color on map
	for (int j = 0; j < hei; j++)
	{
		for (int i = 0; i < wid; i++)
		{
			p = m_curBuffer + ((dxy + j) * m_width + dxy + i) * 3;

			if ((p[2] == 255) && (p[1] == 0) && (p[0] == 0))
			{
				pMap[j * wid + i] = 0;
			}
			else if ((abs((int)(p[0]) - p[1]) < 5) && ((int)(p[2]) - p[1] > 70))
			{
				pMap[j * wid + i] = 255;
			}
			else if ((abs((int)(p[0]) - 80) < 6) && (abs((int)(p[1]) - 94) < 6) && (p[2] > 200))
			{
				pMap[j * wid + i] = 255;
			}
			else if ((abs((int)(p[0]) - 68) < 6) && (abs((int)(p[1]) - 82) < 6) && (p[2] > 180))
			{
				pMap[j * wid + i] = 255;
			}
			else
			{
				pMap[j * wid + i] = 0;
			}
		}
	}

	//set detect area on map
	for (j = 0; j < hei; j++)
	{
		for (i = 0; i < wid; i++)
		{
			if (pMap[j * wid + i] == 0)
			{
				continue;
			}

			for (jj = 0; jj < m_mapRedCirclePattern->height; jj++)
			{
				if (j - jj < 0)
				{
					break;
				}

				for (ii = 0; ii < m_mapRedCirclePattern->width; ii++)
				{
					if (i - ii < 0)
					{
						break;
					}

					pMap[(j - jj) * wid + (i - ii)] |= 1;
				}
			}

		}
	}

	//detect red circle on map
	for (j = 0; j <= hei - m_mapRedCirclePattern->height; j++)
	{
		if (j >= hei)
		{
			continue;
		}

		for (i = 0; i <= wid - m_mapRedCirclePattern->width; i++)
		{
			if (i >= wid)
			{
				continue;
			}

			if (pMap[j * wid + i] == 0)
			{
				continue;
			}

			int cc = 0;

			for (jj = 0; jj < m_mapRedCirclePattern->height; jj++)
			{
				for (ii = 0; ii < m_mapRedCirclePattern->width; ii++)
				{
					if (
						(pMap[(j + jj) * wid + i + ii] > 1) && 
						(m_mapRedCirclePattern->mask[jj * m_mapRedCirclePattern->width + ii])
					)
					{
						cc++;
					}
				}
			}


			if (cc >= m_mapRedCirclePattern->width * 2 / 3)//detect agent icon on map
			{
				ii = dxy + i + (m_mapRedCirclePattern->width - m_mapAgentsPattern[0].width) / 2;
				jj = dxy + j + (m_mapRedCirclePattern->height - m_mapAgentsPattern[0].height) / 2;

				EnemyRect enemyMapRect;
				enemyMapRect.m_mapX = ii + (m_mapAgentsPattern[0].width + 1) / 2 - oxy;
				enemyMapRect.m_mapY = oxy - jj - (m_mapAgentsPattern[0].height + 1) / 2;

				if (enemyMapRect.m_mapY < 1)
				{
					break;
				}

				for (m = 0; m < aryMapEnemyRects.size(); m++)
				{
					if (abs(aryMapEnemyRects[m].m_mapX - enemyMapRect.m_mapX) +
						abs(aryMapEnemyRects[m].m_mapY - enemyMapRect.m_mapY) <
						(m_mapRedCirclePattern->width + m_mapAgentsPattern[0].width) / 2 - 1)
					{
						break;
					}
				}

				if (m < aryMapEnemyRects.size())
				{
					continue;
				}

				m = MatchMapPattern(ii, jj, m_mapAgentsPattern, N_MAPAGENTS);

				if (m != INVALID)
				{
					char szAliveAgent[MAX_PATH] = { 0 };

					m /= 2;
					
					sprintf_s(szAliveAgent, "agent%d", m);
					enemyMapRect.m_enemyAgent = szAliveAgent;
					aryMapEnemyRects.push_back(enemyMapRect);

					if (aryMapEnemyRects.size() == 5)
					{
						break;
					}
				}

			}
		}

		if (aryMapEnemyRects.size() == 5)
		{
			break;
		}
	}

	SAFE_DELETEA(pMap);

	//sort by mapY
	EnemyRect tmp;
	for (i = 0; i < aryMapEnemyRects.size() - 1; i++)
	{
		for (j = i + 1; j < aryMapEnemyRects.size(); j++)
		{
			if (aryMapEnemyRects[i].m_mapY > aryMapEnemyRects[j].m_mapY)
			{
				tmp = aryMapEnemyRects[i]; 
				aryMapEnemyRects[i] = aryMapEnemyRects[j]; 
				aryMapEnemyRects[j] = tmp;
			}
		}
	}

	//detect enemy on screen according mapEnemy
	int k, siz;

	for (k = 0; k < aryMapEnemyRects.size(); k++)
	{
		int dx = aryMapEnemyRects[k].m_mapX;
		int dy = aryMapEnemyRects[k].m_mapY;
		int x;

		siz = m_width / 2 / dy;

		for (j = m_height / 4; j < m_height * 3 / 4; j++)
		{
			x = m_width / 2 + (m_width * 2 / 5) * dx / dy;
			p = m_curBuffer + (j * m_width) * 3;

			int i1, i2;

			i1 = MAX(0, x - siz * 3 / 2); i2 = MIN(x + siz * 3 / 2, m_width);

			for (i = i1; i < i2; i++)
			{
				//avoid past enemy rect
				int kk;

				for (kk = 0; kk < k; kk++)
				{
					if (
						(i >= aryMapEnemyRects[kk].m_enemyNormal.x1) && 
						(i <= aryMapEnemyRects[kk].m_enemyNormal.x2) &&
						(j >= aryMapEnemyRects[kk].m_enemyNormal.y1) && 
						(j <= aryMapEnemyRects[kk].m_enemyNormal.y2)
					)
					{
						break;
					}
				}

				if (kk < k)
				{
					continue;
				}

				if (
					(
						(*(p + i * 3) < 80) && 
						(*(p + i * 3 + 1) < 100) && 
						(*(p + i * 3 + 2) > 252)
					) ||
					(
						(*(p + i * 3) < 120) && 
						(*(p + i * 3 + 1) < 120) && 
						(*(p + i * 3 + 2) > 252) && 
						(abs((int)(*(p + i * 3)) - *(p + i * 3 + 1)) < 4)
					) ||
					(
						(abs((int)(*(p + i * 3)) - *(p + i * 3 + 1)) < 4) && 
						(*(p + i * 3 + 2) > *(p + i * 3 + 1) * 3) && 
						(*(p + i * 3 + 2) > 150)
					)
				)
				{
					aryMapEnemyRects[k].m_enemyNormal.y1 = j;
					aryMapEnemyRects[k].m_enemyNormal.x1 = i - siz * 3 / 4;
					aryMapEnemyRects[k].m_enemyNormal.x2 = i + siz * 3 / 4;
					aryMapEnemyRects[k].m_enemyNormal.y2 = MIN(j + 4 * siz, m_height - 1);
					break;
				}
			}

			if (i < i2)
			{
				break;
			}
		}
	}

	return aryMapEnemyRects;
}

std::vector<EnemyRect> CParserEngine::GetEnemyOnScreen()
{
	std::vector<EnemyRect> aryEnemyRects;

	//enemy detect
	uint8_t* p, * pt = m_curBuffer;
	uint8_t* pp, * pptr = new uint8_t[m_width * m_height * 3];

	memset(pptr, 0, m_width * m_height * 3);

	int i, j, ii, jj, tp = m_height, bt = -1, rt = -1, lt = m_width, cnt = 0;
	int mapSize = 440 * m_height / 1080;
	int nEnemyCount = 0;

	for (j = 5; j < m_height - 5; j++)
	{
		p = pt + j * m_width * 3; p += 3 * 5;
		pp = pptr + j * m_width * 3; pp += 3 * 5;

		for (i = 5; i < m_width - 5; i++)
		{
			if ((i < mapSize) && (j < mapSize))
			{
				p += 3;
				pp += 3;
				continue;
			}

			int v1 = ((*(p + 1) < *p) ? *p - *(p + 1) : *(p + 1) - *p);
			if (
				(*(p + 2) >= 254) && 
				(*(p + 1) != 0) && 
				(*p != 0) && 
				(
					(
						(*(p + 1) < 50) && 
						(*p < 50) && 
						(*(p + 1) < *p * 1.5)
					) || 
					(
						(*(p + 1) < 70) && 
						(*p < 70) && 
						(v1 < 4)
					)
				)
			)
			{
				int cc = 0, st = 2; byte d = 150; byte* zz;

				for (jj = -st; jj <= st; jj++)
				{
					for (ii = -st; ii <= st; ii++)
					{
						zz = pt + ((j + jj) * m_width + i + ii) * 3;
						if (zz[2] < d)cc++;
						if (zz[2] / 3 * 2 < zz[0] / 2 + zz[1] / 2)cc++;
					}
				}

				if (cc >= 23)
				{
					*pp = 255;

					if (i > rt) rt = i;
					if (i < lt) lt = i;
					if (j < tp) tp = j;
					if (j > bt) bt = j;

					cnt++;
				}
			}

			p += 3;
			pp += 3;
		}
	}

	int r, l, t, b;
	int r1, l1, t1, b1;
	double screenWidthRate = ((double)(1080)) * m_width / 1920 / m_height;

	if (cnt > 1)
	{
		double rate = (double(rt - lt)) / (bt - tp) / screenWidthRate;
		//			cout << "  " << rate << "  " << rt - lt << "  " << bt - tp;

		if ((rate < 0.9) || ((bt - tp < 27) && (rate < 3)))//one enemy  
		{
			EnemyRect enemy;
			enemy.m_enemyNormal.x1 = lt;
			enemy.m_enemyNormal.y1 = tp;
			enemy.m_enemyNormal.x2 = rt;
			enemy.m_enemyNormal.y2 = bt;

			aryEnemyRects.push_back(enemy);
			nEnemyCount = 1;
		}
		else//two enemy
		{
			//first enemy
			r = lt; l = rt; t = bt; b = tp;

			for (j = tp; j <= bt; j++)
			{
				pp = pptr + (j * m_width + lt) * 3;

				for (i = lt; i < (lt + rt) / 2; i++)
				{
					if (*pp)
					{
						if (i > r) r = i;
						if (i < l) l = i;
						if (j < t) t = j;
						if (j > b) b = j;
					}

					pp += 3;
				}
			}

			r1 = r; l1 = l; t1 = t; b1 = b;

			//second enemy
			r = lt; l = rt; t = bt; b = tp;

			for (j = tp; j <= bt; j++)
			{
				pp = pptr + (j * m_width + (lt + rt) / 2) * 3;

				for (i = (lt + rt) / 2; i <= rt; i++)
				{
					if (*pp)
					{
						if (i > r) r = i;
						if (i < l) l = i;
						if (j < t) t = j;
						if (j > b) b = j;
					}

					pp += 3;
				}
			}

			if (l - r1 < (rt - lt) * 0.1)//one enemy
			{
				if (((rt - lt) > (bt - tp) * 4) && (bt - tp < 10))
				{
					nEnemyCount = 0;
				}
				else
				{
					EnemyRect enemy;
					enemy.m_enemyNormal.x1 = lt;
					enemy.m_enemyNormal.y1 = tp;
					enemy.m_enemyNormal.x2 = rt;
					enemy.m_enemyNormal.y2 = bt;

					aryEnemyRects.push_back(enemy);
					nEnemyCount = 1;
				}
			}
			else//two enemy
			{
				if ((b1 == t1) || 
					(
						((r1 - l1) > (b1 - t1) * 4) && 
						(b1 - t1 < 15)
					)
				)
				{
					aryEnemyRects.clear();
					nEnemyCount = 0;
				}
				else
				{
					EnemyRect enemy;
					enemy.m_enemyNormal.x1 = l1;
					enemy.m_enemyNormal.y1 = t1;
					enemy.m_enemyNormal.x2 = r1;
					enemy.m_enemyNormal.y2 = b1;

					aryEnemyRects.push_back(enemy);
					nEnemyCount = 1;

				}

				if (!((b == t) || (((r - l) > (b - t) * 4) && (b - t < 15))))
				{
					aryEnemyRects[nEnemyCount].m_enemyNormal.x1 = l;
					aryEnemyRects[nEnemyCount].m_enemyNormal.y1 = t;
					aryEnemyRects[nEnemyCount].m_enemyNormal.x2 = r;
					aryEnemyRects[nEnemyCount].m_enemyNormal.y2 = b;
					nEnemyCount++;
				}
			}
		}
	}

	//recheck rect
	const int bpp = 3;
	for (i = 0; i < aryEnemyRects.size(); i++)
	{
		r = (int)min(
			m_width, 
			aryEnemyRects[i].m_enemyNormal.x2 + 
			max(
				(
					aryEnemyRects[i].m_enemyNormal.x2 -
					aryEnemyRects[i].m_enemyNormal.x1
				) * 0.2, 
				5
			)
		);
		l = (int)max(
			0, 
			aryEnemyRects[i].m_enemyNormal.x1 - 
			max(
				(
					aryEnemyRects[i].m_enemyNormal.x2 - 
					aryEnemyRects[i].m_enemyNormal.x1
				) * 0.2, 
				5
			)
		);
		t = (int)max(
			0, 
			tp - 
			max(
				(
					aryEnemyRects[i].m_enemyNormal.y2 - 
					aryEnemyRects[i].m_enemyNormal.y1
				) * 0.2, 
				30
			)
		);
		b = (int)min(
			m_height, 
			aryEnemyRects[i].m_enemyNormal.y2 + 
			max(
				(
					aryEnemyRects[i].m_enemyNormal.y2 -
					aryEnemyRects[i].m_enemyNormal.y1
				) * 0.2, 
				30
			)
		);

		for (j = t; j < b; j++)
		{
			p = pt + (j * m_width + l) * bpp;
			pp = pptr + (j * m_width + l) * bpp;

			for (int k = l; k < r; k++)
			{
				int v1 = ((*(p + 1) < *p) ? *p-*(p+1) : *(p+1)-*p);
				if (
					(*(p + 2) >= 254) && 
					(*(p + 1) != 0) && 
					(*p != 0) && 
					(
						((*(p + 1) < 50) && (*p < 50)) || 
						(
							(*(p + 1) < 120) && 
							(*p < 120) && 
							(v1 < 4)
						)
					)
				)
				{
					*pp = 255;
					if (k > aryEnemyRects[i].m_enemyNormal.x2)
					{
						aryEnemyRects[i].m_enemyNormal.x2 = k;
					}

					if (k < aryEnemyRects[i].m_enemyNormal.x1)
					{
						aryEnemyRects[i].m_enemyNormal.x1 = k;
					}

					if (j < aryEnemyRects[i].m_enemyNormal.y1)
					{
						aryEnemyRects[i].m_enemyNormal.y1 = j;
					}
					
					if (j > aryEnemyRects[i].m_enemyNormal.y2)
					{
						aryEnemyRects[i].m_enemyNormal.y2 = j;
					}
				}

				p += bpp;
				pp += bpp;
			}
		}

		aryEnemyRects[i].m_enemyNormal.y2 = (int)min(
			m_height, 
			(
				aryEnemyRects[i].m_enemyNormal.y2 + 
				(
					aryEnemyRects[i].m_enemyNormal.y2 - 
					aryEnemyRects[i].m_enemyNormal.y1
				) 
				* 0.3
			)
		);
	}

	SAFE_DELETEA(pptr);

	return aryEnemyRects;
}

std::vector<EnemyRect> CParserEngine::GetEnemy(int otherPlayer_, int8_t health_)
{
	int i, j;

	std::vector<EnemyRect> enemyRects = GetEnemyOnScreen();
	if (otherPlayer_ != 0 && health_ != INVALID)
	{
		std::vector<EnemyRect> enemyMapRects = GetEnemyOnMap();

		// combine two result
		for (i = 0; i < enemyMapRects.size(); i ++)
		{
			if (enemyMapRects[i].m_enemyNormal.x2 == 0)
			{
				continue;
			}

			for (j = 0; j < enemyRects.size(); j ++)
			{
				if (enemyRects[j].m_enemyNormal.x2 == 0)
				{
					continue;
				}

				if (enemyMapRects[i].m_enemyNormal.y1 <= enemyRects[j].m_enemyNormal.y2 &&
					enemyMapRects[i].m_enemyNormal.y2 >= enemyRects[j].m_enemyNormal.y1 &&
					enemyMapRects[i].m_enemyNormal.x1 <= enemyRects[j].m_enemyNormal.x2 &&
					enemyMapRects[i].m_enemyNormal.x2 >= enemyRects[j].m_enemyNormal.x1)
				{
					break;
				}
			}

			if (j < enemyRects.size())
			{
				int64_t v1 =
					(
						enemyRects[j].m_enemyNormal.x2 - 
						enemyRects[j].m_enemyNormal.x1 + 1
					) * 
					(
						enemyRects[j].m_enemyNormal.y2 - 
						enemyRects[j].m_enemyNormal.y1 + 1
					);
				int64_t v2 =
					(
						enemyMapRects[i].m_enemyNormal.x2 - 
						enemyMapRects[i].m_enemyNormal.x1 + 1
					) * 
					(
						enemyRects[i].m_enemyNormal.y2 - 
						enemyRects[i].m_enemyNormal.y1 + 1
					) * 2;
				
				if (v1 < v2)
				{
					enemyMapRects[i].m_enemyNormal.x2 = 
						(
							enemyMapRects[i].m_enemyNormal.x2 + 
							enemyRects[j].m_enemyNormal.x2
						) / 2;
					enemyMapRects[i].m_enemyNormal.x1 = 
						(
							enemyMapRects[i].m_enemyNormal.x1 + 
							enemyRects[j].m_enemyNormal.x1
						) / 2;
					enemyMapRects[i].m_enemyNormal.y1 = 
						min(
							enemyMapRects[i].m_enemyNormal.y1, 
							enemyRects[j].m_enemyNormal.y1
						);
					enemyMapRects[i].m_enemyNormal.y2 = 
						(
							enemyMapRects[i].m_enemyNormal.y2 + 
							enemyRects[j].m_enemyNormal.y2
						) / 2;
				}

				enemyRects[j].m_enemyNormal.x2 = 0;
			}
		}

		size_t nEnemy = enemyRects.size();
		for (j = 0; j < nEnemy; j ++)
		{
			if (enemyRects[j].m_enemyNormal.x2 == 0)
			{
				nEnemy--;
				enemyRects[j] = enemyRects[nEnemy];
				j--;
			}
		}

		for (i = 0; i < enemyMapRects.size(); i ++)
		{
			if (enemyMapRects[i].m_enemyNormal.x2 > 0)
			{
				enemyRects[nEnemy] = enemyMapRects[i];
				nEnemy++;
			}
		}
	}

	std::vector<EnemyRect> res;

	// set head, body, leg
	for (i = 0; i < enemyRects.size(); i ++)
	{
		enemyRects[i].m_enemyHead.x1 = 
			(
				enemyRects[i].m_enemyNormal.x1 * 65 + 
				enemyRects[i].m_enemyNormal.x2 * 35
			) / 100;
		enemyRects[i].m_enemyHead.x2 = 
			(
				enemyRects[i].m_enemyNormal.x1 * 35 + 
				enemyRects[i].m_enemyNormal.x2 * 65
			) / 100;
		enemyRects[i].m_enemyHead.y1 = enemyRects[i].m_enemyNormal.y1;
		enemyRects[i].m_enemyHead.y2 = 
			(
				enemyRects[i].m_enemyNormal.y1 * 90 + 
				enemyRects[i].m_enemyNormal.y2 * 10
			) / 100;

		enemyRects[i].m_enemyBody.x1 = enemyRects[i].m_enemyNormal.x1;
		enemyRects[i].m_enemyBody.x2 = enemyRects[i].m_enemyNormal.x2;
		enemyRects[i].m_enemyBody.y1 = 
			(
				enemyRects[i].m_enemyNormal.y1 * 90 + 
				enemyRects[i].m_enemyNormal.y2 * 10
			) / 100;
		enemyRects[i].m_enemyBody.y2 = 
			(
				enemyRects[i].m_enemyNormal.y1 * 50 + 
				enemyRects[i].m_enemyNormal.y2 * 50
			) / 100;

		enemyRects[i].m_enemyLeg.x1 = enemyRects[i].m_enemyNormal.x1;
		enemyRects[i].m_enemyLeg.x2 = enemyRects[i].m_enemyNormal.x2;
		enemyRects[i].m_enemyLeg.y1 = 
			(
				enemyRects[i].m_enemyNormal.y1 * 50 + 
				enemyRects[i].m_enemyNormal.y2 * 50
			) / 100;
		enemyRects[i].m_enemyLeg.y2 = enemyRects[i].m_enemyNormal.y2;

		enemyRects[i].m_enemyNormal.x1 -= 3;
		enemyRects[i].m_enemyNormal.y1 -= 3;
		enemyRects[i].m_enemyNormal.x2 += 3;
		enemyRects[i].m_enemyNormal.y2 += 3;

		res.push_back(enemyRects[i]);
	}

	return res;
}
