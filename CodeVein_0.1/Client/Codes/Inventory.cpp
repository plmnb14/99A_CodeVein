#include "stdafx.h"
#include "..\Headers\Inventory.h"

#include "QuickSlot.h"
#include "Expendables_Inven.h"
#include "Material_Inven.h"
#include "Weapon_Inven.h"
#include "Armor_Inven.h"
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

	m_fPosX = WINCX * 0.5f;
	m_fPosY = WINCY * 0.5f;
	m_fSizeX = 700.f;
	m_fSizeY = WINCY;
	m_fViewZ = 0.5f;
	m_bIsActive = false;

	SetUp_Default();

	return NOERROR;
}

_int CInventory::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.0f);

	if (g_pInput_Device->Key_Up(DIK_ESCAPE))
		m_bIsActive = !m_bIsActive;

	Click_Icon();

	for (auto& pIcon : m_vecIcon)
	{
		pIcon->Set_Active(m_bIsActive);
	}
	
	if (!m_bIsActive)
	{
		m_pExpInven->Set_Active(false);
		m_pMtrInven->Set_Active(false);
		m_pWeaponInven->Set_Active(false);
		m_pArmorInven->Set_Active(false);
	}

	return NO_EVENT;
}

_int CInventory::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CInventory::Render_GameObject()
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

	CUI::UI_DESC* pDesc = nullptr;
	LOOP(4)
	{
		pDesc = new CUI::UI_DESC;
		pDesc->fPosX = m_fPosX - 75.f + 50.f * i;
		pDesc->fPosY = m_fPosY - 203.f;
		pDesc->fSizeX = 35.f;
		pDesc->fSizeY = 45.f;
		g_pManagement->Add_GameObject_ToLayer(L"GameObject_InvenIcon", SCENE_STAGE, L"Layer_InvenIcon", pDesc);
		CInventory_Icon* pIcon = static_cast<CInventory_Icon*>(g_pManagement->Get_GameObjectBack(L"Layer_InvenIcon", SCENE_STAGE));
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
