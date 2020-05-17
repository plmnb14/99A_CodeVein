#include "stdafx.h"
#include "..\Headers\WeaponUpgradeSuccessPopupOptionUI.h"
#include "UI_Manager.h"

CWeaponUpgradeSuccessPopupOptionUI::CWeaponUpgradeSuccessPopupOptionUI(_Device pDevice)
	: CUI(pDevice)
{
}

CWeaponUpgradeSuccessPopupOptionUI::CWeaponUpgradeSuccessPopupOptionUI(const CWeaponUpgradeSuccessPopupOptionUI & rhs)
	: CUI(rhs)
{
}

HRESULT CWeaponUpgradeSuccessPopupOptionUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CWeaponUpgradeSuccessPopupOptionUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	m_fPosX = 650.f;
	m_fPosY = 350.f;
	m_fSizeX = 100.f;
	m_fSizeY = 100.f;
	m_fViewZ = 0.1f;

	m_bIsActive = false;
	m_iIndex = 1;

	return NOERROR;
}

_int CWeaponUpgradeSuccessPopupOptionUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	switch (m_eOption)
	{
	case Client::CWeaponUpgradeSuccessPopupOptionUI::BUTTON_OK:
	{
		m_iIndex = 6;
		break;
	}
	}

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	return NO_EVENT;
}

_int CWeaponUpgradeSuccessPopupOptionUI::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CWeaponUpgradeSuccessPopupOptionUI::Render_GameObject()
{
	if (!m_bIsActive)
		return NOERROR;
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	_uint iIndex = 0;
	if(m_bIsSelect)
	{
		if (FAILED(SetUp_ConstantTable(m_iIndex)))
			return E_FAIL;

		m_pShaderCom->Begin_Shader();
		m_pShaderCom->Begin_Pass(1);

		m_pBufferCom->Render_VIBuffer();
		m_pShaderCom->End_Pass();
		m_pShaderCom->End_Shader();
	}
	else
	{
		
	}

	return NOERROR;
}

_bool CWeaponUpgradeSuccessPopupOptionUI::Pt_InRect()
{
	return g_pInput_Device->MousePt_InRect(m_fPosX, m_fPosY, m_fSizeX, m_fSizeY, g_hWnd);
}

HRESULT CWeaponUpgradeSuccessPopupOptionUI::Add_Component()
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

HRESULT CWeaponUpgradeSuccessPopupOptionUI::SetUp_ConstantTable(_int iIdx)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, iIdx)))
		return E_FAIL;

	return NOERROR;
}

CWeaponUpgradeSuccessPopupOptionUI * CWeaponUpgradeSuccessPopupOptionUI::Create(_Device pGraphic_Device)
{
	CWeaponUpgradeSuccessPopupOptionUI* pInstance = new CWeaponUpgradeSuccessPopupOptionUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CWeaponUpgradeSuccessPopupOptionUI::Clone_GameObject(void * pArg)
{
	CWeaponUpgradeSuccessPopupOptionUI* pInstance = new CWeaponUpgradeSuccessPopupOptionUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CWeaponUpgradeSuccessPopupOptionUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	
	CUI::Free();
}
