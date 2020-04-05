#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CYachaMan final : public CGameObject
{
public:
	enum MONSTER_ANITYPE { IDLE, MOVE, ATTACK, HIT, DOWN, DEAD };

	enum YACHAMAN_ANI
	{
		Idle = 13, //일반 idle 서있기
		Eat = 10,
		Eat_End = 11,
		Sit = 12,
		Sit_End = 8,

		Alert = 15, //경계 idle

		Run = 0,
		Walk = 7,
		Dodge = 22,

		BiteLRL = 27,
		RDodgeAtk,
		LDodgeAtk,
		Frisbee,
		Dead_Strong = 23,
		Dead,
		Dmg_F,
		Dmg_B,

		Down_Weak_Start = 16,
		Down_Weak_Loop,
		Down_weak_End,

		Down_Strong_Start = 19,
		Down_Strong_Loop,
		Down_Strong_End
	};

	enum BONE_TYPE
	{
		Bone_Range,
		Bone_Body,
		Bone_Head,
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

private:
	void Update_Collider();
	void Render_Collider();
	void Enter_CollisionEvent();
	void Check_CollisionEvent(list<CGameObject*> plistGameObject);

	void Check_Hit();
	void Check_Dist();
	void Set_AniEvent();

	void Skill_RotateBody();
	void Skill_CoolDown();
	void Skill_Movement(_float _fspeed, _v3 _vDir = { V3_NULL });
	void Decre_Skill_Movement(_float _fMutiply = 1.f);
	void Reset_BattleState();


	void Play_Idle();
	void Play_Walk();
	void Play_Run();

	void Play_RandomAtk();

	void Play_Hit();
	void Play_Down_Strong();
	void Play_Down_Weak();

	void Play_Dead();
	void Play_Dead_Strong();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	HRESULT Ready_Collider();
	HRESULT Ready_BoneMatrix();

public:
	static CYachaMan* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CTransform*			m_pTargetTransform = nullptr;

	_v3					m_vBirthPos;
	_mat*				m_matBone[Bone_End];
	_double				m_dAniPlayMul = 1;

	_float				m_fSkillMoveSpeed_Cur = 0.f;
	_float				m_fSkillMoveSpeed_Max = 0.f;
	_float				m_fSkillMoveAccel_Cur = 0.5f;
	_float				m_fSkillMoveAccel_Max = 0.f;
	_float				m_fSkillMoveMultiply = 1.f;

	MONSTER_ANITYPE		m_eFirstIdentify;
	YACHAMAN_ANI		m_eState;
	_bool				m_bEventTrigger[10] = {};

	_bool				m_bInRecognitionRange = false;
	_bool				m_bInAttackRange = false; 
	_bool				m_bCanChase = false;
	_bool				m_bIsDodge = false;
	_bool				m_bIsCoolDown = false;

	_float				m_fRecognitionRange = 10.f;
	_float				m_fAttackRange = 4.f;
	_float				m_fCoolDown = 0.f;

	_int				m_iAttackRandomNumber = 0;
	_int				m_iIdleRandomNumber = 0;
	_int				m_iDodgeCount = 0;

};

END
