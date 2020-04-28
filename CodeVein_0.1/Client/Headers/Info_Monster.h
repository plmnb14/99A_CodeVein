#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"
#include "MonsterUI.h"
#include "Weapon.h"
#include "Haze.h"

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

enum PET_TYPE
{
	PET_AIR,
	PET_LAND,
	PET_INSTALL,
	PET_TYPE_END
};

enum TARGET_TYPE
{
	TARGET_BOSS,
	TARGET_MONSTER,
	TARGET_ITEM,
	TARGET_NONE,
	TARGET_TYPE_END
};

enum MONSTER_GRADETYPE
{
	GRADE_NORMAL,
	GRADE_RARE,
	GRADE_UNIQUE,
	GRADE_LEGEND,
	GRADE_TYPE_END
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
	MOVE_ALERT,
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
