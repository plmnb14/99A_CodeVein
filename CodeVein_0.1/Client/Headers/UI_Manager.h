#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Management.h"

#include "HPBack.h"
#include "PlayerHP.h"
#include "PlayerST.h"
#include "BossDecoUI.h"
#include "BossHP.h"
#include "ItemSlot.h"

#include "Item.h"

/////////////////
#include "Inventory.h"
#include "Inven_Icon.h"
#include "Item_Icon.h"

#include "Inven_Status.h"
#include "Inven_Item.h"
#include "Inven_Skill.h"
/////////////////


#include "Consume_Item.h"



BEGIN(Client)

class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager)

public: //메뉴창 상태
	enum INVEN_STATE
	{
		INVEN_NONE,
		INVEN_BASE,
		INVEN_STATUS,
		INVEN_ITEM,
		INVEN_ACTIVE,
		INVEN_END
	};

private:
	explicit CUI_Manager();
	virtual ~CUI_Manager();

public:
	_bool	Get_OpenInven() { return m_bOpenInven; }

public:
	void	Set_OpenInven() { m_bOpenInven = false; }

public:
	HRESULT Add_UI_Prototype(_Device pDevice);
	HRESULT SetUp_UILayer();
	_int	Update_UI();
	void	Set_InvenState(INVEN_STATE eState) { m_eInvenState = eState; }

	void	Open_Inventory();
	void	Open_Status();
	void	Open_Item();
	void	Open_Skill();

	/////////////////////////////////////////////////////////////////////////////////////////////

private:
	INVEN_STATE		m_eInvenState;
	_bool			m_bOpenInven = false;
	_uint			m_iIndex = 0;

public:
	virtual void Free();
};

END