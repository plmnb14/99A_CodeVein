#include "stdafx.h"
#include "..\Headers\Total_Inven.h"
#include "Inventory_Icon.h"
#include "Inventory.h"
#include "Weapon_Slot.h"
#include "Weapon_Inven.h"
#include "Armor_Slot.h"
#include "Armor_Inven.h"
#include "BloodCode_Icon.h"
#include "Info_Slot.h"
#include "Expendables_Inven.h"
#include "FontNumManager.h"
#include "QuickSlot.h"
#include "UI_Manager.h"

CTotal_Inven::CTotal_Inven(_Device pDevice)
	: CUI(pDevice)
{
}

CTotal_Inven::CTotal_Inven(const CTotal_Inven & rhs)
	: CUI(rhs)
{
}

HRESULT CTotal_Inven::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CTotal_Inven::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	m_fPosX = WINCX * 0.5f;
	m_fPosY = WINCY * 0.5f;
	m_fSizeX = WINCX;
	m_fSizeY = WINCY;
	m_fViewZ = 5.f;

	m_bIsActive = false;

	SetUp_Default();

	return NOERROR;
}

_int CTotal_Inven::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.0f);

	Click_Icon();
	
	CQuickSlot* pQuickSlot = CUI_Manager::Get_Instance()->Get_QuickSlot();

	if (g_pInput_Device->Key_Up(DIK_ESCAPE))
	{
		m_bIsActive = !m_bIsActive;

		if (m_bIsActive)
		{
			CUI_Manager::Get_Instance()->Get_Instance()->Get_Inventory()->Set_Active(false);
			CUI_Manager::Get_Instance()->Get_Instance()->Get_Inventory()->Set_Detail(false);
			pQuickSlot->Set_Active(false);
		}
		else
			pQuickSlot->Set_Active(true);
	}

	if(m_pIcon)
		m_pIcon->Set_Active(m_bIsActive);
	
	LOOP(2)
	{
		CWeapon_Inven* pWeaponInven = CUI_Manager::Get_Instance()->Get_Weapon_Inven();
		m_pWeapon_Slot[i]->Set_Type(pWeaponInven->Get_UseWeaponState(i));
		m_pWeapon_Slot[i]->Set_Active(m_bIsActive);
	}
	
	CArmor_Inven* pArmorInven = CUI_Manager::Get_Instance()->Get_Armor_Inven();
	m_pArmor_Slot->Set_Type(pArmorInven->Get_UseArmorType());
	m_pArmor_Slot->Set_Active(m_bIsActive);
	
	m_pBloodCode->Set_Active(m_bIsActive);

	LOOP(8)
	{
		m_pQuickSlotInfo[i]->Set_Active(m_bIsActive);
	}
		
	return NO_EVENT;
}

_int CTotal_Inven::Late_Update_GameObject(_double TimeDelta)
{
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);

	m_matWorld._11 = m_fSizeX;
	m_matWorld._22 = m_fSizeY;
	m_matWorld._33 = 1.f;
	m_matWorld._41 = m_fPosX - WINCX * 0.5f;
	m_matWorld._42 = -m_fPosY + WINCY * 0.5f;
	m_matWorld._42 = 1.f;

	return NO_EVENT;
}

HRESULT CTotal_Inven::Render_GameObject()
{
	if (!m_bIsActive)
		return NOERROR;

	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);

	m_matOldView = g_pManagement->Get_Transform(D3DTS_VIEW);
	m_matOldProj = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();

	m_pShaderCom->Begin_Pass(1);


	m_pBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();

	m_pShaderCom->End_Shader();

	g_pManagement->Set_Transform(D3DTS_VIEW, m_matOldView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matOldProj);

	return NOERROR;
}

HRESULT CTotal_Inven::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_MenuWindow", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CTotal_Inven::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, 5)))
		return E_FAIL;

	return NOERROR;
}

