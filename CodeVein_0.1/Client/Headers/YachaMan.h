#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CWeapon;
class CMonsterUI;

class CYachaMan final : public CGameObject
{
public:
	enum FBLR { FRONT, FRONTLEFT, FRONTRIGHT, BACK, BACKLEFT, BACKRIGHT, LEFT, RIGHT };
	enum MONSTER_ANITYPE { IDLE, MOVE, ATTACK, HIT, CC, DEAD };

	enum YACHAMAN_IDLETYPE { IDLE_IDLE, IDLE_EAT, IDLE_LURK };
	enum YACHAMAN_MOVETYPE {MOVE_RUN, MOVE_WALK, MOVE_DODGE};
	enum YACHAMAN_ATKTYPE {ATK_NORMAL, ATK_COMBO};
	enum YACHAMAN_HITTYPE { HIT_STRONG, HIT_NORMAL, HIT_WEAK };
	enum YACHAMAN_CCTYPE { CC_STUN, CC_DOWN };
	enum YACHAMAN_DEADTYPE { DEAD_DEAD, DEAD_DEAD_S };

	enum ATK_NORMAL_TYPE 
	{
		NORMAL_RIGHT, NORMAL_LEFT, 
		NORMAL_HAMMERING, NORMAL_SHOULDER, 
		NORMAL_TURNTWICE, NORMAL_HALFCLOCK, 
		NORMAL_TARGETHAMMERING, NORMAL_WHEELWIND 
	};
	enum ATK_COMBO_TYPE { COMBO_R_L, COMBO_R_HAMMERING, COMBO_SHOULDER_TURNTWICE, COMBO_SHOULDER_HALFCLOCK, COMBO_RUNHAMMERING };

	enum YACHAMAN_ANI
	{
		Hammer_Idle,
		Atk_Ani_R,
		Atk_Ani_L,
		Atk_Ani_Hammering,
		Atk_Ani_Shoulder,
		Atk_Ani_TurnTwice,
		Atk_Ani_HalfClock,
		Atk_Ani_TargetHammering,
		Atk_Ani_WheelWind,
		Atk_Ani_Run_Start,
		Atk_Ani_Run_Loop,
		Atk_Ani_Run_End,

		Death_B,
		Death_F,
		Death,

		Run = 64,
		Dodge,
		Walk,

		Lurk = 70,
		Lurk_End,
		Glance,
		LookAround,
		Eat,
		Eat_End,

		Stun=29,
		Stun_End,
		Down_S_Start,
		Down_S_End,
		Down_P_Start,
		Down_P_Loop, //»ç¿ëx
		Down_P_End,
		Fall_Front,
		Fall_Back,
		Parried,	
		Hit_B_WeakFly,
		Hit_F_WeakFly,
		Groggy, //->Stun

		Hit_S_BR,
		Hit_S_BL,
		Hit_S_FR,
		Hit_S_FL,

		Hit_N_BR,
		Hit_N_BL,
		Hit_N_FR,
		Hit_N_FL,

		Hit_W_BR,
		Hit_W_BL,
		Hit_W_FR,
		Hit_W_FL,
	};

	enum BONE_TYPE { Bone_Range, Bone_Body, Bone_LeftArm, Bone_Head, Bone_End };

public:
	struct INITSTRUCT
	{
		INITSTRUCT(
			_float _fDMG,
			_float _fHpMax,
			_float _fArmorMax,
			_float _fKnowRange,
			_float _fAtkRange,
			_int _iDodgeMax)
		{
			tMonterStatus.fDamage = _fDMG;
			tMonterStatus.fHp_Max = _fHpMax;
			tMonterStatus.fArmor_Max = _fArmorMax;

			fKonwingRange = _fKnowRange;
			fCanAttackRange = _fAtkRange;
			iDodgeCountMax = _iDodgeMax;
		}

