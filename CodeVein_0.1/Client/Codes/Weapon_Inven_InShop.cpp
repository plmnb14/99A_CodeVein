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
		m_pWeaponSellPopup->Get_Active()
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
	case Client::CWeapon_Inven_InShop::SHOP_WEAPON_SELL:
	{
		vector<CWeapon_Slot*>* pVecMyWeapon = CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Get_VecWeaponSlot();
		for (_int i = 0; i < pVecMyWeapon->size(); i++)
		{
			Add_Weapon((*pVecMyWeapon)[i]->Get_WeaponParam());
		}
		break;
	}
	}
}

void CWeapon_Inven_InShop::Refresh_Inven()
{
	for (auto& iter : m_vecWeaponSlot)
	{
		iter->Set_Select(false);
	}
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
	m_fSizeX = 280.f;
	m_fSizeY = 471.f;
	m_fViewZ = 4.f;

	m_bIsActive = false;

	m_pWeaponInventory = CUI_Manager::Get_Instance()->Get_Weapon_Inven();

	m_pWeaponBuyPopup = static_cast<CWeaponBuyPopupUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon_BuyPopup", nullptr));
	m_pWeaponBuyPopup->Set_Inven(this);
	m_pWeaponBuyPopup->Set_Type(CWeaponBuyPopupUI::POPUP_WEAPON_BUY);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pWeaponBuyPopup, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pWeaponSellPopup = static_cast<CWeaponBuyPopupUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon_BuyPopup", nullptr));
	m_pWeaponSellPopup->Set_Inven(this);
	m_pWeaponSellPopup->Set_Type(CWeaponBuyPopupUI::POPUP_WEAPON_SELL);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pWeaponSellPopup, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	return NOERROR;
}

_int CWeapon_Inven_InShop::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.0f);

	Click_Inven();
	
	for (auto& pWeaponSlot : m_vecWeaponSlot)
	{
		pWeaponSlot->Set_Active(m_bIsActive);
		pWeaponSlot->Set_ViewZ(m_fViewZ - 0.1f);
	}

	if (m_pWeaponBuyPopup->Get_Active())
	{
		if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_RB))
		{
			m_pWeaponBuyPopup->Set_Active(false);
			Refresh_Inven();
		}
	}

	if (m_pWeaponSellPopup->Get_Active())
	{
		if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_RB))
		{
			m_pWeaponSellPopup->Set_Active(false);
			Refresh_Inven();
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

HRESULT CWeapon_Inven_InShop::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, 2)))
		return E_FAIL;

	return NOERROR;
}

void CWeapon_Inven_InShop::Click_Inven()
{
	if (!m_bIsActive)
		return;

	for (auto& pSlot : m_vecWeaponSlot)
	{
		if (pSlot->Pt_InRect())
		{
			m_pHoverSlot = pSlot;
			
			if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB) &&
				!pSlot->Get_Select())
			{
				m_pSelectedSlot = pSlot;
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
					break;
				case Client::CWeapon_Inven_InShop::SHOP_ARMOR_SELL:
					break;
				case Client::CWeapon_Inven_InShop::SHOP_ITEM_BUY:
					break;
				case Client::CWeapon_Inven_InShop::SHOP_ITEM_SELL:
					break;
				}
			}
			return;
		}
	}

	//m_pSelectedSlot = nullptr;
}

void CWeapon_Inven_InShop::Buy_Weapon()
{
	m_pWeaponInventory->Add_Weapon(m_pSelectedSlot->Get_WeaponParam());
}


void CWeapon_Inven_InShop::Sell_Weapon()
{
	m_pWeaponInventory->Sell_Weapon();
}

HRESULT CWeapon_Inven_InShop::SetUp_WeaponData(INVEN_SHOP_OPTION eShop)
{
	switch (eShop)
	{
	case Client::CWeapon_Inven_InShop::SHOP_WEAPON_BUY:
	{
		CWeapon* pTempWeapon = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));
		if (!pTempWeapon)
			return E_FAIL;
		pTempWeapon->AddRef();

		for (_int i = 0; i < WEAPON_DATA::WPN_DATA_End; i++)
			m_tWeaponParam[i] = pTempWeapon->Get_WeaponParam((WEAPON_DATA)i);

		Safe_Release(pTempWeapon);
		break;
	}
	case Client::CWeapon_Inven_InShop::SHOP_WEAPON_SELL:
		break;
	case Client::CWeapon_Inven_InShop::SHOP_ARMOR_BUY:
		break;
	case Client::CWeapon_Inven_InShop::SHOP_ARMOR_SELL:
		break;
	case Client::CWeapon_Inven_InShop::SHOP_ITEM_BUY:
		break;
	case Client::CWeapon_Inven_InShop::SHOP_ITEM_SELL:
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

	CUI::Free();
}
