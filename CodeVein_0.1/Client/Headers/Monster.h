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
		RIGHT
	};

	enum MONSTER_STATE_TYPE
	{
		IDLE,
		MOVE,
		ATTACK,
		HIT,
		CC,
		DEAD
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
		IDLE_STAND
	};

	enum MONSTER_MOVE_TYPE
	{
		MOVE_WALK,
		MOVE_ALERT,
		MOVE_RUN,
		MOVE_DODGE
	};

	enum MONSTER_ATK_TYPE
	{
		ATK_NORMAL,
		ATK_COMBO
	};

	enum MONSTER_HIT_TYPE
	{
		HIT_STRONG,
		HIT_NORMAL,
		HIT_WEAK
	};

	enum MONSTER_CC_TYPE
	{
		CC_STUN,
		CC_DOWN,
		CC_BLOW
	};

	enum MONSTER_DEAD_TYPE
	{
		DEAD_DEAD,
		DEAD_EXCUTION
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

		MONSTER_STATUS(MONSTER_COLOR_TYPE _eColor, WEAPON_STATE _eWeapon)
		{
			eMonsterColor = _eColor;
			eUseWhatWeapon = _eWeapon;
		}
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
	virtual HRESULT Render_GameObject() PURE;
	virtual HRESULT Render_GameObject_SetShader(CShader* pShader);
	virtual HRESULT Render_GameObject_SetPass(CShader* pShader, _int iPass, _bool _bIsForMotionBlur = false);

protected:	// DJ
	_tchar m_pLayerTag_Of_Target[STR_128] = { L"Layer_Player" };

protected:
	void Set_Target_To_Player();
	void Set_Target_To_Colleague();

	_bool Is_InFov(_float fDegreeOfFov, CTransform* pSelfTransform, _v3 vTargetPos);

	HRESULT Draw_Collider();

protected:
	virtual void Check_CollisionEvent();
	virtual void Check_CollisionPush();
	virtual void Check_CollisionHit(list<CGameObject*> plistGameObject);

	virtual void Function_FBLR(CGameObject* _pGameObject);
	virtual void Function_RotateBody(CGameObject* _pGameObject);
	virtual void Function_MoveAround(CGameObject* _pGameObject, _float _fSpeed, _v3 _vDir = { V3_NULL });
	virtual void Function_CoolDown();
	virtual void Function_Movement(_float _fspeed, _v3 _vDir = { V3_NULL });
	virtual void Function_DecreMoveMent(_float _fMutiply = 1.f);
	virtual void Function_Find_Target();
	virtual void Function_ResetAfterAtk();

protected:
	virtual CGameObject* Clone_GameObject(void* pArg) PURE;
	virtual void Free();

protected:
	CMonsterUI*			m_pMonsterUI = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CNavMesh*			m_pNavMesh = nullptr;
	CCollider*			m_pCollider = nullptr;
	CWeapon*			m_pWeapon = nullptr;

	CGameObject*		m_pTarget = nullptr;

	MONSTER_STATE_TYPE		m_eFirstCategory;
	MONSTER_IDLE_TYPE		m_eSecondCategory_IDLE;
	MONSTER_MOVE_TYPE		m_eSecondCategory_MOVE;
	MONSTER_ATK_TYPE			m_eSecondCategory_ATK;
	MONSTER_HIT_TYPE			m_eSecondCategory_HIT;
	MONSTER_CC_TYPE			m_eSecondCategory_CC;
	MONSTER_DEAD_TYPE		m_eSecondCategory_DEAD;

	WEAPON_STATE			m_eWeaponState;
	FBLR					m_eFBLR;
	MONSTER_COLOR_TYPE		m_eMonsterColor;
	MONSTER_BULLET_TYPE		m_eBulletType;

	_double					m_dTimeDelta;
	_double					m_dAniPlayMul;

	_bool					m_bEventTrigger[30] = {};
	_bool					m_bCanPlayDead; //죽음 애니 재생
	_bool					m_bInRecognitionRange; //인지범위ox
	_bool					m_bInAtkRange; //공격범위ox
	_bool					m_bCanChase; //추적ox
	_bool					m_bCanCoolDown; //쿨타임ox
	_bool					m_bIsCoolDown; //쿨타임 진행중ox
	_bool					m_bCanChooseAtkType; //노말,콤보 가능ox
	_bool					m_bIsCombo; //콤보공격 진행중ox
	_bool					m_bCanIdle; //일상ox
	_bool					m_bIsIdle; //일상 진행중ox
	_bool					m_bCanMoveAround; //경계ox
	_bool					m_bIsMoveAround; //경계동작 진행중

	_float					m_fSkillMoveSpeed_Cur;
	_float					m_fSkillMoveSpeed_Max;
	_float					m_fSkillMoveAccel_Cur;
	_float					m_fSkillMoveAccel_Max;
	_float					m_fSkillMoveMultiply;

	_float					m_fRecognitionRange;
	_float					m_fShotRange;
	_float					m_fAtkRange;
	_float					m_fPersonalRange;
	_float					m_fCoolDownMax;
	_float					m_fCoolDownCur;

	_int					m_iRandom;
	_int					m_iDodgeCount;
	_int					m_iDodgeCountMax;

	_float					m_fDeadEffect_Delay = 0.f;
	_float					m_fDeadEffect_Offset = 0.f;
};

END