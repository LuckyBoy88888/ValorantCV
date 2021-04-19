#include "pch.h"
#include "structs.h"
#include "EventBuilder.h"

CEventBuilder::CEventBuilder()
{
	m_startTickCount = GetTickCount();
	m_tickCount = 0;
	m_shoot = 0;
	m_enemyAppear = 0;
	m_changeTargetShootDely = 1500;
	m_killedEnemy = "";
}

CEventBuilder::~CEventBuilder()
{
}

CTimelineData CEventBuilder::Build(CTimelineData* pCurrSlice, std::vector<CTimelineData>& aryTimelineData)
{
	CTimelineData* pResult = new CTimelineData();
	
	*pResult = *pCurrSlice;

	if (aryTimelineData.empty())
	{
		aryTimelineData.push_back(*pCurrSlice);

		return *pResult;
	}

	CTimelineData prevSlice = aryTimelineData.back();

	m_tickCount = GetTickCount() - m_startTickCount;

	// ShootEvent
	BuildShootEventFromAmmo(pCurrSlice, &prevSlice);
	BuildShootEventFromEtc(pCurrSlice, &prevSlice);

	// KillEvent
	BuildKillEvent(pCurrSlice, &prevSlice);

	// DeathEvent
	BuildDeathEvent(pCurrSlice, &prevSlice);

	// AmmoReloadEvent
	BuildAmmoReloadEvent(pCurrSlice, &prevSlice);

	// EnemyAppearedEvent
	BuildEnemyAppearedEvent(pCurrSlice, &prevSlice);

	// AbilityCastEvent
	BuildAbiltyCastEvent(pCurrSlice, &prevSlice);

	return *pResult;
}

void CEventBuilder::BuildAbiltyCastEvent(CTimelineData* pCurrSlice, CTimelineData* pPrevSlice)
{
	if (pCurrSlice->m_playerState.m_health > 0 && pPrevSlice->m_playerState.m_ammo == INVALID)
	{
		if (pPrevSlice->m_playerState.m_aryAbilityLeft[0] == pCurrSlice->m_playerState.m_aryAbilityLeft[0] + 1 &&
			pPrevSlice->m_playerState.m_aryAbilityLeft[1] == pCurrSlice->m_playerState.m_aryAbilityLeft[1] &&
			pPrevSlice->m_playerState.m_aryAbilityLeft[2] == pCurrSlice->m_playerState.m_aryAbilityLeft[2] &&
			pPrevSlice->m_playerState.m_aryAbilityLeft[3] == pCurrSlice->m_playerState.m_aryAbilityLeft[3])
		{
			// used C skill
			CAbilityCastEvent* pEvent = new CAbilityCastEvent();

			pEvent->m_ability = 1;

			pCurrSlice->m_aryEvents.push_back(pEvent);
		}

		if (pPrevSlice->m_playerState.m_aryAbilityLeft[0] == pCurrSlice->m_playerState.m_aryAbilityLeft[0] &&
			pPrevSlice->m_playerState.m_aryAbilityLeft[1] == pCurrSlice->m_playerState.m_aryAbilityLeft[1] + 1 &&
			pPrevSlice->m_playerState.m_aryAbilityLeft[2] == pCurrSlice->m_playerState.m_aryAbilityLeft[2] &&
			pPrevSlice->m_playerState.m_aryAbilityLeft[3] == pCurrSlice->m_playerState.m_aryAbilityLeft[3])
		{
			// used Q skill
			CAbilityCastEvent* pEvent = new CAbilityCastEvent();

			pEvent->m_ability = 0;

			pCurrSlice->m_aryEvents.push_back(pEvent);
		}

		if (pPrevSlice->m_playerState.m_aryAbilityLeft[0] == pCurrSlice->m_playerState.m_aryAbilityLeft[0] &&
			pPrevSlice->m_playerState.m_aryAbilityLeft[1] == pCurrSlice->m_playerState.m_aryAbilityLeft[1] &&
			pPrevSlice->m_playerState.m_aryAbilityLeft[2] == pCurrSlice->m_playerState.m_aryAbilityLeft[2] + 1 &&
			pPrevSlice->m_playerState.m_aryAbilityLeft[3] == pCurrSlice->m_playerState.m_aryAbilityLeft[3])
		{
			// used E skill
			CAbilityCastEvent* pEvent = new CAbilityCastEvent();

			pEvent->m_ability = 2;

			pCurrSlice->m_aryEvents.push_back(pEvent);
		}

		if (pPrevSlice->m_playerState.m_aryAbilityLeft[0] == pCurrSlice->m_playerState.m_aryAbilityLeft[0] &&
			pPrevSlice->m_playerState.m_aryAbilityLeft[1] == pCurrSlice->m_playerState.m_aryAbilityLeft[1] &&
			pPrevSlice->m_playerState.m_aryAbilityLeft[2] == pCurrSlice->m_playerState.m_aryAbilityLeft[2] &&
			pPrevSlice->m_playerState.m_aryAbilityLeft[3] == pCurrSlice->m_playerState.m_aryAbilityLeft[3] + 1)
		{
			// used X skill
			CAbilityCastEvent* pEvent = new CAbilityCastEvent();

			pEvent->m_ability = 3;

			pCurrSlice->m_aryEvents.push_back(pEvent);
		}
	}
}

