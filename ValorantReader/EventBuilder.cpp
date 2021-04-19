#include "structs.h"
#include "EventBuilder.h"

CEventBuilder::CEventBuilder()
{
	m_shoot = 0;
	m_enemyAppear = 0;
	m_changeTargetShootDely = 1500;
	m_killedEnemy = "";
}

CEventBuilder::~CEventBuilder()
{
}

bool CEventBuilder::Build(CTimelineData& currSlice, std::list<CTimelineData>& aryTimelineData, int32_t timestamp)
{
	if (aryTimelineData.size() == 0)
	{
		aryTimelineData.push_back(currSlice);
		return true;
	}

	CTimelineData prevSlice = aryTimelineData.back();

	m_timestamp = timestamp;

	// ShootEvent
	BuildShootEventFromAmmo(currSlice, prevSlice);
	BuildShootEventFromEtc(currSlice, prevSlice);

	// KillEvent
	BuildKillEvent(currSlice, prevSlice);

	// DeathEvent
	BuildDeathEvent(currSlice, prevSlice);

	// AmmoReloadEvent
	BuildAmmoReloadEvent(currSlice, prevSlice);

	// EnemyAppearedEvent
	BuildEnemyAppearedEvent(currSlice, prevSlice);

	// AbilityCastEvent
	BuildAbiltyCastEvent(currSlice, prevSlice);

	return true;
}

void CEventBuilder::BuildAbiltyCastEvent(CTimelineData& currSlice, CTimelineData& prevSlice)
{
	if (currSlice.m_playerState.m_health > 0 && prevSlice.m_playerState.m_ammo == INVALID)
	{
		if (prevSlice.m_playerState.m_aryAbilityLeft[0] == currSlice.m_playerState.m_aryAbilityLeft[0] + 1 &&
			prevSlice.m_playerState.m_aryAbilityLeft[1] == currSlice.m_playerState.m_aryAbilityLeft[1] &&
			prevSlice.m_playerState.m_aryAbilityLeft[2] == currSlice.m_playerState.m_aryAbilityLeft[2] &&
			prevSlice.m_playerState.m_aryAbilityLeft[3] == currSlice.m_playerState.m_aryAbilityLeft[3])
		{
			// used C skill
			CAbilityCastEvent* pEvent = new CAbilityCastEvent();

			pEvent->m_ability = 1;

			currSlice.m_aryEvents.push_back(pEvent);
		}

		if (prevSlice.m_playerState.m_aryAbilityLeft[0] == currSlice.m_playerState.m_aryAbilityLeft[0] &&
			prevSlice.m_playerState.m_aryAbilityLeft[1] == currSlice.m_playerState.m_aryAbilityLeft[1] + 1 &&
			prevSlice.m_playerState.m_aryAbilityLeft[2] == currSlice.m_playerState.m_aryAbilityLeft[2] &&
			prevSlice.m_playerState.m_aryAbilityLeft[3] == currSlice.m_playerState.m_aryAbilityLeft[3])
		{
			// used Q skill
			CAbilityCastEvent* pEvent = new CAbilityCastEvent();

			pEvent->m_ability = 0;

			currSlice.m_aryEvents.push_back(pEvent);
		}

		if (prevSlice.m_playerState.m_aryAbilityLeft[0] == currSlice.m_playerState.m_aryAbilityLeft[0] &&
			prevSlice.m_playerState.m_aryAbilityLeft[1] == currSlice.m_playerState.m_aryAbilityLeft[1] &&
			prevSlice.m_playerState.m_aryAbilityLeft[2] == currSlice.m_playerState.m_aryAbilityLeft[2] + 1 &&
			prevSlice.m_playerState.m_aryAbilityLeft[3] == currSlice.m_playerState.m_aryAbilityLeft[3])
		{
			// used E skill
			CAbilityCastEvent* pEvent = new CAbilityCastEvent();

			pEvent->m_ability = 2;

			currSlice.m_aryEvents.push_back(pEvent);
		}

		if (prevSlice.m_playerState.m_aryAbilityLeft[0] == currSlice.m_playerState.m_aryAbilityLeft[0] &&
			prevSlice.m_playerState.m_aryAbilityLeft[1] == currSlice.m_playerState.m_aryAbilityLeft[1] &&
			prevSlice.m_playerState.m_aryAbilityLeft[2] == currSlice.m_playerState.m_aryAbilityLeft[2] &&
			prevSlice.m_playerState.m_aryAbilityLeft[3] == currSlice.m_playerState.m_aryAbilityLeft[3] + 1)
		{
			// used X skill
			CAbilityCastEvent* pEvent = new CAbilityCastEvent();

			pEvent->m_ability = 3;

			currSlice.m_aryEvents.push_back(pEvent);
		}
	}
}

