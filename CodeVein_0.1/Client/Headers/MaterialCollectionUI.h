#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CMaterialOptionUI;
class CMaterial_InfoUI;
class CMaterialBuyUI;
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

private:
	void	SetUp_Default();
	void	Update_SubUI();
	void	Click_SubUI();

private:
	vector<CMaterialOptionUI*>	m_vecOption;
	CMaterial_InfoUI*			m_pInfoUI = nullptr;
	_uint						m_iBuyCnt = 1; // 물건 사는 개수
	_uint						m_iCost = 0; // 물건 개당 가격
	CMaterialBuyUI*				m_pBuyUI = nullptr;
	CMaterialOptionUI*			m_pSoundOption = nullptr;

public:
	static CMaterialCollectionUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*			Clone_GameObject(void* pArg);
	virtual void					Free();
};
END
