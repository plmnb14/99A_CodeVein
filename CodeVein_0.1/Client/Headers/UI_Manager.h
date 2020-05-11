#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Item_Manager.h"


#include "PlayerHP.h"
#include "PlayerST.h"
#include "BossDecoUI.h"
#include "BossHP.h"
#include "BossNameUI.h"

#include "QuickSlot.h"

#include "Expendables_Inven.h"
#include "Expendables_Slot.h"
#include "Material_Inven.h"
#include "Material_Slot.h"
#include "Weapon_Slot.h"
#include "Weapon_Inven.h"
#include "Armor_Slot.h"
#include "Armor_Inven.h"
#include "Inventory.h"
#include "Inventory_Icon.h"
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
#include "BloodCodeOwner.h"
#include "BloodCodeSlot.h"
#include "BloodCodeSelectUI.h"
#include "SkillReleaseUI.h"
#include "BloodSkillSlot.h"
#include "BloodSkillCursor.h"

#include "BloodCodeUI.h"
#include "ReleaseQuestionUI.h"
#include "ReleaseOption.h"
#include "SkillAcquisitionUI.h"
#include "OKMessageUI.h"
#include "Skill_Inven.h"
#include "SkillSlot.h"
#include "SkillIcon.h"

#include "Pet_Inven.h"
#include "Pet_Slot.h"

#include "NoticeUI.h"
#include "ExplainSkillUI.h"

#include "HazeUI.h"
#include "ScriptUI.h"

#include "WeaponShopUI.h"
#include "WeaponShopOptionUI.h"
#include "WeaponBuyUI.h"
#include "Weapon_Inven_InShop.h"

BEGIN(Client)

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
	CBloodCodeOwner* Get_BloodCode_Owner() { return m_pBloodCodeOwner; }
	CBloodCodeSelectUI* Get_BloodCode_SelectUI() { return m_pBloodCodeSelectUI; }
	CSkillReleaseUI* Get_Skill_ReleaseUI() { return m_pSkillReleaseUI; }
	CSkillAcquisitionUI* Get_Skill_AcquisitionUI() { return m_pSkillAcquisitionUI; }
	CSkill_Inven* Get_Skill_Inven() { return m_pSkillInven; }
	CHazeUI* Get_HazeUI() { return m_pHazeUI; }
	CScriptUI* Get_ScriptUI() { return m_pScriptUI; }
	
	// �� �κ��丮
	CPet_Inven* Get_Pet_Inven() { return m_pPet_Inven; }

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
	CBloodCodeOwner* m_pBloodCodeOwner = nullptr;
	CBloodCodeSelectUI * m_pBloodCodeSelectUI = nullptr;
	CSkillReleaseUI* m_pSkillReleaseUI = nullptr;
	CSkillAcquisitionUI* m_pSkillAcquisitionUI = nullptr;
	CSkill_Inven* m_pSkillInven = nullptr;
	CHazeUI*	m_pHazeUI = nullptr;
	CScriptUI*	m_pScriptUI = nullptr;
	
	// �� �κ��丮
	CPet_Inven* m_pPet_Inven = nullptr;

public:
	virtual void Free();
};

END