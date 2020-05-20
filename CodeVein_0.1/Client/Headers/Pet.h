#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"
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

	enum PET_STATE_TYPE
	{
		IDLE,
		MOVE,
		ATTACK,
		HIT,
		CC,
		DEAD
	};

	enum PET_COLOR_TYPE
	{
		RED,
		BLUE,
		YELLOW,
		BLACK,
		WHITE,
		COLOR_NONE
	};

	enum PET_IDLE_TYPE
	{
		IDLE_IDLE,
		IDLE_CROUCH,
		IDLE_EAT,
		IDLE_LURK,
		IDLE_SCRATCH,
		IDLE_SIT,
		IDLE_STAND,
		IDLE_END
	};

	enum PET_MOVE_TYPE
	{
		MOVE_WALK,
		MOVE_ALERT,
		MOVE_RUN,
		MOVE_DODGE,
		MOVE_END
	};

	enum PET_ATK_TYPE
	{
		ATK_NORMAL,
		ATK_COMBO,
		ATK_END
	};

	enum PET_HIT_TYPE
	{
		HIT_STRONG,
		HIT_NORMAL,
		HIT_WEAK,
		HIT_END
	};

	enum PET_CC_TYPE
	{
		CC_STUN,
		CC_DOWN,
		CC_BLOW,
		CC_END
	};

	enum PET_DEAD_TYPE
	{
		DEAD_DEAD,
		DEAD_EXCUTION,
		DEAD_END
	};

	enum PET_TYPE
	{
		PET_DEERKING,
		PET_POISONBUTTERFLY,
		PET_TYPE_END
	};

	enum PET_MODE_TYPE
	{
		PET_MODE_ATK, //공격대상->아이템
		PET_MODE_UTILL, //아이템->공격대상
		PET_MODE_END
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
		PET_BULLET_ICICLEBLADE,
		PET_BULLET_ICICLEBEAM,
		PET_BULLET_FIRE,
		PET_BULLET_NONE
	};

	struct PET_STATUS
	{
		PET_STATUS(PET_TYPE _eGrade)
			:ePetType(_eGrade)
		{
		}

		PET_TYPE		ePetType = PET_TYPE::PET_TYPE_END;
		_ushort			sStageIdx = 0;
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
	virtual HRESULT Render_GameObject_Instancing_SetPass(CShader* pShader);
	virtual HRESULT Render_GameObject_SetPass(CShader* pShader, _int iPass);

	virtual void Check_CollisionEvent();
	virtual void Check_CollisionHit(list<CGameObject*> plistGameObject);

	virtual void Play_Idle();
	virtual void Play_Move();
	virtual void Play_Hit();
	virtual void Play_CC();
	virtual void Play_Dead();

protected:
	virtual HRESULT Add_Component(void* pArg);
	virtual HRESULT SetUp_ConstantTable(CShader* pShader);
	virtual HRESULT Ready_Status(void* pArg);
	virtual HRESULT Ready_Weapon(void* pArg);
	virtual HRESULT Ready_Collider(void* pArg);
	virtual HRESULT Ready_BoneMatrix(void* pArg);

public:
	virtual void Play_Deformation() PURE;
	virtual void Set_Summon(_bool _Summon) { m_bCanSummon = _Summon; }
	virtual PET_TYPE Get_PetType() { return m_eType; }
	virtual void Set_PetType(PET_TYPE _eType) { m_eType = _eType; }

	PET_MODE_TYPE Get_Pet_Mode() { return m_eNowPetMode; }
	void Set_Pet_Mode(PET_MODE_TYPE _eMode) { m_eNowPetMode = _eMode; }

	virtual void Function_FBLR(CGameObject* _pGameObject);
	virtual void Function_RotateBody(CGameObject* _pGameObject);
	virtual void Function_MoveAround(CGameObject* _pGameObject, _float _fSpeed, _v3 _vDir = { V3_NULL });
	virtual void Function_CoolDown();
	virtual void Function_Movement(_float _fspeed, _v3 _vDir = { V3_NULL });
	virtual void Function_DecreMoveMent(_float _fMutiply = 1.f);
	virtual void Function_CalcMoveSpeed(_float _fMidDist);
	virtual void Function_Find_Target();
	virtual void Function_ResetAfterAtk();
	virtual void Function_Change_Mode();
	virtual void Function_Check_Navi();
	virtual void Function_Teleport_Near_Player();

protected:
	virtual CGameObject* Clone_GameObject(void* pArg) PURE;
	virtual void Free();

protected:
	CTransform*			m_pTransform = nullptr;
	CRenderer*			m_pRenderer = nullptr;
	CShader*			m_pShader = nullptr;
	CMesh_Dynamic*		m_pMesh = nullptr;
	CNavMesh*			m_pNavMesh = nullptr;
	CCollider*			m_pCollider = nullptr;
	COptimization*		m_pOptimization = nullptr;
	CBattleAgent*		m_pBattleAgent = nullptr;

	CGameObject*		m_pPlayer = nullptr;

	PET_STATE_TYPE		m_eFirstCategory = PET_STATE_TYPE::IDLE;
	PET_IDLE_TYPE		m_eSecondCategory_IDLE = PET_IDLE_TYPE::IDLE_END;
	PET_MOVE_TYPE		m_eSecondCategory_MOVE = PET_MOVE_TYPE::MOVE_END;
	PET_ATK_TYPE		m_eSecondCategory_ATK = PET_ATK_TYPE::ATK_END;
	PET_HIT_TYPE		m_eSecondCategory_HIT = PET_HIT_TYPE::HIT_END;
	PET_CC_TYPE			m_eSecondCategory_CC = PET_CC_TYPE::CC_END;
	PET_DEAD_TYPE		m_eSecondCategory_DEAD = PET_DEAD_TYPE::DEAD_END;

	PET_TARGET_TYPE		m_eTarget = PET_TARGET_TYPE::PET_TARGET_TYPE_END;
	PET_MODE_TYPE		m_eNowPetMode = PET_MODE_TYPE::PET_MODE_END;
	PET_MODE_TYPE		m_eOldPetMdoe = PET_MODE_TYPE::PET_MODE_END;
	FBLR				m_eFBLR= FBLR::FRONT;

	PET_TYPE			m_eType = PET_TYPE::PET_TYPE_END;

	_double				m_dTimeDelta = 0;
	_double				m_dAniPlayMul = 1;

	_float				m_fSkillMoveSpeed_Cur = 0.f;
	_float				m_fSkillMoveSpeed_Max = 0.f;
	_float				m_fSkillMoveAccel_Cur = 0.f;
	_float				m_fSkillMoveAccel_Max = 0.f;
	_float				m_fSkillMoveMultiply = 0.f;
	_float				m_fPersonalRange = 4.f; //player, 목표와의 사회적 거리두기 변수

	_bool				m_bEventTrigger[30] = {};
	_bool				m_bCanSummon = false; //소환 여부
	_bool				m_bIsSummon = false; //소환중 여부

	//플레이어와 연관된 변수
	_bool				m_bAbsoluteOrder = false; //우선 순위 결정
	_float				m_fLimitRange = 40.f; //최대 제한 거리, 해당 범위 밖인 경우 무조건 복귀
	_float				m_fActiveRange = 30.f; //행동 가능 거리, 행동 가능
	_bool				m_bInLimitRange = false; //최대 제한 거리
	_bool				m_bInActiveRange = false; //자유 행동 거리

	//펫 자체 이용 변수
	_float				m_fRecognitionRange = 20.f; //펫 인지 범위
	_float				m_fShotRange = 10.f; //펫 원거리 범위
	_float				m_fAtkRange = 5.f; //펫 근거리 범위
	_float				m_fCoolDownCur = 0.f; //쿨타임 누적
	_float				m_fCoolDownMax = 0.f; //해당 카운트를 기준으로 쿨타임해제
	_float				m_fDeadEffect_Delay = 0.f;
	_float				m_fDeadEffect_Offset = 0.f;

	_bool				m_bCanPlayDead = false; //죽을 경우 디졸브,이펙트 작동하기 위한 변수
	_bool				m_bInRecognitionRange = false; //인지 가능 여부
	_bool				m_bInShotRange = false; //원거리 가능 여부
	_bool				m_bInAtkRange = false; //근거리 가능 여부
	_bool				m_bCanChase = false; //추적 가능 여부

	_bool				m_bCanCoolDown = false; //행동이 종료된 후 대부분 사용되는 쿨타임 가능 여부
	_bool				m_bIsCoolDown = false; //쿨타임 진행중 여부

	_bool				m_bCanIdle =true; //기본 동작 진행여부
	_bool				m_bIsIdle =false; //기본 동작 진행중 여부

	_bool				m_bCanMoveAround = true; //경계 가능 여부
	_bool				m_bIsMoveAround = false; //경계 진행중 여부

	_bool				m_bCanChooseAtkType = true; //일반,콤보 공격 여부
	_bool				m_bIsCombo = false; //콤보 진행 여부

	_int				m_iRandom = 0; //랜덤 변수 사용할 일이 거의 없음, 폐기 예정?

};

END