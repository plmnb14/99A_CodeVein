#pragma once

#include "GameObject.h"
#include "Management.h"

class CTestWeapon : public CGameObject
{
protected:
	explicit CTestWeapon(_Device pGraphic_Device);
	explicit CTestWeapon(LPDIRECT3DDEVICE9 pGraphic_Device, _tchar* szWeaponname);
	explicit CTestWeapon(const CTestWeapon& rhs);
	virtual ~CTestWeapon() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Render_GameObject_SetPass(CShader * pShader, _int iPass);

public:
	void Change_Matrix(_mat* pParent, _mat* pBone) { m_pmatParent = pParent,m_pmatAttach=pBone; }
	void Change_Radius(_float Radius);
	void Change_Height(_float Height);

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CTestWeapon* Create(LPDIRECT3DDEVICE9 pGraphic_Device, _tchar* szWeaponName);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

private:
	CManagement*			m_pManagement = nullptr;
	CTransform*				m_pTransform = nullptr;
	CRenderer*				m_pRenderer = nullptr;
	CShader*				m_pShader = nullptr;
	CMesh_Static*			m_pMesh = nullptr;

	_tchar*					m_pszWeaponName;
	_float					m_fRadius = 1.f;
	_float					m_fHeight = 1.f;
	_mat*					m_pmatAttach;
	_mat*					m_pmatParent;
};

