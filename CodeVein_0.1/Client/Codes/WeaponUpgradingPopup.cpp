#include "stdafx.h"
#include "..\Headers\WeaponUpgradingPopup.h"
#include "UI_Manager.h"
#include "OrthoEffect.h"

CWeaponUpgradingPopup::CWeaponUpgradingPopup(_Device pDevice)
	: CUI(pDevice)
{
}

CWeaponUpgradingPopup::CWeaponUpgradingPopup(const CWeaponUpgradingPopup & rhs)
	: CUI(rhs)
{
}

void CWeaponUpgradingPopup::Set_Active(_bool bIsActive)
{
	m_bIsActive = bIsActive;

	m_fAlpha = 0.f;
	m_fEffAlpha = 0.f;
	m_fFadeDelay = 0.f;
	m_iEffFadeCnt = 0;
	m_bEffFadeToggle = false;

	m_pUpgradingEff->Set_Active(m_bIsActive);
	m_pShopItemIcon->Set_Active(m_bIsActive);

	if (bIsActive)
		m_bFadeInStart = true;
}

void CWeaponUpgradingPopup::Set_PopupType(POPUP_TYPE eType)
{
	m_ePopupType = eType;
}

HRESULT CWeaponUpgradingPopup::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CWeaponUpgradingPopup::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	m_fPosX = WINCX * 0.5f;
	m_fPosY = WINCY * 0.5f;
	m_fSizeX = 1024.f;
	m_fSizeY = 512.f;

	m_bIsActive = false;
	
	m_iTexIdx = 12;

	SetUp_Default();

	return NOERROR;
}

_int CWeaponUpgradingPopup::Update_GameObject(_double TimeDelta)
{
	if (!m_bIsActive)
		return S_OK;

	CUI::Update_GameObject(TimeDelta);

	m_pShopItemIcon->Set_UI_Pos(WINCX * 0.5f, WINCY * 0.48f);

	m_pShopItemIcon->Set_Alpha(m_fAlpha);
	m_pShopItemIcon->Set_ViewZ(m_fViewZ - 0.1f);
	m_pUpgradingEff->Set_Alpha(m_fEffAlpha);
	m_pUpgradingEff->Set_ViewZ(m_fViewZ - 0.1f);

	Check_ItemIcon();

	if (m_bFadeInStart)
	{
		m_fAlpha += _float(TimeDelta) * 6.f;
		m_pUpgradingEff->Set_Alpha(0.f);

		if (m_fAlpha >= 1.f)
		{
			m_bFadeInStart = false;
			m_bFadeOutStart = true;
		}
	}

	if (m_bFadeOutStart)
	{
		m_fFadeDelay += _float(TimeDelta);

		if (m_fFadeDelay > 4.2f)
		{
			m_bFadeOutStart = false;
			Set_Active(false);
		}
	}

	if (6 > m_iEffFadeCnt)
	{
		_float fFadeSpeed = 1.1f;
		if (!m_bEffFadeToggle)
			m_fEffAlpha += _float(TimeDelta) * fFadeSpeed;
		else
			m_fEffAlpha += _float(TimeDelta) * -fFadeSpeed;

		if ((!m_bEffFadeToggle && m_fEffAlpha >= 0.5f) ||
			(m_bEffFadeToggle && m_fEffAlpha <= 0.f))
		{
			m_iEffFadeCnt += 1;
			m_bEffFadeToggle = !m_bEffFadeToggle;

			if (5 == m_iEffFadeCnt)
			{
				g_pSoundManager->Stop_Sound(CSoundManager::UI_SFX_01);
				g_pSoundManager->Play_Sound(L"UI_Upgrading_End.wav", CSoundManager::UI_SFX_01, CSoundManager::Effect_Sound);
			}
		}

		if (!m_bEffParticle && m_fEffAlpha > 0.4f)
		{
			m_bEffParticle = true;

			g_pSoundManager->Stop_Sound(CSoundManager::UI_SFX_02);
			g_pSoundManager->Play_Sound(L"UI_Upgrading.wav", CSoundManager::UI_SFX_02, CSoundManager::Effect_Sound);

			for (_int i = 0; i < 10; i++)
			{
				COrthoEffect* pEff0 = static_cast<COrthoEffect*>(CParticleMgr::Get_Instance()->Create_EffectReturn(L"Ortho_UpgradeParticle_0"));
				COrthoEffect* pEff1 = static_cast<COrthoEffect*>(CParticleMgr::Get_Instance()->Create_EffectReturn(L"Ortho_UpgradeParticle_1"));
				pEff0->Set_UI_Layer();
				pEff0->Set_ViewZ(m_fViewZ - 0.1f);
				pEff1->Set_UI_Layer();
				pEff1->Set_ViewZ(m_fViewZ - 0.1f);
			}
		}

		if (m_fEffAlpha < 0.1f)
			m_bEffParticle = false;

	}
	else if (6 <= m_iEffFadeCnt)
	{
		switch (m_ePopupType)
		{
		case Client::CWeaponUpgradingPopup::POPUP_SUCCESS:
		{
			m_fEffAlpha += _float(TimeDelta) * 1.f;
			break;
		}
		case Client::CWeaponUpgradingPopup::POPUP_FAILED:
		{
			if(m_fEffAlpha >= 0.6f)
				m_iEffFadeCnt += 1;

			if (m_iEffFadeCnt > 6)
			{
				m_fEffAlpha -= _float(TimeDelta) * 2.5f;
			}
			else if(m_iEffFadeCnt == 6)
			{
				m_fEffAlpha += _float(TimeDelta) * 1.f;
			}

			break;
		}
		}
	}

	if (m_fEffAlpha >= 1.f)
		m_fEffAlpha = 1.f;

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);
	return NO_EVENT;
}

