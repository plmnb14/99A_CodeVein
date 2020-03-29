#pragma once

#include "GameObject.h"
#include "Management.h"

class CTestObject final : public Engine::CGameObject
{
protected:
	explicit CTestObject(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CTestObject(LPDIRECT3DDEVICE9 pGraphic_Device, _tchar* szMeshname);
	explicit CTestObject(const CTestObject& rhs);
	virtual ~CTestObject() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	void	Set_AniSpeed(_float _Speed) { m_fAniPlayMul = _Speed; }

public:
	static CTestObject* Create(LPDIRECT3DDEVICE9 pGraphic_Device, _tchar* szMeshname);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

private:
	CTransform*			m_pTransform = nullptr;
	CRenderer*			m_pRenderer = nullptr;
	CShader*			m_pShader = nullptr;
	CMesh_Dynamic*		m_pMesh = nullptr;

	_tchar*				m_pszMeshName;
	_double				m_dTimeDelta = 0;
	_float				m_fAniPlayMul = 2.f;
};

