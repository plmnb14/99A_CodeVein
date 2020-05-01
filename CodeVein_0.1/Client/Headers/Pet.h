#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"
#include "MonsterUI.h"
#include "Weapon.h"
#include "Haze.h"
#include "ObjectPool_Manager.h"
#include "Trail_VFX.h"
#include "Effect.h"

BEGIN(Client)

class CPet abstract : public CGameObject
{
public:
	enum FBLR
	{
		FRONT,
		FRONTLEFT,
		FRONTRIGHT,
		BACK,
		BACKLEFT,
		BACKRIGHT,
		LEFT,
		RIGHT
	};

	enum PET_STATETYPE
	{
		IDLE,
		MOVE,
		ATTACK,
		HIT,
		CC,
		DEAD
	};

	enum PET_COLORTYPE
	{
		RED,
		BLUE,
		YELLOW,
		BLACK,
		WHITE,
		COLOR_NONE
	};

	enum PET_IDLETYPE
	{
		IDLE_IDLE,
		IDLE_CROUCH,
		IDLE_EAT,
		IDLE_LURK,
		IDLE_SCRATCH,
		IDLE_SIT,
		IDLE_STAND
	};

	enum PET_MOVETYPE
	{
		MOVE_WALK,
		MOVE_ALERT,
		MOVE_RUN,
		MOVE_DODGE
	};

	enum PET_ATKTYPE
	{
		ATK_NORMAL,
		ATK_COMBO
	};

	enum PET_HITTYPE
	{
		HIT_STRONG,
		HIT_NORMAL,
		HIT_WEAK
	};

	enum PET_CCTYPE
	{
		CC_STUN,
		CC_DOWN,
		CC_BLOW
	};

	enum PET_DEADTYPE
	{
		DEAD_DEAD,
		DEAD_EXCUTION
	};

	enum PET_GRADETYPE
	{
		PET_GRADE_NORMAL,
		PET_GRADE_RARE,
		PET_GRADE_UNIQUE,
		PET_GRADE_LEGEND,
		PET_GRADE_TYPE_END
	};

	enum PET_TYPE
	{
		PET_AIR,
		PET_LAND,
		PET_INSTALL,
		PET_TYPE_END
	};

	enum PET_TARGET_TYPE
	{
		PET_TARGET_BOSS,
		PET_TARGET_MONSTER,
		PET_TARGET_ITEM,
		PET_TARGET_NONE,
		PET_TARGET_TYPE_END
	};

	enum PET_BULLET_TYPE
	{
		PET_BULLET_POISON,
		PET_BULLET_ICE,
		PET_BULLET_FIRE,
		PET_BULLET_NONE
	};

	struct PET_STATUS
	{
		PET_GRADETYPE	eUseWhatGrade = PET_GRADETYPE::PET_GRADE_NORMAL;
		WEAPON_STATE		eUseWhatWeapon = WEAPON_STATE::WEAPON_None;

		PET_STATUS(PET_GRADETYPE _eGrade, WEAPON_STATE _eWeapon)
		{
			eUseWhatGrade = _eGrade;
			eUseWhatWeapon = _eWeapon;
		}
	};

	struct PET_BULLET_STATUS
	{
		PET_BULLET_STATUS(PET_BULLET_TYPE _eType, _v3 _vCreatePos, _v3 _vDir, _float _fSpeed, _double _dLifeTime)
			: eType(_eType), vCreatePos(_vCreatePos), vDir(_vDir), fSpeed(_fSpeed), dLifeTime(_dLifeTime)
		{}

