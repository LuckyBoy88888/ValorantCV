#pragma once

#include <string>

#ifndef MAX
#define MAX(a, b)		(((a) > (b)) ? (a) : (b))
#endif // !MAX

#ifndef MIN
#define MIN(a,b)		(((a) < (b)) ? (a) : (b))
#endif // !MIN

#define SETTING_INI		_T(".\\set.ini")
#define POS_INI			(".\\pos.ini")

#define INVALID			-1
#define MATCH_ERROR		-2
#define MASK_LEN		1600
#define MAX_ABILITY		4
#define MAX_DIGITS		10
#define MAX_ENEMIES		5
#define MAX_WEAPONS		18

#define N_AGENTS		20
#define N_MAPAGENTS		28

#define SAFE_DELETE(p)	if (p){delete p;	p = nullptr;}
#define SAFE_DELETEA(p)	if (p){delete []p;	p = nullptr;}


enum EventType
{
	kEventNone = 0,

	kEventAbilityCast,
	kEventAmmoReload,
	kEventEnemyAppeared,
	kEventEnemyKill,
	kEventAllyDeath,
	kEventYouKilled,
	kEventShoot,

	kEventCount
};
