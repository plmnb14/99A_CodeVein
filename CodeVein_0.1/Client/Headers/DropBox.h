#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

//#include "ActeiveObject.h"
#include "DropItem.h"

BEGIN(Client)

class CDropBox final : public CGameObject
{
private:
	explicit CDropBox(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CDropBox(const CDropBox& rhs);
	virtual ~CDropBox() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg) override;
	virtual _int Update_GameObject(_double TimeDelta) override;
	virtual _int Late_Update_GameObject(_double TimeDelta) override;
	virtual HRESULT Render_GameObject() override;

private:
	HRESULT Add_Component(void* _pArg);
	HRESULT Ready_Status(void * _pArg);
	HRESULT SetUp_ConstantTable(CShader* pShader);

private:
	HRESULT SetUp_Default();
	HRESULT	Default_Setting();

public:
	static	CDropBox* Create(_Device pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

private:
	CTransform*				m_pTransform = nullptr;
	CRenderer*				m_pRenderer = nullptr;
	CShader*				m_pShader = nullptr;
	CMesh_Static*			m_pMesh_Static = nullptr;
	CCollider*				m_pCollider = nullptr;
	COptimization*			m_pOptimization = nullptr;
	CBattleAgent*			m_pBattleAgent = nullptr;


	_bool	m_bOpenBox = false;
};

END