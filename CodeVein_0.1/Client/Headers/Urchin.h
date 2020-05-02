#pragma once

#include "Monster.h"

BEGIN(Client)

class CUrchin final : public CMonster
{
public:
	enum URCHIN_ANI 
	{
		Idle,
		Run,
		Dmg,
		Death,
		Atk_Step,
		Atk_Rush
	};

	enum BONE_TYPE 
	{ 
		Bone_Range,
		Bone_Body,
		Bone_Head,
		Bone_End
	};

protected:
	explicit CUrchin(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CUrchin(const CUrchin& rhs);
	virtual ~CUrchin() = default;

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
	void Check_DeadEffect(_double TimeDelta);

	void Play_Rolling();
	void Play_RollingRush();

	void Play_Idle();
	void Play_Hit();
	void Play_Dead();

private:
	HRESULT Add_Component(void * pArg);
	HRESULT SetUp_ConstantTable();
	HRESULT Ready_Status(void* pArg);
	HRESULT Ready_Collider();
	HRESULT Ready_BoneMatrix();
public:
	static CUrchin* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
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
	URCHIN_ANI				m_eState;

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
	
private: // For Effect
	_float			m_fDeadEffect_Delay = 0.f;
	_float			m_fDeadEffect_Offset = 0.f;

};

END