#include "stdafx.h"
#include "..\Headers\ShopActionFailedPopup.h"
#include "UI_Manager.h"
#include "Total_Inven.h"

CShopActionFailedPopup::CShopActionFailedPopup(_Device pDevice)
	: CUI(pDevice)
{
}

CShopActionFailedPopup::CShopActionFailedPopup(const CShopActionFailedPopup & rhs)
	: CUI(rhs)
{
}

void CShopActionFailedPopup::Set_Active(_bool bIsActive)
{
	m_bIsActive = bIsActive;

	m_fAlpha = 0.f;
	m_fFadeDelay = 0.f;

	if (bIsActive)
		m_bFadeInStart = true;
}

void CShopActionFailedPopup::Set_PopupType(POPUP_TYPE eType)
{
	m_ePopupType = eType;

	switch (m_ePopupType)
	{
	case Client::CShopActionFailedPopup::POPUP_BUY:
		m_iTexIdx = 21;
		break;
	case Client::CShopActionFailedPopup::POPUP_UPGRADE:
		m_iTexIdx = 22;
		break;
	}
}

HRESULT CShopActionFailedPopup::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CShopActionFailedPopup::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	m_fPosX = WINCX * 0.5f;
	m_fPosY = WINCY * 0.5f;
	m_fSizeX = 1024.f;
	m_fSizeY = 512.f;

	m_bIsActive = false;
	
	m_iTexIdx = 0;

	SetUp_Default();

	return NOERROR;
}

_int CShopActionFailedPopup::Update_GameObject(_double TimeDelta)
{
	if (!m_bIsActive)
		return S_OK;

	CUI::Update_GameObject(TimeDelta);

	m_fSizeX = 1024.f;
	m_fSizeY = 512.f;

	SetUp_Default();

	if (m_bFadeInStart)
	{
		m_fAlpha += _float(TimeDelta) * 6.f;
		if (m_fAlpha >= 1.f)
		{
			m_bFadeInStart = false;
			m_bFadeOutStart = true;
		}
	}

	if (m_bFadeOutStart)
	{
		m_fFadeDelay += _float(TimeDelta);

		if (m_fFadeDelay < 0.5f)
			return;

		m_fAlpha += _float(TimeDelta) * -1.3f;
		if (m_fAlpha <= 0.f)
			m_bFadeOutStart = false;
	}

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);
	return NO_EVENT;
}

_int CShopActionFailedPopup::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CShopActionFailedPopup::Render_GameObject()
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


HRESULT CShopActionFailedPopup::Add_Component()
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

HRESULT CShopActionFailedPopup::SetUp_ConstantTable()
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

void CShopActionFailedPopup::Change_Texture(const _tchar * _Name)
{
	auto& iter = m_pmapComponents.find(L"Com_Texture");

	Safe_Release(m_pTextureCom);
	Safe_Release(iter->second);

	iter->second = m_pTextureCom = static_cast<CTexture*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, _Name));
	Safe_AddRef(iter->second);
}

void CShopActionFailedPopup::SetUp_Default()
{

}

CShopActionFailedPopup * CShopActionFailedPopup::Create(_Device pGraphic_Device)
{
	CShopActionFailedPopup* pInstance = new CShopActionFailedPopup(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CShopActionFailedPopup::Clone_GameObject(void * pArg)
{
	CShopActionFailedPopup* pInstance = new CShopActionFailedPopup(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CShopActionFailedPopup::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
