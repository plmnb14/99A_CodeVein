#include "stdafx.h"
#include "..\Headers\Weapon_Inven_InShop.h"

#include "UI_Manager.h"
#include "Weapon.h"

CWeapon_Inven_InShop::CWeapon_Inven_InShop(_Device pDevice)
	: CUI(pDevice)
{
}

CWeapon_Inven_InShop::CWeapon_Inven_InShop(const CWeapon_Inven_InShop & rhs)
	: CUI(rhs)
{
}

_bool CWeapon_Inven_InShop::Get_PopupOn()
{
	if (
		m_pWeaponBuyPopup->Get_Active() ||
		m_pWeaponSellPopup->Get_Active() ||
		m_pArmorBuyPopup->Get_Active() ||
		m_pArmorSellPopup->Get_Active() ||
		m_pWeaponUpgradePopup->Get_Active()
		)
		return true;
	else
		return false;
}

void CWeapon_Inven_InShop::Setup_InvenType(INVEN_SHOP_OPTION eOption)
{
	m_eOption = eOption;

	if (FAILED(SetUp_WeaponData(m_eOption)))
		return;

	switch (m_eOption)
	{
	case Client::CWeapon_Inven_InShop::SHOP_WEAPON_BUY:
	{
		for (_int i = 0; i < WPN_DATA_End; i++)
		{
			Add_Weapon(m_tWeaponParam[i]);;
		}
		break;
	}
	case Client::CWeapon_Inven_InShop::SHOP_ARMOR_BUY:
	{
		for (_int i = 0; i < ArmorAll_END; i++)
		{
			Add_Armor(m_tArmorParam[i]);;
		}
		break;
	}
	case Client::CWeapon_Inven_InShop::SHOP_WEAPON_SELL:
	case Client::CWeapon_Inven_InShop::SHOP_WEAPON_UPGRADE:
	{
		//CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Set_WeaponData_FromWeapon();
		vector<CWeapon_Slot*>* pVecMyWeapon = CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Get_VecWeaponSlot();
		for (_int i = 0; i < pVecMyWeapon->size(); i++)
		{
			Add_Weapon((*pVecMyWeapon)[i]->Get_WeaponParam());
		}
		break;
	}
	case Client::CWeapon_Inven_InShop::SHOP_ARMOR_SELL:
	case Client::CWeapon_Inven_InShop::SHOP_ARMOR_UPGRADE:
	{
		vector<CArmor_Slot*>* pVecMyArmor = CUI_Manager::Get_Instance()->Get_Armor_Inven()->Get_VecArmorSlot();
		for (_int i = 0; i < pVecMyArmor->size(); i++)
		{
			Add_Armor((*pVecMyArmor)[i]->Get_ArmorParam());
		}
		break;
	}
	}
}

void CWeapon_Inven_InShop::Refresh_Inven()
{

	for (auto& iter : m_vecWeaponSlot)
	{
		iter->Set_Dead();
		//m_vecWeaponSlot.erase(iter);
	}
	m_vecWeaponSlot.clear();

	for (auto& iter : m_vecArmorSlot)
		iter->Set_Dead();
	m_vecArmorSlot.clear();

	Setup_InvenType(m_eOption);

	m_pSelectedSlot_Weapon = nullptr;
	m_pSelectedSlot_Armor = nullptr;
}

HRESULT CWeapon_Inven_InShop::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CWeapon_Inven_InShop::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	m_fPosX = 229.5f;
	m_fPosY = 325.5f;
	m_fSizeX = 290.f;
	m_fSizeY = 471.f;
	m_fViewZ = 4.f;

	m_bIsActive = false;

	m_pWeaponInventory = CUI_Manager::Get_Instance()->Get_Weapon_Inven();
	m_pArmorInventory = CUI_Manager::Get_Instance()->Get_Armor_Inven();

	m_pWeaponBuyPopup = static_cast<CWeaponBuyPopupUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon_BuyPopup", nullptr));
	m_pWeaponBuyPopup->Set_Inven(this);
	m_pWeaponBuyPopup->Set_Type(CWeaponBuyPopupUI::POPUP_WEAPON_BUY);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pWeaponBuyPopup, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pWeaponSellPopup = static_cast<CWeaponBuyPopupUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon_BuyPopup", nullptr));
	m_pWeaponSellPopup->Set_Inven(this);
	m_pWeaponSellPopup->Set_Type(CWeaponBuyPopupUI::POPUP_WEAPON_SELL);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pWeaponSellPopup, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pArmorBuyPopup = static_cast<CWeaponBuyPopupUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon_BuyPopup", nullptr));
	m_pArmorBuyPopup->Set_Inven(this);
	m_pArmorBuyPopup->Set_Type(CWeaponBuyPopupUI::POPUP_ARMOR_BUY);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pArmorBuyPopup, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pArmorSellPopup = static_cast<CWeaponBuyPopupUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon_BuyPopup", nullptr));
	m_pArmorSellPopup->Set_Inven(this);
	m_pArmorSellPopup->Set_Type(CWeaponBuyPopupUI::POPUP_ARMOR_SELL);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pArmorSellPopup, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pWeaponUpgradePopup = static_cast<CWeaponUpgradeUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon_Upgrade", nullptr));
	m_pWeaponUpgradePopup->Set_Inven(this);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pWeaponUpgradePopup, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	return NOERROR;
}

