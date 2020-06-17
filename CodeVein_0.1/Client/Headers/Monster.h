#pragma once

#include "Client_Defines.h"
#include "Client_Item_Enum.h"
#include "GameObject.h"
#include "Management.h"
#include "ObjectPool_Manager.h"
#include "Trail_VFX.h"
#include "Effect.h"

#include "MonsterUI.h"
#include "Weapon.h"
#include "Haze.h"
#include "DropItem.h"

BEGIN(Client)

class CMonster abstract : public CGameObject
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
		RIGHT,
		FBLR_END
	};

	enum MONSTER_STATE_TYPE
	{
		IDLE,
		MOVE,
		ATTACK,
		HIT,
		CC,
		DEAD,
		STATE_END
	};

	enum MONSTER_COLOR_TYPE
	{
		RED,
		BLUE,
		GREEN,
		YELLOW,
		BLACK,
		WHITE,
		COLOR_NONE
	};

	enum MONSTER_IDLE_TYPE
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

	enum MONSTER_MOVE_TYPE
	{
		MOVE_WALK,
		MOVE_ALERT,
		MOVE_RUN,
		MOVE_DODGE,
		MOVE_END
	};

	enum MONSTER_ATK_TYPE
	{
		ATK_NORMAL,
		ATK_COMBO,
		ATK_SEQUNCE,
		ATK_END
	};

	enum MONSTER_HIT_TYPE
	{
		HIT_STRONG,
		HIT_NORMAL,
		HIT_WEAK,
		HIT_END
	};

	enum MONSTER_CC_TYPE
	{
		CC_STUN,
		CC_DOWN,
		CC_BLOW,
		CC_END
	};

	enum MONSTER_DEAD_TYPE
	{
		DEAD_DEAD,
		DEAD_EXCUTION,
		DEAD_FALL,
		DEAD_END
	};

	enum MONSTER_BULLET_TYPE
	{
		BULLET_NORMAL, //기본 핏덩이 색상
		BULLET_FIRE, //불이펙트
		BULLET_ICE, //얼음이펙트
		BULLET_ELECTRON //전기이펙트
	};

	struct MONSTER_STATUS
	{
		MONSTER_COLOR_TYPE	eMonsterColor = MONSTER_COLOR_TYPE::COLOR_NONE;
		WEAPON_STATE		eUseWhatWeapon = WEAPON_STATE::WEAPON_None;

		_bool				bSpawnOnTrigger = false;
		_v3					vPos = {};
		_v3					vAngle = {};
		_ushort				eStageIdx = 0;

		MONSTER_STATUS(MONSTER_COLOR_TYPE _eColor, WEAPON_STATE _eWeapon, _bool _bSpawn = false,
			_v3 vPos = V3_NULL, _v3 vAngle = V3_NULL, _ushort eStageIdx = 0)
			:eMonsterColor(_eColor), eUseWhatWeapon(_eWeapon) ,bSpawnOnTrigger(_bSpawn),
			vPos(vPos), vAngle(vAngle), eStageIdx(eStageIdx)
		{}
	};

	struct MONSTER_BULLET_STATUS
	{
		MONSTER_BULLET_STATUS(MONSTER_BULLET_TYPE _eType, _v3 _vCreatePos, _v3 _vDir, _float _fSpeed, _double _dLifeTime)
			: eBulletType(_eType), vCreatePos(_vCreatePos), vDir(_vDir), fSpeed(_fSpeed), dLifeTime(_dLifeTime)
		{}

		_v3			vCreatePos = _v3(0.f, 0.f, 0.f);
		_v3			vDir = _v3(0.f, 0.f, 0.f);
		_float		fSpeed = 0.f;
		_double		dLifeTime = 0;
		MONSTER_BULLET_TYPE	eBulletType;
	};

