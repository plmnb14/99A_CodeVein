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
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_PlayerFontUI", CPlayerFontUI::Create(pDevice))))
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


	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_ExpUI", CExpUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_ConditionUI", CConditionUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_StatusUI", CStatusUI::Create(pDevice))))
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
	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_StageUI")))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Layer(SCENE_MORTAL, L"Layer_PlayerUI")))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Layer(SCENE_STATIC, L"Layer_StaticUI")))
		return E_FAIL;
	
	// 소비 인벤토리
	m_pExpendables_Inven = static_cast<CExpendables_Inven*>(g_pManagement->Clone_GameObject_Return(L"GameObject_ExpendablesInven", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pExpendables_Inven, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	// 재료 인벤토리
	m_pMaterial_Inven = static_cast<CMaterial_Inven*>(g_pManagement->Clone_GameObject_Return(L"GameObject_MaterialInven", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pMaterial_Inven, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	
	// 무기 인벤토리
	m_pWeapon_Inven = static_cast<CWeapon_Inven*>(g_pManagement->Clone_GameObject_Return(L"GameObject_WeaponInven", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pWeapon_Inven, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	
	// 장비 인벤토리
	m_pArmor_Inven = static_cast<CArmor_Inven*>(g_pManagement->Clone_GameObject_Return(L"GameObject_ArmorInven", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pArmor_Inven, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	
	// 전체 인벤토리
	m_pTotal_Inven = static_cast<CTotal_Inven*>(g_pManagement->Clone_GameObject_Return(L"GameObject_TotalInven", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pTotal_Inven, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	
	// 인벤토리 관리 클래스
	m_pInventory = static_cast<CInventory*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Inventory", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pInventory, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	
	// 퀵슬롯
	m_pQuickSlot = static_cast<CQuickSlot*>(g_pManagement->Clone_GameObject_Return(L"GameObject_QuickSlot", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pQuickSlot, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	// 스킬 UI
	m_pSkillUI = static_cast<CSkillUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_SkillUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pSkillUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	
	// 스테이지 선택 UI
	m_pStageSelectUI = static_cast<CStageSelectUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_StageSelectUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pStageSelectUI, SCENE_STAGE, L"Layer_StageUI", nullptr);

	// 겨우살이 UI
	m_pMistletoeUI = static_cast<CMistletoeUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_MistletoeUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pMistletoeUI, SCENE_STAGE, L"Layer_StageUI", nullptr);
	
	//// 스테이터스 UI
	//m_pStatusUI = static_cast<CStatusUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_StatusUI", nullptr));
	//g_pManagement->Add_GameOject_ToLayer_NoClone(m_pStatusUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	//m_pStatusUI->Set_Active(true);
	return NOERROR;
}

_int CUI_Manager::Update_UI()
{
	//if (g_pInput_Device->Key_Up(DIK_O))
	//{
	//	m_bTest = !m_bTest;
	//	m_pMistletoeUI->Set_Active(m_bTest);
	//}
	//if (g_pInput_Device->Key_Up(DIK_P))
	//{
	//	m_pMistletoeUI->Active_SubUI(); // 선택된 항목의 UI On/Off
	//}
	//if (g_pInput_Device->Key_Up(DIK_LEFT))
	//	m_pStageSelectUI->Move_Left(); // 스테이지UI 왼쪽이동
	//if (g_pInput_Device->Key_Up(DIK_RIGHT))
	//	m_pStageSelectUI->Move_Right(); // 스테이지UI 오른쪽 이동

	//if (g_pInput_Device->Key_Up(DIK_J))
	//	m_pStageSelectUI->MoveUp_SubStage();
	//if (g_pInput_Device->Key_Up(DIK_K))
	//	m_pStageSelectUI->MoveDown_SubStage();
	//

	//if (g_pInput_Device->Key_Up(DIK_RETURN))
	//	cout << m_pStageSelectUI->Teleport_Stage() << endl; // 스테이지 선택시, 각각 다른 _uint값 반환

	
	return 0;
}

void CUI_Manager::Set_BossUI_Active(_bool bIsActive)
{
	list<CGameObject*> listBossUI = g_pManagement->Get_GameObjectList(L"Layer_BossHP", SCENE_STAGE);

	for (auto& iter : listBossUI)
	{
		static_cast<CBossHP*>(iter)->Set_Active(bIsActive);
	}
}

void CUI_Manager::Free()
{
}