_int CWeapon_Inven_InShop::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.0f);

	Click_WeaponInven();
	Click_ArmorInven();

	_uint iIdx = 0;
	
	for (auto& pWeaponSlot : m_vecWeaponSlot)
	{
		if (pWeaponSlot->Get_Dead())
			continue;

		pWeaponSlot->Set_Active(m_bIsActive);
		pWeaponSlot->Set_ViewZ(m_fViewZ - 0.1f);
		pWeaponSlot->Set_UI_Pos(m_fPosX - 100.f + 52.f * (iIdx % 5), m_fPosY - 150.f + 52.f * (iIdx / 5));
		iIdx++;
	}
	for (auto& pArmorSlot : m_vecArmorSlot)
	{
		if (pArmorSlot->Get_Dead())
			continue;

		pArmorSlot->Set_Active(m_bIsActive);
		pArmorSlot->Set_ViewZ(m_fViewZ - 0.1f);
		pArmorSlot->Set_UI_Pos(m_fPosX - 100.f + 52.f * (iIdx % 5), m_fPosY - 150.f + 52.f * (iIdx / 5));
		iIdx++;
	}

	if (m_pWeaponBuyPopup->Get_Active())
	{
		if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_RB))
		{
			m_pWeaponBuyPopup->Set_Active(false);
		}
	}

	if (m_pWeaponSellPopup->Get_Active())
	{
		if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_RB))
		{
			m_pWeaponSellPopup->Set_Active(false);
		}
	}

	if (m_pArmorBuyPopup->Get_Active())
	{
		if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_RB))
		{
			m_pArmorBuyPopup->Set_Active(false);
		}
	}

	if (m_pArmorSellPopup->Get_Active())
	{
		if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_RB))
		{
			m_pArmorSellPopup->Set_Active(false);
		}
	}

	if (m_pWeaponUpgradePopup->Get_Active())
	{
		if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_RB))
		{
			m_pWeaponUpgradePopup->Set_Active(false);
			m_bCloseUpgradePopup = true;
			Set_Active(true);
		}
	}
	
	return NO_EVENT;
}

_int CWeapon_Inven_InShop::Late_Update_GameObject(_double TimeDelta)
{
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);

	m_matWorld._11 = m_fSizeX;
	m_matWorld._22 = m_fSizeY;
	m_matWorld._33 = 1.f;
	m_matWorld._41 = m_fPosX - WINCX * 0.5f;
	m_matWorld._42 = -m_fPosY + WINCY * 0.5f;

	return NO_EVENT;
}

HRESULT CWeapon_Inven_InShop::Render_GameObject()
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

