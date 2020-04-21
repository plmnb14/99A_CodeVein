#include "stdafx.h"
#include "..\Headers\UI_Manager.h"

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
#include "FontNumManager.h"
#include "Active_Icon.h"
#include "SkillUI.h"
#include "LoadingScreen.h"
#include "LoadingBar.h"
#include "StageUI.h"
#include "StageSelectUI.h"
#include "MistletoeUI.h"
#include "MistletoeOptionUI.h"
#include "ConditionUI.h"
#include "ExpUI.h"
#include "StatusUI.h"

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
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_ActiveIcon", CActive_Icon::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_SkillUI", CSkillUI::Create(pDevice))))
		return E_FAIL;
	
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_StageUI", CStageUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_StageSelectUI", CStageSelectUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_MistletoeUI", CMistletoeUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_MistletoeOptionUI", CMistletoeOptionUI::Create(pDevice))))
		return E_FAIL;
	/*if (FAILED(g_pManagement->Add_Prototype(L"GameObject_ConditionUI", CConditionUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_ExpUI", CExpUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_StatusUI", CStatusUI::Create(pDevice))))
		return E_FAIL;*/
	
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
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_QuickSlot", SCENE_MORTAL, L"Layer_QuickSlot");
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_ExpendablesInven", SCENE_MORTAL, L"Layer_ExpendablesInven");
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_MaterialInven", SCENE_MORTAL, L"Layer_MaterialInven");
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_WeaponInven", SCENE_MORTAL, L"Layer_WeaponInven");
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_ArmorInven", SCENE_MORTAL, L"Layer_ArmorInven");
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_TotalInven", SCENE_MORTAL, L"Layer_TotalInven");
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_Inventory", SCENE_MORTAL, L"Layer_Inventory");
	
	//g_pManagement->Add_GameObject_ToLayer(L"GameObject_SkillUI", SCENE_MORTAL, L"Layer_SkillUI");
	
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_MistletoeUI", SCENE_STAGE, L"Layer_MistletoeUI");
	//g_pManagement->Add_GameObject_ToLayer(L"GameObject_StatusUI", SCENE_STAGE, L"Layer_StatusUI");

	return NOERROR;
}

_int CUI_Manager::Update_UI()
{
	//if (g_pInput_Device->Key_Up(DIK_O))
	//{
	//	m_bTest = !m_bTest;
	//	Active_MistletoeUI(m_bTest); // 겨우살이 UI On/Off
	//}
	//if (g_pInput_Device->Key_Up(DIK_P))
	//{
	//	Active_Mistletoe_SubUI(); // 선택된 항목의 UI On/Off
	//}
	//if (g_pInput_Device->Key_Up(DIK_LEFT))
	//	Move_StageUI_Left(); // 스테이지UI 왼쪽이동
	//if (g_pInput_Device->Key_Up(DIK_RIGHT))
	//	Move_StageUI_Right(); // 스테이지UI 오른쪽 이동
	//if (g_pInput_Device->Key_Up(DIK_J))
	//	MoveUp_SubStage(); // 겨우살이UI 위쪽 이동
	//if (g_pInput_Device->Key_Up(DIK_K))
	//	MoveDown_SubStage(); // 겨우살이UI 아래쪽 이동
	//if (g_pInput_Device->Key_Up(DIK_RETURN))
	//	cout << Teleport_Stage() << endl; // 스테이지 선택시, 각각 다른 _uint값 반환
	return 0;
}

CExpendables::EXPEND_TYPE CUI_Manager::Use_Item()
{
	CQuickSlot* pQuickSlot = nullptr;
	pQuickSlot = static_cast<CQuickSlot*>(g_pManagement->Get_GameObjectBack(L"Layer_QuickSlot", SCENE_MORTAL));
	if (nullptr == pQuickSlot)
		return CExpendables::EXPEND_END;

	return pQuickSlot->Use_Item();
}

WEAPON_STATE CUI_Manager::Get_UseWeaponState(_uint iIndex)
{
	CWeapon_Inven* pWeaponInven = nullptr;
	pWeaponInven = static_cast<CWeapon_Inven*>(g_pManagement->Get_GameObjectBack(L"Layer_WeaponInven", SCENE_MORTAL));
	if (nullptr == pWeaponInven)
		return WEAPON_None;

	return pWeaponInven->Get_UseWeaponState(iIndex);
}

CArmor::ARMOR_TYPE CUI_Manager::Get_UseArmorType()
{
	CArmor_Inven* pArmorInven = nullptr;
	pArmorInven = static_cast<CArmor_Inven*>(g_pManagement->Get_GameObjectBack(L"Layer_ArmorInven", SCENE_MORTAL));
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
	pExInven = static_cast<CExpendables_Inven*>(g_pManagement->Get_GameObjectBack(L"Layer_ExpendablesInven", SCENE_MORTAL));
	if (nullptr == pExInven)
		return;

	pExInven->Add_MultiExpendables(eType, iCnt);
}

