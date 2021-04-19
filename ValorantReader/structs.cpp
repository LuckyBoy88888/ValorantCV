#include "Logs.h"
#include "structs.h"
#include "GameState.h"
#include "PlayerState.h"

std::string IEvent::GetEventName(uint8_t eventType)
{
    switch (eventType)
    {
        case kEventNone:
        {
            return "UnknownEvent";
        }

        case kEventAbilityCast:
        {
            return "AbilityCast";
        }

        case kEventAmmoReload:
        {
            return "AmmoReload";
        }

        case kEventAllyDeath:
        {
            return "AllyDeath";
        }

        case kEventEnemyKill:
        {
            return "EnemyKill";
        }

        case kEventYouKilled:
        {
            return "YouKilled";
        }
        case kEventShoot:
        {
            return "Shoot";
        }
        default:
        {
            return "UnknownEvent";
        }
    }

    return "UnknownEvent";
}

std::string IEvent::GetAgentNameFromNo(int number)
{
    auto search = kAgentNameMap.find(number);
    if (search == kAgentNameMap.end())
    {
        return kUnknownAgent;
    }

    return search->second;
}

void CAbilityCastEvent::Export()
{
    const char kSkillName[] = "QCEX";
    GLogA(
        "{eventType: '%s', ability: '%s'}", 
        IEvent::GetEventName(m_eventType), 
        kSkillName[m_ability]
    );
}

void CAmmoReloadEvent::Export()
{
    GLogA(
        "{eventType: '%s', oldAmmo: %d, newAmmo: %d}", 
        IEvent::GetEventName(m_eventType), 
        m_oldAmmo,
        m_newAmmo
    );
}

void CDeathEvent::Export()
{
    GLogA(
        "{eventType: '%s', agent: '%s'}", 
        IEvent::GetEventName(m_eventType), 
        m_killerAgent
    );
}

void CEnemyAppearedEvent::Export()
{
    GLogA(
        "{eventType: '%s', agent: '%s', Rect:(%d,%d,%d,%d)}", 
        IEvent::GetEventName(m_eventType), 
        m_enemyAgent,
        m_enemyRect.m_enemyNormal.x1,
        m_enemyRect.m_enemyNormal.y1,
        m_enemyRect.m_enemyNormal.x2,
        m_enemyRect.m_enemyNormal.y2
    );
}

void CKillEvent::Export()
{
    GLogA(
        "{eventType: '%s', agent: '%s'}", 
        IEvent::GetEventName(m_eventType), 
        m_killerAgent
    );
}

void CShootEvent::Export()
{
    GLogA(
        "{eventType: '%s', agent: '%s', firstShoot: %b, reactionTime: %d, weapon: %d, crosshair: (%d,%d,%d), hitType:'%d', killedEnemy: %b}", 
        IEvent::GetEventName(m_eventType), 
        m_targetAgent,
        (bool)m_isFirstShot,
        m_reactionTimeMs,
        m_weaponId,
        m_crossHairX,
        m_crossHairY,
        m_crossHairZ,
        m_hitType,
        (bool)m_hasKilledEnemy
    );
}

void CYouKilledEvent::Export()
{
    GLogA(
        "{eventType: '%s', agent: '%s'}", 
        IEvent::GetEventName(m_eventType), 
        m_killerAgent
    );
}

void EnemyRect::Export()
{
    GLogA(
        "{agent: '%s', Rect:(%d,%d,%d,%d)}", 
        m_enemyAgent,
        m_enemyNormal.x1,
        m_enemyNormal.y1,
        m_enemyNormal.x2,
        m_enemyNormal.y2
    );
}
void CGameState::Export()
{
    GLogA(
        "{round: %d, yourScore: %d, enemyScore: %d, aliveAllies: %d, aliveEnemies: %d}", 
        m_round, 
        m_yourTeamScore, 
        m_enemyTeamScore,
        m_aryYourAgents.size(),
        m_aryEnemyAgents.size()
    );
}

void CPlayerState::Export()
{
    GLogA(
        "{health: %d, armor: %d, ammo: %d, ammoLeft: %d, credits: %d, weapon: %d, abilities:(%d,%d,%d,%d)}", 
        m_health, 
        m_armor, 
        m_ammo,
        m_ammoLeft,
        m_credits,
        m_weaponId,
        m_aryAbilityLeft[0],
        m_aryAbilityLeft[1],
        m_aryAbilityLeft[2],
        m_aryAbilityLeft[3]
    );
}