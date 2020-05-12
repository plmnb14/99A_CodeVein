#include "stdafx.h"
#include "..\Headers\WeaponBuyPopupOptionUI.h"
#include "UI_Manager.h"
#include "Total_Inven.h"

CWeaponBuyPopupOptionUI::CWeaponBuyPopupOptionUI(_Device pDevice)
	: CUI(pDevice)
{
}

CWeaponBuyPopupOptionUI::CWeaponBuyPopupOptionUI(const CWeaponBuyPopupOptionUI & rhs)
	: CUI(rhs)
{
}

HRESULT CWeaponBuyPopupOptionUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CWeaponBuyPopupOptionUI::Ready_GameObject(void * pArg)
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

	m_pCollider->Set_Radius(_v3{ 0.5f, 0.07f, 0.1f });
	m_pCollider->Set_Dynamic(true);
	m_pCollider->Set_Type(COL_SPHERE);
	m_pCollider->Set_CenterPos(m_pTransformCom->Get_Pos());

	return NOERROR;
}

_int CWeaponBuyPopupOptionUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	switch (m_eOption)
	{
	case Client::CWeaponBuyPopupOptionUI::BUTTON_LEFT:
	{
		m_iIndex = 2;
		m_fPosX = 500.f;
		m_fPosY = 350.f;
		break;
	}
	case Client::CWeaponBuyPopupOptionUI::BUTTON_RIGHT:
	{
		m_iIndex = 3;
		m_fPosX = 800.f;
		m_fPosY = 350.f;
		break;
	}
	case Client::CWeaponBuyPopupOptionUI::BUTTON_OK:
	{
		m_iIndex = 4;
		m_fSizeX = 150.f;
		m_fSizeY = 80.f;
		m_fPosX = 650.f;
		m_fPosY = 420.f;
		break;
	}
	}

	m_pCollider->Update(m_pTransformCom->Get_Pos());
	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	return NO_EVENT;
}

_int CWeaponBuyPopupOptionUI::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CWeaponBuyPopupOptionUI::Render_GameObject()
{
	if (!m_bIsActive)
		return NOERROR;
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	_uint iIndex = 0;
	if (!m_bIsSelect)
	{
		iIndex = m_iIndex;

		g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);

		g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
		g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

		if (FAILED(SetUp_ConstantTable(iIndex)))
			return E_FAIL;

		m_pShaderCom->Begin_Shader();

		m_pShaderCom->Begin_Pass(1);

		m_pBufferCom->Render_VIBuffer();

		m_pShaderCom->End_Pass();

		m_pShaderCom->End_Shader();
	}
	else if(4 == m_iIndex)
	{
		LOOP(2)
		{
			(i == 0) ? (iIndex = 10) : (iIndex = m_iIndex);

			if (FAILED(SetUp_ConstantTable(iIndex)))
				return E_FAIL;

			m_pShaderCom->Begin_Shader();
			m_pShaderCom->Begin_Pass(1);

			m_pBufferCom->Render_VIBuffer();
			m_pShaderCom->End_Pass();
			m_pShaderCom->End_Shader();
		}
	}




	

	return NOERROR;
}

HRESULT CWeaponBuyPopupOptionUI::Add_Component()
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

	// for.Com_Collider
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CWeaponBuyPopupOptionUI::SetUp_ConstantTable(_int iIdx)
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

CWeaponBuyPopupOptionUI * CWeaponBuyPopupOptionUI::Create(_Device pGraphic_Device)
{
	CWeaponBuyPopupOptionUI* pInstance = new CWeaponBuyPopupOptionUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CWeaponBuyPopupOptionUI::Clone_GameObject(void * pArg)
{
	CWeaponBuyPopupOptionUI* pInstance = new CWeaponBuyPopupOptionUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CWeaponBuyPopupOptionUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pCollider);
	
	CUI::Free();
}
