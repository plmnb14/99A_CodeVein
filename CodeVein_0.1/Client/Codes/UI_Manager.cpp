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

HRESULT CUI_Manager::SetUp_UILayer()
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;
	Safe_AddRef(pManagement);

	if (FAILED(pManagement->Add_GameObject_ToLayer(L"GameObject_MenuBase", SCENE_STAGE, L"Layer_MenuBase")))
		return E_FAIL;

	if (FAILED(pManagement->Add_GameObject_ToLayer(L"GameObject_HPBack", SCENE_STAGE, L"Layer_HPBack")))
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

	//if (FAILED(pManagement->Add_GameObject_ToLayer(L"GameObject_ItemPalette", SCENE_STAGE, L"Layer_ItemPalette")))
	//	return E_FAIL;

	
	if (FAILED(pManagement->Add_GameObject_ToLayer(L"GameObject_ItemSlot", SCENE_STAGE, L"Layer_ItemSlot")))
		return E_FAIL;

	if (FAILED(pManagement->Add_GameObject_ToLayer(L"GameObject_CntSlotUI", SCENE_STAGE, L"Layer_SlotCnt")))
		return E_FAIL;
	
	Safe_Release(pManagement);

	return NOERROR;
}

void CUI_Manager::Free()
{
}


