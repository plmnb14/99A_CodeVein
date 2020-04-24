#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"
#include "MonsterUI.h"
#include "Weapon.h"

enum FBLR { FRONT, FRONTLEFT, FRONTRIGHT, BACK, BACKLEFT, BACKRIGHT, LEFT, RIGHT };

enum MONSTER_ANITYPE { IDLE, MOVE, ATTACK, HIT, CC, DEAD };

enum MONSTER_WEAPONTYPE { GUN, HALBERD, HAMMER, LSWORD, SWORD, WEAPON_NONE };

enum MONSTER_COLORTYPE {RED, BLUE, YELLOW, BLACK, WHITE, COLOR_NONE};

struct MONSTER_STATUS
{
	MONSTER_STATUS(
		_float _fDMG,
		_float _fHpMax,
		_float _fArmorMax,
		_float _fKnowRange,
		_float _fShotRange,
		_float _fAtkRange,
		_float _fPesonalRange,
		_int _iDodgeMax = 5,
		MONSTER_COLORTYPE _eColor = MONSTER_COLORTYPE::COLOR_NONE,
		MONSTER_WEAPONTYPE _eWeapon = MONSTER_WEAPONTYPE::WEAPON_NONE)
	{
		tMonterStatus.fDamage = _fDMG;
		tMonterStatus.fHp_Max = _fHpMax;
		tMonterStatus.fArmor_Max = _fArmorMax;

		eMonsterColor = _eColor;
		eUseWhatWeapon = _eWeapon;

		fCanKonwRange = _fKnowRange;
		fCanShotRange = _fShotRange;
		fCanAtkRange = _fAtkRange;
		fPersonalRange = _fPesonalRange;
		iDodgeCountMax = _iDodgeMax;
	}

	OBJECT_PARAM		tMonterStatus;
	MONSTER_COLORTYPE	eMonsterColor;
	MONSTER_WEAPONTYPE	eUseWhatWeapon;
	_float				fCanKonwRange;
	_float				fCanShotRange;
	_float				fCanAtkRange;
	_float				fPersonalRange;
	_int				iDodgeCountMax;

};
