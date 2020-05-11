#pragma once

#include "Client_Defines.h"
#include "UI.h"

#include "Expendables_Slot.h"

BEGIN(Client)

class CButton_UI;
class CNumberUI;
class CPlayerFontUI;
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
	HRESULT SetUp_ConstantTable(_uint iIndex);
	void	SetUp_Default();
	void	Move_QuickSlot();

public:
	CExpendables::EXPEND_TYPE Use_Item(); // 퀵슬롯에서 아이템 사용시 해당 아이템 제거 및 사용 아이템 타입 반환

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
	//CNumberUI*					m_pNumberUI = nullptr;
	CExpendables::EXPEND_TYPE	m_eType = CExpendables::EXPEND_END;
	CPlayerFontUI*			m_pItemCntFont = nullptr;

public:
	static CQuickSlot*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END