void CEventBuilder::BuildAmmoReloadEvent(CTimelineData* pCurrSlice, CTimelineData* pPrevSlice)
{
	if (pPrevSlice->m_playerState.m_ammo >= 0 && pPrevSlice->m_playerState.m_ammoLeft >= 0 &&
		pCurrSlice->m_playerState.m_ammo >= 0 && pCurrSlice->m_playerState.m_ammoLeft >= 0 &&
		pPrevSlice->m_playerState.m_ammoLeft > pCurrSlice->m_playerState.m_ammoLeft &&
		pPrevSlice->m_playerState.m_ammo + pPrevSlice->m_playerState.m_ammoLeft == pCurrSlice->m_playerState.m_ammo + pCurrSlice->m_playerState.m_ammoLeft)
	{
		CAmmoReloadEvent* pEvent = new CAmmoReloadEvent();

		pEvent->m_oldAmmo = pPrevSlice->m_playerState.m_ammo;
		pEvent->m_newAmmo = pCurrSlice->m_playerState.m_ammo;
		
		pCurrSlice->m_aryEvents.push_back(pEvent);
	}
}

void CEventBuilder::BuildKillEvent(CTimelineData* pCurrSlice, CTimelineData* pPrevSlice)
{
	int cntAlly0 = 0, cntEnemy0 = 0, cntAlly = 0, cntEnemy = 0;
	int i, j;

	cntAlly = (int)pPrevSlice->m_gameState.m_aryYourAgents.size();
	cntEnemy = (int)pPrevSlice->m_gameState.m_aryEnemyAgents.size();
	cntAlly0 = (int)pCurrSlice->m_gameState.m_aryYourAgents.size();
	cntEnemy0 = (int)pCurrSlice->m_gameState.m_aryEnemyAgents.size();

	if (cntAlly > cntAlly0 && (!(cntAlly0 == 0 && cntEnemy0 == 0)))
	{
		for (i = 0; i < (int)pPrevSlice->m_gameState.m_aryYourAgents.size(); i ++)
		{
			for (j = 0; j < (int)pCurrSlice->m_gameState.m_aryYourAgents.size(); j ++)
			{
				if (pPrevSlice->m_gameState.m_aryYourAgents[i].compare(pCurrSlice->m_gameState.m_aryYourAgents[j]) == 0)
				{
					break;
				}
			}

			if (j == pCurrSlice->m_gameState.m_aryYourAgents.size())
			{
				if (pCurrSlice->m_playerState.m_isAlive &&
					pPrevSlice->m_playerState.m_health == INVALID &&
					pPrevSlice->m_playerState.m_ammo == INVALID)
				{
					pCurrSlice->m_playerState.m_isAlive = false;
					pCurrSlice->m_playerState.m_health = 0;
					pCurrSlice->m_playerState.m_ammo = 0;

					CYouKilledEvent* pEvent = new CYouKilledEvent();

					pEvent->m_killerAgent = pPrevSlice->m_gameState.m_aryYourAgents[i];
					pCurrSlice->m_aryEvents.push_back(pEvent);
				}
				else
				{
					CKillEvent * pEvent = new CKillEvent();

					pEvent->m_killerAgent = pPrevSlice->m_gameState.m_aryYourAgents[i];
					pCurrSlice->m_aryEvents.push_back(pEvent);
				}
			}
		}
	}
}

