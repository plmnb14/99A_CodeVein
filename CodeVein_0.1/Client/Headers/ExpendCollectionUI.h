#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CExpendOptionUI;
class CExpend_InfoUI;
class CExpendCollectionUI final : public CUI
{
private:
	explicit CExpendCollectionUI(_Device pDevice);
	explicit CExpendCollectionUI(const CExpendCollectionUI& rhs);
	virtual ~CExpendCollectionUI() = default;

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
	void	Click_SubUI();
	void	Buy_Expendable(CExpendOptionUI* pOption);

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	vector<CExpendOptionUI*>	m_vecOption;
	CExpend_InfoUI*				m_pInfoUI = nullptr;
	_uint						m_iBuyCnt = 1; // 물건 사는 개수
	_uint						m_iCost = 0; // 물건 개당 가격


public:
	static CExpendCollectionUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};
END
