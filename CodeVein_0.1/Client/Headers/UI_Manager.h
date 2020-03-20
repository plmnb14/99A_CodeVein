#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Management.h"

BEGIN(Client)

class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager)

private:
	explicit CUI_Manager();
	virtual ~CUI_Manager();

public:
	HRESULT SetUp_UILayer();

public:
	virtual void Free();
};

END