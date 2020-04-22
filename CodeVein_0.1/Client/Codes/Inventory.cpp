#include "stdafx.h"
#include "..\Headers\Inventory.h"

#include "QuickSlot.h"
#include "Expendables_Inven.h"
#include "Material_Inven.h"
#include "Weapon_Inven.h"
#include "Armor_Inven.h"
#include "Total_Inven.h"
#include "Inventory_Icon.h"
#include "UI_Manager.h"

CInventory::CInventory(_Device pDevice)
	: CUI(pDevice)
{
}

CInventory::CInventory(const CInventory & rhs)
	: CUI(rhs)
{
}

HRESULT CInventory::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CInventory::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	m_fPosX = WINCX * 0.3f;
	m_fPosY = WINCY * 0.5f;
	
	m_fViewZ = 0.5f;
	m_bIsActive = false;

	SetUp_Default();

	return NOERROR;
}

_int CInventory::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	
	
	if (m_bIsActive && !m_bIsDetail)
	{
		
		m_vecIcon[CInventory_Icon::ICON_EXPEND]->Set_Click(true);
		

		m_bIsDetail = true;
	}

	

	Click_Icon();

	for (auto& pIcon : m_vecIcon)
		pIcon->Set_Active(m_bIsActive);
	
	if (!m_bIsActive && !m_bIsDetail)
	{
		m_pExpInven->Set_Active(false);
		m_pMtrInven->Set_Active(false);
		m_pWeaponInven->Set_Active(false);
		m_pArmorInven->Set_Active(false);	

		for (auto& pIcon : m_vecIcon)
			pIcon->Set_Click(false);
	}


	return NO_EVENT;
}

_int CInventory::Late_Update_GameObject(_double TimeDelta)
{
	

	return NO_EVENT;
}

HRESULT CInventory::Render_GameObject()
{
	

	return NOERROR;
}

HRESULT CInventory::Add_Component()
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

HRESULT CInventory::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, 0)))
		return E_FAIL;

	return NOERROR;
}

void CInventory::SetUp_Default()
{
	m_pQuickSlot = static_cast<CQuickSlot*>(g_pManagement->Get_GameObjectBack(L"Layer_QuickSlot", SCENE_MORTAL));
	m_pExpInven = CUI_Manager::Get_Instance()->Get_Expendables_Inven();
	m_pMtrInven = static_cast<CMaterial_Inven*>(g_pManagement->Get_GameObjectBack(L"Layer_MaterialInven", SCENE_MORTAL));
	m_pWeaponInven = static_cast<CWeapon_Inven*>(g_pManagement->Get_GameObjectBack(L"Layer_WeaponInven", SCENE_MORTAL));
	m_pArmorInven = static_cast<CArmor_Inven*>(g_pManagement->Get_GameObjectBack(L"Layer_ArmorInven", SCENE_MORTAL));
	m_pTotalInven = static_cast<CTotal_Inven*>(g_pManagement->Get_GameObjectBack(L"Layer_TotalInven", SCENE_MORTAL));

	CUI::UI_DESC* pDesc = nullptr;
	LOOP(5)
	{
		pDesc = new CUI::UI_DESC;
		pDesc->fPosX = m_fPosX - 100.f + 50.f * i;
		pDesc->fPosY = m_fPosY - 203.f;
		pDesc->fSizeX = 35.f;
		pDesc->fSizeY = 45.f;
		g_pManagement->Add_GameObject_ToLayer(L"GameObject_InvenIcon", SCENE_MORTAL, L"Layer_InvenIcon", pDesc);
		CInventory_Icon* pIcon = static_cast<CInventory_Icon*>(g_pManagement->Get_GameObjectBack(L"Layer_InvenIcon", SCENE_MORTAL));
		pIcon->Set_Type(CInventory_Icon::ICON_TYPE(i));
		m_vecIcon.push_back(pIcon);
	}
}

void CInventory::Click_Icon()
{
	if (!m_bIsActive)
		return;
	
	if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
	{
		for (auto& pIcon : m_vecIcon)
		{
		
			if (pIcon->Pt_InRect())
			{
				pIcon->Set_Click(true);
				
			}
			else
				pIcon->Set_Click(false);
		}
		
	}

	for (auto& pIcon : m_vecIcon)
	{
		if (pIcon->Get_Click())
		{
			switch (pIcon->Get_Type())
			{
			case CInventory_Icon::ICON_EXPEND:
				m_pExpInven->Set_Active(true);
				m_pMtrInven->Set_Active(false);
				m_pWeaponInven->Set_Active(false);
				m_pArmorInven->Set_Active(false);
				
				break;
			case CInventory_Icon::ICON_MTRL:
				m_pExpInven->Set_Active(false);
				m_pMtrInven->Set_Active(true);
				m_pWeaponInven->Set_Active(false);
				m_pArmorInven->Set_Active(false);
				
				break;
			case CInventory_Icon::ICON_WEAPON:
				m_pExpInven->Set_Active(false);
				m_pMtrInven->Set_Active(false);
				m_pWeaponInven->Set_Active(true);
				m_pArmorInven->Set_Active(false);
				
				break;
			case CInventory_Icon::ICON_ARMOR:
				m_pExpInven->Set_Active(false);
				m_pMtrInven->Set_Active(false);
				m_pWeaponInven->Set_Active(false);
				m_pArmorInven->Set_Active(true);
				
				break;
			case CInventory_Icon::ICON_ALL:
				m_pTotalInven->Set_Active(true);
				m_bIsDetail = false;
				m_bIsActive = false;
				for (auto& pIcon : m_vecIcon)
					pIcon->Set_Click(false);
				break;
			}
		}
	}

	
}

CInventory * CInventory::Create(_Device pGraphic_Device)
{
	CInventory* pInstance = new CInventory(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CInventory::Clone_GameObject(void * pArg)
{
	CInventory* pInstance = new CInventory(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CInventory::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	CUI::Free();
}
