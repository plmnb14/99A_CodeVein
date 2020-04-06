#include "stdafx.h"
#include "..\Headers\Inventory.h"

#include "QuickSlot.h"
#include "Expendables_Inven.h"
#include "Material_Inven.h"
#include "Weapon_Inven.h"
#include "Armor_Inven.h"
#include "Total_Inven.h"
#include "Inventory_Icon.h"

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

	//m_pRendererCom->Add_RenderList(RENDER_UI, this);

	//D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.0f);

	if (g_pInput_Device->Key_Up(DIK_ESCAPE))
	{
		m_bIsActive = !m_bIsActive;

		if (m_bIsActive)
		{
			m_pTotalInven->Set_Active(true);
			m_pExpInven->Set_Active(false);
			m_pMtrInven->Set_Active(false);
			m_pWeaponInven->Set_Active(false);
			m_pArmorInven->Set_Active(false);
		}
		else
			for (auto& pIcon : m_vecIcon)
				pIcon->Set_Click(false);
	}
		

	Click_Icon();

	for (auto& pIcon : m_vecIcon)
		pIcon->Set_Active(m_bIsActive);
	m_pDetailIcon->Set_Active(m_bIsActive);
	
	if (!m_bIsActive)
	{
		m_pExpInven->Set_Active(false);
		m_pMtrInven->Set_Active(false);
		m_pWeaponInven->Set_Active(false);
		m_pArmorInven->Set_Active(false);
		m_pTotalInven->Set_Active(false);
	}

	/*switch (m_eType)
	{
	case INVEN_TOTAL:
		m_pTotalInven->Set_Active(true);
		break;
	case INVEN_DETAIL:
		break;
	}*/
	

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
	m_pQuickSlot = static_cast<CQuickSlot*>(g_pManagement->Get_GameObjectBack(L"Layer_QuickSlot", SCENE_STAGE));
	m_pExpInven = static_cast<CExpendables_Inven*>(g_pManagement->Get_GameObjectBack(L"Layer_ExpendablesInven", SCENE_STAGE));
	m_pMtrInven = static_cast<CMaterial_Inven*>(g_pManagement->Get_GameObjectBack(L"Layer_MaterialInven", SCENE_STAGE));
	m_pWeaponInven = static_cast<CWeapon_Inven*>(g_pManagement->Get_GameObjectBack(L"Layer_WeaponInven", SCENE_STAGE));
	m_pArmorInven = static_cast<CArmor_Inven*>(g_pManagement->Get_GameObjectBack(L"Layer_ArmorInven", SCENE_STAGE));
	m_pTotalInven = static_cast<CTotal_Inven*>(g_pManagement->Get_GameObjectBack(L"Layer_TotalInven", SCENE_STAGE));

	CUI::UI_DESC* pDesc = nullptr;
	LOOP(5)
	{
		pDesc = new CUI::UI_DESC;
		pDesc->fPosX = m_fPosX - 100.f + 50.f * i;
		pDesc->fPosY = m_fPosY - 203.f;
		pDesc->fSizeX = 35.f;
		pDesc->fSizeY = 45.f;
		g_pManagement->Add_GameObject_ToLayer(L"GameObject_InvenIcon", SCENE_STAGE, L"Layer_InvenIcon", pDesc);
		CInventory_Icon* pIcon = static_cast<CInventory_Icon*>(g_pManagement->Get_GameObjectBack(L"Layer_InvenIcon", SCENE_STAGE));
		pIcon->Set_Type(CInventory_Icon::ICON_TYPE(i));
		m_vecIcon.push_back(pIcon);
	}
	
	pDesc = new CUI::UI_DESC;
	pDesc->fPosX = m_fPosX - 200.f;
	pDesc->fPosY = m_fPosY - 220.f;
	pDesc->fSizeX = 50.f;
	pDesc->fSizeY = 60.f;
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_InvenIcon", SCENE_STAGE, L"Layer_InvenIcon", pDesc);
	m_pDetailIcon = static_cast<CInventory_Icon*>(g_pManagement->Get_GameObjectBack(L"Layer_InvenIcon", SCENE_STAGE));
	m_pDetailIcon->Set_Type(CInventory_Icon::ICON_ALL);
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
				m_pTotalInven->Set_Active(false);
				break;
			case CInventory_Icon::ICON_MTRL:
				m_pExpInven->Set_Active(false);
				m_pMtrInven->Set_Active(true);
				m_pWeaponInven->Set_Active(false);
				m_pArmorInven->Set_Active(false);
				m_pTotalInven->Set_Active(false);
				break;
			case CInventory_Icon::ICON_WEAPON:
				m_pExpInven->Set_Active(false);
				m_pMtrInven->Set_Active(false);
				m_pWeaponInven->Set_Active(true);
				m_pArmorInven->Set_Active(false);
				m_pTotalInven->Set_Active(false);
				break;
			case CInventory_Icon::ICON_ARMOR:
				m_pExpInven->Set_Active(false);
				m_pMtrInven->Set_Active(false);
				m_pWeaponInven->Set_Active(false);
				m_pArmorInven->Set_Active(true);
				m_pTotalInven->Set_Active(false);
				break;
			case CInventory_Icon::ICON_ALL:
				m_pExpInven->Set_Active(false);
				m_pMtrInven->Set_Active(false);
				m_pWeaponInven->Set_Active(false);
				m_pArmorInven->Set_Active(false);
				m_pTotalInven->Set_Active(true);
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
