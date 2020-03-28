#include "stdafx.h"
#include "..\Headers\UI_Manager.h"
#include "Button_UI.h"
#include "HPBack.h"
#include "PlayerHP.h"
#include "PlayerST.h"
#include "BossDecoUI.h"
#include "BossHP.h"
#include "Item_Slot.h"

#include "Item.h"
#include "QuickSlot.h"
/////////////////
#include "Inventory.h"
#include "Inven_Icon.h"
#include "ItemIcon.h"

#include "Inven_Status.h"
#include "Inven_Item.h"
#include "Inven_Skill.h"
//////////////////
#include "Tab_Consume.h"

/////////////////
#include "MiniMap.h"

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
	////////////////////////////////////////////////////////////////////////////////////////////

	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Inventory", CInventory::Create(pDevice))))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_InvenIcon", CInven_Icon::Create(pDevice))))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_InvenStatus", CInven_Status::Create(pDevice))))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_InvenItem", CInven_Item::Create(pDevice))))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_InvenSkill", CInven_Skill::Create(pDevice))))
		return E_FAIL;
	//////////////////////////////////////////////////////////
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_MiniMap", CMiniMap::Create(pDevice))))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_ItemIcon", CItemIcon::Create(pDevice))))
		return E_FAIL;
	//////////////////////////////////////////////////////////
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_TabConsume", CTab_Consume::Create(pDevice))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_ItemSlot", CItem_Slot::Create(pDevice))))
		return E_FAIL;
	return NOERROR;
}

HRESULT CUI_Manager::SetUp_UILayer()
{
	/*g_pManagement->Add_GameObject_ToLayer(L"GameObject_TabConsume", SCENE_STAGE, L"Layer_TabConsume");

	CUI::UI_DESC* pDesc = nullptr;
	pDesc = new CUI::UI_DESC;

	pDesc->fPosX = 100.f;
	pDesc->fPosY = 600.f;
	pDesc->fSizeX = 100.f;
	pDesc->fSizeY = 100.f;
	pDesc->iIndex = 0;
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_QuickSlot", SCENE_STAGE, L"Layer_QuickSlot", pDesc);*/
	//g_pManagement->Add_GameObject_ToLayer(L"GameObject_Inventory", SCENE_STAGE, L"Layer_Inventory");
	return NOERROR;
}

_int CUI_Manager::Update_UI()
{
	/*if (g_pInput_Device->Get_DIKeyState(DIK_ESCAPE))
		Open_Inventory();*/
	return 0;
}

void CUI_Manager::Open_Inventory()
{
	m_eInvenState = INVEN_BASE;

	g_pManagement->Add_GameObject_ToLayer(L"GameObject_Inventory", SCENE_STAGE, L"Layer_Inventory");
}


void CUI_Manager::Open_Status()
{

	g_pManagement->Add_GameObject_ToLayer(L"GameObject_InvenStatus", SCENE_STAGE, L"Layer_InvenStatus");
}

void CUI_Manager::Open_Item()
{
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_InvenItem", SCENE_STAGE, L"Layer_InvenItem");
}

void CUI_Manager::Open_Skill()
{
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_InvenSkill", SCENE_STAGE, L"Layer_InvenSkill");
}



void CUI_Manager::Free()
{
}