protected:
	explicit CMonster(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype() PURE;
	virtual HRESULT Ready_GameObject(void* pArg) PURE;
	virtual _int Update_GameObject(_double TimeDelta) PURE;
	virtual _int Late_Update_GameObject(_double TimeDelta) PURE;
	virtual HRESULT LateInit_GameObject();
	virtual HRESULT Render_GameObject();
	virtual HRESULT Render_GameObject_SetShader(CShader* pShader);
	virtual HRESULT Render_GameObject_SetPass(CShader* pShader, _int iPass, _bool _bIsForMotionBlur = false);

protected:	// DJ
	_tchar m_pLayerTag_Of_Target[STR_128] = { L"Layer_Player" };
	
	_float		m_fAggroTime = 0;	//어그로 끌린 시간
	_float		m_fMaxAggroTime = 7.f;
	_float		m_fOffsetAggroTime = 0.f;

protected:
	void Set_Target_To_Player();
	void Set_Target_To_Colleague();

	_bool Is_InFov(_float fDegreeOfFov, CTransform* pSelfTransform, _v3 vTargetPos);

	/*
	Ransdom_Aggro : true면 랜덤 어그로 핑퐁,  false면 가까운 녀석이 타겟
	*/
	void Set_Target_Auto(_bool Ransdom_Aggro = false);

	CGameObject* Get_pTargetObject();

	HRESULT Draw_Collider();

protected:
	void Check_DropItem(MONSTER_NAMETYPE eMonsterName);
	void Check_CollisionEvent();
	void Check_CollisionPush();
	void Check_CollisionHit(list<CGameObject*> plistGameObject);

	void Function_FBLR(CGameObject* _pGameObject);
	void Function_RotateBody(CGameObject* _pGameObject);
	void Function_MoveAround(CGameObject* _pGameObject, _float _fSpeed, _v3 _vDir = { V3_NULL });
	void Function_CoolDown();
	void Function_Movement(_float _fspeed, _v3 _vDir = { V3_NULL });
	void Function_DecreMoveMent(_float _fMutiply = 1.f);
	void Function_Find_Target();
	void Function_ResetAfterAtk();

	void Give_Mana_To_Player(_byte _Mana);

protected:
	virtual CGameObject* Clone_GameObject(void* pArg) PURE;
	virtual void Free();

protected:
	CGameObject*		m_pAggroTarget = nullptr;
	CMonsterUI*			m_pMonsterUI = nullptr;
	CWeapon*			m_pWeapon = nullptr;

	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CNavMesh*			m_pNavMeshCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;
	COptimization*		m_pOptimizationCom = nullptr;
	CBattleAgent*		m_pBattleAgentCom = nullptr;
	CAIController*		m_pAIControllerCom = nullptr;
	CRigidBody*			m_pRigidCom = nullptr;

protected:
	MONSTER_STATE_TYPE		m_eFirstCategory = MONSTER_STATE_TYPE::IDLE;
	MONSTER_IDLE_TYPE		m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_END;
	MONSTER_MOVE_TYPE		m_eSecondCategory_MOVE = MONSTER_MOVE_TYPE::MOVE_END;
	MONSTER_ATK_TYPE		m_eSecondCategory_ATK = MONSTER_ATK_TYPE::ATK_END;
	MONSTER_HIT_TYPE		m_eSecondCategory_HIT = MONSTER_HIT_TYPE::HIT_END;
	MONSTER_CC_TYPE			m_eSecondCategory_CC = MONSTER_CC_TYPE::CC_END;
	MONSTER_DEAD_TYPE		m_eSecondCategory_DEAD = MONSTER_DEAD_TYPE::DEAD_END;

	MONSTER_COLOR_TYPE		m_eMonsterColor = MONSTER_COLOR_TYPE::COLOR_NONE;
	MONSTER_BULLET_TYPE		m_eBulletType = MONSTER_BULLET_TYPE::BULLET_NORMAL;
	WEAPON_STATE			m_eWeaponState = WEAPON_STATE::WEAPON_None;
	FBLR					m_eFBLR = FBLR::FBLR_END;

	_double					m_dTimeDelta = 0;
	_double					m_dAniPlayMul = 1;

	_bool					m_bEventTrigger[30] = {};
	_bool					m_bCanSequencePattern = true; //차례대로 패턴 보여주기ox, 1회 진행 후 랜덤으로 단타,콤보로 넘어갑니다
	_bool					m_bCanPlayDead = false; //죽음 애니 재생
	_bool					m_bInRecognitionRange = false; //인지범위ox
	_bool					m_bInAtkRange = false; //공격범위ox
	_bool					m_bCanChase = false; //추적ox

	_bool					m_bCanCoolDown = false; //쿨타임ox
	_bool					m_bIsCoolDown = false; //쿨타임 진행중ox

	_bool					m_bCanChooseAtkType = false; //노말,콤보 가능ox
	_bool					m_bIsCombo = false; //콤보공격 진행중ox

	_bool					m_bCanIdle = true; //일상ox
	_bool					m_bIsIdle = false; //일상 진행중ox

	_bool					m_bCanMoveAround = false; //경계ox
	_bool					m_bIsMoveAround = false; //경계동작 진행중ox

	_float					m_fSkillMoveSpeed_Cur = 0.f;
	_float					m_fSkillMoveSpeed_Max = 0.f;
	_float					m_fSkillMoveAccel_Cur = 0.5f;
	_float					m_fSkillMoveAccel_Max = 0.f;
	_float					m_fSkillMoveMultiply = 1.f;

	_float					m_fRecognitionRange = 0.f;
	_float					m_fShotRange = 0.f;
	_float					m_fAtkRange = 0.f;
	_float					m_fPersonalRange = 0.f;
	_float					m_fCoolDownMax = 0.f;
	_float					m_fCoolDownCur = 0.f;

	_float					m_fDeadEffect_Delay = 0.f;
	_float					m_fDeadEffect_Offset = 0.f;

	_int					m_iRandom = 0;
	_int					m_iPatternCount = 0;
	_int					m_iPatternCountMax = 0;
	_int					m_iDodgeCount = 0;
	_int					m_iDodgeCountMax = 0;

};

END