void CEventBuilder::BuildAmmoReloadEvent(CTimelineData& currSlice, CTimelineData& prevSlice)
{
	if (prevSlice.m_playerState.m_ammo >= 0 && prevSlice.m_playerState.m_ammoLeft >= 0 &&
		currSlice.m_playerState.m_ammo >= 0 && currSlice.m_playerState.m_ammoLeft >= 0 &&
		prevSlice.m_playerState.m_ammoLeft > currSlice.m_playerState.m_ammoLeft &&
		prevSlice.m_playerState.m_ammo + prevSlice.m_playerState.m_ammoLeft == currSlice.m_playerState.m_ammo + currSlice.m_playerState.m_ammoLeft)
	{
		CAmmoReloadEvent* pEvent = new CAmmoReloadEvent();

		pEvent->m_oldAmmo = prevSlice.m_playerState.m_ammo;
		pEvent->m_newAmmo = currSlice.m_playerState.m_ammo;
		
		currSlice.m_aryEvents.push_back(pEvent);
	}
}

void CEventBuilder::BuildKillEvent(CTimelineData& currSlice, CTimelineData& prevSlice)
{
	int cntAlly0 = 0, cntEnemy0 = 0, cntAlly = 0, cntEnemy = 0;
	int i, j;

	cntAlly = (int)prevSlice.m_gameState.m_aryYourAgents.size();
	cntEnemy = (int)prevSlice.m_gameState.m_aryEnemyAgents.size();
	cntAlly0 = (int)currSlice.m_gameState.m_aryYourAgents.size();
	cntEnemy0 = (int)currSlice.m_gameState.m_aryEnemyAgents.size();

	if (cntAlly > cntAlly0 && (!(cntAlly0 == 0 && cntEnemy0 == 0)))
	{
		for (i = 0; i < (int)prevSlice.m_gameState.m_aryYourAgents.size(); i ++)
		{
			for (j = 0; j < (int)currSlice.m_gameState.m_aryYourAgents.size(); j ++)
			{
				if (prevSlice.m_gameState.m_aryYourAgents[i].compare(currSlice.m_gameState.m_aryYourAgents[j]) == 0)
				{
					break;
				}
			}

			if (j == currSlice.m_gameState.m_aryYourAgents.size())
			{
				if (currSlice.m_playerState.m_isAlive &&
					prevSlice.m_playerState.m_health == INVALID &&
					prevSlice.m_playerState.m_ammo == INVALID)
				{
					currSlice.m_playerState.m_isAlive = false;
					currSlice.m_playerState.m_health = 0;
					currSlice.m_playerState.m_ammo = 0;

					CYouKilledEvent* pEvent = new CYouKilledEvent();

					pEvent->m_killerAgent = prevSlice.m_gameState.m_aryYourAgents[i];
					currSlice.m_aryEvents.push_back(pEvent);
				}
				else
				{
					CKillEvent * pEvent = new CKillEvent();

					pEvent->m_killerAgent = prevSlice.m_gameState.m_aryYourAgents[i];
					currSlice.m_aryEvents.push_back(pEvent);
				}
			}
		}
	}
}

