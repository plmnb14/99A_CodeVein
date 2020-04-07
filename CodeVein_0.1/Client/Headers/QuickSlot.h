#pragma once

#include "Client_Defines.h"
#include "UI.h"

#include "Expendables_Slot.h"

BEGIN(Client)

class CButton_UI;
class CNumberUI;
class CQuickSlot final : public CUI
{
private:
	explicit CQuickSlot(_Device pDevice);
	explicit CQuickSlot(const CQuickSlot& rhs);
	virtual ~CQuickSlot() = default;

public:
	vector<CExpendables_Slot*> Get_QuickSlotData() { return m_vecQuickSlot; }

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

public:
	CExpendables::EXPEND_TYPE Use_Item();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	vector<CExpendables_Slot*>	m_vecQuickSlot;
	_uint						m_iSelect = 0;
	vector<CButton_UI*>			m_vecDecoUI;
	CNumberUI*					m_pNumberUI = nullptr;

public:
	static CQuickSlot*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END

