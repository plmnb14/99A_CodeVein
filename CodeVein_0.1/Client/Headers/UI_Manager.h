#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Item_Manager.h"


#include "Button_UI.h"
#include "PlayerHP.h"
#include "PlayerST.h"
#include "BossDecoUI.h"
#include "BossHP.h"

#include "QuickSlot.h"

#include "Expendables_Inven.h"
#include "Expendables_Slot.h"
#include "Select_UI.h"
#include "Material_Inven.h"
#include "Material_Slot.h"
#include "Weapon_Slot.h"
#include "Weapon_Inven.h"
#include "Armor_Slot.h"
#include "Armor_Inven.h"
#include "NumberUI.h"
#include "CursorUI.h"
#include "Inventory.h"
#include "Inventory_Icon.h"
#include "ClickUI.h"
#include "Total_Inven.h"
#include "BloodCode_Icon.h"
#include "Info_Slot.h"
#include "FontNumUI.h"
#include "PlayerFontUI.h"
#include "Active_Icon.h"
#include "SkillUI.h"
#include "LoadingScreen.h"
#include "LoadingBar.h"
#include "StageUI.h"
#include "StageSelectUI.h"
#include "MistletoeUI.h"
#include "MistletoeOptionUI.h"
#include "ConditionUI.h"
#include "StatusUI.h"
#include "ExpUI.h"
#include "BloodCodeMenuUI.h"
#include "CodeOwnerUI.h"
#include "BloodCodeSlot.h"

BEGIN(Client)
class CStatusUI;
class CMistletoeUI;
class CStageSelectUI;
class CExpendables_Inven;
class CSkillUI;
class CQuickSlot;
class CMaterial_Inven;
class CWeapon_Inven;
class CArmor_Inven;
class CTotal_Inven;
class CInventory;
class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager)

private:
	explicit CUI_Manager();
	virtual ~CUI_Manager();

public:
	_uint Get_CoundItem() { return m_uiCoundItem; }
	void Set_CoundItem(_uint _Cound) { m_uiCoundItem = _Cound; }

public:
	HRESULT Add_UI_Prototype(_Device pDevice);
	HRESULT SetUp_UILayer();
	_int	Update_UI();
	CStatusUI* Get_StatusUI() { return m_pStatusUI; }
	CMistletoeUI* Get_MistletoeUI() { return m_pMistletoeUI; }
	CStageSelectUI* Get_StageSelectUI() { return m_pStageSelectUI; }
	CExpendables_Inven* Get_Expendables_Inven() { return m_pExpendables_Inven; }
	CSkillUI* Get_Skill_UI() { return m_pSkillUI; }
	CQuickSlot* Get_QuickSlot() { return m_pQuickSlot; }
	CMaterial_Inven* Get_Material_Inven() { return m_pMaterial_Inven; }
	CWeapon_Inven* Get_Weapon_Inven() { return m_pWeapon_Inven; }
	CArmor_Inven* Get_Armor_Inven() { return m_pArmor_Inven; }
	CTotal_Inven* Get_Total_Inven() { return m_pTotal_Inven; }
	CInventory*		Get_Inventory() { return m_pInventory; }
	void Set_BossUI_Active(_bool bIsActive);
	CBloodCodeMenuUI* Get_BloodCode_Menu() { return m_pBloodCodeMenu; }

private:
	_uint m_uiCoundItem = 0;
	_bool m_bTest = false;
	CStatusUI* m_pStatusUI = nullptr;
	CMistletoeUI* m_pMistletoeUI = nullptr;
	CStageSelectUI* m_pStageSelectUI = nullptr;
	CExpendables_Inven* m_pExpendables_Inven = nullptr;
	CSkillUI* m_pSkillUI = nullptr;
	CQuickSlot* m_pQuickSlot = nullptr;
	CMaterial_Inven* m_pMaterial_Inven = nullptr;
	CWeapon_Inven* m_pWeapon_Inven = nullptr;
	CArmor_Inven* m_pArmor_Inven = nullptr;
	CTotal_Inven* m_pTotal_Inven = nullptr;
	CInventory*		m_pInventory = nullptr;
	CBloodCodeMenuUI* m_pBloodCodeMenu = nullptr;

public:
	virtual void Free();
};

END