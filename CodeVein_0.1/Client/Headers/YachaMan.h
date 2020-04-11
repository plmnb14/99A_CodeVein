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
	enum MONSTER_ANITYPE { IDLE, MOVE, ATTACK, HIT, DOWN, DEAD };
	
	enum YACHAMAN_IDLETYPE { IDLE_IDLE, IDLE_EAT, IDLE_LURK };
	enum YACHAMAN_MOVETYPE {MOVE_RUN, MOVE_WALK, MOVE_DODGE};
	enum YACHAMAN_ATKTYPE {ATK_NORMAL, ATK_COMBO};
	enum YACHAMAN_HITTYPE { HIT_HIT, HIT_HIT_F, HIT_HIT_B };
	enum YACHAMAN_DOWNTYPE { DOWN_DOWN, DOWN_DOWN_S, DOWN_DOWN_W };
	enum YACHAMAN_DEADTYPE { DEAD_DEAD, DEAD_DEAD_S };

	enum ATK_NORMAL_TYPE { NORMAL_RIGHT, NORMAL_LEFT, NORMAL_HAMMERING, NORMAL_SHOULDER, NORMAL_TURNTWICE, NORMAL_HALFCLOCK, NORMAL_TARGETHAMMERING, NORMAL_WHEELWIND };
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

		Stun=29,
		Stun_End,
		Down_S_Start,
		Down_S_End,
		Down_P_Start,
		Down_P_Loop, //사용x
		Down_P_End,
		Fall_Front,
		Fall_Back,
		Parried,	
		Hit_B_WeakFly,
		Hit_F_WeakFly,
		Groggy,
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

	enum BONE_TYPE { Bone_Range, Bone_Body, Bone_LeftArm, Bone_End };

	enum FBLR { FRONT, BACK, LEFT, RIGHT };

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

	//피격,회피,인식,범위,공격가능성,엇박자
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
	void Play_Lurk();
	void Play_Eat();

	void Play_Walk();
	void Play_Run();
	void Play_Dodge();

	void Play_RandomAtkNormal();
	void Play_R(); //1.3~1.9 이동 1.8~2.0 타격
	void Play_L(); //0.6~1.1 이동 1.2~1.4타격
	void Play_Hammering(); //0.9~1.8 이동 1.4~1.8 타격
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
	HRESULT Ready_Weapon();
	HRESULT Ready_Collider();
	HRESULT Ready_BoneMatrix();

public:
	static CYachaMan* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

private:
	//////////// 채유미
	// 몬스터 HP바 UI
	CMonsterUI*			m_pMonsterUI = nullptr;

	_mat*				m_matHeadBone_for_YM;

private:
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
	_double				m_dTimeDelta;
	_double				m_dAniPlayMul = 1;

	_float				m_fSkillMoveSpeed_Cur = 0.f;
	_float				m_fSkillMoveSpeed_Max = 0.f;
	_float				m_fSkillMoveAccel_Cur = 0.5f;
	_float				m_fSkillMoveAccel_Max = 0.f;
	_float				m_fSkillMoveMultiply = 1.f;

	MONSTER_ANITYPE		m_eFirstCategory; //대분류
	YACHAMAN_IDLETYPE	m_eSecondCategory_IDLE; //중분류
	YACHAMAN_MOVETYPE	m_eSecondCategory_MOVE;
	YACHAMAN_ATKTYPE	m_eSecondCategory_ATK;
	YACHAMAN_HITTYPE	m_eSecondCategory_HIT;
	YACHAMAN_DOWNTYPE	m_eSecondCategory_DOWN;
	YACHAMAN_DEADTYPE	m_eSecondCategory_DEAD;

	ATK_COMBO_TYPE		m_eAtkCombo;
	YACHAMAN_ANI		m_eState;
	_bool				m_bEventTrigger[20] = {};

	_bool				m_bCanPlayDeadAni = false;
	_bool				m_bIsPlayDeadAni = false;

	_bool				m_bInRecognitionRange = false;
	_bool				m_bInAtkRange = false;
	
	_bool				m_bCanChase = false;
	
	_bool				m_bIsDodge = false;

	_bool				m_bCanCoolDown = false;
	_bool				m_bIsCoolDown = false;

	_bool				m_bCanAtkCategoryRandom = true;
	_bool				m_bIsAtkCombo = false;
	_bool				m_bCanIdleRandom = true;

	_float				m_fRecognitionRange = 10.f;
	_float				m_fAtkRange = 4.f;
	_float				m_fCoolDownMax = 0.f;
	_float				m_fCoolDownCur = 0.f;
	_float				m_fSpeedForCollisionPush = 2.f;

	_int				m_iRandom = 0;
	_int				m_iDodgeCount = 0;

};

END