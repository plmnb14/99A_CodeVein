#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CBuyOptionUI;
class CPlayerFontUI;
class CExpendables_Slot;
class CExpendSellUI final : public CUI
{
private:
	explicit CExpendSellUI(_Device pDevice);
	explicit CExpendSellUI(const CExpendSellUI& rhs);
	virtual ~CExpendSellUI() = default;

public:
	void Set_SellSlot(CExpendables_Slot* pSlot) { m_pSellSlot = pSlot; }

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
	void	Update_SubUI(_double TimeDelta);
	void	Click_SubUI();
	void	SetUp_Cost();
	void	Sell_Item();
	void	SetUp_SubUI_Active(_bool bIsActive);

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	CBuyOptionUI*			m_pCntMinusOption = nullptr; // �Ǹ� ���� ���̳ʽ�
	CBuyOptionUI*			m_pCntPlusOption = nullptr; // �Ǹ� ���� �÷���
	CBuyOptionUI*			m_pDecisionOption = nullptr; // �Ǹ� ����
	_uint					m_iSellCnt = 0;
	CPlayerFontUI*			m_pSellCntFont = nullptr;
	_uint					m_iCost = 0;	// ���� ���� ����
	_float					m_fAlpha = 0.f;
	CExpendables_Slot*		m_pSellSlot = nullptr;
	_bool					m_bIsSubActive = false;

public:
	static CExpendSellUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END