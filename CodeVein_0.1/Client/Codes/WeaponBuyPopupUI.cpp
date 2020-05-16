#include "stdafx.h"
#include "..\Headers\WeaponBuyPopupUI.h"
#include "UI_Manager.h"
#include "Total_Inven.h"
#include "CollisionMgr.h"

CWeaponBuyPopupUI::CWeaponBuyPopupUI(_Device pDevice)
	: CUI(pDevice)
{
}

CWeaponBuyPopupUI::CWeaponBuyPopupUI(const CWeaponBuyPopupUI & rhs)
	: CUI(rhs)
{
}

void CWeaponBuyPopupUI::Set_Active(_bool bIsActive)
{
	m_bIsActive = bIsActive;
	if (m_bIsActive)
		m_iCount = 0;
}

HRESULT CWeaponBuyPopupUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CWeaponBuyPopupUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	m_fPosX = 650.f;
	m_fPosY = 350.f;
	m_fSizeX = 512.f;
	m_fSizeY = 256.f;
	m_fViewZ = 0.5f;

	m_bIsActive = false;
	m_iTexIndex = 1;

	SetUp_Default();
	
	CWeaponBuyPopupOptionUI* pInstance = nullptr;
	LOOP(3)
	{
		pInstance = static_cast<CWeaponBuyPopupOptionUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon_BuyPopupOption", nullptr));
		//pInstance->Setup_InvenType(CWeaponBuyPopupOptionUI::WEAPONSHOP_OPTION(i));
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
		m_vecOption.push_back(pInstance);
	}

	return NOERROR;
}

_int CWeaponBuyPopupUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	Check_ItemOption_Weapon();
	Check_ItemOption_Armor();

	_float fPosX = WINCX * 0.505f;
	if(m_iCount > 9)
		fPosX = WINCX * 0.520f;
	m_pFontItemCount->Set_UI_Pos(fPosX, WINCY * 0.475f);

	LOOP(3)
	{
		TARGET_TO_TRANS(m_vecOption[i])->Set_Angle(m_pTransformCom->Get_Angle());
		TARGET_TO_TRANS(m_vecOption[i])->Set_Scale(_v3(1.f, 1.f, 1.f));
		TARGET_TO_TRANS(m_vecOption[i])->Set_At(m_pTransformCom->Get_At());

		_v3 vPos;
		if (0 == i)
			vPos = m_pTransformCom->Get_Pos() + _v3(-10.f, 0.f, 0.f);
		else if (1 == i)
			vPos = m_pTransformCom->Get_Pos() + _v3(10.f, 0.f, 0.f);
		else if (2 == i)
			vPos = m_pTransformCom->Get_Pos() + _v3(0.f, 10.f, 0.f);
		TARGET_TO_TRANS(m_vecOption[i])->Set_Pos(vPos);

		m_vecOption[i]->Set_ButtonType(CWeaponBuyPopupOptionUI::BUTTON_TYPE(i));
		if(POPUP_WEAPON_SELL == m_eType ||
			POPUP_ARMOR_SELL == m_eType || 
			POPUP_ITEM_SELL == m_eType)
			m_vecOption[i]->Set_IsSell(true);
		m_vecOption[i]->Set_Active(m_bIsActive);
	}

	Click_Option();

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);
	return NO_EVENT;
}

_int CWeaponBuyPopupUI::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CWeaponBuyPopupUI::Render_GameObject()
{
	if (!m_bIsActive)
		return NOERROR;
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);

	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();

	m_pShaderCom->Begin_Pass(1);

	m_pBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();

	m_pShaderCom->End_Shader();

	return NOERROR;
}


