#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CExpendables_Slot;
class CExpendSellUI final : public CUI
{
private:
	explicit CExpendSellUI(_Device pDevice);
	explicit CExpendSellUI(const CExpendSellUI& rhs);
	virtual ~CExpendSellUI() = default;

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

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	_bool						m_bIsSubActive = false;
	vector<CExpendables_Slot*>	m_vecSlot;

public:
	static CExpendSellUI*		Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};
END
