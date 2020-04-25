#pragma once

#include "Info_Monster.h"

BEGIN(Client)

class CYachaMan final : public CGameObject
{
public:
	enum ATK_NORMAL_TYPE 
	{
		NORMAL_RIGHT,
		NORMAL_SHOULDER,  
		NORMAL_TARGETHAMMERING,
		NORMAL_WHEELWIND 
	};

	enum ATK_COMBO_TYPE
	{ 
		COMBO_R_L,
		COMBO_R_HAMMERING,
		COMBO_SHOULDER_TURNTWICE,
		COMBO_SHOULDER_HALFCLOCK,
		COMBO_RUNHAMMERING
	};

	enum YACHAMAN_ANI
	{
		Hammer_Idle,
		NF_Eat,
		NF_Eat_End,
		NF_Lurk,
		NF_Lurk_End,
		NF_Sit,
		NF_Sit_End,

		Run,
		Walk_R,
		Walk_L,
		Walk_F,
		Walk_B,
		Dodge,

		Dmg_W_BR,
		Dmg_W_BL,
		Dmg_W_FR,
		Dmg_W_FL,

		Dmg_N_BR,
		Dmg_N_BL,
		Dmg_N_FR,
		Dmg_N_FL,

		Down_S_End,
		Down_S_Start,

		Down_P_End,
		Down_P_Loop,
		Down_P_Start,

		DmgBlow_F,
		DmgBlow_B,

		DmgStrike_S,
		DmgStrike_P,

		Stun_End,
		Stun_Loop,
		Stun_Start,

		Death_F,
		Death_B,
		Death,

		Hammer_Sp03_End,
		Hammer_Sp03_Loop,
		Hammer_Sp03_Start,
		Hammer_Sp02,
		Hammer_Sp01,
		Hammer_S03,
		Hammer_S02,
		Hammer_S01,
		Hammer_N03,
		Hammer_N02,
		Hammer_N01
	};

	enum BONE_TYPE
	{ 
		Bone_Range,
		Bone_Head,
		Bone_Body,
		Bone_LeftArm,
		Bone_End
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

	void Function_FBLR();
	void Function_RotateBody();
	void Function_CoolDown();
	void Function_Movement(_float _fspeed, _v3 _vDir = { V3_NULL });
	void Function_DecreMoveMent(_float _fMutiply = 1.f);
	void Function_ResetAfterAtk();

	void Check_PosY();
	void Check_Hit();
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
	HRESULT Add_Component(void* pArg);
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

	_mat*					m_matBone[Bone_End];
	MONSTER_STATETYPE		m_eFirstCategory;
	MONSTER_IDLETYPE		m_eSecondCategory_IDLE;
	MONSTER_MOVETYPE		m_eSecondCategory_MOVE;
	MONSTER_ATKTYPE			m_eSecondCategory_ATK;
	MONSTER_HITTYPE			m_eSecondCategory_HIT;
	MONSTER_CCTYPE			m_eSecondCategory_CC;
	MONSTER_DEADTYPE		m_eSecondCategory_DEAD;

	WEAPON_STATE			m_eWeaponState;
	FBLR					m_eFBLR;
	ATK_COMBO_TYPE			m_eAtkCombo;
	YACHAMAN_ANI			m_eState;

	_bool	m_bEventTrigger[20] = {};
	_bool	m_bCanPlayDead;
	_bool	m_bInRecognitionRange;
	_bool	m_bInAtkRange;
	_bool	m_bCanChase;
	_bool	m_bCanCoolDown;
	_bool	m_bIsCoolDown;
	_bool	m_bCanChooseAtkType;
	_bool	m_bIsCombo;
	_bool	m_bCanIdle;
	_bool	m_bIsIdle;
	_bool	m_bCanMoveAround;
	_bool	m_bIsMoveAround;

	_double	m_dTimeDelta;
	_double	m_dAniPlayMul;

	_float	m_fSkillMoveSpeed_Cur;
	_float	m_fSkillMoveSpeed_Max;
	_float	m_fSkillMoveAccel_Cur;
	_float	m_fSkillMoveAccel_Max;
	_float	m_fSkillMoveMultiply;

	_float	m_fRecognitionRange;
	_float	m_fShotRange;
	_float	m_fAtkRange;
	_float	m_fPersonalRange;
	_float	m_fCoolDownMax;
	_float	m_fCoolDownCur;

	_int	m_iRandom;
	_int	m_iDodgeCount;
	_int	m_iDodgeCountMax;

};

END