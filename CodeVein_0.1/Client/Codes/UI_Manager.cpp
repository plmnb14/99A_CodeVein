#include "stdafx.h"
#include "..\Headers\UI_Manager.h"

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
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);
	// UI
	if (FAILED(pManagement->Add_Prototype(L"GameObject_HPBack", CHPBack::Create(pDevice))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(L"GameObject_PlayerHP", CPlayerHP::Create(pDevice))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(L"GameObject_PlayerST", CPlayerST::Create(pDevice))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(L"GameObject_BossDecoUI", CBossDecoUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(L"GameObject_BossHP", CBossHP::Create(pDevice))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(L"GameObject_ItemSlot", CItemSlot::Create(pDevice))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(L"GameObject_RightArrow", CRightArrow::Create(pDevice))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(L"GameObject_LeftArrow", CLeftArrow::Create(pDevice))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype(L"GameObject_Item", CItem::Create(pDevice))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(L"GameObject_MenuBase", CMenuBaseUI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(L"GameObject_MenuIcon", CMenuIcon::Create(pDevice))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(L"GameObject_CntSlotUI", CSlotCnt_UI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(L"GameObject_ItemQuickSlot", CItem_QuickSlot::Create(pDevice))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(L"GameObject_MenuStatus", CMenu_Status::Create(pDevice))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(L"GameObject_MenuItem", CMenu_Item::Create(pDevice))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(L"GameObject_ActionUI", CActiveSkill_UI::Create(pDevice))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(L"GameObject_MenuSkill", CMenu_Skill::Create(pDevice))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(L"GameObject_SubeQuickSlot", CSubQuickSlot::Create(pDevice))))
		return E_FAIL;

	Safe_Release(pManagement);

	return NOERROR;
}

HRESULT CUI_Manager::SetUp_UILayer()
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;
	Safe_AddRef(pManagement);

	if (FAILED(pManagement->Add_GameObject_ToLayer(L"GameObject_MenuBase", SCENE_STAGE, L"Layer_MenuBase")))
		return E_FAIL;

	if (FAILED(pManagement->Add_GameObject_ToLayer(L"GameObject_MenuStatus", SCENE_STAGE, L"Layer_MenuStatus")))
		return E_FAIL;

	if (FAILED(pManagement->Add_GameObject_ToLayer(L"GameObject_MenuItem", SCENE_STAGE, L"Layer_MenuItem")))
		return E_FAIL;

	// Player HP Back
	CUI::UI_DESC* pHPBackDesc = new CUI::UI_DESC;
	pHPBackDesc->fPosX = 200.f;
	pHPBackDesc->fPosY = 650.f;
	pHPBackDesc->fSizeX = 284.f;
	pHPBackDesc->fSizeY = 32.f;

	if (FAILED(pManagement->Add_GameObject_ToLayer(L"GameObject_HPBack", SCENE_STAGE, L"Layer_HPBack", pHPBackDesc)))
		return E_FAIL;
	
	if (FAILED(pManagement->Add_GameObject_ToLayer(L"GameObject_PlayerHP", SCENE_STAGE, L"Layer_PlayerHP")))
		return E_FAIL;

	if (FAILED(pManagement->Add_GameObject_ToLayer(L"GameObject_PlayerST", SCENE_STAGE, L"Layer_PlayerST")))
		return E_FAIL;

	if (FAILED(pManagement->Add_GameObject_ToLayer(L"GameObject_BossDecoUI", SCENE_STAGE, L"Layer_BossDecoUI")))
		return E_FAIL;

	if (FAILED(pManagement->Add_GameObject_ToLayer(L"GameObject_BossHP", SCENE_STAGE, L"Layer_BossHP")))
		return E_FAIL;

	if (FAILED(pManagement->Add_GameObject_ToLayer(L"GameObject_RightArrow", SCENE_STAGE, L"Layer_RightArrow")))
		return E_FAIL;

	if (FAILED(pManagement->Add_GameObject_ToLayer(L"GameObject_LeftArrow", SCENE_STAGE, L"Layer_LeftArrow")))
		return E_FAIL;
	
	CUI::UI_DESC* pQuickSlotDesc = new CUI::UI_DESC;
	pQuickSlotDesc->fPosX = 300.f;
	pQuickSlotDesc->fPosY = 500.f;
	pQuickSlotDesc->fSizeX = 50.f;
	pQuickSlotDesc->fSizeY = 50.f;

	if (FAILED(pManagement->Add_GameObject_ToLayer(L"GameObject_ItemQuickSlot", SCENE_STAGE, L"Layer_ItemQuickSlot", pQuickSlotDesc)))
		return E_FAIL;

	CUI::UI_DESC* pRightSlotDesc = new CUI::UI_DESC;
	pRightSlotDesc->fPosX = 350.f;
	pRightSlotDesc->fPosY = 500.f;
	pRightSlotDesc->fSizeX = 30.f;
	pRightSlotDesc->fSizeY = 30.f;

	if (FAILED(pManagement->Add_GameObject_ToLayer(L"GameObject_SubeQuickSlot", SCENE_STAGE, L"Layer_RightQuickSlot", pRightSlotDesc)))
		return E_FAIL;

	CUI::UI_DESC* pLeftSlotDesc = new CUI::UI_DESC;
	pLeftSlotDesc->fPosX = 250.f;
	pLeftSlotDesc->fPosY = 500.f;
	pLeftSlotDesc->fSizeX = 30.f;
	pLeftSlotDesc->fSizeY = 30.f;

	if (FAILED(pManagement->Add_GameObject_ToLayer(L"GameObject_SubeQuickSlot", SCENE_STAGE, L"Layer_LeftQuickSlot", pLeftSlotDesc)))
		return E_FAIL;

	

	CUI::UI_DESC* pActDesc = new CUI::UI_DESC;
	pActDesc->fPosX = 1100.f;
	pActDesc->fPosY = 550.f;
	pActDesc->fSizeX = 50.f;
	pActDesc->fSizeY = 50.f;

	if (FAILED(pManagement->Add_GameObject_ToLayer(L"GameObject_ActionUI", SCENE_STAGE, L"Layer_ActionUI", pActDesc)))
		return E_FAIL;

	CUI::UI_DESC* pActDesc2 = new CUI::UI_DESC;
	pActDesc2->fPosX = 1150.f;
	pActDesc2->fPosY = 600.f;
	pActDesc2->fSizeX = 50.f;
	pActDesc2->fSizeY = 50.f;

	if (FAILED(pManagement->Add_GameObject_ToLayer(L"GameObject_ActionUI", SCENE_STAGE, L"Layer_ActionUI2", pActDesc2)))
		return E_FAIL;

	if (FAILED(pManagement->Add_GameObject_ToLayer(L"GameObject_MenuSkill", SCENE_STAGE, L"Layer_MenuSkill")))
		return E_FAIL;
	
	Safe_Release(pManagement);

	m_eMenuState = WIN_NONE;

	return NOERROR;
}

_int CUI_Manager::Update_UI()
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return -1;

	Safe_AddRef(pManagement);

	CMenuBaseUI* pBase = static_cast<CMenuBaseUI*>(pManagement->Get_GameObjectBack(L"Layer_MenuBase", SCENE_STAGE));
	CMenu_Status* pStatus = static_cast<CMenu_Status*>(pManagement->Get_GameObjectBack(L"Layer_MenuStatus", SCENE_STAGE));
	CMenu_Item* pItem = static_cast<CMenu_Item*>(pManagement->Get_GameObjectBack(L"Layer_MenuItem", SCENE_STAGE));

	CItem_QuickSlot* pQuickSlot = static_cast<CItem_QuickSlot*>(pManagement->Get_GameObjectBack(L"Layer_ItemQuickSlot", SCENE_STAGE));
	CSubQuickSlot* pLeftSubSlot = static_cast<CSubQuickSlot*>(pManagement->Get_GameObjectBack(L"Layer_LeftQuickSlot", SCENE_STAGE));
	CSubQuickSlot* pRightSubSlot = static_cast<CSubQuickSlot*>(pManagement->Get_GameObjectBack(L"Layer_RightQuickSlot", SCENE_STAGE));

	CLeftArrow* pLeftArrow = static_cast<CLeftArrow*>(pManagement->Get_GameObjectBack(L"Layer_LeftArrow", SCENE_STAGE));
	CRightArrow* pRightArrow = static_cast<CRightArrow*>(pManagement->Get_GameObjectBack(L"Layer_RightArrow", SCENE_STAGE));
	CActiveSkill_UI* pActiveUI = static_cast<CActiveSkill_UI*>(pManagement->Get_GameObjectBack(L"Layer_ActionUI", SCENE_STAGE));
	CActiveSkill_UI* pActiveUI2 = static_cast<CActiveSkill_UI*>(pManagement->Get_GameObjectBack(L"Layer_ActionUI2", SCENE_STAGE));
	CMenu_Skill* pSkillMenu = static_cast<CMenu_Skill*>(pManagement->Get_GameObjectBack(L"Layer_MenuSkill", SCENE_STAGE));

	pLeftSubSlot->Set_Index(true);
	pRightSubSlot->Set_Index(false);

	if (WIN_BASE == m_eMenuState)
	{
		if (CInput_Device::Get_Instance()->Key_Up(DIK_ESCAPE))
			m_eMenuState = WIN_NONE;
	}
	else if (WIN_BASE != m_eMenuState)
	{
		if (CInput_Device::Get_Instance()->Key_Up(DIK_ESCAPE))
			m_eMenuState = WIN_BASE;
	}

	if (WIN_NONE == m_eMenuState)
	{
		pQuickSlot->Set_OpenUI(true);
		pLeftArrow->Set_OpenUI(true);
		pRightArrow->Set_OpenUI(true);
		pActiveUI->Set_OpenUI(true);
		pActiveUI2->Set_OpenUI(true);
		pLeftSubSlot->Set_OpenUI(true);
		pRightSubSlot->Set_OpenUI(true);
	}
	else
	{
		pQuickSlot->Set_OpenUI(false);
		pLeftArrow->Set_OpenUI(false);
		pRightArrow->Set_OpenUI(false);
		pActiveUI->Set_OpenUI(false);
		pActiveUI2->Set_OpenUI(false);
		pLeftSubSlot->Set_OpenUI(false);
		pRightSubSlot->Set_OpenUI(false);
	}

	switch (m_eMenuState)
	{
	case WIN_NONE:
		pBase->Set_WindowState(false);
		pStatus->Set_WindowState(false);
		pItem->Set_WindowState(false);
		pSkillMenu->Set_WindowState(false);
		break;
	case WIN_BASE:
		pBase->Set_WindowState(true);
		pStatus->Set_WindowState(false);
		pItem->Set_WindowState(false);
		pSkillMenu->Set_WindowState(false);
		break;
	case WIN_STATUS:
		pBase->Set_WindowState(false);
		pStatus->Set_WindowState(true);
		pItem->Set_WindowState(false);
		pSkillMenu->Set_WindowState(false);
		break;
	case WIN_ITEM:
		pBase->Set_WindowState(false);
		pStatus->Set_WindowState(false);
		pItem->Set_WindowState(true);
		pSkillMenu->Set_WindowState(false);
		break;
	case WIN_ACTIVE:
		pBase->Set_WindowState(false);
		pStatus->Set_WindowState(false);
		pItem->Set_WindowState(false);
		pSkillMenu->Set_WindowState(true);
		break;
	}


	if (CInput_Device::Get_Instance()->Key_Up(DIK_NUMPAD1))
		pActiveUI->Set_Skill_Index(pActiveUI->Get_Skill_Index() + 1);
	if (CInput_Device::Get_Instance()->Key_Up(DIK_NUMPAD2))
		pActiveUI2->Set_Skill_Index(pActiveUI2->Get_Skill_Index() + 1);


	
	Safe_Release(pManagement);

	return 0;
}

