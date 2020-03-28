#pragma once

#include "MenuBaseUI.h"

class CMenu_UI : public CUI
{
private:
	enum MENU_UI
	{
		MENU_BASE,
		MENU_ITEM,
		MENU_END
	};
private:
	explicit CMenu_UI(_Device pGraphic_Device);
	explicit CMenu_UI(const CMenu_UI& rhs);
	virtual ~CMenu_UI() = default;

public:
	HRESULT			Ready_MenuUI();
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

public:
	static CMenu_UI*	Create(_Device pGraphic_Device);
	virtual CMenu_UI*	Clone_GameObject(void* pArg);
	virtual void		Free();

};