void CUI_Manager::Add_Material(CMaterial::MATERIAL_TYPE eType, _uint iCnt)
{
	CMaterial_Inven* pMatInven = nullptr;
	pMatInven = static_cast<CMaterial_Inven*>(g_pManagement->Get_GameObjectBack(L"Layer_MaterialInven", SCENE_MORTAL));
	if (nullptr == pMatInven)
		return;

	pMatInven->Add_MultiMaterial(eType, iCnt);
}

void CUI_Manager::Add_Weapon(WEAPON_STATE eType)
{
	CWeapon_Inven* pWeaponInven = nullptr;
	pWeaponInven = static_cast<CWeapon_Inven*>(g_pManagement->Get_GameObjectBack(L"Layer_WeaponInven", SCENE_MORTAL));
	if (nullptr == pWeaponInven)
		return;

	pWeaponInven->Add_Weapon(eType);
}

void CUI_Manager::Add_Armor(CArmor::ARMOR_TYPE eType)
{
	CArmor_Inven* pArmorInven = nullptr;
	pArmorInven = static_cast<CArmor_Inven*>(g_pManagement->Get_GameObjectBack(L"Layer_ArmorInven", SCENE_MORTAL));
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

void CUI_Manager::Set_BossHP_Active(_bool bIsActive)
{
	CBossHP* pBossUI = nullptr;

	pBossUI = static_cast<CBossHP*>(g_pManagement->Get_GameObjectBack(L"Layer_BossHP", SCENE_STAGE));
	if (nullptr == pBossUI)
		return;

	pBossUI->Set_Active(false);
}

_uint CUI_Manager::Select_Stage()
{
	CStageSelectUI* pStageUI = static_cast<CStageSelectUI*>(g_pManagement->Get_GameObjectBack(L"Layer_StageSelectUI", SCENE_STAGE));
	if (nullptr == pStageUI)
		return _uint(CStageUI::Teleport_End);
	Active_MistletoeUI(false);
	return _uint(pStageUI->Select_Stage());
}

_uint CUI_Manager::Teleport_Stage()
{
	CStageSelectUI* pStageUI = static_cast<CStageSelectUI*>(g_pManagement->Get_GameObjectBack(L"Layer_StageSelectUI", SCENE_STAGE));
	if (nullptr == pStageUI)
		return 0;
	return _uint(pStageUI->Teleport_Stage());
}

void CUI_Manager::MoveUp_SubStage()
{
	CStageSelectUI* pStageUI = static_cast<CStageSelectUI*>(g_pManagement->Get_GameObjectBack(L"Layer_StageSelectUI", SCENE_STAGE));
	if (nullptr == pStageUI)
		return;

	pStageUI->MoveUp_SubStage();
}

void CUI_Manager::MoveDown_SubStage()
{
	CStageSelectUI* pStageUI = static_cast<CStageSelectUI*>(g_pManagement->Get_GameObjectBack(L"Layer_StageSelectUI", SCENE_STAGE));
	if (nullptr == pStageUI)
		return;

	pStageUI->MoveDown_SubStage();
}

void CUI_Manager::Move_StageUI_Right()
{
	CStageSelectUI* pStageUI = static_cast<CStageSelectUI*>(g_pManagement->Get_GameObjectBack(L"Layer_StageSelectUI", SCENE_STAGE));
	if (nullptr == pStageUI)
		return;

	pStageUI->Move_Right();
}

void CUI_Manager::Move_StageUI_Left()
{
	CStageSelectUI* pStageUI = static_cast<CStageSelectUI*>(g_pManagement->Get_GameObjectBack(L"Layer_StageSelectUI", SCENE_STAGE));
	if (nullptr == pStageUI)
		return;

	pStageUI->Move_Left();
}

void CUI_Manager::Active_MistletoeUI(_bool bIsActive)
{
	CMistletoeUI* pMistletoeUI = static_cast<CMistletoeUI*>(g_pManagement->Get_GameObjectBack(L"Layer_MistletoeUI", SCENE_STAGE));
	if (nullptr == pMistletoeUI)
		return;

	pMistletoeUI->Set_Active(bIsActive);
}

void CUI_Manager::Active_Mistletoe_SubUI()
{
	CMistletoeUI* pMistletoeUI = static_cast<CMistletoeUI*>(g_pManagement->Get_GameObjectBack(L"Layer_MistletoeUI", SCENE_STAGE));
	if (nullptr == pMistletoeUI)
		return;

	pMistletoeUI->Active_SubUI();
}

void CUI_Manager::Move_MistletoeUI_Up()
{
	CMistletoeUI* pMistletoeUI = static_cast<CMistletoeUI*>(g_pManagement->Get_GameObjectBack(L"Layer_MistletoeUI", SCENE_STAGE));
	if (nullptr == pMistletoeUI)
		return;

	pMistletoeUI->Move_Up();
}

void CUI_Manager::Move_MistletoeUI_Down()
{
	CMistletoeUI* pMistletoeUI = static_cast<CMistletoeUI*>(g_pManagement->Get_GameObjectBack(L"Layer_MistletoeUI", SCENE_STAGE));
	if (nullptr == pMistletoeUI)
		return;

	pMistletoeUI->Move_Down();
}

void CUI_Manager::Free()
{
}