void CEventBuilder::BuildDeathEvent(CTimelineData& currSlice, CTimelineData& prevSlice)
{
	int cntAlly0 = 0, cntEnemy0 = 0, cntAlly = 0, cntEnemy = 0;
	int i, j;

	cntAlly = (int)prevSlice.m_gameState.m_aryYourAgents.size();
	cntEnemy = (int)prevSlice.m_gameState.m_aryEnemyAgents.size();
	cntAlly0 = (int)currSlice.m_gameState.m_aryYourAgents.size();
	cntEnemy0 = (int)currSlice.m_gameState.m_aryEnemyAgents.size();

	if (cntEnemy > cntEnemy0 && (!(cntAlly0 == 0 && cntEnemy0 == 0)))
	{
		for (i = 0; i < 5; i ++)
		{
			for (j = 0; j < (int)prevSlice.m_gameState.m_aryEnemyAgents.size(); j ++)
			{
				if (prevSlice.m_gameState.m_aryEnemyAgents[i].compare(currSlice.m_gameState.m_aryEnemyAgents[j]) == 0)
				{
					break;
				}
			}

			if (j == prevSlice.m_gameState.m_aryEnemyAgents.size())
			{
				CDeathEvent* pEvent = new CDeathEvent();

				pEvent->m_killerAgent = prevSlice.m_gameState.m_aryEnemyAgents[i];
				m_killedEnemy = prevSlice.m_gameState.m_aryEnemyAgents[i];
				currSlice.m_aryEvents.push_back(pEvent);
			}
		}
	}
}

void CEventBuilder::BuildEnemyAppearedEvent(CTimelineData& currSlice, CTimelineData& prevSlice)
{
	int j, k;
	int idx = 0, fOldj[5] = { 0 }, fOldk[5] = { 0 }, d, dd;

	if (prevSlice.m_gameState.m_enemyList.size() < currSlice.m_gameState.m_enemyList.size())
	{
		for (j = 0; j < (int)prevSlice.m_gameState.m_enemyList.size(); j ++)
		{
			for (k = 0; k < (int)currSlice.m_gameState.m_enemyList.size(); k++)
			{
				if (fOldk[k])
				{
					continue;
				}

				if (prevSlice.m_gameState.m_enemyList[j].m_enemyAgent.compare(currSlice.m_gameState.m_enemyList[k].m_enemyAgent) == 0)
				{
					fOldj[j] = 1;
					fOldk[k] = 1;
					break;
				}
			}

			if (k < (int)currSlice.m_gameState.m_enemyList.size())
			{
				continue;
			}
		}

		for (j = 0; j < (int)prevSlice.m_gameState.m_enemyList.size(); j ++)
		{
			if (fOldj[j])
			{
				continue;
			}

			d = 100000;

			for (k = 0; k < (int)currSlice.m_gameState.m_enemyList.size(); k++)
			{
				if (fOldk[k])
					continue;

				auto curr = currSlice.m_gameState.m_enemyList[k];
				auto prev = prevSlice.m_gameState.m_enemyList[j];

				dd = abs(curr.m_enemyNormal.x1 + curr.m_enemyNormal.x2 - prev.m_enemyNormal.x1 - prev.m_enemyNormal.x2)
					+ abs(curr.m_enemyNormal.y1 + curr.m_enemyNormal.y2 - prev.m_enemyNormal.y1 - prev.m_enemyNormal.y2);

				if (dd < d)
				{
					d = dd;
					idx = k;
				}
			}

			fOldk[idx] = 1;
		}

		for (k = 0; k < (int)currSlice.m_gameState.m_enemyList.size(); k ++)
		{
			CEnemyAppearedEvent* pEvent = new CEnemyAppearedEvent();

			pEvent->m_enemyAgent = currSlice.m_gameState.m_enemyList[k].m_enemyAgent;
			pEvent->m_enemyRect = currSlice.m_gameState.m_enemyList[k];
		}

		if (prevSlice.m_gameState.m_enemyList.size() == 0)
		{
			m_enemyAppear = m_timestamp;
		}
	}
}