HRESULT CWeapon_Inven_InShop::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_Weapon_Buy_UI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CWeapon_Inven_InShop::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	_float fYPosOffset = 195.f;
	m_matWorld._11 = m_fSizeX * 2.1f;
	m_matWorld._22 = m_fSizeY * 0.325f;
	m_matWorld._33 = 1.f;
	m_matWorld._41 = m_fPosX - WINCX * 0.5f;
	m_matWorld._42 = -m_fPosY + WINCY * 0.5f + fYPosOffset;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;

	_int iTexIdx = 15;
	switch (m_eOption)
	{
	case Client::CWeapon_Inven_InShop::SHOP_WEAPON_BUY:
		iTexIdx = 15;
		break;
	case Client::CWeapon_Inven_InShop::SHOP_WEAPON_SELL:
		iTexIdx = 16;
		break;
	case Client::CWeapon_Inven_InShop::SHOP_ARMOR_BUY:
		iTexIdx = 17;
		break;
	case Client::CWeapon_Inven_InShop::SHOP_ARMOR_SELL:
		iTexIdx = 18;
		break;
	case Client::CWeapon_Inven_InShop::SHOP_WEAPON_UPGRADE:
		iTexIdx = 19;
		break;
	case Client::CWeapon_Inven_InShop::SHOP_ARMOR_UPGRADE:
		iTexIdx = 20;
		break;
	}

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, iTexIdx)))
		return E_FAIL;

	return NOERROR;
}

void CWeapon_Inven_InShop::Click_ArmorInven()
{
	if (!m_bIsActive)
		return;

	_int iIdx = 0;
	for (auto& pSlot : m_vecArmorSlot)
	{
		if (pSlot->Get_Dead())
			continue;

		if (pSlot->Pt_InRect())
		{
			if (m_pHoverSlot_Armor != pSlot)
			{
				g_pSoundManager->Stop_Sound(CSoundManager::UI_SFX_01);
				g_pSoundManager->Play_Sound(L"UI_CommonHover.wav", CSoundManager::UI_SFX_01, CSoundManager::Effect_Sound);
			}

			m_pHoverSlot_Armor = pSlot;
			
			if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB) &&
				!pSlot->Get_Select())
			{
				g_pSoundManager->Stop_Sound(CSoundManager::UI_SFX_01);
				g_pSoundManager->Play_Sound(L"UI_CommonClick.wav", CSoundManager::UI_SFX_01, CSoundManager::Effect_Sound);

				//Refresh_Inven();

				m_pSelectedSlot_Armor = pSlot;
				if (SHOP_ARMOR_SELL == m_eOption ||
					SHOP_ARMOR_UPGRADE == m_eOption)
				{
					vector<CArmor_Slot*>* pArmorSlot = m_pArmorInventory->Get_VecArmorSlot();
					(*pArmorSlot)[iIdx]->Set_SelectShop(true);
					pSlot->Set_SelectShop(true);
				}

				switch (m_eOption)
				{
				case Client::CWeapon_Inven_InShop::SHOP_WEAPON_BUY:
				{
					m_pWeaponBuyPopup->Set_Active(true);
					return;
				}
				case Client::CWeapon_Inven_InShop::SHOP_WEAPON_SELL:
				{
					m_pWeaponSellPopup->Set_Active(true);
					return;
				}
				case Client::CWeapon_Inven_InShop::SHOP_ARMOR_BUY:
				{
					m_pArmorBuyPopup->Set_Active(true);
					return;
				}
				case Client::CWeapon_Inven_InShop::SHOP_ARMOR_SELL:
				{
					m_pArmorSellPopup->Set_Active(true);
					return;
				}
				case  Client::CWeapon_Inven_InShop::SHOP_WEAPON_UPGRADE:
				{
					m_pWeaponUpgradePopup->Set_Active(true);
					m_pWeaponUpgradePopup->Set_UpgradeType(CWeaponUpgradeUI::UPGRADE_WEAPON);
					Set_Active(false);
					return;
				}
				case  Client::CWeapon_Inven_InShop::SHOP_ARMOR_UPGRADE:
				{
					m_pWeaponUpgradePopup->Set_Active(true);
					m_pWeaponUpgradePopup->Set_UpgradeType(CWeaponUpgradeUI::UPGRADE_ARMOR);
					Set_Active(false);
					return;
				}
				}
			}
			return;
		}
		iIdx++;
	}

	m_pHoverSlot_Armor = nullptr;
	//m_pSelectedSlot_Weapon = nullptr;
}

