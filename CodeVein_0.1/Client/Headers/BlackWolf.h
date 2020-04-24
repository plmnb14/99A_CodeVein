#pragma once

#include "Info_Monster.h"

BEGIN(Client)

class CBlackWolf final : public CGameObject
{
public:
	enum FBLR { FRONT, FRONTLEFT, FRONTRIGHT, BACK, BACKLEFT, BACKRIGHT, LEFT, RIGHT };
	enum MONSTER_ANITYPE { IDLE, MOVE, ATTACK, HIT, CC, DEAD };

	enum WOLF_IDLETYPE { IDLE_IDLE, IDLE_EAT, IDLE_SIT };
	enum WOLF_MOVETYPE { MOVE_RUN, MOVE_WALK, MOVE_DODGE };
	enum WOLF_ATKTYPE { ATK_NORMAL, ATK_COMBO };
	enum WOLF_HITTYPE { HIT_STRONG, HIT_NORMAL };
	enum WOLF_CCTYPE { CC_DOWN_P, CC_DOWN_S };
	enum WOLF_DEADTYPE { DEAD_DEAD, DEAD_DEAD_S };

	enum WOLF_ANI
	{
		Alert, //사용하지 않음
		Walk,
		Run,
		Idle,
		Sit_End,
		Sit,
		Eat_End,
		Eat,

		Threat, //IDLE에 포함되서 진행함
		
		Down_Weak_Start,
		Down_Weak_Loop,
		Down_weak_End,

		Down_Strong_Start,
		Down_Strong_Loop,
		Down_Strong_End,
		
		Dodge,
		
		Dead_Strong,
		Dead,

		Dmg_F,
		Dmg_B,

		BiteLRL,
		RDodgeAtk,
		LDodgeAtk,
		Frisbee
	};
	
	enum BONE_TYPE { Bone_Range, Bone_Body, Bone_Head, Bone_End };

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
	explicit CBlackWolf(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CBlackWolf(const CBlackWolf& rhs);
	virtual ~CBlackWolf() = default;

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

	void Play_Bite_LRL();
	void Play_RDodgeAtk();
	void Play_LDodgeAtk();
	void Play_Frisbee();

	void Play_Idle();
	void Play_Move();
	void Play_Hit();
	void Play_CC();
	void Play_Dead();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	HRESULT Ready_Status(void* pArg);
	HRESULT Ready_Collider();
	HRESULT Ready_BoneMatrix();

public:
	static CBlackWolf* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
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

	CTransform*			m_pTargetTransform = nullptr;

	_v3					m_vBirthPos;
	_mat*				m_matBone[Bone_End];
	_double				m_dTimeDelta;
	_double				m_dAniPlayMul = 1;

	_float				m_fSkillMoveSpeed_Cur = 0.f;
	_float				m_fSkillMoveSpeed_Max = 0.f;
	_float				m_fSkillMoveAccel_Cur = 0.5f;
	_float				m_fSkillMoveAccel_Max = 0.f;
	_float				m_fSkillMoveMultiply = 1.f;

	MONSTER_ANITYPE		m_eFirstCategory;
	WOLF_IDLETYPE		m_eSecondCategory_IDLE;
	WOLF_MOVETYPE		m_eSecondCategory_MOVE;
	WOLF_ATKTYPE		m_eSecondCategory_ATK;
	WOLF_HITTYPE		m_eSecondCategory_HIT;
	WOLF_CCTYPE			m_eSecondCategory_CC;
	WOLF_DEADTYPE		m_eSecondCategory_DEAD;

	WOLF_ANI			m_eState;
	FBLR				m_eFBLR;

	_bool				m_bEventTrigger[10] = {};
	_bool				m_bCanPlayDead = false;
	_bool				m_bInRecognitionRange = false;
	_bool				m_bInAtkRange = false;
	_bool				m_bCanChase = false;
	_bool				m_bCanRandomAtk = true;
	_bool				m_bCanCoolDown = false;
	_bool				m_bIsCoolDown = false;
	_bool				m_bCanIdle = true;
	_bool				m_bIsIdle = false;

	_float				m_fRecognitionRange = 15.f;
	_float				m_fAtkRange = 4.f;
	_float				m_fCoolDownMax = 0.f;
	_float				m_fCoolDownCur = 0.f;
	_float				m_fSpeedForCollisionPush = 2.f;

	_int				m_iRandom = 0; //랜덤 받을 숫자

	_int				m_iDodgeCountMax = 3; //3회 피격시 회피
	_int				m_iDodgeCount = 0; //n회 피격시 회피

};

END