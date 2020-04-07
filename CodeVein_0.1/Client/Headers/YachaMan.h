#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CYachaMan final : public CGameObject
{
public:
	enum MONSTER_ANITYPE { IDLE, MOVE, ATTACK, HIT, DOWN, DEAD };
	enum YACHAMAN_IDLETYPE { IDLE_IDLE, IDLE_EAT, IDLE_LURK };
	enum YACHAMAN_MOVETYPE {MOVE_RUN, MOVE_WALK, MOVE_DODGE};
	enum YACHAMAN_ATKTYPE {ATK_NORMAL, ATK_COMBO};
	enum YACHAMAN_HITTYPE { HIT_HIT, HIT_HIT_F, HIT_HIT_B };
	enum YACHAMAN_DOWNTYPE { DOWN_DOWN, DOWN_DOWN_W,DOWN_DOWN_P };
	enum YACHAMAN_DEADTYPE { DEAD_DEAD, DEAD_DEAD_P };

	enum ATK_NORMAL_TYPE { NORMAL_RIGHT, NORMAL_LEFT, NORMAL_HAMMERING, NORMAL_SHOULDER, NORMAL_TURNTWICE, NORMAL_HALFCLOCK, NORMAL_TARGETHAMMERING, NORMAL_WHEELWIND };
	enum ATK_COMBO_TYPE { COMBO_R_L, COMBO_R_HAMMERING, COMBO_SHOULDE_L, COMBO_SHOULDER_TURNTWICE, COMBO_RUNHAMMERING };

	enum YACHAMAN_ANI
	{
		Hammer_Idle = 0,
		Atk_Ani_R,
		Atk_Ani_L,
		Atk_Ani_Hammering,
		Atk_Ani_Shoulder,
		Atk_Ani_HalfClock,
		Atk_Ani_TurnTwice,
		Atk_Ani_TargetHammering,
		Atk_Ani_WheelWind,
		Atk_Ani_Run_Start,
		Atk_Ani_Run_Loop,
		Atk_Ani_Run_End,

		Dead_B,
		Dead_F,
		Dead,

		Run = 64,
		Dodge,
		Walk,

		Lurk = 70,
		Lurk_End,
		Glance,
		LookAround,
		Eat,
		Eat_End,
		//Down_HitF_Start = 31,
		//Down_HitF_End,
		//Down_HitB_Start,
		//Down_HitB_Loop,
		//Down_HitB_End,
		//DownFly_Weak_Start = 36,
		//DownFly_Weak_End == 32번 애니로
		//DownFly_Strong_Start=37,
		//DownFly_Strong_End,
		//DownFly_Weak_End,
		Groggy = 41
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

	//피격,회피,인식,범위,공격가능성,엇박자
	void Check_Hit();
	void Check_Dist();
	void Set_AniEvent();

	void Skill_RotateBody();
	void Skill_CoolDown();
	void Skill_Movement(_float _fspeed, _v3 _vDir = { V3_NULL });
	void Decre_Skill_Movement(_float _fMutiply = 1.f);
	void Reset_BattleState();

	void Play_Idle();
	void Play_Lurk();
	void Play_Glance();
	void Play_LookAround();
	void Play_Eat();

	void Play_Walk();
	void Play_Run();
	void Play_Dodge();

	void Play_RandomAtkNormal();
	void Play_R(); //1.3~1.9 이동 1.8~2.0 타격
	void Play_L(); //0.6~1.1 이동 1.2~1.4타격
	void play_Hammering(); //0.9~1.8 이동 1.4~1.8 타격
	void Play_Shoulder(); //0.9~1.3 이동 0.9~1.3 타격
	void Play_TurnTwice(); //0.8~2.0 2.9~3.3 이동 0.9~1.2 1.6~1.9 타격
	void Play_HalfClock(); //0.8~2.0회전이동 2.9~3.3 짧게복귀 0.8~1.2타격
	void Play_TargetHammering(); //1.9~3.0 회전이동 3.9~5.6짧게복귀 1.9~2.7 타격
	void Play_WheelWind();
	//1.4~1.8 이동 1.4~1.8 타격
	//	1.9~2.2 이동 2.1~2.4 타격
	//	2.3~2.6 이동 2.7~2.8 타격
	//	2.7~2.9 이동 3.1~3.3 타격
	//	3.0~3.3 이동
	void Play_RandomAtkCombo();
	void Play_Combo_R_L(); //1,2 0.45 0.95
	void Play_Combo_R_Hammering(); //1,3 0.5 0.95
	void Play_Combo_Shoulder_TurnTwice(); //4,5 0.28 0.95
	void Play_Combo_Shoulder_HalfClock(); //4,6 0.28 0.95
	void Play_Combo_RunHammering(); //9,10,11 달려가서 내리치기 0.9 0.9 0.9

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

	MONSTER_ANITYPE		m_eFirstCategory; //대분류
	YACHAMAN_IDLETYPE	m_eSecondCategory_Idle; //중분류
	YACHAMAN_MOVETYPE	m_eSecondCategory_Move;
	YACHAMAN_ATKTYPE	m_eSecondCategory_Atk;
	YACHAMAN_HITTYPE	m_eSecondCategory_Hit;
	YACHAMAN_DOWNTYPE	m_eSecondCategory_Down;
	YACHAMAN_DEADTYPE	m_eSecondCategory_Dead;

	ATK_COMBO_TYPE		m_eAtkCombo;
	YACHAMAN_ANI		m_eState;
	_bool				m_bEventTrigger[20] = {};

	_bool				m_bInRecognitionRange = false;
	_bool				m_bInAttackRange = false; 
	_bool				m_bCanChase = false;
	//_bool				m_bIsDodge = false;
	_bool				m_bIsCoolDown = false;

	_bool				m_bCanRandomAtkCategory = true;
	_bool				m_bIsAtkCombo = false;
	_bool				m_bCanRandomIdle = true;

	_float				m_fRecognitionRange = 10.f;
	_float				m_fAttackRange = 4.f;
	_float				m_fCoolDown = 0.f;

	_int				m_iAtkRandomType = 0;
	_int				m_iAtkRandom = 0;
	_int				m_iIdleRandomNumber = 0;
	_int				m_iDodgeCount = 0;

};

END
