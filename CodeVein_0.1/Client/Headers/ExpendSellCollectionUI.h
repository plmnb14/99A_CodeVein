#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CExpendables_Slot;
class CExpendSellUI;
class CExpendSellCollectionUI final : public CUI
{
private:
	explicit CExpendSellCollectionUI(_Device pDevice);
	explicit CExpendSellCollectionUI(const CExpendSellCollectionUI& rhs);
	virtual ~CExpendSellCollectionUI() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_uint iIndex);
	void	SetUp_SubUI_Active(_bool bIsActive);
	void	SetUp_SlotPos();
	void	Click_SubUI();
	void	SetUp_Default();

public:
	void Sell_ExpendItem(CExpendables_Slot* pSlot);

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	_bool						m_bIsSubActive = false;
	vector<CExpendables_Slot*>	m_vecSlot;
	CExpendSellUI*				m_pExpendSellUI = nullptr;

public:
	static CExpendSellCollectionUI*		Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};
END