void CEventBuilder::BuildDeathEvent(CTimelineData* pCurrSlice, CTimelineData* pPrevSlice)
{
	int cntAlly0 = 0, cntEnemy0 = 0, cntAlly = 0, cntEnemy = 0;
	int i, j;

	cntAlly = (int)pPrevSlice->m_gameState.m_aryYourAgents.size();
	cntEnemy = (int)pPrevSlice->m_gameState.m_aryEnemyAgents.size();
	cntAlly0 = (int)pCurrSlice->m_gameState.m_aryYourAgents.size();
	cntEnemy0 = (int)pCurrSlice->m_gameState.m_aryEnemyAgents.size();

	if (cntEnemy > cntEnemy0 && (!(cntAlly0 == 0 && cntEnemy0 == 0)))
	{
		for (i = 0; i < 5; i ++)
		{
			for (j = 0; j < (int)pPrevSlice->m_gameState.m_aryEnemyAgents.size(); j ++)
			{
				if (pPrevSlice->m_gameState.m_aryEnemyAgents[i].compare(pCurrSlice->m_gameState.m_aryEnemyAgents[j]) == 0)
				{
					break;
				}
			}

			if (j == pPrevSlice->m_gameState.m_aryEnemyAgents.size())
			{
				CDeathEvent* pEvent = new CDeathEvent();

				pEvent->m_killerAgent = pPrevSlice->m_gameState.m_aryEnemyAgents[i];
				m_killedEnemy = pPrevSlice->m_gameState.m_aryEnemyAgents[i];
				pCurrSlice->m_aryEvents.push_back(pEvent);
			}
		}
	}
}

