#include "stdafx.h"
#include "..\Headers\UI_Manager.h"

// 유미
#include "MassageUI.h"

//============================
#include "UI_FontNum.h"
//============================

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
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_BossNameUI", CBossNameUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_BossHP", CBossHP::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_QuickSlot", CQuickSlot::Create(pDevice))))
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
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Inventory", CInventory::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_InvenIcon", CInventory_Icon::Create(pDevice))))
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

	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_BloodCodeMenuUI", CBloodCodeMenuUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_BloodCodeOwner", CBloodCodeOwner::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_BloodCodeSlot", CBloodCodeSlot::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_BloodCodeSelectUI", CBloodCodeSelectUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_BloodSkillSlot", CBloodSkillSlot::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_SkillReleaseUI", CSkillReleaseUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_BloodSkillCursor", CBloodSkillCursor::Create(pDevice))))
		return E_FAIL;

	
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_BloodCodeUI", CBloodCodeUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_ReleaseQuestionUI", CReleaseQuestionUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_ReleaseOption", CReleaseOption::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_SkillAcquisitionUI", CSkillAcquisitionUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_OKMessageUI", COKMessageUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_SkillInven", CSkill_Inven::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_SkillSlot", CSkillSlot::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_SkillIcon", CSkillIcon::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_NoticeUI", CNoticeUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_ExplainSkillUI", CExplainSkillUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_HazeUI", CHazeUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_ScriptUI", CScriptUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_ExplainArmorUI", CExplainArmorUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_ExplainWeaponUI", CExplainWeaponUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_ExplainMaterialUI", CExplainMaterialUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_ExplainExpendUI", CExplainExpendUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_GeneralStoreUI", CGeneralStoreUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_MaterialOptionUI", CMaterialOptionUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_MaterialCollectionUI", CMaterialCollectionUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Material_InfoUI", CMaterial_InfoUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_ExpendCollectionUI", CExpendCollectionUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_ExpendOptionUI", CExpendOptionUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Expend_InfoUI", CExpend_InfoUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_ExpendBuyUI", CExpendBuyUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_MaterialBuyUI", CMaterialBuyUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_BuyOptionUI", CBuyOptionUI::Create(pDevice))))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_SkillGauge", CSkillGauge::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_SkillPointUI", CSkillPointUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_PurchaseFailUI", CPurchaseFailUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_GeneralStoreSellUI", CGeneralStoreSellUI::Create(pDevice))))
		return E_FAIL;
	
	//////////////// Chae
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_BossMassageUI", CMassageUI::Create(pDevice))))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_PickUP_ItemUI", CPickUp_ItemUI::Create(pDevice))))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Calling_Colleague", CCalling_Colleague::Create(pDevice))))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_PIckUP_ItemUIMgr", CPickUp_ItemUIManager::Create(pDevice))))
		return E_FAIL;
	
	//if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Get_ItemUI", CGet_ItemUI::Create(pDevice))))
	//	return E_FAIL;

	//=======================================================
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_FontNum", CUI_FontNum::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_LockOn_UI", CLockOn_UI::Create(pDevice))))
		return E_FAIL;
	//=======================================================

	// ================== 펫 인벤토리 ==================
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_PetInven", CPet_Inven::Create(pDevice))))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_PetSlot", CPet_Slot::Create(pDevice))))
		return E_FAIL;

	// ==============================================================================
	// NPC
	// ==============================================================================
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_WeaponShopUI", CWeaponShopUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_WeaponShopOptionUI", CWeaponShopOptionUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_WeaponBuyUI", CWeaponBuyUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Weapon_Inven_InShop", CWeapon_Inven_InShop::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Weapon_BuyPopup", CWeaponBuyPopupUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Weapon_BuyPopupOption", CWeaponBuyPopupOptionUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Weapon_Upgrade", CWeaponUpgradeUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Weapon_UpgradeOption", CWeaponUpgradeOptionUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Weapon_UpgradeSuccessPopup", CWeaponUpgradeSuccessPopupUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Weapon_UpgradeSuccessPopupOption", CWeaponUpgradeSuccessPopupOptionUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Weapon_UpgradingPopup", CWeaponUpgradingPopup::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Weapon_UpgradingEff", CWeaponUpgradingEff::Create(pDevice))))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_NPC_Interaction", CNPC_InteractionUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_ShopActionFailedPopup", CShopActionFailedPopup::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_ShopItemIcon", CShopItemIcon::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_GeneralStoreOption", CGeneralStoreOption::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_ExpendSellUI", CExpendSellUI::Create(pDevice))))
		return E_FAIL;
	

	// ===================================================================================
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_GeneralStoreNPCUI", CGeneralStoreNPCUI::Create(pDevice))))
		return E_FAIL;

	// ===================================================================================

	return NOERROR;
}

