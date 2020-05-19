#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CGeneralStoreOption;
class CGeneralStoreNPCUI final : public CUI
{
private:
	explicit CGeneralStoreNPCUI(_Device pDevice);
	explicit CGeneralStoreNPCUI(const CGeneralStoreNPCUI& rhs);
	virtual ~CGeneralStoreNPCUI() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	void	SetUp_Default();
	void	Click_Option();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	vector<CGeneralStoreOption*> m_vecOption;
	_float							m_fCross = 100.f;

public:
	static CGeneralStoreNPCUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};

END