void CTotal_Inven::SetUp_Default()
{
	CUI::UI_DESC* pDesc = nullptr;
	
	pDesc = new CUI::UI_DESC;
	pDesc->fPosX = 220.f;
	pDesc->fPosY = 100.f;
	pDesc->fSizeX = 40.f;
	pDesc->fSizeY = 50.f;
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_InvenIcon", SCENE_MORTAL, L"Layer_PlayerUI", pDesc);
	m_pIcon = static_cast<CInventory_Icon*>(g_pManagement->Get_GameObjectBack(L"Layer_PlayerUI", SCENE_MORTAL));
	m_pIcon->Set_Type(CInventory_Icon::ICON_ALL);

	LOOP(3)
	{
		pDesc = new CUI::UI_DESC;
		pDesc->fPosX = 232.f + 59.f * i;
		pDesc->fPosY = 198.f;
		pDesc->fSizeX = 55.f;
		pDesc->fSizeY = 55.f;

		if (2 > i)
		{
			g_pManagement->Add_GameObject_ToLayer(L"GameObject_WeaponSlot", SCENE_MORTAL, L"Layer_PlayerUI", pDesc);
			m_pWeapon_Slot[i] = static_cast<CWeapon_Slot*>(g_pManagement->Get_GameObjectBack(L"Layer_PlayerUI", SCENE_MORTAL));
			m_pWeapon_Slot[i]->Set_Type(WEAPON_None);
		}
		else if (2 == i)
		{
			g_pManagement->Add_GameObject_ToLayer(L"GameObject_ArmorSlot", SCENE_MORTAL, L"Layer_PlayerUI", pDesc);
			m_pArmor_Slot = static_cast<CArmor_Slot*>(g_pManagement->Get_GameObjectBack(L"Layer_PlayerUI", SCENE_MORTAL));
			m_pArmor_Slot->Set_Type(CArmor::ARMOR_END);
		}
	}
	
	pDesc = new CUI::UI_DESC;
	pDesc->fPosX = 375.f;
	pDesc->fPosY = 322.f;
	pDesc->fSizeX = 90.f;
	pDesc->fSizeY = 67.f;
	pDesc->iIndex = 8;
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_BloodCodeIcon", SCENE_MORTAL, L"Layer_PlayerUI", pDesc);
	m_pBloodCode = static_cast<CBloodCode_Icon*>(g_pManagement->Get_GameObjectBack(L"Layer_PlayerUI", SCENE_MORTAL));

	LOOP(8)
	{
		pDesc = new CUI::UI_DESC;
		pDesc->fPosX = 205.f + 42.f * i;
		pDesc->fPosY = 494.f;
		pDesc->fSizeX = 40.f;
		pDesc->fSizeY = 40.f;
		pDesc->iIndex = 0;
		g_pManagement->Add_GameObject_ToLayer(L"GameObject_InfoSlot", SCENE_MORTAL, L"Layer_PlayerUI", pDesc);
		m_pQuickSlotInfo[i] = static_cast<CInfo_Slot*>(g_pManagement->Get_GameObjectBack(L"Layer_PlayerUI", SCENE_MORTAL));
	}

	LOOP(8)
	{
		pDesc = new CUI::UI_DESC;
		pDesc->fPosX = m_pQuickSlotInfo[i]->Get_UI_Pos().x - m_pQuickSlotInfo[i]->Get_UI_Size().x * 0.25f;
		pDesc->fPosY = m_pQuickSlotInfo[i]->Get_UI_Pos().y + m_pQuickSlotInfo[i]->Get_UI_Size().y * 0.25f;
		pDesc->fSizeX = m_pQuickSlotInfo[i]->Get_UI_Size().x * 0.25f;
		pDesc->fSizeY = m_pQuickSlotInfo[i]->Get_UI_Size().y * 0.25f;
		pDesc->iIndex = 0;
		g_pManagement->Add_GameObject_ToLayer(L"GameObject_NumberUI", SCENE_MORTAL, L"Layer_PlayerUI", pDesc);
		m_pNumberUI[i] = static_cast<CNumberUI*>(g_pManagement->Get_GameObjectBack(L"Layer_PlayerUI", SCENE_MORTAL));
	}

	
}

void CTotal_Inven::Click_Icon()
{
	if (!m_bIsActive)
		return;

	if (m_pIcon->Pt_InRect() &&
		g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
	{
		m_pInventory = CUI_Manager::Get_Instance()->Get_Inventory();
		m_pInventory->Set_Active(true);
		m_bIsActive = false;
	}
	
	vector<CExpendables_Slot*> vecQuickSlot = * CUI_Manager::Get_Instance()->Get_Expendables_Inven()->Get_QuickSlot();
	
	for (_uint i = 0; i < 8; ++i)
	{
		if (i < vecQuickSlot.size())
		{
			m_pQuickSlotInfo[i]->Set_SlotInfo(CExpendables::EXPEND_TYPE(vecQuickSlot[i]->Get_Type()));
			m_pQuickSlotInfo[i]->Set_Number(vecQuickSlot[i]->Get_Size());	
		}			
		else if (i < 8)
		{
			m_pQuickSlotInfo[i]->Set_SlotInfo(CExpendables::EXPEND_END);
			m_pQuickSlotInfo[i]->Set_Number(0);
		}
			
	}
}

CTotal_Inven * CTotal_Inven::Create(_Device pGraphic_Device)
{
	CTotal_Inven* pInstance = new CTotal_Inven(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CTotal_Inven::Clone_GameObject(void * pArg)
{
	CTotal_Inven* pInstance = new CTotal_Inven(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CTotal_Inven::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	CUI::Free();
}
