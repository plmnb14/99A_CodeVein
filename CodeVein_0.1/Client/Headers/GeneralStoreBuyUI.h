#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CBuyOptionUI;
class CGeneralStoreBuyUI final : public CUI
{
private:
	explicit CGeneralStoreBuyUI(_Device pDevice);
	explicit CGeneralStoreBuyUI(const CGeneralStoreBuyUI& rhs);
	virtual ~CGeneralStoreBuyUI() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_uint iIndex);
	void	SetUp_Default();
	void	Update_SubUI();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	CBuyOptionUI*			m_pLeftArrow = nullptr;

public:
	static CGeneralStoreBuyUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};
END