		PET_BULLET_TYPE	eType = PET_BULLET_NONE;
		_v3			vCreatePos = _v3(0.f, 0.f, 0.f);
		_v3			vDir = _v3(0.f, 0.f, 0.f);
		_float		fSpeed = 0.f;
		_double		dLifeTime = 0;
	};

protected:
	explicit CPet(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CPet(const CPet& rhs);
	virtual ~CPet() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype() PURE;
	virtual HRESULT Ready_GameObject(void* pArg) PURE;
	virtual _int Update_GameObject(_double TimeDelta) PURE;
	virtual _int Late_Update_GameObject(_double TimeDelta) PURE;
	virtual HRESULT Render_GameObject() PURE;
	virtual HRESULT Render_GameObject_SetPass(CShader* pShader, _int iPass);

protected:
	virtual void Update_Collider();
	virtual void Render_Collider();
	virtual void Check_CollisionEvent();
	virtual void Check_CollisionPush();
	virtual void Check_CollisionHit(list<CGameObject*> plistGameObject);

	virtual void Function_FBLR(CGameObject* _pGameObject);
	virtual void Function_RotateBody(CGameObject* _pGameObject);
	virtual void Function_MoveAround(CGameObject* _pGameObject, _float _fSpeed, _v3 _vDir = { V3_NULL });
	virtual void Function_CoolDown();
	virtual void Function_Movement(_float _fspeed, _v3 _vDir = { V3_NULL });
	virtual void Function_DecreMoveMent(_float _fMutiply = 1.f);
	virtual void Function_CalcMoveSpeed(_float _fMidDist);
	virtual void Function_Find_Target();
	virtual void Function_ResetAfterAtk();

	virtual void Play_Idle();
	virtual void Play_Move();
	virtual void Play_Hit();
	virtual void Play_CC();
	virtual void Play_Dead();

protected:
	virtual HRESULT Add_Component(void* pArg);
	virtual HRESULT SetUp_ConstantTable();
	virtual HRESULT Ready_Status(void* pArg);
	virtual HRESULT Ready_Weapon(void* pArg);
	virtual HRESULT Ready_Collider(void* pArg);
	virtual HRESULT Ready_BoneMatrix(void* pArg);

protected:
	virtual CGameObject* Clone_GameObject(void* pArg) PURE;
	virtual void Free();

protected:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CNavMesh*			m_pNavMesh = nullptr;
	CCollider*			m_pCollider = nullptr;
	CMonsterUI*			m_pMonsterUI = nullptr;
	CWeapon*			m_pWeapon = nullptr;

	CGameObject*		m_pPlayer = nullptr;
	CGameObject*		m_pTarget = nullptr;

	PET_STATETYPE	m_eFirstCategory;

	PET_IDLETYPE	m_eSecondCategory_IDLE;
	PET_MOVETYPE	m_eSecondCategory_MOVE;
	PET_ATKTYPE		m_eSecondCategory_ATK;
	PET_HITTYPE		m_eSecondCategory_HIT;
	PET_CCTYPE		m_eSecondCategory_CC;
	PET_DEADTYPE	m_eSecondCategory_DEAD;

	WEAPON_STATE		m_eWeaponState = WEAPON_STATE::WEAPON_None;
	FBLR				m_eFBLR;
	PET_TARGET_TYPE		m_eTarget = PET_TARGET_TYPE::PET_TARGET_TYPE_END;

	_double				m_dTimeDelta = 0;
	_double				m_dAniPlayMul = 1;

	_float				m_fSkillMoveSpeed_Cur;
	_float				m_fSkillMoveSpeed_Max;
	_float				m_fSkillMoveAccel_Cur;
	_float				m_fSkillMoveAccel_Max;
	_float				m_fSkillMoveMultiply;

	//1.플레이어와의 거리
	//최대 거리
	//행동선택가능 거리
	//개인적인 거리
	_float				m_fLimitRange = 40.f; //최대 제한 거리, player에게로 달려간다
	_float				m_fActiveRange = 30.f; //Player에게 달려가는 도중 액티브한 선택을 할수 있다
	_bool				m_bCanActive = false; //임시로 넣은 루프 방지변수
	_bool				m_bInLimitRange = false; //최대 제한 거리
	_bool				m_bInActiveRange = false; //자유 행동 거리

	//2.목표와의 거리,펫 자체의 인지,공격,원거리,개인거리
	_float				m_fRecognitionRange = 20.f; //목표와의 거리가 인지 범위에 있다
	_float				m_fShotRange = 10.f; //목표와의 거리가 원거리 범위 안에 있다
	_float				m_fAtkRange = 5.f; //목표와의 거리가 근접공격 범위 안에 있다

	//player 또는 목표와의 거리가 너무 가까운 것을 체크
	_float				m_fPersonalRange = 2.f;

	_float				m_fCoolDownMax = 0.f;
	_float				m_fCoolDownCur = 0.f;
	// For Effect
	_float				m_fDeadEffect_Delay = 0.f;
	_float				m_fDeadEffect_Offset = 0.f;

	_int				m_iDodgeCount = 0;
	_int				m_iDodgeCountMax = 0;

	_bool				m_bEventTrigger[30] = {};
	_bool				m_bCanPlayDead = false;
	_bool				m_bInRecognitionRange = false;
	_bool				m_bInShotRange = false;
	_bool				m_bInAtkRange = false;
	_bool				m_bCanChase = false;
	_bool				m_bCanCoolDown = false;
	_bool				m_bIsCoolDown = false;
	_bool				m_bCanChooseAtkType = true;
	_bool				m_bIsCombo = false;
	_bool				m_bCanIdle =true;
	_bool				m_bIsIdle =false;
	_bool				m_bCanMoveAround = true;
	_bool				m_bIsMoveAround = false;

	_int				m_iRandom = 0;
};

END