_int CWeaponUpgradingPopup::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CWeaponUpgradingPopup::Render_GameObject()
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


HRESULT CWeaponUpgradingPopup::Add_Component()
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

HRESULT CWeaponUpgradingPopup::SetUp_ConstantTable()
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

void CWeaponUpgradingPopup::Change_Texture(const _tchar * _Name)
{
	auto& iter = m_pmapComponents.find(L"Com_Texture");

	Safe_Release(m_pTextureCom);
	Safe_Release(iter->second);

	iter->second = m_pTextureCom = static_cast<CTexture*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, _Name));
	Safe_AddRef(iter->second);
}

void CWeaponUpgradingPopup::SetUp_Default()
{
	m_pShopItemIcon = static_cast<CShopItemIcon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_ShopItemIcon", nullptr));
	m_pShopItemIcon->Set_UI_Pos(WINCX * 0.5f, WINCY * 0.4f);
	m_pShopItemIcon->Set_UI_Size(65.f, 65.f);
	m_pShopItemIcon->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pShopItemIcon, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pUpgradingEff = static_cast<CWeaponUpgradingEff*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon_UpgradingEff", nullptr));
	m_pUpgradingEff->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pUpgradingEff, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
}

void CWeaponUpgradingPopup::Check_ItemIcon()
{
	CWeapon_Slot* pWeaponSlot = m_pInven->Get_SelectedSlot_Weapon();
	if (pWeaponSlot)
		m_pShopItemIcon->Set_WeaponDescType((WEAPON_ALL_DATA)pWeaponSlot->Get_WeaponParam().iWeaponName_InShop);

	CArmor_Slot* pArmorSlot = m_pInven->Get_SelectedSlot_Armor();
	if (pArmorSlot)
		m_pShopItemIcon->Set_ArmorDescType((ARMOR_All_DATA)pArmorSlot->Get_ArmorParam().iArmorName);
}

CWeaponUpgradingPopup * CWeaponUpgradingPopup::Create(_Device pGraphic_Device)
{
	CWeaponUpgradingPopup* pInstance = new CWeaponUpgradingPopup(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CWeaponUpgradingPopup::Clone_GameObject(void * pArg)
{
	CWeaponUpgradingPopup* pInstance = new CWeaponUpgradingPopup(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CWeaponUpgradingPopup::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
