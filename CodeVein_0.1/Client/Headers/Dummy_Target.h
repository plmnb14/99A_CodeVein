#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CDummy_Target final : public CGameObject
{
protected:
	explicit CDummy_Target(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CDummy_Target(const CDummy_Target& rhs);
	virtual ~CDummy_Target() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

public:
	virtual vector<CCollider*> Get_ColliderVector() { return m_vecPhysicCollider; }

private:
	CTransform*			m_pTransform = nullptr;
	CRenderer*			m_pRenderer = nullptr;
	CShader*			m_pShader = nullptr;
	CMesh_Dynamic*		m_pDynamic_Mesh = nullptr;
	
private:
	vector<CCollider*>			m_vecPhysicCollider;

	_double				m_dTimeDelta = 0;

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CDummy_Target* Create(_Device pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END