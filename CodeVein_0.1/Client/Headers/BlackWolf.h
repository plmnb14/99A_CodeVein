#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CBlackWolf final : public CGameObject
{
public:
	enum MONSTER_ANITYPE {IDLE, MOVE, ATTACK, HIT, DOWN, DEAD}; //down시 많이 넘어져 있을 수도 있으니까

	enum WOLF_ANI
	{
		Idle = 3, //일반 idle 서있기
		Sit_End = 4,
		Sit = 5,
		Eat_End = 6,
		Eat = 7,

		Alert = 15, //경계 idle
		Threat = 6, //위협애니

		Run = 0,
		Walk = 7,
		Dodge = 22,

		BiteLRL = 27,
		RDodgeAtk,
		LDodgeAtk,
		Frisbee,
		Dead_Strong=23,
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

	enum Cardinal_Point
	{
		East,
		West,
		South,
		North,
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

	void Play_Idle();//일상 애니
	void Play_Eat();
	void Play_Sit();
	void Play_Walk(); //이동 애니
	void Play_Run();
	void Play_Dodge();
	void Play_RandomAtk(); //공격 애니
	void Play_Bite_LRL();
	void Play_RDodgeAtk();
	void Play_LDodgeAtk();
	void Play_Frisbee();
	void Play_Hit(); //피격 애니
	void Play_Down_Strong();
	void Play_Down_Weak();
	void Play_Dead(); //죽음 애니
	void Play_Dead_Strong();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	HRESULT Ready_Collider();
	HRESULT Ready_BoneMatrix();

public:
	static CBlackWolf* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
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

	MONSTER_ANITYPE		m_eFirstIdentify; //대분류
	WOLF_ANI			m_eState; //애니 분류
	_bool				m_bEventTrigger[10] = {}; //이벤트 조건 조절

	_bool				m_bInRecognitionRange = false; //인지 범위 여부
	_bool				m_bInAttackRange = false; //공격 범위 여부
	_bool				m_bCanChase = false; //추격 여부
	_bool				m_bIsCoolDown = false; //쿨타임 진행중 여부
	_bool				m_bIsDodge = false; //회피 진행중 여부

	_float				m_fRecognitionRange = 10.f; //인지 범위
	_float				m_fAttackRange = 4.f; //공격 범위

	_float				m_fCoolDown = 0.f; //쿨타임

	_int				m_iAttackRandomNumber = 0; //공격애니 랜덤화
	_int				m_iIdleRandomNumber = 0;//일상 애니 랜덤화
	_int				m_iDodgeCount = 0; //n회 피격시 바로 회피

	//_float			m_fHitDmgLimit = 50.f; //일정 데미지 퍼센트를 맞을 경우 강하게 쓰러짐, 역치
	//_int				m_iHitCountLimit = 5; //5대 맞으면 약하게 쓰러짐, 역치
	//_int				m_iHitCount = 0;  //맞을때마다 증가하기
	//Cardinal_Point	m_eTargetCardinal; //방위

};

END