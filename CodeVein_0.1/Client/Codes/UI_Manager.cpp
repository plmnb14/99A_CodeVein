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

	return NOERROR;
}

HRESULT CUI_Manager::SetUp_UILayer()
{

	return NOERROR;
}

_int CUI_Manager::Update_UI()
{
	if (g_pInput_Device->Key_Up(DIK_ESCAPE))
	{
		g_pManagement->Add_GameObject_ToLayer(L"GameObject_Inventory", SCENE_STAGE, L"Layer_Inventory");
	}

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



