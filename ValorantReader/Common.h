#pragma once

#include <map>
#include <string>
#include <stdint.h>

#include <Windows.h>

#ifndef MAX
#define MAX(a, b)		(((a) > (b)) ? (a) : (b))
#endif // !MAX

#ifndef MIN
#define MIN(a,b)		(((a) < (b)) ? (a) : (b))
#endif // !MIN

#define SETTING_INI		("./resources/set.ini")
#define POS_INI			("./resources/pos.ini")
#define RESOURCE_PATH	"./resources/"

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

const std::map<int8_t, std::string> kAgentNameMap = {
	{0,		"Brimstone"},
	{2,		"Pheonix"},
	{3,		"Sage"},
	{4,		"Sova"},
	{12,	"Viper"},
	{6,		"Cypher"},
	{10,	"Reyna"},
	{7,		"Killjoy"},
	{5,		"Breach"},
	{8,		"Omen"},
	{1,		"Jett"},
	{9,		"Raze"},
	{11,	"Skye"},
	{13,	"Yoru"},
	{14,	"Astra"},
};

const std::string kUnknownAgent = "Unknown";