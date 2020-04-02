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

private:
	_bool m_bIsOpenEx = false;
	_bool m_bIsOpenMat = false;
	_bool m_bIsOpenWea = false;

public:
	virtual void Free();
};

END