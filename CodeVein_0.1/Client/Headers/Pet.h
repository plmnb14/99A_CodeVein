#pragma once

#include "Client_Defines.h"
#include "Client_Item_Enum.h"
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
		PET_STATUS(ITEM_NAMETYPE _eName)
			:ePetName(_eName)
		{
		}

		ITEM_NAMETYPE	ePetName = ITEM_NAMETYPE::NAMETYPE_End;
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
	virtual ITEM_NAMETYPE Get_PetName() { return m_eType; }
	virtual void Set_PetName(ITEM_NAMETYPE _eType) { m_eType = _eType; }

	virtual void Function_FBLR(CGameObject* _pGameObject);
	virtual void Function_RotateBody(CGameObject* _pGameObject);
	virtual void Function_MoveAround(CGameObject* _pGameObject, _float _fSpeed, _v3 _vDir = { V3_NULL });
	virtual void Function_CoolDown();
	virtual void Function_Movement(_float _fspeed, _v3 _vDir = { V3_NULL });
	virtual void Function_DecreMoveMent(_float _fMutiply = 1.f);
	virtual void Function_CalcMoveSpeed(_float _fMidDist);
	virtual void Function_Find_Target();
	virtual void Function_ResetAfterAtk();
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
	FBLR				m_eFBLR= FBLR::FRONT;

	ITEM_NAMETYPE		m_eType = ITEM_NAMETYPE::NAMETYPE_End;

	_double				m_dTimeDelta = 0;
	_double				m_dAniPlayMul = 1;

	_float				m_fSkillMoveSpeed_Cur = 0.f;
	_float				m_fSkillMoveSpeed_Max = 0.f;
	_float				m_fSkillMoveAccel_Cur = 0.f;
	_float				m_fSkillMoveAccel_Max = 0.f;
	_float				m_fSkillMoveMultiply = 0.f;
	_float				m_fPersonalRange = 4.f; //player, ��ǥ���� ��ȸ�� �Ÿ��α� ����

	_bool				m_bEventTrigger[30] = {};
	_bool				m_bCanSummon = false; //��ȯ ����
	_bool				m_bIsSummon = false; //��ȯ�� ����

	//�÷��̾�� ������ ����
	_bool				m_bAbsoluteOrder = false; //�켱 ���� ����
	_float				m_fLimitRange = 25.f; //�ִ� ���� �Ÿ�, �ش� ���� ���� ��� ������ ����
	_float				m_fActiveRange = 20.f; //�ൿ ���� �Ÿ�, �ൿ ����
	_bool				m_bInLimitRange = false; //�ִ� ���� �Ÿ�
	_bool				m_bInActiveRange = false; //���� �ൿ �Ÿ�

	//�� ��ü �̿� ����
	_float				m_fRecognitionRange = 20.f; //�� ���� ����
	_float				m_fShotRange = 10.f; //�� ���Ÿ� ����
	_float				m_fAtkRange = 5.f; //�� �ٰŸ� ����
	_float				m_fCoolDownCur = 0.f; //��Ÿ�� ����
	_float				m_fCoolDownMax = 0.f; //�ش� ī��Ʈ�� �������� ��Ÿ������
	_float				m_fDeadEffect_Delay = 0.f;
	_float				m_fDeadEffect_Offset = 0.f;

	_bool				m_bCanPlayDead = false; //���� ��� ������,����Ʈ �۵��ϱ� ���� ����
	_bool				m_bInRecognitionRange = false; //���� ���� ����
	_bool				m_bInShotRange = false; //���Ÿ� ���� ����
	_bool				m_bInAtkRange = false; //�ٰŸ� ���� ����
	_bool				m_bCanChase = false; //���� ���� ����

	_bool				m_bCanCoolDown = false; //�ൿ�� ����� �� ��κ� ���Ǵ� ��Ÿ�� ���� ����
	_bool				m_bIsCoolDown = false; //��Ÿ�� ������ ����

	_bool				m_bCanIdle =true; //�⺻ ���� ���࿩��
	_bool				m_bIsIdle =false; //�⺻ ���� ������ ����

	_bool				m_bCanChooseAtkType = true; //�Ϲ�,�޺� ���� ����
	_bool				m_bIsCombo = false; //�޺� ���� ����

	_int				m_iRandom = 0; //���� ���� ����� ���� ���� ����, ��� ����?

};

END