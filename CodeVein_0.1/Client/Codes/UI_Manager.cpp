#include "stdafx.h"
#include "..\Headers\UI_Manager.h"

#include "Button_UI.h"
#include "HPBack.h"
#include "PlayerHP.h"
#include "PlayerST.h"
#include "BossDecoUI.h"
#include "BossHP.h"

#include "QuickSlot.h"

////////////////////
//유미작업
//#include "MonsterUI.h"
//#include "MassageUI.h"
//#include "Get_ItemUI.h"
///////////////////

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
	
	//////////////////////////////////////////////////////////////////////////
	// 유미 작업
	//if (FAILED(g_pManagement->Add_Prototype(L"GameObject_MonsterUI", CMonsterUI::Create(pDevice))))
	//	return E_FAIL;

	//if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Start_BossUI", CMassageUI::Create(pDevice))))
	//	return E_FAIL;

	//if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Get_ItemUI", CGet_ItemUI::Create(pDevice))))
	//	return E_FAIL;
	////////////////////////////////////////////////////////////////////////////////////////////
	
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
	return NOERROR;
}

HRESULT CUI_Manager::SetUp_UILayer()
{
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_QuickSlot", SCENE_STAGE, L"Layer_QuickSlot");
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_ExpendablesInven", SCENE_STAGE, L"Layer_ExpendablesInven");
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_MaterialInven", SCENE_STAGE, L"Layer_MaterialInven");
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_WeaponInven", SCENE_STAGE, L"Layer_WeaponInven");
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_ArmorInven", SCENE_STAGE, L"Layer_ArmorInven");

	return NOERROR;
}

_int CUI_Manager::Update_UI()
{
	/*CExpendables_Inven* pExpendInven = static_cast<CExpendables_Inven*>(g_pManagement->Get_GameObjectBack(L"Layer_ExpendablesInven", SCENE_STAGE));
	CMaterial_Inven*	pMaterialInven = static_cast<CMaterial_Inven*>(g_pManagement->Get_GameObjectBack(L"Layer_MaterialInven", SCENE_STAGE));
	CWeapon_Inven* pWeaponInven = static_cast<CWeapon_Inven*>(g_pManagement->Get_GameObjectBack(L"Layer_WeaponInven", SCENE_STAGE));
	CArmor_Inven* pArmorInven = static_cast<CArmor_Inven*>(g_pManagement->Get_GameObjectBack(L"Layer_ArmorInven", SCENE_STAGE));*/
	
	
	return 0;
}

void CUI_Manager::Free()
{
}



