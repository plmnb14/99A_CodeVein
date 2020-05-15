#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CMaterialOptionUI;
class CMaterial_InfoUI;
class CGeneralStoreBuyUI;
class CMaterialCollectionUI final : public CUI
{
private:
	explicit CMaterialCollectionUI(_Device pDevice);
	explicit CMaterialCollectionUI(const CMaterialCollectionUI& rhs);
	virtual ~CMaterialCollectionUI() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);

private:
	HRESULT Add_Component();
	void	SetUp_Default();
	void	Update_SubUI();
	void	Click_SubUI();
	void	Buy_Material(CMaterialOptionUI* pOption);

private:
	CTransform*				m_pTransformCom = nullptr;

private:
	vector<CMaterialOptionUI*>	m_vecOption;
	CMaterial_InfoUI*			m_pInfoUI = nullptr;
	_uint						m_iBuyCnt = 1; // 물건 사는 개수
	_uint						m_iCost = 0; // 물건 개당 가격
	CGeneralStoreBuyUI*			m_pBuyUI = nullptr;

public:
	static CMaterialCollectionUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*			Clone_GameObject(void* pArg);
	virtual void					Free();
};
END