		OBJECT_PARAM		tMonterStatus;
		_float				fKonwingRange = 20.f;
		_float				fCanAttackRange = 5.f;
		_int				iDodgeCountMax = 3;
	};
protected:
	explicit CYachaMan(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CYachaMan(const CYachaMan& rhs);
	virtual ~CYachaMan() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Render_GameObject_SetPass(CShader* pShader, _int iPass);

private:
	void Update_Collider();
	void Render_Collider();
	void Enter_Collision();
	void Check_CollisionPush();
	void Check_CollisionEvent(list<CGameObject*> plistGameObject);

	void Function_RotateBody();
	void Function_CoolDown();
	void Function_Movement(_float _fspeed, _v3 _vDir = { V3_NULL });
	void Function_DecreMoveMent(_float _fMutiply = 1.f);
	void Function_ResetAfterAtk();

	void Check_PosY();
	void Check_Hit();
	void Check_FBLR();
	void Check_Dist();
	void Check_AniEvent();

	void Play_RandomAtkNormal();
	void Play_RandomAtkCombo();

	void Play_R();
	void Play_L();
	void Play_Hammering();
	void Play_Shoulder();
	void Play_TurnTwice();
	void Play_HalfClock();
	void Play_TargetHammering();
	void Play_WheelWind();
	void Play_Combo_R_L();
	void Play_Combo_R_Hammering();
	void Play_Combo_Shoulder_TurnTwice();
	void Play_Combo_Shoulder_HalfClock();
	void Play_Combo_RunHammering();

	void Play_Idle();
	void Play_Move();
	void Play_Hit();
	void Play_CC();
	void Play_Dead();

private:
	void Check_DeadEffect(_double TimeDelta);

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	HRESULT Ready_Status(void* pArg);
	HRESULT Ready_Weapon();
	HRESULT Ready_Collider();
	HRESULT Ready_BoneMatrix();

public:
	static CYachaMan* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

private:
	CMonsterUI*			m_pMonsterUI = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CNavMesh*			m_pNavMesh = nullptr;
	CCollider*			m_pCollider = nullptr;
	CWeapon*			m_pWeapon = nullptr;

	CTransform*			m_pTargetTransform = nullptr;

	_v3					m_vBirthPos;
	_mat*				m_matBone[Bone_End];
	_double				m_dTimeDelta = 0;
	_double				m_dAniPlayMul = 1;

	_float				m_fSkillMoveSpeed_Cur = 0.f;
	_float				m_fSkillMoveSpeed_Max = 0.f;
	_float				m_fSkillMoveAccel_Cur = 0.5f;
	_float				m_fSkillMoveAccel_Max = 0.f;
	_float				m_fSkillMoveMultiply = 1.f;

	MONSTER_ANITYPE		m_eFirstCategory;
	YACHAMAN_IDLETYPE	m_eSecondCategory_IDLE;
	YACHAMAN_MOVETYPE	m_eSecondCategory_MOVE;
	YACHAMAN_ATKTYPE	m_eSecondCategory_ATK;
	YACHAMAN_HITTYPE	m_eSecondCategory_HIT;
	YACHAMAN_CCTYPE		m_eSecondCategory_CC;
	YACHAMAN_DEADTYPE	m_eSecondCategory_DEAD;

	ATK_COMBO_TYPE		m_eAtkCombo;
	YACHAMAN_ANI		m_eState;
	FBLR				m_eFBLR;

	_bool				m_bEventTrigger[20] = {};

	_bool				m_bCanPlayDead = false;
	_bool				m_bCanDissolve = false;

	_bool				m_bInRecognitionRange = false;
	_bool				m_bInAtkRange = false;
	
	_bool				m_bCanChase = false;

	_bool				m_bCanCoolDown = false;
	_bool				m_bIsCoolDown = false;

	_bool				m_bAtkCategory = true;
	_bool				m_bCanInterrupt = true;
	_bool				m_bCanCombo = true;
	_bool				m_bIsCombo = false;

	_bool				m_bCanIdle = true;
	_bool				m_bIsIdle = false;

	_float				m_fRecognitionRange = 15.f;
	_float				m_fAtkRange = 4.f;
	_float				m_fCoolDownMax = 0.f;
	_float				m_fCoolDownCur = 0.f;
	_float				m_fSpeedForCollisionPush = 2.f;

	_int				m_iRandom = 0;
	_int				m_iDodgeCountMax = 3;
	_int				m_iDodgeCount = 0;

private: // For Effect
	_float			m_fDeadEffect_Delay = 0.f;
	_float			m_fDeadEffect_Offset = 0.f;

};

END