HRESULT CUI_Manager::SetUp_UILayer()
{
	if (FAILED(g_pManagement->Add_Layer(SCENE_MORTAL, L"Layer_StageUI")))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Layer(SCENE_MORTAL, L"Layer_PlayerUI")))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Layer(SCENE_STATIC, L"Layer_StaticUI")))
		return E_FAIL;
	// 마우스 UI
	m_pMouseUI = static_cast<CMouseUI*>(g_pManagement->Get_GameObjectBack(L"Layer_MouseUI", SCENE_STATIC));

	// 플레이어 HP
	m_pPlayerHP = static_cast<CPlayerHP*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerHP", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pPlayerHP, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	// 플레이어 ST
	m_pPlayerST = static_cast<CPlayerST*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerST", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pPlayerST, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	
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
	m_pWeaponInventory = static_cast<CInventory*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Inventory", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pWeaponInventory, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	
	// 퀵슬롯
	m_pQuickSlot = static_cast<CQuickSlot*>(g_pManagement->Clone_GameObject_Return(L"GameObject_QuickSlot", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pQuickSlot, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	// 스킬 UI
	m_pSkillUI = static_cast<CSkillUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_SkillUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pSkillUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	
	// 스테이터스 UI
	m_pStatusUI = static_cast<CStatusUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_StatusUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pStatusUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	
	// 스킬 인벤
	m_pSkillInven = static_cast<CSkill_Inven*>(g_pManagement->Clone_GameObject_Return(L"GameObject_SkillInven", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pSkillInven, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	// 대화 UI
	m_pScriptUI = static_cast<CScriptUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_ScriptUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pScriptUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	// 스킬 포인트 UI
	m_pSkillPointUI = static_cast<CSkillPointUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_SkillPointUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pSkillPointUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	///////////////////////////////////////// 3D /////////////////////////////////////
	// 스테이지 선택 UI
	m_pStageSelectUI = static_cast<CStageSelectUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_StageSelectUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pStageSelectUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	// 겨우살이 UI
	m_pMistletoeUI = static_cast<CMistletoeUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_MistletoeUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pMistletoeUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	// 블러드코드메뉴창 UI
	m_pBloodCodeMenu = static_cast<CBloodCodeMenuUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_BloodCodeMenuUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBloodCodeMenu, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	// 블러드코드 주인
	m_pBloodCodeOwner = static_cast<CBloodCodeOwner*>(g_pManagement->Clone_GameObject_Return(L"GameObject_BloodCodeOwner", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBloodCodeOwner, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	// 블러드코드 선택 UI
	m_pBloodCodeSelectUI = static_cast<CBloodCodeSelectUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_BloodCodeSelectUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBloodCodeSelectUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	// 스킬(연혈) 릴리즈 UI
	m_pSkillReleaseUI = static_cast<CSkillReleaseUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_SkillReleaseUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pSkillReleaseUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	// 스킬 획득 UI
	m_pSkillAcquisitionUI = static_cast<CSkillAcquisitionUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_SkillAcquisitionUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pSkillAcquisitionUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	// 헤이즈 UI
	m_pHazeUI = static_cast<CHazeUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_HazeUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pHazeUI, SCENE_MORTAL, L"Layer_HazeUI", nullptr);
	// 소비, 재료 상점 UI
	m_pGeneralStoreUI = static_cast<CGeneralStoreUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_GeneralStoreUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pGeneralStoreUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	// 잡화점 NPC UI
	m_pGeneralStoreNPCUI = static_cast<CGeneralStoreNPCUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_GeneralStoreNPCUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pGeneralStoreNPCUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pWeaponShopUI = static_cast<CWeaponShopUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_WeaponShopUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pWeaponShopUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pNPC_InterActionUI = static_cast<CNPC_InteractionUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_NPC_Interaction", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pNPC_InterActionUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	// 잡화점 판매
	m_pGeneralStoreSellUI = static_cast<CGeneralStoreSellUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_GeneralStoreSellUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pGeneralStoreSellUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	//////////////////////////////////// 펫 UI /////////////////////////////////////////////////////////////////////
	// 펫 인벤토리
	m_pPet_Inven = static_cast<CPet_Inven*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PetInven", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pPet_Inven, SCENE_MORTAL, L"Layer_PetUI", nullptr);


	////////////Get Item UI
	m_pPickUp_ItemUI = static_cast<CPickUp_ItemUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PickUP_ItemUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pPickUp_ItemUI, SCENE_MORTAL, L"Layer_PickUp_ItemUI", nullptr);

	m_pPickUP_ItemUIMgr = static_cast<CPickUp_ItemUIManager*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PIckUP_ItemUIMgr", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pPickUP_ItemUIMgr, SCENE_MORTAL, L"Layer_PickUP_ItemUIMgr", nullptr);

	//m_pGet_ItemUI = static_cast<CGet_ItemUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Get_ItemUI", nullptr));
	//g_pManagement->Add_GameOject_ToLayer_NoClone(m_pGet_ItemUI, SCENE_MORTAL, L"Layer_Get_ItemUI", nullptr);

	// 동료 활성화/비활성화 UI
	m_pCalling_Colleague = static_cast<CCalling_Colleague*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Calling_Colleague", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pCalling_Colleague, SCENE_MORTAL, L"Layer_Calling_ColleagueUI", nullptr);

	// 코스튬
	m_pCustomCategory = static_cast<CCustomCategory*>(g_pManagement->Clone_GameObject_Return(L"GameObject_CustomCategory", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pCustomCategory, SCENE_MORTAL, L"Layer_Custom", nullptr);
	
	return NOERROR;
}

_int CUI_Manager::Update_UI()
{
	/*if (g_pInput_Device->Key_Up(DIK_O))
	{
		m_bTest = !m_bTest;
		m_pMistletoeUI->Set_Active(m_bTest);
	}*/
	//if (g_pInput_Device->Key_Up(DIK_P))
	//{
	//	m_pMistletoeUI->Active_SubUI(); // 선택된 항목의 UI On/Off
	//}
	//if (g_pInput_Device->Key_Up(DIK_LEFT))
	//	m_pStageSelectUI->Move_Left(); // 스테이지UI 왼쪽이동
	//if (g_pInput_Device->Key_Up(DIK_RIGHT))
	//	m_pStageSelectUI->Move_Right(); // 스테이지UI 오른쪽 이동
	//if (g_pInput_Device->Key_Up(DIK_G))
	//	m_pStageSelectUI->MoveUp_SubStage(); // 스테이지UI 위 이동
	//if (g_pInput_Device->Key_Up(DIK_H))
	//	m_pStageSelectUI->MoveDown_SubStage(); // 스테이지UI 아래 이동

	//if (g_pInput_Device->Key_Up(DIK_J))
	//	m_pMistletoeUI->Move_Up();
	//if (g_pInput_Device->Key_Up(DIK_K))
	//	m_pMistletoeUI->Move_Down();
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



