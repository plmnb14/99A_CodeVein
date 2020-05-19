#include "stdafx.h"
#include "..\Headers\Inventory.h"
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
	CUI::Ready_GameObject(pArg);

	m_fPosX = 229.5f;
	m_fPosY = 325.5f;
	m_fSizeX = 280.f;
	m_fSizeY = 471.f;
	m_fViewZ = 0.5f;
	m_bIsActive = false;

	SetUp_Default();

	return NOERROR;
}

_int CInventory::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	Click_Icon();

	if (m_bIsActive && !m_bIsSubActive)
	{
		SetUp_SubUI_Active(true);
		m_bIsSubActive = true;
	}
	else if (!m_bIsActive && m_bIsSubActive)
	{
		SetUp_SubUI_Active(false);

		m_pExpInven->Set_Active(false);
		m_pMtrInven->Set_Active(false);
		m_pWeaponInven->Set_Active(false);
		m_pArmorInven->Set_Active(false);

		m_bIsSubActive = false;
	}
	
	/*if (!m_bIsActive)
	{
		m_pExpInven->Set_Active(false);
		m_pMtrInven->Set_Active(false);
		m_pWeaponInven->Set_Active(false);
		m_pArmorInven->Set_Active(false);
	}*/

	return NO_EVENT;
}

void CInventory::SetUp_Default()
{
	m_pUIManager = CUI_Manager::Get_Instance();

	m_pQuickSlot = m_pUIManager->Get_QuickSlot();
	m_pExpInven = m_pUIManager->Get_Expendables_Inven();
	m_pMtrInven = m_pUIManager->Get_Material_Inven();
	m_pWeaponInven = m_pUIManager->Get_Weapon_Inven();
	m_pArmorInven = m_pUIManager->Get_Armor_Inven();
	m_pTotalInven = m_pUIManager->Get_Total_Inven();
	
	CUI::UI_DESC* pDesc = nullptr;
	LOOP(5)
	{
		pDesc = new CUI::UI_DESC;
		pDesc->fPosX = m_fPosX - 100.f + 50.f * i;
		pDesc->fPosY = m_fPosY - 203.f;
		pDesc->fSizeX = 40.f;
		pDesc->fSizeY = 40.f;
		g_pManagement->Add_GameObject_ToLayer(L"GameObject_InvenIcon", SCENE_MORTAL, L"Layer_PlayerUI", pDesc);
		CInventory_Icon* pIcon = static_cast<CInventory_Icon*>(g_pManagement->Get_GameObjectBack(L"Layer_PlayerUI", SCENE_MORTAL));
		pIcon->Set_Type(CInventory_Icon::ICON_TYPE(i));
		m_vecIcon.push_back(pIcon);
	}
}

void CInventory::Click_Icon()
{
	if (!m_bIsActive)
		return;
	
	for (auto& pIcon : m_vecIcon)
	{
		if (pIcon->Pt_InRect() && g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
		{
			switch (pIcon->Get_Type())
			{
			case CInventory_Icon::ICON_EXPEND:
			{
				g_pSoundManager->Play_Sound(L"UI_CommonHover.wav", CSoundManager::Inven_Icon_Expend, CSoundManager::Ambient_Sound);

				m_pExpInven->Set_Active(true);
				m_pMtrInven->Set_Active(false);
				m_pWeaponInven->Set_Active(false);
				m_pArmorInven->Set_Active(false);
			}			
				break;
			case CInventory_Icon::ICON_MTRL:
			{
				g_pSoundManager->Play_Sound(L"UI_CommonHover.wav", CSoundManager::Inven_Icon_Material, CSoundManager::Ambient_Sound);

				m_pExpInven->Set_Active(false);
				m_pMtrInven->Set_Active(true);
				m_pWeaponInven->Set_Active(false);
				m_pArmorInven->Set_Active(false);
			}
				break;
			case CInventory_Icon::ICON_WEAPON:
			{
				g_pSoundManager->Play_Sound(L"UI_CommonHover.wav", CSoundManager::Inven_Icon_Weapon, CSoundManager::Ambient_Sound);

				m_pExpInven->Set_Active(false);
				m_pMtrInven->Set_Active(false);
				m_pWeaponInven->Set_Active(true);
				m_pArmorInven->Set_Active(false);
			}	
				break;
			case CInventory_Icon::ICON_ARMOR:
			{
				g_pSoundManager->Play_Sound(L"UI_CommonHover.wav", CSoundManager::Inven_Icon_Armor, CSoundManager::Ambient_Sound);

				m_pExpInven->Set_Active(false);
				m_pMtrInven->Set_Active(false);
				m_pWeaponInven->Set_Active(false);
				m_pArmorInven->Set_Active(true);
			}	
				break;
			case CInventory_Icon::ICON_ALL:
			{
				g_pSoundManager->Play_Sound(L"UI_CommonHover.wav", CSoundManager::Inven_Icon_All, CSoundManager::Ambient_Sound);

				m_pTotalInven->Set_Active(true);
				m_bIsDetail = false;
				m_bIsActive = false;
			}			
				break;
			}
		}
	}

	
}

void CInventory::SetUp_SubUI_Active(_bool bIsActive)
{
	for (auto& pIcon : m_vecIcon)
		pIcon->Set_Active(bIsActive);
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
	CUI::Free();
}
