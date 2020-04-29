#pragma once
#include "Info_Monster.h"

BEGIN(Client)

class CPet abstract : public CGameObject
{
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
	virtual HRESULT Render_GameObject_SetPass(CShader* pShader, _int iPass) PURE;

protected:
	virtual void Update_Collider() PURE;
	virtual void Render_Collider() PURE;
	virtual void Check_CollisionEvent() PURE;
	virtual void Check_CollisionPush() PURE;
	virtual void Check_CollisionHit(list<CGameObject*> plistGameObject) PURE;

	virtual void Function_FBLR(CGameObject* _pGameObject);
	virtual void Function_RotateBody(CGameObject* _pGameObject);
	virtual void Function_MoveAround(CGameObject* _pGameObject, _float _fSpeed, _v3 _vDir = { V3_NULL });
	virtual void Function_CoolDown();
	virtual void Function_Movement(_float _fspeed, _v3 _vDir = { V3_NULL });
	virtual void Function_DecreMoveMent(_float _fMutiply = 1.f);
	virtual void Function_CalcMoveSpeed(_float _fMidDist);
	virtual void Function_Find_Target();
	virtual void Function_ResetAfterAtk();

	virtual void Play_Idle() PURE;
	virtual void Play_Move() PURE;
	virtual void Play_Hit() PURE;
	virtual void Play_CC() PURE;
	virtual void Play_Dead() PURE;

protected:
	virtual HRESULT Add_Component(void* pArg) PURE;
	virtual HRESULT SetUp_ConstantTable() PURE;
	virtual HRESULT Ready_Status(void* pArg) PURE;
	virtual HRESULT Ready_Weapon(void* pArg) PURE;
	virtual HRESULT Ready_Collider(void* pArg) PURE;
	virtual HRESULT Ready_BoneMatrix(void* pArg) PURE;

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

	MONSTER_STATETYPE	m_eFirstCategory;

	MONSTER_IDLETYPE	m_eSecondCategory_IDLE;
	MONSTER_MOVETYPE	m_eSecondCategory_MOVE;
	MONSTER_ATKTYPE		m_eSecondCategory_ATK;
	MONSTER_HITTYPE		m_eSecondCategory_HIT;
	MONSTER_CCTYPE		m_eSecondCategory_CC;
	MONSTER_DEADTYPE	m_eSecondCategory_DEAD;

	WEAPON_STATE		m_eWeaponState = WEAPON_STATE::WEAPON_None;
	FBLR				m_eFBLR;
	TARGET_TYPE			m_eTarget = TARGET_TYPE::TARGET_NONE;

	_double				m_dTimeDelta = 0;
	_double				m_dAniPlayMul = 1;

	_float				m_fSkillMoveSpeed_Cur;
	_float				m_fSkillMoveSpeed_Max;
	_float				m_fSkillMoveAccel_Cur;
	_float				m_fSkillMoveAccel_Max;
	_float				m_fSkillMoveMultiply;

	//1.�÷��̾���� �Ÿ�
	//�ִ� �Ÿ�
	//�ൿ���ð��� �Ÿ�
	//�������� �Ÿ�
	_float				m_fLimitRange = 40.f; //�ִ� ���� �Ÿ�, player���Է� �޷�����
	_float				m_fActiveRange = 30.f; //Player���� �޷����� ���� ��Ƽ���� ������ �Ҽ� �ִ�
	_bool				m_bInLimitRange = false; //�ִ� ���� �Ÿ�
	_bool				m_bInActiveRange = false; //���� �ൿ �Ÿ�

	//2.��ǥ���� �Ÿ�,�� ��ü�� ����,����,���Ÿ�,���ΰŸ�
	_float				m_fRecognitionRange = 20.f; //��ǥ���� �Ÿ��� ���� ������ �ִ�
	_float				m_fShotRange = 10.f; //��ǥ���� �Ÿ��� ���Ÿ� ���� �ȿ� �ִ�
	_float				m_fAtkRange = 5.f; //��ǥ���� �Ÿ��� �������� ���� �ȿ� �ִ�

	//player �Ǵ� ��ǥ���� �Ÿ��� �ʹ� ����� ���� üũ
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