void CWeapon_Inven_InShop::Click_WeaponInven()
{
	if (!m_bIsActive)
		return;

	_int iIdx = 0;
	for (auto& pSlot : m_vecWeaponSlot)
	{
		if (pSlot->Get_Dead())
			continue;

		if (pSlot->Pt_InRect())
		{
			if (m_pHoverSlot_Weapon != pSlot)
			{
				g_pSoundManager->Stop_Sound(CSoundManager::UI_SFX_01);
				g_pSoundManager->Play_Sound(L"UI_CommonHover.wav", CSoundManager::UI_SFX_01, CSoundManager::Effect_Sound);
			}

			m_pHoverSlot_Weapon = pSlot;
			
			if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB) &&
				!pSlot->Get_Select())
			{
				g_pSoundManager->Stop_Sound(CSoundManager::UI_SFX_01);
				g_pSoundManager->Play_Sound(L"UI_CommonClick.wav", CSoundManager::UI_SFX_01, CSoundManager::Effect_Sound);

				//Refresh_Inven();

				m_pSelectedSlot_Weapon = pSlot;
				if (SHOP_WEAPON_SELL == m_eOption ||
					SHOP_WEAPON_UPGRADE == m_eOption)
				{
					vector<CWeapon_Slot*>* pWeaponSlot = m_pWeaponInventory->Get_VecWeaponSlot();
					(*pWeaponSlot)[iIdx]->Set_SelectShop(true);
					pSlot->Set_SelectShop(true);
				}

				switch (m_eOption)
				{
				case Client::CWeapon_Inven_InShop::SHOP_WEAPON_BUY:
				{
					m_pWeaponBuyPopup->Set_Active(true);
					return;
				}
				case Client::CWeapon_Inven_InShop::SHOP_WEAPON_SELL:
				{
					m_pWeaponSellPopup->Set_Active(true);
					return;
				}
				case Client::CWeapon_Inven_InShop::SHOP_ARMOR_BUY:
				{
					m_pArmorBuyPopup->Set_Active(true);
					return;
				}
				case Client::CWeapon_Inven_InShop::SHOP_ARMOR_SELL:
				{
					m_pArmorSellPopup->Set_Active(true);
					return;
				}
				case  Client::CWeapon_Inven_InShop::SHOP_WEAPON_UPGRADE:
				{
					m_pWeaponUpgradePopup->Set_Active(true);
					m_pWeaponUpgradePopup->Set_UpgradeType(CWeaponUpgradeUI::UPGRADE_WEAPON);
					Set_Active(false);
					return;
				}
				case  Client::CWeapon_Inven_InShop::SHOP_ARMOR_UPGRADE:
				{
					m_pWeaponUpgradePopup->Set_Active(true);
					m_pWeaponUpgradePopup->Set_UpgradeType(CWeaponUpgradeUI::UPGRADE_ARMOR);
					Set_Active(false);
					return;
				}
				}
			}
			return;
		}
		iIdx++;
	}

	m_pHoverSlot_Weapon = nullptr;
}

void CWeapon_Inven_InShop::Buy_Weapon()
{
	m_pWeaponInventory->Add_Weapon(m_pSelectedSlot_Weapon->Get_WeaponParam());
}

void CWeapon_Inven_InShop::Buy_Armor()
{
	m_pArmorInventory->Add_Armor(m_pSelectedSlot_Armor->Get_ArmorParam());
}

void CWeapon_Inven_InShop::Sell_Weapon()
{
	m_pWeaponInventory->Sell_Weapon();

	_ulong idx = 0;
	for (auto& pSlot : m_vecWeaponSlot)
	{
		if (pSlot->Get_Dead())
			continue;

		if (pSlot->Get_SelectShop())
		{
			pSlot->Set_Dead();
			m_vecWeaponSlot.erase(m_vecWeaponSlot.begin() + idx);
			m_vecWeaponSlot.shrink_to_fit();
			Refresh_Inven();
			break;
		}
		++idx;
	}
}

void CWeapon_Inven_InShop::Sell_Armor()
{
	m_pArmorInventory->Sell_Armor();

	_ulong idx = 0;
	for (auto& pSlot : m_vecWeaponSlot)
	{
		if (pSlot->Get_Dead())
			continue;

		if (pSlot->Get_SelectShop())
		{
			pSlot->Set_Dead();
			m_vecWeaponSlot.erase(m_vecWeaponSlot.begin() + idx);
			m_vecWeaponSlot.shrink_to_fit();
			Refresh_Inven();
			break;
		}
		++idx;
	}
}

void CWeapon_Inven_InShop::Upgrade_Weapon(WPN_PARAM tParam)
{
	_ulong idx = 0;

	vector<CWeapon_Slot*>* pVecSlot = m_pWeaponInventory->Get_VecWeaponSlot();
	for (auto& pSlot : m_vecWeaponSlot)
	{
		if (pSlot->Get_SelectShop())
		{
			pSlot->Set_WeaponParam(tParam);

			if (idx >= pVecSlot->size())
				return;

			(*pVecSlot)[idx]->Set_WeaponParam(tParam);
			break;
		}
		++idx;
	}
	//Refresh_Inven();
}