void CEventBuilder::BuildShootEventFromAmmo(CTimelineData& currSlice, CTimelineData& prevSlice)
{
	if (prevSlice.m_playerState.m_ammo >= 0 &&
		currSlice.m_playerState.m_ammo >= 0 &&
		prevSlice.m_playerState.m_ammo > currSlice.m_playerState.m_ammo &&
		prevSlice.m_playerState.m_ammoLeft == currSlice.m_playerState.m_ammoLeft)
	{
		int32_t tOldShoot = m_shoot;

		m_shoot = m_timestamp;

		CShootEvent* pEvent = new CShootEvent();

		if (prevSlice.m_gameState.m_enemyList.size() > 0 &&
			m_enemyAppear)
		{
			pEvent->m_isFirstShot = 1;
			pEvent->m_reactionTimeMs = m_shoot - m_enemyAppear;
			m_enemyAppear = 0;
		}
		else
		{
			pEvent->m_reactionTimeMs = -1;
			pEvent->m_isFirstShot = 0;
		}
		
		pEvent->m_weaponId = prevSlice.m_playerState.m_weaponId;

		if (m_shoot - tOldShoot > m_changeTargetShootDely)
		{
			pEvent->m_isTargetChanged = 1;
		}
		else
		{
			pEvent->m_isTargetChanged = 0;
		}

		if (prevSlice.m_gameState.m_enemyList.size() == 0)
		{
			pEvent->m_crossHairX = currSlice.m_width / 2;
			pEvent->m_crossHairY = currSlice.m_height / 2;
			pEvent->m_crossHairZ = 0;
			pEvent->m_hitType = -1;
		}
		else
		{
			int m, n = 0, d = 100000, dd;
			
			for (m = 0; m < (int)prevSlice.m_gameState.m_enemyList.size(); m ++)
			{
				auto enemy = prevSlice.m_gameState.m_enemyList[m];

				dd = abs(enemy.m_enemyNormal.x1 + enemy.m_enemyNormal.x2 - currSlice.m_width)
					+ abs(enemy.m_enemyNormal.y1 + enemy.m_enemyNormal.y2 - currSlice.m_height);

				if (dd < d)
				{
					d = dd;
					n = m;
				}
			}

			pEvent->m_crossHairX = currSlice.m_width / 2;
			pEvent->m_crossHairY = currSlice.m_height / 2;
			pEvent->m_crossHairZ = prevSlice.m_gameState.m_enemyList[n].m_mapY;

			auto enemyRect = prevSlice.m_gameState.m_enemyList[n];

			pEvent->m_aryEnemyRects.push_back(enemyRect);

			if (enemyRect.m_enemyBody.x1 <= currSlice.m_width / 2 &&
				enemyRect.m_enemyBody.x2 >= currSlice.m_width / 2 &&
				enemyRect.m_enemyHead.y1 <= currSlice.m_height / 2 &&
				enemyRect.m_enemyBody.y2 >= currSlice.m_height / 2)
			{
				pEvent->m_hitType = 2;
			}
			else if (enemyRect.m_enemyLeg.x1 <= currSlice.m_width / 2 &&
				enemyRect.m_enemyLeg.x2 >= currSlice.m_width / 2 &&
				enemyRect.m_enemyLeg.y1	<= currSlice.m_height / 2 &&
				enemyRect.m_enemyLeg.y2 >= currSlice.m_height / 2)
			{
				pEvent->m_hitType = 3;
			}
			else
			{
				pEvent->m_hitType = 0;
			}
		}

		pEvent->m_hasKilledEnemy = 0;

		currSlice.m_aryEvents.push_back(pEvent);
	}
}

