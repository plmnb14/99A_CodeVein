#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"
#include "MonsterUI.h"
#include "Weapon.h"
#include "Haze.h"
#include "ObjectPool_Manager.h"

enum FBLR 
{ 
	FRONT,
	FRONTLEFT,
	FRONTRIGHT,
	BACK,
	BACKLEFT,
	BACKRIGHT,
	LEFT,
	RIGHT
};

enum MONSTER_STATETYPE
{
	IDLE,
	MOVE,
	ATTACK,
	HIT,
	CC,
	DEAD 
};

enum MONSTER_COLORTYPE
{
	RED,
	BLUE,
	YELLOW,
	BLACK,
	WHITE,
	COLOR_NONE
};

enum MONSTER_IDLETYPE
{
	IDLE_IDLE,
	IDLE_CROUCH,
	IDLE_EAT,
	IDLE_LURK,
	IDLE_SCRATCH,
	IDLE_SIT,
	IDLE_STAND
};

enum MONSTER_MOVETYPE
{
	MOVE_WALK,
	MOVE_RUN,
	MOVE_DODGE
};

enum MONSTER_ATKTYPE
{
	ATK_NORMAL,
	ATK_COMBO
};

enum MONSTER_HITTYPE
{
	HIT_STRONG,
	HIT_NORMAL,
	HIT_WEAK
};

enum MONSTER_CCTYPE
{
	CC_STUN,
	CC_DOWN,
	CC_BLOW
};

enum MONSTER_DEADTYPE
{
	DEAD_DEAD, 
	DEAD_EXCUTION
};

struct MONSTER_STATUS
{
	MONSTER_COLORTYPE	eMonsterColor = MONSTER_COLORTYPE::COLOR_NONE;
	WEAPON_STATE		eUseWhatWeapon = WEAPON_STATE::WEAPON_None;

	MONSTER_STATUS(MONSTER_COLORTYPE _eColor, WEAPON_STATE _eWeapon)
	{
		eMonsterColor = _eColor;
		eUseWhatWeapon = _eWeapon;
	}
};