void CWeapon_Inven_InShop::Upgrade_Armor(ARMOR_PARAM tParam)
{
	_ulong idx = 0;

	vector<CArmor_Slot*>* pVecSlot = m_pArmorInventory->Get_VecArmorSlot();
	for (auto& pSlot : m_vecArmorSlot)
	{
		if (pSlot->Get_SelectShop())
		{
			pSlot->Set_ArmorParam(tParam);

			if (idx >= pVecSlot->size())
				return;

			(*pVecSlot)[idx]->Set_ArmorParam(tParam);
			break;
		}
		++idx;
	}
	//Refresh_Inven();
}

_float CWeapon_Inven_InShop::Get_UpgradePrice(_int iReinforce)
{
	return (iReinforce * 1.5f) * 80 + 50;
}

_float CWeapon_Inven_InShop::Get_PlusDamage(_float fDamage, _int iReinforce)
{
	return fDamage + (fDamage * 0.3f * iReinforce);
}

_float CWeapon_Inven_InShop::Get_PlusDef(_float fDef, _int iReinforce)
{
	return fDef + (fDef * 0.1 * iReinforce);
}

HRESULT CWeapon_Inven_InShop::SetUp_WeaponData(INVEN_SHOP_OPTION eShop)
{
	switch (eShop)
	{
	case Client::CWeapon_Inven_InShop::SHOP_WEAPON_BUY:
	{
		m_pTempWeapon = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));
		if (!m_pTempWeapon)
			return E_FAIL;

		for (_int i = 0; i < WEAPON_DATA::WPN_DATA_End; i++)
			m_tWeaponParam[i] = m_pTempWeapon->Get_WeaponParam((WEAPON_DATA)i);

		break;
	}
	case Client::CWeapon_Inven_InShop::SHOP_WEAPON_SELL:
		break;
	case Client::CWeapon_Inven_InShop::SHOP_ARMOR_BUY:
	{
		// 
		ARMOR_PARAM tParam;

		tParam.iArmorType = ARMOR_Drape;
		tParam.iArmorName = ArmorAll_Drape_DarkNightHook;
		tParam.iReinforce = 0;
		tParam.fDef = 100;
		tParam.fPlusDef = 30;
		tParam.fHP = 930;
		tParam.fPlusHP = 60;
		tParam.iPrice = 700;
		m_tArmorParam[tParam.iArmorName] = tParam;

		tParam.iArmorType = ARMOR_Gauntlet;
		tParam.iArmorName = ArmorAll_Gauntlet_DarkNightHook;
		tParam.iReinforce = 0;
		tParam.fDef = 120;
		tParam.fPlusDef = 10;
		tParam.fHP = 1000;
		tParam.fPlusHP = 100;
		tParam.iPrice = 1200;
		m_tArmorParam[tParam.iArmorName] = tParam;

		tParam.iArmorType = ARMOR_Gauntlet;
		tParam.iArmorName = ArmorAll_Gauntlet_MangSikHook;
		tParam.iReinforce = 0;
		tParam.fDef = 105;
		tParam.fPlusDef = 30;
		tParam.fHP = 1095;
		tParam.fPlusHP = 60;
		tParam.iPrice = 1200;
		m_tArmorParam[tParam.iArmorName] = tParam;

		tParam.iArmorType = ARMOR_LongCoat;
		tParam.iArmorName = ArmorAll_LongCoat_DarkNightSpear;
		tParam.iReinforce = 0;
		tParam.fDef = 110;
		tParam.fPlusDef = 30;
		tParam.fHP = 1400;
		tParam.fPlusHP = 160;
		tParam.iPrice = 1500;
		m_tArmorParam[tParam.iArmorName] = tParam;

		tParam.iArmorType = ARMOR_LongCoat;
		tParam.iArmorName = ArmorAll_LongCoat_WhiteSilver;
		tParam.iReinforce = 0;
		tParam.fDef = 120;
		tParam.fPlusDef = 30;
		tParam.fHP = 1300;
		tParam.fPlusHP = 120;
		tParam.iPrice = 1500;
		m_tArmorParam[tParam.iArmorName] = tParam;

		tParam.iArmorType = ARMOR_LongCoat;
		tParam.iArmorName = ArmorAll_LongCoat_QueenKiller;
		tParam.iReinforce = 0;
		tParam.fDef = 100;
		tParam.fPlusDef = 30;
		tParam.fHP = 1900;
		tParam.fPlusHP = 110;
		tParam.iPrice = 1500;
		m_tArmorParam[tParam.iArmorName] = tParam;

		tParam.iArmorType = ARMOR_Muffler;
		tParam.iArmorName = ArmorAll_Muffler_DarkNightSpike;
		tParam.iReinforce = 0;
		tParam.fDef = 160;
		tParam.fPlusDef = 30;
		tParam.fHP = 1590;
		tParam.fPlusHP = 140;
		tParam.iPrice = 2000;
		m_tArmorParam[tParam.iArmorName] = tParam;

		tParam.iArmorType = ARMOR_Muffler;
		tParam.iArmorName = ArmorAll_Muffler_WhiteGraze;
		tParam.iReinforce = 0;
		tParam.fDef = 140;
		tParam.fPlusDef = 30;
		tParam.fHP = 1550;
		tParam.fPlusHP = 200;
		tParam.iPrice = 2000;
		m_tArmorParam[tParam.iArmorName] = tParam;

		break;
	}
	case Client::CWeapon_Inven_InShop::SHOP_ARMOR_SELL:
		break;
	case Client::CWeapon_Inven_InShop::SHOP_ITEM_BUY:
		break;
	case Client::CWeapon_Inven_InShop::SHOP_ITEM_SELL:
		break;
	case  Client::CWeapon_Inven_InShop::SHOP_WEAPON_UPGRADE:
		break;
	}

	return S_OK;
}