void CWeaponBuyPopupUI::Click_Option()
{
	if (!m_bIsActive)
		return;
	
	_int iIdx = 0;
	
	for (auto& iter : m_vecOption)
	{
		if (iter->Pt_InRect())
		{
			Reset_Option();
			iter->Set_Select(true);
			
			if (m_iOption != iIdx)
			{
				g_pSoundManager->Stop_Sound(CSoundManager::UI_SFX_01);
				g_pSoundManager->Play_Sound(L"UI_CommonHover.wav", CSoundManager::UI_SFX_01, CSoundManager::Effect_Sound);
			}
			m_iOption = iIdx;

			if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
			{
				g_pSoundManager->Stop_Sound(CSoundManager::UI_SFX_01);
				g_pSoundManager->Play_Sound(L"UI_CommonClick.wav", CSoundManager::UI_SFX_01, CSoundManager::Effect_Sound);

				if (0 == iIdx)
					--m_iCount;
				else if (1 == iIdx)
					++m_iCount;
				else if (2 == iIdx)
				{
					switch (m_eType)
					{
					case Client::CWeaponBuyPopupUI::POPUP_WEAPON_BUY:
					{
						Buy_Weapon();

						g_pSoundManager->Stop_Sound(CSoundManager::UI_SFX_02);
						g_pSoundManager->Play_Sound(L"UI_WeaponBuy.wav", CSoundManager::UI_SFX_02, CSoundManager::Effect_Sound);
						
						g_pSoundManager->Stop_Sound(CSoundManager::UI_SFX_03);
						TCHAR szBuff[128] = L"UI_Money_%d";
						swprintf_s(szBuff, szBuff, CCalculater::Random_Num(0, 6));
						lstrcat(szBuff, L".wav");
						g_pSoundManager->Play_Sound(szBuff, CSoundManager::UI_SFX_02, CSoundManager::Effect_Sound);

						break;
					}
					case Client::CWeaponBuyPopupUI::POPUP_WEAPON_SELL:
					{
						Sell_Weapon();

						g_pSoundManager->Stop_Sound(CSoundManager::UI_SFX_02);
						g_pSoundManager->Play_Sound(L"UI_WeaponSell.wav", CSoundManager::UI_SFX_02, CSoundManager::Effect_Sound);

						g_pSoundManager->Stop_Sound(CSoundManager::UI_SFX_03);
						TCHAR szBuff[128] = L"UI_Money_%d";
						swprintf_s(szBuff, szBuff, CCalculater::Random_Num(0, 6));
						lstrcat(szBuff, L".wav");
						g_pSoundManager->Play_Sound(szBuff, CSoundManager::UI_SFX_02, CSoundManager::Effect_Sound);

						break;
					}
					case Client::CWeaponBuyPopupUI::POPUP_ARMOR_BUY:
					{
						Buy_Armor();

						g_pSoundManager->Stop_Sound(CSoundManager::UI_SFX_02);
						g_pSoundManager->Play_Sound(L"UI_ArmorBuy.wav", CSoundManager::UI_SFX_02, CSoundManager::Effect_Sound);

						g_pSoundManager->Stop_Sound(CSoundManager::UI_SFX_03);
						TCHAR szBuff[128] = L"UI_Money_%d";
						swprintf_s(szBuff, szBuff, CCalculater::Random_Num(0, 6));
						lstrcat(szBuff, L".wav");
						g_pSoundManager->Play_Sound(szBuff, CSoundManager::UI_SFX_02, CSoundManager::Effect_Sound);

						break;
					}
					case Client::CWeaponBuyPopupUI::POPUP_ARMOR_SELL:
					{
						Sell_Armor();

						g_pSoundManager->Stop_Sound(CSoundManager::UI_SFX_02);
						g_pSoundManager->Play_Sound(L"UI_ArmorSell.wav", CSoundManager::UI_SFX_02, CSoundManager::Effect_Sound);

						g_pSoundManager->Stop_Sound(CSoundManager::UI_SFX_03);
						TCHAR szBuff[128] = L"UI_Money_%d";
						swprintf_s(szBuff, szBuff, CCalculater::Random_Num(0, 6));
						lstrcat(szBuff, L".wav");
						g_pSoundManager->Play_Sound(szBuff, CSoundManager::UI_SFX_02, CSoundManager::Effect_Sound);

						break;
					}
					}

					Set_Active(false);
					m_pInven->Refresh_Inven();
				}
			}

			if (POPUP_WEAPON_SELL == m_eType)
			{
				if (m_iCount > 1)
					m_iCount = 1;
			}

			if (0 > m_iCount)
				m_iCount = 0;
			if (m_iCount > 99)
				m_iCount = 99;
		}
		else
		{
			iter->Set_Select(false);
		}
	
		++iIdx;
	}
}

void CWeaponBuyPopupUI::Reset_Option()
{
	for (auto& iter : m_vecOption)
	{
		iter->Set_Select(false);
	}
}

HRESULT CWeaponBuyPopupUI::Add_Component()
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

HRESULT CWeaponBuyPopupUI::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, m_iTexIndex)))
		return E_FAIL;

	return NOERROR;
}

void CWeaponBuyPopupUI::Change_Texture(const _tchar * _Name)
{
	auto& iter = m_pmapComponents.find(L"Com_Texture");

	Safe_Release(m_pTextureCom);
	Safe_Release(iter->second);

	iter->second = m_pTextureCom = static_cast<CTexture*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, _Name));
	Safe_AddRef(iter->second);
}

