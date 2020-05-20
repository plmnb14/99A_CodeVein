#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CInventory_Icon;
class CExpendSellCollectionUI;
class CGeneralStoreSellUI final : public CUI
{
private:
	explicit CGeneralStoreSellUI(_Device pDevice);
	explicit CGeneralStoreSellUI(const CGeneralStoreSellUI& rhs);
	virtual ~CGeneralStoreSellUI() = default;

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
	void	Click_SubUI();
	void	SetUp_SubUI_Active(_bool bIsActive);

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	CInventory_Icon*			m_pShopIcon[2];
	CExpendSellCollectionUI*	m_pExpendSellUI = nullptr;
	_bool						m_bIsSubActive = false;

public:
	static CGeneralStoreSellUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};
END