void CWeapon_Inven_InShop::Add_Weapon(WPN_PARAM tAddWpnParam)
{
	CWeapon_Slot* pSlot = static_cast<CWeapon_Slot*>(g_pManagement->Clone_GameObject_Return(L"GameObject_WeaponSlot", nullptr));
	pSlot->Set_UI_Size(50.f, 50.f);
	pSlot->Set_WeaponParam(tAddWpnParam);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pSlot, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);	
	m_vecWeaponSlot.push_back(pSlot);

	// 슬롯 생성시 위치 조정
	for (_uint i = 0; i < m_vecWeaponSlot.size(); ++i)
	{
		m_vecWeaponSlot[i]->Set_Active(m_bIsActive);
		m_vecWeaponSlot[i]->Set_ViewZ(m_fViewZ - 0.1f);
		m_vecWeaponSlot[i]->Set_UI_Pos(m_fPosX - 103.f + 52.f * (i % 5), m_fPosY - 140.f + 52.f * (i / 5));
	}
}

void CWeapon_Inven_InShop::Add_Armor(ARMOR_PARAM tAddArmorParam)
{
	CArmor_Slot* pSlot = static_cast<CArmor_Slot*>(g_pManagement->Clone_GameObject_Return(L"GameObject_ArmorSlot", nullptr));
	pSlot->Set_UI_Size(50.f, 50.f);
	pSlot->Set_ArmorParam(tAddArmorParam);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pSlot, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	m_vecArmorSlot.push_back(pSlot);

	// 슬롯 생성시 위치 조정
	for (_uint i = 0; i < m_vecArmorSlot.size(); ++i)
	{
		m_vecArmorSlot[i]->Set_Active(m_bIsActive);
		m_vecArmorSlot[i]->Set_ViewZ(m_fViewZ - 0.1f);
		m_vecArmorSlot[i]->Set_UI_Pos(m_fPosX - 103.f + 52.f * (i % 5), m_fPosY - 140.f + 52.f * (i / 5));
	}
}

CWeapon_Inven_InShop * CWeapon_Inven_InShop::Create(_Device pGraphic_Device)
{
	CWeapon_Inven_InShop* pInstance = new CWeapon_Inven_InShop(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CWeapon_Inven_InShop::Clone_GameObject(void * pArg)
{
	CWeapon_Inven_InShop* pInstance = new CWeapon_Inven_InShop(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CWeapon_Inven_InShop::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	if (m_pTempWeapon)
		Safe_Release(m_pTempWeapon);

	CUI::Free();
}
