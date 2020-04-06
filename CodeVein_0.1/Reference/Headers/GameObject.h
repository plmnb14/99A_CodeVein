#pragma once

#include "Base.h"
#include "Component.h"

BEGIN(Engine)

class CCollider;
class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	explicit CGameObject(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype(); // 원복객체 생성 시, 호출될 함수.
	virtual HRESULT Ready_GameObject(void* pArg); // 인게임 객체 생성 시, 호출될 함수.
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT LateInit_GameObject();
	virtual HRESULT Render_GameObject();

public:
	virtual _ulong		Get_LayerIdx() { return m_dwLayerIdx; }
	virtual CComponent*	Get_Component(const _tchar* pComponentTag);
	_float				Get_ViewZ(void) { return m_fViewZ; }
	virtual _bool		Get_Enable() { return m_bEnable; }

public:
	virtual void	Set_Target(CGameObject* _pTarget) { m_pTarget = _pTarget; }
	virtual void	Set_LayerIdx(_ulong _dwLayerIdx) { m_dwLayerIdx = _dwLayerIdx; }
	void			Set_Enable(_bool _bEnable) {m_bEnable = _bEnable;}
	void			Set_Dead();
	void			Set_ViewZ(_float fViewZ) { m_fViewZ = fViewZ; }

public:
	void			Start_Dissolve(_float fFxSpeed = 0.7f, _bool bFadeIn = false, _bool bReadyDead = false);

public:
	HRESULT			Add_Component(_uint iSceneID, const _tchar* pPrototypeTag, const _tchar* pComponentTag, CComponent** ppComponent, void* pArg = nullptr);

protected:
	void			Compute_ViewZ(const _v3* pPos);
	void			Check_Dissolve(_double TimeDelta);

	//======================================================================================================================
	// 임시 충돌	
	//======================================================================================================================
protected:
	OBJECT_PARAM m_tObjParam;

public:
	virtual OBJECT_PARAM Get_Target_Param() { return m_tObjParam; }
	virtual OBJECT_PARAM* Get_pTarget_Param() { return &m_tObjParam; }
	virtual _bool Get_Target_IsAir() { return m_tObjParam.bAir; }
	virtual _bool Get_Target_IsDown() { return m_tObjParam.bDown; }
	virtual _bool Get_Target_Dodge() { return m_tObjParam.bDodge; };
	virtual _bool Get_Target_IsHit() { return m_tObjParam.bIsHit; }
	virtual _bool Get_Target_CanHit() { return m_tObjParam.bCanHit; }
	virtual _bool Get_Target_KnockBack() { return m_tObjParam.bKnockBack; }
	virtual _bool Get_Target_IsAttack() { return m_tObjParam.bIsAttack; }
	virtual _bool Get_Target_CanAttack() { return m_tObjParam.bCanAttack; }
	virtual _float Get_Target_Hp() { return m_tObjParam.fHp_Cur; };
	virtual _float Get_Target_Stamina() { return m_tObjParam.fStamina_Cur; };
	virtual _float Get_Target_HoldGage() { return m_tObjParam.fHoldGage_Cur; };

public:
	virtual void Add_Target_Hp(_float _fHp) { m_tObjParam.fHp_Cur += _fHp; };
	virtual void Add_Target_Stamina(_float _fStamina) { m_tObjParam.fStamina_Cur += _fStamina; };
	virtual void Add_Target_HoldGage(_float _fHoldGage) { m_tObjParam.fHoldGage_Cur += _fHoldGage; };

public:
	virtual void Set_Target_IsAir(_bool _bAir) { m_tObjParam.bAir = _bAir; }
	virtual void Set_Target_IsDown(_bool _bDown) { m_tObjParam.bDown = _bDown; }
	virtual void Set_Target_IsHit(_bool _bHit) { m_tObjParam.bIsHit = _bHit; }
	virtual void Set_Target_CanHit(_bool _bCanHit) { m_tObjParam.bCanHit = _bCanHit; }
	virtual void Set_Target_IsAttack(_bool _bIsAttack) { m_tObjParam.bIsAttack = _bIsAttack; }
	virtual void Set_Target_CanAttack(_bool _bCanAttack) { m_tObjParam.bCanAttack = _bCanAttack; }
	virtual void Set_Target_KnockBack(_bool _bKnockBack) { m_tObjParam.bKnockBack = _bKnockBack; }
	virtual void Set_Target_Dodge(_bool _bDodge) { m_tObjParam.bDodge = _bDodge; }
	
public:
	vector<CCollider*>		Get_PhysicColVector() { return m_vecPhysicCol; };	// 충돌 체크용 콜라이더 벡터
	vector<CCollider*>		Get_AtkColVector() { return m_vecAttackCol; };		// 공격용 콜라이더 벡터

protected:
	vector<CCollider*>		m_vecPhysicCol;		// 충돌 체크용 콜라이더 벡터
	vector<CCollider*>		m_vecAttackCol;		// 공격용 콜라이더 벡터

//======================================================================================================================
// 임시 충돌	
//======================================================================================================================

protected:
	LPDIRECT3DDEVICE9		m_pGraphic_Dev = nullptr;

protected:
	CGameObject*	m_pTarget = nullptr;

protected:
	_bool	m_bIsDead = false;
	_bool	m_bIsInit = false;
	_bool	m_bEnable = true;

protected:
	
	_float	m_fViewZ = 0.f;

	// For	MotionBlur
	_mat	m_matLastWVP;

	// For Dissolve
	_bool	m_bFadeIn = false;
	_bool	m_bReadyDead = false;
	_float	m_fFXSpeed = 0.f;
	_float	m_fFXAlpha = 0.f;
	_int	m_iPass = 0;

protected:
	_ulong			m_dwLayerIdx = 0;			// 생성될 레이어 번호

protected:
	map<const _tchar*, CComponent*>			m_pmapComponents;
	typedef map<const _tchar*, CComponent*>	COMPONENTS;

public:
	CComponent*	Find_Component(const _tchar* pComponentTag);

public:
	virtual CGameObject* Clone_GameObject(void* pArg) = 0;
	virtual void Free();
};

END