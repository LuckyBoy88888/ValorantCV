#pragma once

#include <string>
#include <vector>

#include "Common.h"

////////////////////////////////////////////////////////////////////////////////
// Common Structures
////////////////////////////////////////////////////////////////////////////////
template<typename T>
struct rectangle
{
    rectangle() 
    {
        x1 = 0;
        y1 = 0;
        x2 = 0;
        y2 = 0;
    }

	T width(void) const
	{
		return x2 - x1 > 0 ? x2 - x1 : x1 - x2;
	}

	T height(void) const
	{
		return y2 - y1 > 0 ? y2 - y1 : y1 - y2;
	}

	T x1, y1;
	T x2, y2;
};

typedef rectangle<unsigned short> recti16;

typedef struct EnemyRect
{
    EnemyRect() : m_mapX(0), m_mapY(0) {}

	int16_t         m_mapX;
	int16_t         m_mapY;
	std::string     m_enemyAgent;
	recti16         m_enemyNormal;
	recti16         m_enemyHead;
	recti16         m_enemyBody;
	recti16         m_enemyLeg;
} EnemyRect;

typedef struct WhitePattern {
	int             width;
	int             height;
	int8_t          mask[MASK_LEN]; //0: no mask, 1: mask, valid length is width*height
} WhitePattern;

typedef struct ColorPattern {
	int             width;
	int             height;
	uint8_t         R[MASK_LEN];
    uint8_t         G[MASK_LEN];
    uint8_t         B[MASK_LEN];
} ColorPattern;

////////////////////////////////////////////////////////////////////////////////
// Event Structures
////////////////////////////////////////////////////////////////////////////////
struct IEvent
{
    IEvent(uint8_t eventType = kEventNone) : m_eventType(eventType) {}

	uint8_t         m_eventType;
 	// uint64_t     m_timestamp;
};

struct CAbilityCastEvent : public IEvent
{
    CAbilityCastEvent() : m_ability(-1), IEvent(kEventAbilityCast) {}

	int8_t          m_ability;	// 0 - Q, 1 - C, 2 - E, 3 - X
};

struct CAmmoReloadEvent : public IEvent
{
    CAmmoReloadEvent() : m_oldAmmo(-1), m_newAmmo(-1), IEvent(kEventAmmoReload) {}

	int16_t         m_oldAmmo;
	int16_t         m_newAmmo;
};

struct CDeathEvent : public IEvent
{
    CDeathEvent() : IEvent(kEventAllyDeath) {}
	std::string     m_killerAgent;
};

struct CEnemyAppearedEvent : public IEvent
{
    CEnemyAppearedEvent() : IEvent(kEventEnemyAppeared) {}

	std::string     m_enemyAgent;
	EnemyRect       m_enemyRect;
};

struct CKillEvent : public IEvent
{
    CKillEvent() : IEvent(kEventEnemyKill) {}

    std::string     m_killerAgent;
    std::string     m_targetAgent;
};

struct CShootEvent : public IEvent
{
    CShootEvent() : m_isFirstShot(-1), 
                    m_reactionTimeMs(-1), 
                    m_weaponId(-1), 
                    m_isTargetChanged(-1), 
                    m_crossHairX(-1), 
                    m_crossHairY(-1), 
                    m_crossHairZ(-1), 
                    m_hitType(0), 
                    m_hasKilledEnemy(false),
                    IEvent(kEventShoot)
                    {}

    int8_t          m_isFirstShot;
    int32_t         m_reactionTimeMs;
    int8_t          m_weaponId;
    int8_t          m_isTargetChanged;
    std::string     m_targetAgent;
    int16_t         m_crossHairX;
    int16_t         m_crossHairY;
    int16_t         m_crossHairZ;
    int8_t          m_hitType;
    int8_t          m_hasKilledEnemy;

    std::vector<EnemyRect> m_aryEnemyRects;
};

struct CYouKilledEvent : public IEvent
{
    CYouKilledEvent() : IEvent(kEventYouKilled) {}

	std::string     m_killerAgent;
};


