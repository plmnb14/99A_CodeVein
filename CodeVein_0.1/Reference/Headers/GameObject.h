#pragma once

#include "Base.h"
#include "Component.h"

BEGIN(Engine)

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

public:
	virtual void Set_Target(CGameObject* _pTarget) { m_pTarget = _pTarget; }
	virtual void Set_LayerIdx(_ulong _dwLayerIdx) { m_dwLayerIdx = _dwLayerIdx; }
	void Set_Dead();

public:
	HRESULT			Add_Component(_uint iSceneID, const _tchar* pPrototypeTag, const _tchar* pComponentTag, CComponent** ppComponent, void* pArg = nullptr);

protected:
	void			Compute_ViewZ(const _v3* pPos);

protected:
	LPDIRECT3DDEVICE9		m_pGraphic_Dev = nullptr;

protected:
	CGameObject*	m_pTarget = nullptr;

protected:
	_bool	m_bIsDead = false;
	_bool	m_bIsInit = false;
	_float	m_fViewZ = 0.f;

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