void CEventBuilder::BuildShootEventFromEtc(CTimelineData& currSlice, CTimelineData& prevSlice)
{
	// If kill enemy(no headshot)
	if (
		prevSlice.m_playerState.m_hitHeadShot == INVALID &&
		currSlice.m_playerState.m_hitHeadShot == 0
	)
	{
		CShootEvent* pEvent = new CShootEvent();

		pEvent->m_isFirstShot = 0;
		pEvent->m_reactionTimeMs = -1;
		pEvent->m_weaponId = prevSlice.m_playerState.m_weaponId;
		pEvent->m_isTargetChanged = 0;

		if (prevSlice.m_gameState.m_enemyList.size() == 0)
		{
			pEvent->m_targetAgent = m_killedEnemy;
			pEvent->m_crossHairX = currSlice.m_width / 2;
			pEvent->m_crossHairY = currSlice.m_height / 2;
		}
		else
		{
			int m, n = 0, d = 100000, dd;

			for (m = 0; m < (int)prevSlice.m_gameState.m_enemyList.size(); m ++)
			{
				auto enemy = prevSlice.m_gameState.m_enemyList[m];

				dd = abs(enemy.m_enemyNormal.x1 + enemy.m_enemyNormal.x2 - currSlice.m_width)
					+ abs(enemy.m_enemyNormal.y1 + enemy.m_enemyNormal.y2 - currSlice.m_height);

				if (dd < d)
				{
					d = dd;
					n = m;
				}
			}

			pEvent->m_targetAgent = m_killedEnemy;
			pEvent->m_crossHairX = currSlice.m_width / 2;
			pEvent->m_crossHairY = currSlice.m_height / 2;
			pEvent->m_crossHairZ = prevSlice.m_gameState.m_enemyList[n].m_mapY;

			auto enemyRt = prevSlice.m_gameState.m_enemyList[n];

			pEvent->m_aryEnemyRects.push_back(enemyRt);

			if (enemyRt.m_enemyLeg.x1 <= currSlice.m_width / 2 &&
				enemyRt.m_enemyLeg.x2 >= currSlice.m_width / 2 &&
				enemyRt.m_enemyLeg.y1 <= currSlice.m_height / 2 &&
				enemyRt.m_enemyLeg.y2 >= currSlice.m_height / 2)
			{
				pEvent->m_hitType = 3;
			}
			else
			{
				pEvent->m_hitType = 2;
			}

			pEvent->m_hasKilledEnemy = 1;
		}

		currSlice.m_aryEvents.push_back(pEvent);
	}
	else if (prevSlice.m_playerState.m_hitHeadShot != 1 &&
		currSlice.m_playerState.m_hitHeadShot == 1)
	{
		CShootEvent* pEvent = new CShootEvent();

		pEvent->m_isFirstShot = 0;
		pEvent->m_reactionTimeMs = -1;
		pEvent->m_weaponId = prevSlice.m_playerState.m_weaponId;
		pEvent->m_isTargetChanged = 0;

		if (prevSlice.m_gameState.m_enemyList.size() == 0)
		{
			pEvent->m_targetAgent = m_killedEnemy;
			pEvent->m_crossHairX = currSlice.m_width / 2;
			pEvent->m_crossHairY = currSlice.m_height / 2;
			pEvent->m_crossHairZ = 0;
			pEvent->m_hitType = 1;
		}
		else
		{
			int m, n = 0, d = 100000, dd;

			for (m = 0; m < (int)prevSlice.m_gameState.m_enemyList.size(); m ++)
			{
				auto enemy = prevSlice.m_gameState.m_enemyList[m];

				dd = abs(enemy.m_enemyNormal.x1 + enemy.m_enemyNormal.x2 - currSlice.m_width)
					+ abs(enemy.m_enemyNormal.y1 + enemy.m_enemyNormal.y2 - currSlice.m_height);

				if (dd < d)
				{
					d = dd;
					n = m;
				}
			}

			auto enemyRt = prevSlice.m_gameState.m_enemyList[n];

			pEvent->m_targetAgent = m_killedEnemy;
			pEvent->m_crossHairX = currSlice.m_width / 2;
			pEvent->m_crossHairY = currSlice.m_height / 2;
			pEvent->m_crossHairZ = enemyRt.m_mapY;
			pEvent->m_aryEnemyRects.push_back(enemyRt);

			pEvent->m_hitType = 1;
		}

		pEvent->m_hasKilledEnemy = 1;

		currSlice.m_aryEvents.push_back(pEvent);
	}
}
