#pragma once

#include "Base.h"
#include "Component.h"

BEGIN(Engine)

class CCollider;
class CShader;
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
	virtual HRESULT Render_GameObject_SetShader(CShader* pShader);
	virtual HRESULT Render_GameObject_Instancing_SetPass(CShader* pShader);
	virtual HRESULT Render_GameObject_SetPass(CShader* pShader, _int iPass , _bool _bIsForMotionBlur = false);

public:
	_ulong		Get_LayerIdx() { return m_dwLayerIdx; }
	CComponent*	Get_Component(const _tchar* pComponentTag);
	_float				Get_ViewZ(void) { return m_fViewZ; }
	_bool		Get_Enable() { return m_bEnable; }
	_bool		Get_Dead() { return m_bIsDead; }

public:
	void	Set_Target(CGameObject* _pTarget) { m_pTarget = _pTarget; }
	void	Set_LayerIdx(_ulong _dwLayerIdx) { m_dwLayerIdx = _dwLayerIdx; }
	void			Set_Enable(_bool _bEnable) {m_bEnable = _bEnable;}
	void			Set_Dead();
	void			Set_ViewZ(_float fViewZ) { m_fViewZ = fViewZ; }

public:
	void			Start_Dissolve(_float fFxSpeed = 0.7f, _bool bFadeIn = false, _bool bReadyDead = false, _float fDelay = 0.f);

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
	OBJECT_PARAM Get_Target_Param() { return m_tObjParam; }
	OBJECT_PARAM* Get_pTarget_Param() { return &m_tObjParam; }
	_bool Get_Target_IsAir() { return m_tObjParam.bAir; }
	_bool Get_Target_IsDown() { return m_tObjParam.bDown; }
	_bool Get_Target_IsDodge() { return m_tObjParam.bIsDodge; };
	_bool Get_Target_CanDodge() { return m_tObjParam.bCanDodge; };
	_bool Get_Target_IsHit() { return m_tObjParam.bIsHit; }
	_bool Get_Target_CanHit() { return m_tObjParam.bCanHit; }
	_bool Get_Target_HitAgain() { return m_tObjParam.bHitAgain; }
	_bool Get_Target_IsKnockBack() { return m_tObjParam.bIsKnockBack; }
	_bool Get_Target_IsAttack() { return m_tObjParam.bIsAttack; }
	_bool Get_Target_CanAttack() { return m_tObjParam.bCanAttack; }
	_bool Get_Target_SuperArmor() { return m_tObjParam.bSuperArmor; };
	_bool Get_Target_IsExicution() { return m_tObjParam.bIsExecution; }
	_bool Get_Target_CanExicution() { return m_tObjParam.bCanExecution; }
	_bool Get_Target_IsCounter() { return m_tObjParam.bIsCounter; }
	_bool Get_Target_CanCounter() { return m_tObjParam.bCanCounter; }
	_bool Get_Target_IsRepel() { return m_tObjParam.bIsRepel; }
	_bool Get_Target_CanRepel() { return m_tObjParam.bCanRepel; }
	_float Get_Target_Hp() { return m_tObjParam.fHp_Cur; };
	_float Get_Target_Stamina() { return m_tObjParam.fStamina_Cur; };
	_float Get_Target_HoldGage() { return m_tObjParam.fHoldGage_Cur; };
	_float Get_Target_DodgeTimer_Cur() { return m_tObjParam.fDodgeTimer_Cur; };
	_float Get_Target_DodgeTimer_Max() { return m_tObjParam.fDodgeTimer_Max; };
	_float Get_Target_GuardTimer_Cur() { return m_tObjParam.fGuardTimer_Cur; };
	_float Get_Target_GuardTimer_Max() { return m_tObjParam.fGuardTimer_Max; };
	EXE_TYPE Get_Target_Execution_Type() { return m_tObjParam.eExecutionType; }
	EXE_WPN Get_Target_Execution_Wpn() { return m_tObjParam.eExecutionWpn; }
	_short Get_Target_Mana_Cur() { return m_tObjParam.sMana_Cur; }
	_short Get_Target_Mana_Max() { return m_tObjParam.sMana_Max; }

public:
	void Add_Target_Mana(_short _sMana) { m_tObjParam.sMana_Cur += _sMana; };
	void Add_Target_Hp(_float _fHp) {
										m_tObjParam.fHp_Cur += _fHp;
										if (m_tObjParam.fHp_Cur > m_tObjParam.fHp_Max) m_tObjParam.fHp_Cur = m_tObjParam.fHp_Max;
									 };
	void Add_Target_Stamina(_float _fStamina) { m_tObjParam.fStamina_Cur += _fStamina; };
	void Add_Target_HoldGage(_float _fHoldGage) { m_tObjParam.fHoldGage_Cur += _fHoldGage; };

	void Hit_Target(_float _fDamage);

