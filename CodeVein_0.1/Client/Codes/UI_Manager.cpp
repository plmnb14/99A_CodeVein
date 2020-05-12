#include "stdafx.h"
#include "..\Headers\UI_Manager.h"

// ����
#include "MassageUI.h"
#include "Get_ItemUI.h"
#include "PickUp_ItemUI.h"

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
	
	//////////////// Chae
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_BossMassageUI", CMassageUI::Create(pDevice))))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Get_Item", CGet_ItemUI::Create(pDevice))))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Pickup_Item", CPickUp_ItemUI::Create(pDevice))))
		return E_FAIL;
	

	//=======================================================
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_FontNum", CUI_FontNum::Create(pDevice))))
		return E_FAIL;
	//=======================================================

	// ================== �� �κ��丮 ==================
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
	
	// �Һ� �κ��丮
	m_pExpendables_Inven = static_cast<CExpendables_Inven*>(g_pManagement->Clone_GameObject_Return(L"GameObject_ExpendablesInven", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pExpendables_Inven, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	// ��� �κ��丮
	m_pMaterial_Inven = static_cast<CMaterial_Inven*>(g_pManagement->Clone_GameObject_Return(L"GameObject_MaterialInven", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pMaterial_Inven, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	
	// ���� �κ��丮
	m_pWeapon_Inven = static_cast<CWeapon_Inven*>(g_pManagement->Clone_GameObject_Return(L"GameObject_WeaponInven", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pWeapon_Inven, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	
	// ��� �κ��丮
	m_pArmor_Inven = static_cast<CArmor_Inven*>(g_pManagement->Clone_GameObject_Return(L"GameObject_ArmorInven", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pArmor_Inven, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	
	// ��ü �κ��丮
	m_pTotal_Inven = static_cast<CTotal_Inven*>(g_pManagement->Clone_GameObject_Return(L"GameObject_TotalInven", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pTotal_Inven, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	
	// �κ��丮 ���� Ŭ����
	m_pWeaponInventory = static_cast<CInventory*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Inventory", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pWeaponInventory, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	
	// ������
	m_pQuickSlot = static_cast<CQuickSlot*>(g_pManagement->Clone_GameObject_Return(L"GameObject_QuickSlot", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pQuickSlot, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	// ��ų UI
	m_pSkillUI = static_cast<CSkillUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_SkillUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pSkillUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	
	// �������ͽ� UI
	m_pStatusUI = static_cast<CStatusUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_StatusUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pStatusUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	
	// ��ų �κ�
	m_pSkillInven = static_cast<CSkill_Inven*>(g_pManagement->Clone_GameObject_Return(L"GameObject_SkillInven", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pSkillInven, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	// ��ȭ UI
	m_pScriptUI = static_cast<CScriptUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_ScriptUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pScriptUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	///////////////////////////////////////// 3D /////////////////////////////////////
	// �������� ���� UI
	m_pStageSelectUI = static_cast<CStageSelectUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_StageSelectUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pStageSelectUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	// �ܿ���� UI
	m_pMistletoeUI = static_cast<CMistletoeUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_MistletoeUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pMistletoeUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	// �����ڵ�޴�â UI
	m_pBloodCodeMenu = static_cast<CBloodCodeMenuUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_BloodCodeMenuUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBloodCodeMenu, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	// �����ڵ� ����
	m_pBloodCodeOwner = static_cast<CBloodCodeOwner*>(g_pManagement->Clone_GameObject_Return(L"GameObject_BloodCodeOwner", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBloodCodeOwner, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	// �����ڵ� ���� UI
	m_pBloodCodeSelectUI = static_cast<CBloodCodeSelectUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_BloodCodeSelectUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBloodCodeSelectUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	// ��ų(����) ������ UI
	m_pSkillReleaseUI = static_cast<CSkillReleaseUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_SkillReleaseUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pSkillReleaseUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	// ��ų ȹ�� UI
	m_pSkillAcquisitionUI = static_cast<CSkillAcquisitionUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_SkillAcquisitionUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pSkillAcquisitionUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	// ������ UI
	m_pHazeUI = static_cast<CHazeUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_HazeUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pHazeUI, SCENE_MORTAL, L"Layer_HazeUI", nullptr);

	//////////////////////////////////// �� UI /////////////////////////////////////////////////////////////////////
	// �� �κ��丮
	m_pPet_Inven = static_cast<CPet_Inven*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PetInven", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pPet_Inven, SCENE_MORTAL, L"Layer_PetUI", nullptr);
	
	return NOERROR;
}

_int CUI_Manager::Update_UI()
{
	if (g_pInput_Device->Key_Up(DIK_O))
	{
		m_bTest = !m_bTest;
		m_pMistletoeUI->Set_Active(m_bTest);
	}
	//if (g_pInput_Device->Key_Up(DIK_P))
	//{
	//	m_pMistletoeUI->Active_SubUI(); // ���õ� �׸��� UI On/Off
	//}
	//if (g_pInput_Device->Key_Up(DIK_LEFT))
	//	m_pStageSelectUI->Move_Left(); // ��������UI �����̵�
	//if (g_pInput_Device->Key_Up(DIK_RIGHT))
	//	m_pStageSelectUI->Move_Right(); // ��������UI ������ �̵�
	//if (g_pInput_Device->Key_Up(DIK_G))
	//	m_pStageSelectUI->MoveUp_SubStage(); // ��������UI �� �̵�
	//if (g_pInput_Device->Key_Up(DIK_H))
	//	m_pStageSelectUI->MoveDown_SubStage(); // ��������UI �Ʒ� �̵�

	//if (g_pInput_Device->Key_Up(DIK_J))
	//	m_pMistletoeUI->Move_Up();
	//if (g_pInput_Device->Key_Up(DIK_K))
	//	m_pMistletoeUI->Move_Down();
	//

	//if (g_pInput_Device->Key_Up(DIK_RETURN))
	//	cout << m_pStageSelectUI->Teleport_Stage() << endl; // �������� ���ý�, ���� �ٸ� _uint�� ��ȯ

	
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



