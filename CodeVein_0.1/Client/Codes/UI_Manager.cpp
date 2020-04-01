#include "stdafx.h"
#include "..\Headers\UI_Manager.h"

#include "Button_UI.h"
#include "HPBack.h"
#include "PlayerHP.h"
#include "PlayerST.h"
#include "BossDecoUI.h"
#include "BossHP.h"

#include "QuickSlot.h"
#include "Inven_Icon.h"
#include "ItemIcon.h"

/////////////////
#include "Inventory.h"
#include "Inven_Icon.h"
#include "ItemIcon.h"


//////////////////
#include "Expendables_Inven.h"
#include "Expendables_Slot.h"
#include "Select_UI.h"
#include "Material_Inven.h"
#include "Material_Slot.h"
#include "Weapon_Slot.h"
#include "Weapon_Inven.h"
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

	

	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_InvenIcon", CInven_Icon::Create(pDevice))))
		return E_FAIL;

	//////////////////////////////////////////////////////////
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_MiniMap", CMiniMap::Create(pDevice))))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_ItemIcon", CItemIcon::Create(pDevice))))
		return E_FAIL;
	//////////////////////////////////////////////////////////
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

	return NOERROR;
}

HRESULT CUI_Manager::SetUp_UILayer()
{
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_QuickSlot", SCENE_STAGE, L"Layer_QuickSlot");
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_ExpendablesInven", SCENE_STAGE, L"Layer_ExpendablesInven");
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_MaterialInven", SCENE_STAGE, L"Layer_MaterialInven");
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_WeaponInven", SCENE_STAGE, L"Layer_WeaponInven");
	return NOERROR;
}

_int CUI_Manager::Update_UI()
{
	/*if (g_pInput_Device->Key_Up(DIK_1))
	{
		m_bIsOpenEx = !m_bIsOpenEx;

		if (m_bIsOpenEx)
		{
			g_pManagement->Add_GameObject_ToLayer(L"GameObject_ExpendablesInven", SCENE_STAGE, L"Layer_ExpendablesInven");
		}
		else
		{
			static_cast<CExpendables_Inven*>(g_pManagement->Get_GameObjectBack(L"Layer_ExpendablesInven", SCENE_STAGE))->Set_Dead();
		}
	}
	if (g_pInput_Device->Key_Up(DIK_2))
	{
		m_bIsOpenMat = !m_bIsOpenMat;

		if (m_bIsOpenMat)
		{
			g_pManagement->Add_GameObject_ToLayer(L"GameObject_MaterialInven", SCENE_STAGE, L"Layer_MaterialInven");
		}
		else
		{
			static_cast<CMaterial_Inven*>(g_pManagement->Get_GameObjectBack(L"Layer_MaterialInven", SCENE_STAGE))->Set_Dead();
		}
	}
	if (g_pInput_Device->Key_Up(DIK_3))
	{
		m_bIsOpenWea = !m_bIsOpenWea;

		if (m_bIsOpenWea)
		{
			g_pManagement->Add_GameObject_ToLayer(L"GameObject_WeaponInven", SCENE_STAGE, L"Layer_WeaponInven");
		}
		else
		{
			static_cast<CWeapon_Inven*>(g_pManagement->Get_GameObjectBack(L"Layer_WeaponInven", SCENE_STAGE))->Set_Dead();
		}
	}
*/
	
	return 0;
}

void CUI_Manager::Free()
{
}