void CWeaponBuyPopupUI::SetUp_Default()
{
	m_pFontItemCount = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pFontItemCount->Set_UI_Pos(WINCX * 0.505f, WINCY * 0.475f);
	m_pFontItemCount->Set_UI_Size(35.f, 55.f);
	m_pFontItemCount->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFontItemCount, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pShopActionFailedPopup = static_cast<CShopActionFailedPopup*>(g_pManagement->Clone_GameObject_Return(L"GameObject_ShopActionFailedPopup", nullptr));
	m_pShopActionFailedPopup->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pShopActionFailedPopup, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	
}

void CWeaponBuyPopupUI::Check_ItemOption_Armor()
{
	if (!m_bIsActive)
	{
		m_pFontItemCount->Set_Active(false);
		return;
	}

	CArmor_Slot* pArmorSlot = m_pInven->Get_SelectedSlot_Armor();

	if (!pArmorSlot)
		return;

	//==============================================================================================================
	// Count
	if (!m_pFontItemCount)
		return;
	m_pFontItemCount->Update_NumberValue((_float)m_iCount);
	m_pFontItemCount->Set_Active(true);
}

void CWeaponBuyPopupUI::Check_ItemOption_Weapon()
{
	if (!m_bIsActive)
	{
		m_pFontItemCount->Set_Active(false);
		return;
	}

	CWeapon_Slot* pWeaponSlot = m_pInven->Get_SelectedSlot_Weapon();

	if (!pWeaponSlot)
		return;

	//==============================================================================================================
	// Count
	if (!m_pFontItemCount)
		return;
	m_pFontItemCount->Update_NumberValue((_float)m_iCount);
	m_pFontItemCount->Set_Active(true);

}

void CWeaponBuyPopupUI::Buy_Weapon()
{
	_int iPrice = m_pInven->Get_SelectedSlot_Weapon()->Get_WeaponParam().iPrice;
	_int iTotalPrice = m_iCount * iPrice;
	_int iMyHaze = (_int)CUI_Manager::Get_Instance()->Get_HazeUI()->Get_Haze_Cnt();
	if (iTotalPrice > iMyHaze)
	{
		m_pShopActionFailedPopup->Set_Active(true);
		m_pShopActionFailedPopup->Set_PopupType(CShopActionFailedPopup::POPUP_BUY);
		return;
	}

	for (_int i = 0; i < m_iCount; i++)
	{
		m_pInven->Buy_Weapon();
	}
	CUI_Manager::Get_Instance()->Get_HazeUI()->Accumulate_Haze(-iTotalPrice);
}

void CWeaponBuyPopupUI::Sell_Weapon()
{
	_int iPrice = m_pInven->Get_SelectedSlot_Weapon()->Get_WeaponParam().iPrice;

	for (_int i = 0; i < m_iCount; i++)
	{
		m_pInven->Sell_Weapon();
	}
	CUI_Manager::Get_Instance()->Get_HazeUI()->Accumulate_Haze(iPrice);
}

void CWeaponBuyPopupUI::Buy_Armor()
{
	_int iPrice = m_pInven->Get_SelectedSlot_Armor()->Get_ArmorParam().iPrice;
	_int iTotalPrice = m_iCount * iPrice;
	_int iMyHaze = (_int)CUI_Manager::Get_Instance()->Get_HazeUI()->Get_Haze_Cnt();
	if (iTotalPrice > iMyHaze)
	{
		m_pShopActionFailedPopup->Set_Active(true);
		m_pShopActionFailedPopup->Set_PopupType(CShopActionFailedPopup::POPUP_BUY);
		return;
	}

	for (_int i = 0; i < m_iCount; i++)
	{
		m_pInven->Buy_Armor();
	}
	CUI_Manager::Get_Instance()->Get_HazeUI()->Accumulate_Haze(-iTotalPrice);
}

void CWeaponBuyPopupUI::Sell_Armor()
{
	_int iPrice = m_pInven->Get_SelectedSlot_Armor()->Get_ArmorParam().iPrice;

	for (_int i = 0; i < m_iCount; i++)
	{
		m_pInven->Sell_Armor();
	}
	CUI_Manager::Get_Instance()->Get_HazeUI()->Accumulate_Haze(iPrice);
}

CWeaponBuyPopupUI * CWeaponBuyPopupUI::Create(_Device pGraphic_Device)
{
	CWeaponBuyPopupUI* pInstance = new CWeaponBuyPopupUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CWeaponBuyPopupUI::Clone_GameObject(void * pArg)
{
	CWeaponBuyPopupUI* pInstance = new CWeaponBuyPopupUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CWeaponBuyPopupUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
