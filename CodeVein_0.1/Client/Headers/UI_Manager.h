#pragma once

#include "Client_Defines.h"
#include "UI.h"



BEGIN(Client)

class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager)

private:
	explicit CUI_Manager();
	virtual ~CUI_Manager();

public:
	HRESULT Add_UI_Prototype(_Device pDevice);
	HRESULT SetUp_UILayer();
	_int	Update_UI();

public:
	_uint	Get_CoundItem() { return m_uiCoundItem; }
	void	Set_CoundItem(_uint _Cound) { m_uiCoundItem = _Cound; }

private:
	_bool m_bIsOpenEx = false;
	_bool m_bIsOpenMat = false;
	_bool m_bIsOpenWea = false;

	_uint m_uiCoundItem = 0;

//private:
//	CGet_ItemUI*	m_pGet_ItemUI = nullptr;
//	CPickUp_ItemUI*	m_pPickUp_ItemUI = nullptr;

public:
	virtual void Free();
};

END