void CEventBuilder::BuildEnemyAppearedEvent(CTimelineData* pCurrSlice, CTimelineData* pPrevSlice)
{
	int j, k;
	int idx = 0, fOldj[5] = { 0 }, fOldk[5] = { 0 }, d, dd;

	if (pPrevSlice->m_gameState.m_enemyList.size() < pCurrSlice->m_gameState.m_enemyList.size())
	{
		for (j = 0; j < (int)pPrevSlice->m_gameState.m_enemyList.size(); j ++)
		{
			for (k = 0; k < (int)pCurrSlice->m_gameState.m_enemyList.size(); k++)
			{
				if (fOldk[k])
				{
					continue;
				}

				if (pPrevSlice->m_gameState.m_enemyList[j].m_enemyAgent.compare(pCurrSlice->m_gameState.m_enemyList[k].m_enemyAgent) == 0)
				{
					fOldj[j] = 1;
					fOldk[k] = 1;
					break;
				}
			}

			if (k < (int)pCurrSlice->m_gameState.m_enemyList.size())
			{
				continue;
			}
		}

		for (j = 0; j < (int)pPrevSlice->m_gameState.m_enemyList.size(); j ++)
		{
			if (fOldj[j])
			{
				continue;
			}

			d = 100000;

			for (k = 0; k < (int)pCurrSlice->m_gameState.m_enemyList.size(); k++)
			{
				if (fOldk[k])
					continue;

				auto curr = pCurrSlice->m_gameState.m_enemyList[k];
				auto prev = pPrevSlice->m_gameState.m_enemyList[j];

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

		for (k = 0; k < (int)pCurrSlice->m_gameState.m_enemyList.size(); k ++)
		{
			CEnemyAppearedEvent* pEvent = new CEnemyAppearedEvent();

			pEvent->m_enemyAgent = pCurrSlice->m_gameState.m_enemyList[k].m_enemyAgent;
			pEvent->m_enemyRect = pCurrSlice->m_gameState.m_enemyList[k];
		}

		if (pPrevSlice->m_gameState.m_enemyList.size() == 0)
		{
			m_enemyAppear = m_tickCount;
		}
	}
}

void CEventBuilder::BuildShootEventFromAmmo(CTimelineData* pCurrSlice, CTimelineData* pPrevSlice)
{
	if (pPrevSlice->m_playerState.m_ammo >= 0 &&
		pCurrSlice->m_playerState.m_ammo >= 0 &&
		pPrevSlice->m_playerState.m_ammo > pCurrSlice->m_playerState.m_ammo &&
		pPrevSlice->m_playerState.m_ammoLeft == pCurrSlice->m_playerState.m_ammoLeft)
	{
		int32_t tOldShoot = m_shoot;

		m_shoot = m_tickCount;

		CShootEvent* pEvent = new CShootEvent();

		if (pPrevSlice->m_gameState.m_enemyList.size() > 0 &&
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
		
		pEvent->m_weaponId = pPrevSlice->m_playerState.m_weaponId;

		if (m_shoot - tOldShoot > m_changeTargetShootDely)
		{
			pEvent->m_isTargetChanged = 1;
		}
		else
		{
			pEvent->m_isTargetChanged = 0;
		}

		if (pPrevSlice->m_gameState.m_enemyList.size() == 0)
		{
			pEvent->m_crossHairX = pCurrSlice->m_width / 2;
			pEvent->m_crossHairY = pCurrSlice->m_height / 2;
			pEvent->m_crossHairZ = 0;
			pEvent->m_hitType = -1;
		}
		else
		{
			int m, n = 0, d = 100000, dd;
			
			for (m = 0; m < (int)pPrevSlice->m_gameState.m_enemyList.size(); m ++)
			{
				auto enemy = pPrevSlice->m_gameState.m_enemyList[m];

				dd = abs(enemy.m_enemyNormal.x1 + enemy.m_enemyNormal.x2 - pCurrSlice->m_width)
					+ abs(enemy.m_enemyNormal.y1 + enemy.m_enemyNormal.y2 - pCurrSlice->m_height);

				if (dd < d)
				{
					d = dd;
					n = m;
				}
			}

			pEvent->m_crossHairX = pCurrSlice->m_width / 2;
			pEvent->m_crossHairY = pCurrSlice->m_height / 2;
			pEvent->m_crossHairZ = pPrevSlice->m_gameState.m_enemyList[n].m_mapY;

			auto enemyRect = pPrevSlice->m_gameState.m_enemyList[n];

			pEvent->m_aryEnemyRects.push_back(enemyRect);

			if (enemyRect.m_enemyBody.x1 <= pCurrSlice->m_width / 2 &&
				enemyRect.m_enemyBody.x2 >= pCurrSlice->m_width / 2 &&
				enemyRect.m_enemyHead.y1 <= pCurrSlice->m_height / 2 &&
				enemyRect.m_enemyBody.y2 >= pCurrSlice->m_height / 2)
			{
				pEvent->m_hitType = 2;
			}
			else if (enemyRect.m_enemyLeg.x1 <= pCurrSlice->m_width / 2 &&
				enemyRect.m_enemyLeg.x2 >= pCurrSlice->m_width / 2 &&
				enemyRect.m_enemyLeg.y1	<= pCurrSlice->m_height / 2 &&
				enemyRect.m_enemyLeg.y2 >= pCurrSlice->m_height / 2)
			{
				pEvent->m_hitType = 3;
			}
			else
			{
				pEvent->m_hitType = 0;
			}
		}

		pEvent->m_hasKilledEnemy = 0;

		pCurrSlice->m_aryEvents.push_back(pEvent);
	}
}

void CEventBuilder::BuildShootEventFromEtc(CTimelineData* pCurrSlice, CTimelineData* pPrevSlice)
{
	if (pPrevSlice->m_playerState.m_hitHeadShot == INVALID &&
		pCurrSlice->m_playerState.m_hitHeadShot == 0)
	{//kill enemy(no headshot)
		CShootEvent* pEvent = new CShootEvent();

		pEvent->m_isFirstShot = 0;
		pEvent->m_reactionTimeMs = -1;
		pEvent->m_weaponId = pPrevSlice->m_playerState.m_weaponId;
		pEvent->m_isTargetChanged = 0;

		if (pPrevSlice->m_gameState.m_enemyList.size() == 0)
		{
			pEvent->m_targetAgent = m_killedEnemy;
			pEvent->m_crossHairX = pCurrSlice->m_width / 2;
			pEvent->m_crossHairY = pCurrSlice->m_height / 2;
		}
		else
		{
			int m, n = 0, d = 100000, dd;

			for (m = 0; m < (int)pPrevSlice->m_gameState.m_enemyList.size(); m ++)
			{
				auto enemy = pPrevSlice->m_gameState.m_enemyList[m];

				dd = abs(enemy.m_enemyNormal.x1 + enemy.m_enemyNormal.x2 - pCurrSlice->m_width)
					+ abs(enemy.m_enemyNormal.y1 + enemy.m_enemyNormal.y2 - pCurrSlice->m_height);

				if (dd < d)
				{
					d = dd;
					n = m;
				}
			}

			pEvent->m_targetAgent = m_killedEnemy;
			pEvent->m_crossHairX = pCurrSlice->m_width / 2;
			pEvent->m_crossHairY = pCurrSlice->m_height / 2;
			pEvent->m_crossHairZ = pPrevSlice->m_gameState.m_enemyList[n].m_mapY;

			auto enemyRt = pPrevSlice->m_gameState.m_enemyList[n];

			pEvent->m_aryEnemyRects.push_back(enemyRt);

			if (enemyRt.m_enemyLeg.x1 <= pCurrSlice->m_width / 2 &&
				enemyRt.m_enemyLeg.x2 >= pCurrSlice->m_width / 2 &&
				enemyRt.m_enemyLeg.y1 <= pCurrSlice->m_height / 2 &&
				enemyRt.m_enemyLeg.y2 >= pCurrSlice->m_height / 2)
			{
				pEvent->m_hitType = 3;
			}
			else
			{
				pEvent->m_hitType = 2;
			}

			pEvent->m_hasKilledEnemy = 1;
		}

		pCurrSlice->m_aryEvents.push_back(pEvent);
	}
	else if (pPrevSlice->m_playerState.m_hitHeadShot != 1 &&
		pCurrSlice->m_playerState.m_hitHeadShot == 1)
	{
		CShootEvent* pEvent = new CShootEvent();

		pEvent->m_isFirstShot = 0;
		pEvent->m_reactionTimeMs = -1;
		pEvent->m_weaponId = pPrevSlice->m_playerState.m_weaponId;
		pEvent->m_isTargetChanged = 0;

		if (pPrevSlice->m_gameState.m_enemyList.size() == 0)
		{
			pEvent->m_targetAgent = m_killedEnemy;
			pEvent->m_crossHairX = pCurrSlice->m_width / 2;
			pEvent->m_crossHairY = pCurrSlice->m_height / 2;
			pEvent->m_crossHairZ = 0;
			pEvent->m_hitType = 1;
		}
		else
		{
			int m, n = 0, d = 100000, dd;

			for (m = 0; m < (int)pPrevSlice->m_gameState.m_enemyList.size(); m ++)
			{
				auto enemy = pPrevSlice->m_gameState.m_enemyList[m];

				dd = abs(enemy.m_enemyNormal.x1 + enemy.m_enemyNormal.x2 - pCurrSlice->m_width)
					+ abs(enemy.m_enemyNormal.y1 + enemy.m_enemyNormal.y2 - pCurrSlice->m_height);

				if (dd < d)
				{
					d = dd;
					n = m;
				}
			}

			auto enemyRt = pPrevSlice->m_gameState.m_enemyList[n];

			pEvent->m_targetAgent = m_killedEnemy;
			pEvent->m_crossHairX = pCurrSlice->m_width / 2;
			pEvent->m_crossHairY = pCurrSlice->m_height / 2;
			pEvent->m_crossHairZ = enemyRt.m_mapY;
			pEvent->m_aryEnemyRects.push_back(enemyRt);

			pEvent->m_hitType = 1;
		}

		pEvent->m_hasKilledEnemy = 1;

		pCurrSlice->m_aryEvents.push_back(pEvent);
	}
}
