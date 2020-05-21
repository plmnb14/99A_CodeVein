#include "stdafx.h"
#include "..\Headers\WeaponUpgradeSuccessPopupUI.h"
#include "UI_Manager.h"
#include "Total_Inven.h"
#include "CollisionMgr.h"

CWeaponUpgradeSuccessPopupUI::CWeaponUpgradeSuccessPopupUI(_Device pDevice)
	: CUI(pDevice)
{
}

CWeaponUpgradeSuccessPopupUI::CWeaponUpgradeSuccessPopupUI(const CWeaponUpgradeSuccessPopupUI & rhs)
	: CUI(rhs)
{
}

void CWeaponUpgradeSuccessPopupUI::Set_Active(_bool bIsActive)
{
	m_bIsActive = bIsActive;
	m_pShopItemIcon->Set_Active(m_bIsActive);

	Reset_Option();
}

void CWeaponUpgradeSuccessPopupUI::Set_Fade(_bool bIsActive)
{
	m_bIsActive = bIsActive;
	m_pShopItemIcon->Set_Active(m_bIsActive);

	m_fAlpha = 0.f;
	m_fStartDelay = 4.45f;
	m_pShopItemIcon->Set_Alpha(m_fAlpha);

	if (bIsActive)
		m_bFadeStart = true;
}

void CWeaponUpgradeSuccessPopupUI::Set_PopupType(POPUP_TYPE eType)
{
	m_ePopupType = eType;

	switch (m_ePopupType)
	{
	case Client::CWeaponUpgradeSuccessPopupUI::POPUP_SUCCESS:
		m_iTexIdx = 5;
		break;
	case Client::CWeaponUpgradeSuccessPopupUI::POPUP_FAILED:
		m_iTexIdx = 11;
		break;
	}
}

HRESULT CWeaponUpgradeSuccessPopupUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CWeaponUpgradeSuccessPopupUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	m_fPosX = WINCX * 0.5f;
	m_fPosY = WINCY * 0.5f;
	m_fSizeX = 1280.f;// * 0.7f;
	m_fSizeY = 720.f;//* 0.7f;

	m_bIsActive = false;
	
	m_iTexIdx = 0;

	SetUp_Default();

	return NOERROR;
}

_int CWeaponUpgradeSuccessPopupUI::Update_GameObject(_double TimeDelta)
{
	if (!m_bIsActive)
		return S_OK;

	CUI::Update_GameObject(TimeDelta);

	m_fStartDelay -= _float(TimeDelta);
	m_pShopItemIcon->Set_Active(false);

	if (m_fStartDelay > 0.f)
		return S_OK;

	// ======================================
	Check_ItemOption();
	Check_Option();
	Check_ItemIcon();

	if (m_bFadeStart)
	{
		m_fAlpha += _float(TimeDelta) * 10.f;
		

		if (m_fAlpha >= 1.f)
		{
			m_bFadeStart = false;

			g_pSoundManager->Stop_Sound(CSoundManager::UI_SFX_01);
			switch (m_ePopupType)
			{
			case Client::CWeaponUpgradeSuccessPopupUI::POPUP_SUCCESS:
				g_pSoundManager->Play_Sound(L"UI_UpgradeSuccess.wav", CSoundManager::UI_SFX_01, CSoundManager::Effect_Sound);
				break;
			case Client::CWeaponUpgradeSuccessPopupUI::POPUP_FAILED:
				g_pSoundManager->Play_Sound(L"UI_UpgradeFailed.wav", CSoundManager::UI_SFX_01, CSoundManager::Effect_Sound);
				break;
			}

			if (m_ePopupType == POPUP_SUCCESS)
			{
				CWeapon_Slot* pWeaponSlot = m_pInven->Get_SelectedSlot_Weapon();
				if (pWeaponSlot)
				{
					WPN_PARAM tParam = pWeaponSlot->Get_WeaponParam();
					tParam.iReinforce += 1;
					tParam.fPlusDamage = m_pInven->Get_PlusDamage(tParam.fDamage, tParam.iReinforce);
					pWeaponSlot->Set_WeaponParam(tParam);
					m_pInven->Upgrade_Weapon(tParam);
				}
				else
				{
					CArmor_Slot* pArmorSlot = m_pInven->Get_SelectedSlot_Armor();

					ARMOR_PARAM tParam = pArmorSlot->Get_ArmorParam();
					tParam.iReinforce += 1;
					tParam.fPlusDef = m_pInven->Get_PlusDamage(tParam.fDef, tParam.iReinforce);
					pArmorSlot->Set_ArmorParam(tParam);
					m_pInven->Upgrade_Armor(tParam);
				}
			}
		}
	}

	if (m_bLateInit)
	{
		LOOP(1)
		{
			TARGET_TO_TRANS(m_vecOption[i])->Set_Angle(m_pTransformCom->Get_Angle());
			TARGET_TO_TRANS(m_vecOption[i])->Set_Scale(_v3(1.f, 1.f, 1.f));
			TARGET_TO_TRANS(m_vecOption[i])->Set_At(m_pTransformCom->Get_At());

			_v3 vPos;
			if (0 == i)
				m_vecOption[i]->Set_UI_Pos(Get_UI_Pos().x, Get_UI_Pos().y + 76.f);

			m_vecOption[i]->Set_UI_Size(180, 70);
			m_vecOption[i]->Set_ViewZ(m_fViewZ - 0.01f);

			TARGET_TO_TRANS(m_vecOption[i])->Set_Pos(vPos);
			m_vecOption[i]->Set_ButtonType(CWeaponUpgradeOptionUI::BUTTON_TYPE(i));
			m_vecOption[i]->Set_Active(m_bIsActive);
		}
	}
	
	Click_Option();

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);
	return NO_EVENT;
}

