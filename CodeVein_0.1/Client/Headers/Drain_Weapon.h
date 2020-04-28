#pragma once

#include "Client_Defines.h"
#include "RenderObject.h"
#include "Management.h"
#include "Player.h"

BEGIN(Client)

class CDrain_Weapon : public CGameObject
{
public:
	typedef enum CDrain_Weapon_Type
	{
		Drain_Tail, Drain_Wolf, Drain_Ivy, Drain_End
	}DRAIN_TYPE;

	typedef enum Drain_Weapon_Skill
	{
		Drain_Parry,
		Drain_Charge_Start,
		Drain_Charge_End,
		Drain_ComboSuck,
		Drain_GroundSuck,
		Drain_SpecialSuck_01,
		Drain_SpecialSuck_02
	}DRAIN_SKILL;

protected:
	explicit CDrain_Weapon(_Device pGraphic_Device);
	explicit CDrain_Weapon(const CDrain_Weapon& rhs);
	virtual ~CDrain_Weapon() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Render_GameObject_SetPass(CShader * pShader, _int iPass);

public:
	virtual void			Set_Active(_bool _bActiveDrain);
	// 무기를 붙일 뼈의 메트릭스 주소를 받습니다.
	virtual void			Set_AttachBoneMartix(_mat* _matAttachBone) { m_pmatAttach = _matAttachBone; }
	// 무기를 붙인 부모의 메트릭스 주소를 받습니다.
	virtual void			Set_ParentMatrix(_mat* _matParent) { m_pmatParent = _matParent; }
	// 흡혈 타입 설정
	virtual void			Set_DrainType(DRAIN_TYPE _eDrainType) { meDrainType = _eDrainType; }
	// 충돌 콜라이더 활성화
	virtual void			Set_ActiveCollider(_bool _bActiveCollider);
	virtual void			Set_ResetOldAnimIdx();
	virtual void			Set_AnimIdx(_ulong _eAnimState);
	virtual void			Set_Enable_Record(_bool _bOnRecord);
	virtual void			Set_OnExecution(_bool _bOnExecution) { m_bOnExecution = _bOnExecution; };

public:
	virtual void			Find_TailHeadBone();

public:
	virtual void OnCollisionEnter();
	virtual void OnCollisionEvent(list<CGameObject*> plistGameObject);

public:
	virtual void Update_Trails(_double TimeDelta);
	virtual void Update_Collider();
	virtual void Draw_Collider();

public:
	virtual _bool Update_CollisionRecord(CGameObject* pObject);
	virtual void Clear_CollisionRecordList();

private:
	// 애니메이션 인덱스는 공유함.
	DRAIN_SKILL				m_eAnimnum = Drain_Parry;
	DRAIN_TYPE				meDrainType = Drain_Tail;

private:
	list<CGameObject*>		m_listCollisionRecord;

private:
	_bool					m_bActive = false;
	_bool					m_bRecordCollision = false;
	_bool					m_bOffDissolve = false;
	_bool					m_bOnExecution = false;

private:
	CTransform*				m_pTransform = nullptr;
	CRenderer*				m_pRenderer = nullptr;
	CShader*				m_pShader = nullptr;
	CMesh_Dynamic*			m_pMesh_Dynamic = nullptr;
	CCollider*				m_pCollider = nullptr;
	CBattleAgent*			m_pBattleAgent = nullptr;

private:
	_mat*					m_matTailSword = nullptr;
	_mat*					m_matTailHead = nullptr;

private:
	_float					m_fCreateEffectTime = 0.f;

private:
	_tchar					m_szName[MAX_STR] = L"";

private:
	_mat*					m_pmatAttach = nullptr;
	_mat*					m_pmatParent = nullptr;

private:
	_float					m_fAnimMultiply = 0.f;

private:
	_bool					m_bEventTrigger[4] = {};

private:
	HRESULT Add_Component();
	HRESULT SetUp_Default();
	HRESULT SetUp_ConstantTable();

private:
	virtual HRESULT Parameter_State();

private:
	virtual void Cacl_AttachBoneTransform();
	virtual void Change_WeaponMesh(const _tchar* _MeshName);

public:
	static	CDrain_Weapon* Create(_Device pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);

public:
	virtual void Free();
};

END