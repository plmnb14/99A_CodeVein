#include "stdafx.h"
#include "..\Headers\UI_Manager.h"

#include "Button_UI.h"
#include "HPBack.h"
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
#include "FontNumManager.h"

#include "MassageUI.h"
#include "Get_ItemUI.h"
#include "PickUp_ItemUI.h"

IMPLEMENT_SINGLETON(CUI_Manager)

CUI_Manager::CUI_Manager()
{
}

CUI_Manager::~CUI_Manager()
{
	Free();
}

HRESULT CUI_Manager::Add_UI_Prototype(_Device pDevice)
{
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_HPBack", CHPBack::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_PlayerHP", CPlayerHP::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_PlayerST", CPlayerST::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_BossDecoUI", CBossDecoUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_BossHP", CBossHP::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_ButtonUI", CButton_UI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_QuickSlot", CQuickSlot::Create(pDevice))))
		return E_FAIL;
	

	
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_SelectUI", CSelect_UI::Create(pDevice))))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_ExpendSlot", CExpendables_Slot::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_ExpendablesInven", CExpendables_Inven::Create(pDevice))))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_MaterialSlot", CMaterial_Slot::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_MaterialInven", CMaterial_Inven::Create(pDevice))))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_WeaponSlot", CWeapon_Slot::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_WeaponInven", CWeapon_Inven::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_ArmorSlot", CArmor_Slot::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_ArmorInven", CArmor_Inven::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_NumberUI", CNumberUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_CursorUI", CCursorUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Inventory", CInventory::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_InvenIcon", CInventory_Icon::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_ClickUI", CClickUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_TotalInven", CTotal_Inven::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_BloodCodeIcon", CBloodCode_Icon::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_InfoSlot", CInfo_Slot::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_FontNumUI", CFontNumUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_FontNumManager", CFontNumManager::Create(pDevice))))
		return E_FAIL;


	//////////////// Chae
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_BossMassageUI", CMassageUI::Create(pDevice))))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Get_Item", CGet_ItemUI::Create(pDevice))))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Pickup_Item", CPickUp_ItemUI::Create(pDevice))))
		return E_FAIL;

	return NOERROR;
}

HRESULT CUI_Manager::SetUp_UILayer()
{
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_QuickSlot", SCENE_STAGE, L"Layer_QuickSlot");
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_ExpendablesInven", SCENE_STAGE, L"Layer_ExpendablesInven");
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_MaterialInven", SCENE_STAGE, L"Layer_MaterialInven");
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_WeaponInven", SCENE_STAGE, L"Layer_WeaponInven");
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_ArmorInven", SCENE_STAGE, L"Layer_ArmorInven");
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_TotalInven", SCENE_STAGE, L"Layer_TotalInven");
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_Inventory", SCENE_STAGE, L"Layer_Inventory");

	return NOERROR;
}

_int CUI_Manager::Update_UI()
{
	
	return 0;
}

CExpendables::EXPEND_TYPE CUI_Manager::Use_Item()
{
	CQuickSlot* pQuickSlot = nullptr;
	pQuickSlot = static_cast<CQuickSlot*>(g_pManagement->Get_GameObjectBack(L"Layer_QuickSlot", SCENE_STAGE));
	if (nullptr == pQuickSlot)
		return CExpendables::EXPEND_END;

	return pQuickSlot->Use_Item();
}

WEAPON_STATE CUI_Manager::Get_UseWeaponState(_uint iIndex)
{
	CWeapon_Inven* pWeaponInven = nullptr;
	pWeaponInven = static_cast<CWeapon_Inven*>(g_pManagement->Get_GameObjectBack(L"Layer_WeaponInven", SCENE_STAGE));
	if (nullptr == pWeaponInven)
		return WEAPON_None;

	return pWeaponInven->Get_UseWeaponState(iIndex);
}

CArmor::ARMOR_TYPE CUI_Manager::Get_UseArmorType()
{
	CArmor_Inven* pArmorInven = nullptr;
	pArmorInven = static_cast<CArmor_Inven*>(g_pManagement->Get_GameObjectBack(L"Layer_ArmorInven", SCENE_STAGE));
	if (nullptr == pArmorInven)
		return CArmor::ARMOR_END;
	
	return pArmorInven->Get_UseArmorType();
}

void CUI_Manager::OnOff_BossUI(_bool bIsActive)
{
	CBossHP* pBossUI = static_cast<CBossHP*>(g_pManagement->Get_GameObjectBack(L"Layer_BossHP", SCENE_STAGE));
	if (pBossUI == nullptr)
		return;

	pBossUI->Set_Active(bIsActive);
}

void CUI_Manager::Add_Expendables(CExpendables::EXPEND_TYPE eType, _uint iCnt)
{
	CExpendables_Inven* pExInven = nullptr;
	pExInven = static_cast<CExpendables_Inven*>(g_pManagement->Get_GameObjectBack(L"Layer_ExpendablesInven", SCENE_STAGE));
	if (nullptr == pExInven)
		return;

	pExInven->Add_MultiExpendables(eType, iCnt);
}

void CUI_Manager::Add_Material(CMaterial::MATERIAL_TYPE eType, _uint iCnt)
{
	CMaterial_Inven* pMatInven = nullptr;
	pMatInven = static_cast<CMaterial_Inven*>(g_pManagement->Get_GameObjectBack(L"Layer_MaterialInven", SCENE_STAGE));
	if (nullptr == pMatInven)
		return;

	pMatInven->Add_MultiMaterial(eType, iCnt);
}

void CUI_Manager::Add_Weapon(WEAPON_STATE eType)
{
	CWeapon_Inven* pWeaponInven = nullptr;
	pWeaponInven = static_cast<CWeapon_Inven*>(g_pManagement->Get_GameObjectBack(L"Layer_WeaponInven", SCENE_STAGE));
	if (nullptr == pWeaponInven)
		return;

	pWeaponInven->Add_Weapon(eType);
}

void CUI_Manager::Add_Armor(CArmor::ARMOR_TYPE eType)
{
	CArmor_Inven* pArmorInven = nullptr;
	pArmorInven = static_cast<CArmor_Inven*>(g_pManagement->Get_GameObjectBack(L"Layer_ArmorInven", SCENE_STAGE));
	if (nullptr == pArmorInven)
		return;

	pArmorInven->Add_Armor(eType);
}

_bool CUI_Manager::Get_UI_Active(const _tchar * pLayerTag)
{
	return static_cast<CUI*>(g_pManagement->Get_GameObjectBack(pLayerTag, SCENE_STAGE))->Get_Active();
}

void CUI_Manager::Set_UI_Active(const _tchar * pLayerTag, _bool bIsActive)
{
	static_cast<CUI*>(g_pManagement->Get_GameObjectBack(pLayerTag, SCENE_STAGE))->Set_Active(bIsActive);
}

void CUI_Manager::Free()
{
}



