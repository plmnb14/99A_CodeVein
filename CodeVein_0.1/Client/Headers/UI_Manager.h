#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Management.h"

#include "MenuBaseUI.h"
#include "Menu_Item.h"
#include "Item_QuickSlot.h"
#include "LeftArrow.h"
#include "RightArrow.h"
#include "ActiveSkill_UI.h"
#include "Menu_Active.h"

BEGIN(Client)

class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager)

public: //메뉴창 상태
	enum MENUWIN_STATE
	{
		WIN_NONE,
		WIN_BASE,
		WIN_STATUS,
		WIN_ITEM,
		WIN_ACTIVE,
		WIN_END
	};

private:
	explicit CUI_Manager();
	virtual ~CUI_Manager();

public:
	HRESULT SetUp_UILayer();
	_int	Update_UI();
	void	Set_MenuState(MENUWIN_STATE eState) { m_eMenuState = eState; }
	void	Add_Item(CItem::ITEM_TYPE eType); // 해당 타입의 아이템 인벤토리 창에 추가
	void	Use_Item(); // 퀵슬롯에 표시된 아이템 수 감소
	CItem::ITEM_TYPE Get_QuickItemType(); // 현재 퀵슬롯의 아이템 타입 반환

private:
	MENUWIN_STATE m_eMenuState;

public:
	virtual void Free();
};

END