#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CBlackWolf final : public CGameObject
{
public:
	enum MONSTER_ANITYPE {IDLE, MOVE, ATTACK, HIT, DOWN, DEAD}; //down시 많이 넘어져 있을 수도 있으니까
	
	enum WOLF_IDLETYPE { IDLE_IDLE, IDLE_EAT, IDLE_SIT };
	enum WOLF_MOVETYPE { MOVE_RUN, MOVE_WALK, MOVE_DODGE };
	enum WOLF_ATKTYPE { ATK_NORMAL, ATK_COMBO };
	enum WOLF_HITTYPE { HIT_HIT_F, HIT_HIT_B };
	enum WOLF_DOWNTYPE { DOWN_DOWN, DOWN_DOWN_W, DOWN_DOWN_S };
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

	enum FBLR { FRONT, BACK, LEFT, RIGHT };

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
	void Enter_Collision();
	void Check_CollisionPush();
	void Check_CollisionEvent(list<CGameObject*> plistGameObject);

	void Check_Hit();
	void Check_FBLR();
	void Check_Dist();
	void Set_AniEvent();

	void Function_RotateBody();
	void Function_CoolDown();
	void Function_Movement(_float _fspeed, _v3 _vDir = { V3_NULL });
	void Function_DecreMoveMent(_float _fMutiply = 1.f);
	void Function_ResetAfterAtk();

	void Play_Idle();
	void Play_Eat();
	void Play_Sit();

	void Play_Walk();
	void Play_Run();
	void Play_Dodge();

	void Play_Bite_LRL();
	void Play_RDodgeAtk();
	void Play_LDodgeAtk();
	void Play_Frisbee();

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
	static CBlackWolf* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CNavMesh*			m_pNavMesh = nullptr;
	CCollider*			m_pCollider = nullptr;

	CTransform*			m_pTargetTransform = nullptr;

	_v3					m_vBirthPos;
	_mat*				m_matBone[Bone_End];
	_double				m_dAniPlayMul = 1;

	_float				m_fSkillMoveSpeed_Cur = 0.f;
	_float				m_fSkillMoveSpeed_Max = 0.f;
	_float				m_fSkillMoveAccel_Cur = 0.5f;
	_float				m_fSkillMoveAccel_Max = 0.f;
	_float				m_fSkillMoveMultiply = 1.f;

	MONSTER_ANITYPE		m_eFirstCategory; //대분류
	WOLF_IDLETYPE		m_eSecondCategory_IDLE; //중분류
	WOLF_MOVETYPE		m_eSecondCategory_MOVE;
	WOLF_ATKTYPE		m_eSecondCategory_ATK;
	WOLF_HITTYPE		m_eSecondCategory_HIT;
	WOLF_DOWNTYPE		m_eSecondCategory_DOWN;
	WOLF_DEADTYPE		m_eSecondCategory_DEAD;

	WOLF_ANI			m_eState; //애니 분류
	_bool				m_bEventTrigger[10] = {}; //이벤트 조건 조절

	_bool				m_bInRecognitionRange = false; //인지 범위 여부
	_bool				m_bInAtkRange = false; //공격 범위 여부
	
	_bool				m_bCanChase = false; //추격 여부
	
	_bool				m_bIsDodge = false; //회피 진행중 여부
	
	_bool				m_bCanCoolDown = false; //쿨타임 여부
	_bool				m_bIsCoolDown = false; //쿨타임 진행중 여부

	_bool				m_bCanIdleRandom = true;
	_bool				m_bCanAtkCategoryRandom = true; //미사용
	_bool				m_bIsAtkCombo = false; //미사용
	_bool				m_bCanAtkRandom = true;

	_float				m_fRecognitionRange = 10.f;
	_float				m_fAtkRange = 4.f;
	_float				m_fCoolDownMax = 0.f;
	_float				m_fCoolDownCur = 0.f;
	_float				m_fSpeedForCollisionPush = 2.f;

	_int				m_iRandom = 0; //랜덤 받을 숫자
	_int				m_iDodgeCount = 0; //n회 피격시 회피

};

END