public:
	void Set_Target_IsAir(_bool _bAir) { m_tObjParam.bAir = _bAir; }
	void Set_Target_IsDown(_bool _bDown) { m_tObjParam.bDown = _bDown; }
	void Set_Target_IsHit(_bool _bHit) { m_tObjParam.bIsHit = _bHit; }
	void Set_Target_CanHit(_bool _bCanHit) { m_tObjParam.bCanHit = _bCanHit; }
	void Set_HitAgain(_bool _bHitAgain) { m_tObjParam.bHitAgain = _bHitAgain; }
	void Set_Target_IsAttack(_bool _bIsAttack) { m_tObjParam.bIsAttack = _bIsAttack; }
	void Set_Target_CanAttack(_bool _bCanAttack) { m_tObjParam.bCanAttack = _bCanAttack; }
	void Set_Target_IsKnockBack(_bool _bKnockBack) { m_tObjParam.bIsKnockBack = _bKnockBack; }
	void Set_Target_IsDodge(_bool _bDodge) { m_tObjParam.bIsDodge = _bDodge; }
	void Set_Target_CanDodge(_bool _bDodge) { m_tObjParam.bCanDodge = _bDodge; }
	void Set_Target_SuperArmor(_bool _bSuperArmor) { m_tObjParam.bSuperArmor = _bSuperArmor; }
	void Set_Target_IsExicution(_bool _bIsExicution) { m_tObjParam.bIsExecution = _bIsExicution; }
	void Set_Target_CanExicution(_bool _bCanExicution) { m_tObjParam.bCanExecution = _bCanExicution; }
	void Set_Target_IsCounter(_bool _bIsCounter) { m_tObjParam.bIsCounter = _bIsCounter; }
	void Set_Target_CanCounter(_bool _bCanCounter) { m_tObjParam.bCanCounter = _bCanCounter; }
	void Set_Target_IsRepel(_bool _bIsRepel) { m_tObjParam.bIsRepel = _bIsRepel; }
	void Set_Target_CanRepel(_bool _bCanRepel) { m_tObjParam.bCanRepel = _bCanRepel; }
	void Set_Target_DodgeTimer_Cur(_float _fDodgeTimer) { m_tObjParam.fDodgeTimer_Cur = _fDodgeTimer; }
	void Set_Target_DodgeTimer_Max(_float _fDodgeTimer) { m_tObjParam.fDodgeTimer_Max = _fDodgeTimer; }
	void Set_Target_GuardTimer_Cur(_float _fDodgeTimer) { m_tObjParam.fGuardTimer_Cur = _fDodgeTimer; }
	void Set_Target_GuardTimer_Max(_float _fDodgeTimer) { m_tObjParam.fGuardTimer_Max = _fDodgeTimer; }
	void Set_Target_Execution_Type(EXE_TYPE _eType) { m_tObjParam.eExecutionType = _eType; }
	void Set_Target_Execution_Wpn(EXE_WPN _eWpn) { m_tObjParam.eExecutionWpn = _eWpn; }
	void Set_CounterTarget(CGameObject* pGameObj) { pCounterTarget = pGameObj; };
	void Set_Target_Mana_Cur(_short _sMana) { m_tObjParam.sMana_Cur = _sMana; }
	void Set_Target_Mana_Max(_short _sMana) { m_tObjParam.sMana_Max = _sMana; }


	void Set_Target_HitDir(_v3 _vHitDir) { m_tObjParam.vHitDir = _vHitDir; }
	void Set_Target_HitPos(_v3 _vHitPos) { m_tObjParam.vHitPos = _vHitPos; }

public:
	vector<CCollider*>&		Get_PhysicColVector() { return m_vecPhysicCol; };	// 충돌 체크용 콜라이더 벡터
	vector<CCollider*>&		Get_AtkColVector() { return m_vecAttackCol; };		// 공격용 콜라이더 벡터

public:
	void Calulate_MeshContainerSize(void* _pMesh , _bool _bIsDynamic);

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
	CGameObject*	pCounterTarget = nullptr;

protected:
	_bool	m_bIsDead = false;
	_bool	m_bIsInit = false;
	_bool	m_bEnable = true;

protected:
	
	_float	m_fViewZ = 0.f;

	// For	MotionBlur
	_mat	m_matLastWVP;

	// For Dissolve
	_bool	m_bOnDissolve = false;
	_bool	m_bFadeIn = false;
	_bool	m_bDissolve = false;
	_bool	m_bReadyDead = false;
	_float	m_fFXSpeed = 0.f;
	_float	m_fFXAlpha = 0.f;
	_float	m_fFXDelay = 0.f;

	_int	m_iPass = 0;
	_int    m_iTempPass = 0;
protected:
	_ulong			m_dwLayerIdx = 0;			// 생성될 레이어 번호

protected:
	_bool			m_bInFrustum = false;	// 절두체 내부임?

protected:
	_ulong			m_dwMeshContainer = 0;
	_ulong*			m_arrSubsetCnt = nullptr;

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