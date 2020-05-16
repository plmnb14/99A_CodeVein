#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Expendables.h"

BEGIN(Client)
class CBuyOptionUI;
class CPlayerFontUI;
class CExpendBuyUI final : public CUI
{
private:
	explicit CExpendBuyUI(_Device pDevice);
	explicit CExpendBuyUI(const CExpendBuyUI& rhs);
	virtual ~CExpendBuyUI() = default;

public:
	void Set_Type(CExpendables::EXPEND_TYPE eType) { m_eType = eType; }

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
	void	Buy_Item();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	CBuyOptionUI*			m_pCntMinusOption = nullptr; // 구매 수량 마이너스
	CBuyOptionUI*			m_pCntPlusOption = nullptr; // 구매 수량 플러스
	CBuyOptionUI*			m_pDecisionOption = nullptr; // 구매 결정
	_uint					m_iBuyCnt = 0; // 구매할 물건 개수
	CPlayerFontUI*			m_pBuyCntFont = nullptr;
	CExpendables::EXPEND_TYPE			m_eType = CExpendables::EXPEND_END;
	_uint					m_iCost = 0;	// 물건 개당 가격
	_float					m_fAlpha = 0.f;

public:
	static CExpendBuyUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};
END