_int CWeaponUpgradeSuccessPopupUI::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CWeaponUpgradeSuccessPopupUI::Render_GameObject()
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

	m_pShaderCom->Begin_Pass(5);

	m_pBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();

	m_pShaderCom->End_Shader();

	return NOERROR;
}


void CWeaponUpgradeSuccessPopupUI::Click_Option()
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
	
			if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
			{
				if (0 == iIdx)
				{
					Set_Active(false);

				}
			}
		}
		else
		{
			iter->Set_Select(false);
		}
	
		++iIdx;
	}
}

void CWeaponUpgradeSuccessPopupUI::Reset_Option()
{
	for (auto& iter : m_vecOption)
	{
		iter->Set_Select(false);
	}
}

HRESULT CWeaponUpgradeSuccessPopupUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_WeaponShop_Upgrade_UI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CWeaponUpgradeSuccessPopupUI::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, m_iTexIdx)))
		return E_FAIL;

	return NOERROR;
}

void CWeaponUpgradeSuccessPopupUI::Change_Texture(const _tchar * _Name)
{
	auto& iter = m_pmapComponents.find(L"Com_Texture");

	Safe_Release(m_pTextureCom);
	Safe_Release(iter->second);

	iter->second = m_pTextureCom = static_cast<CTexture*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, _Name));
	Safe_AddRef(iter->second);
}

void CWeaponUpgradeSuccessPopupUI::SetUp_Default()
{
	m_pShopItemIcon = static_cast<CShopItemIcon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_ShopItemIcon", nullptr));
	m_pShopItemIcon->Set_UI_Pos(WINCX * 0.5f, WINCY * 0.4f);
	m_pShopItemIcon->Set_UI_Size(65.f, 65.f);
	m_pShopItemIcon->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pShopItemIcon, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
}

void CWeaponUpgradeSuccessPopupUI::Check_LateInit()
{
	if (m_bLateInit)
		return;
	m_bLateInit = true;

}

void CWeaponUpgradeSuccessPopupUI::Check_ItemOption()
{
	if (!m_bIsActive)
		return;

	CWeapon_Slot* pWeaponSlot = m_pInven->Get_HoverSlot_Weapon();
	

	if (!pWeaponSlot || m_pInven->Get_PopupOn())
		pWeaponSlot = m_pInven->Get_SelectedSlot_Weapon();

	if (!pWeaponSlot)
		return;

	WPN_PARAM tParam = pWeaponSlot->Get_WeaponParam();

}

void CWeaponUpgradeSuccessPopupUI::Check_Option()
{
	if (!m_bLateInit)
	{
		m_bLateInit = true;
		CWeaponUpgradeOptionUI* pInstance = nullptr;
		LOOP(1)
		{
			pInstance = static_cast<CWeaponUpgradeOptionUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon_UpgradeSuccessPopupOption", nullptr));
			g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
			m_vecOption.push_back(pInstance);
		}
	}
}

void CWeaponUpgradeSuccessPopupUI::Check_ItemIcon()
{
	m_pShopItemIcon->Set_Active(m_bIsActive);
	m_pShopItemIcon->Set_Alpha(m_fAlpha);
	//cout <<"Icon Alpha"<< m_fAlpha << endl;

	CWeapon_Slot* pWeaponSlot = m_pInven->Get_SelectedSlot_Weapon();
	if (pWeaponSlot)
		m_pShopItemIcon->Set_WeaponDescType((WEAPON_ALL_DATA)pWeaponSlot->Get_WeaponParam().iWeaponName_InShop);

	CArmor_Slot* pArmorSlot = m_pInven->Get_SelectedSlot_Armor();
	if (pArmorSlot)
		m_pShopItemIcon->Set_ArmorDescType((ARMOR_All_DATA)pArmorSlot->Get_ArmorParam().iArmorName);

	m_pShopItemIcon->Set_UI_Pos(WINCX * 0.5f, WINCY * 0.4f);
	m_pShopItemIcon->Set_UI_Size(65.f, 65.f);
	m_pShopItemIcon->Set_ViewZ(m_fViewZ - 0.1f);

	if (POPUP_FAILED == m_ePopupType)
		m_pShopItemIcon->Set_Active(false);
}

CWeaponUpgradeSuccessPopupUI * CWeaponUpgradeSuccessPopupUI::Create(_Device pGraphic_Device)
{
	CWeaponUpgradeSuccessPopupUI* pInstance = new CWeaponUpgradeSuccessPopupUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CWeaponUpgradeSuccessPopupUI::Clone_GameObject(void * pArg)
{
	CWeaponUpgradeSuccessPopupUI* pInstance = new CWeaponUpgradeSuccessPopupUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CWeaponUpgradeSuccessPopupUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