// 인벤토리에 아이템 추가
void CUI_Manager::Add_Item(CItem::ITEM_TYPE eType)
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return;

	Safe_AddRef(pManagement);

	CMenu_Item* pItem = static_cast<CMenu_Item*>(pManagement->Get_GameObjectBack(L"Layer_MenuItem", SCENE_STAGE));

	pItem->Add_Item(eType, 0);

	Safe_Release(pManagement);
}

// 퀵슬롯 아이템 사용(슬롯 상의 아이템 제거)
void CUI_Manager::Use_Item()
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return;

	Safe_AddRef(pManagement);

	CItem_QuickSlot* pQuickSlot = static_cast<CItem_QuickSlot*>(pManagement->Get_GameObjectBack(L"Layer_ItemQuickSlot", SCENE_STAGE));

	pQuickSlot->Use_Item();

	Safe_Release(pManagement);
}

// 퀵슬롯 아이템 타입 반환
CItem::ITEM_TYPE CUI_Manager::Get_QuickItemType()
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return CItem::ITEM_NONE;

	Safe_AddRef(pManagement);

	CItem_QuickSlot* pQuickSlot = static_cast<CItem_QuickSlot*>(pManagement->Get_GameObjectBack(L"Layer_ItemQuickSlot", SCENE_STAGE));

	CItem::ITEM_TYPE eType = pQuickSlot->Get_ItemType();

	Safe_Release(pManagement);

	return CItem::ITEM_TYPE(eType);
}

void CUI_Manager::Free()
{
}


