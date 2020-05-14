#include "stdafx.h"
#include "..\Headers\WeaponShopOptionUI.h"


CWeaponShopOptionUI::CWeaponShopOptionUI(_Device pDevice)
	: CUI(pDevice)
{
}

CWeaponShopOptionUI::CWeaponShopOptionUI(const CWeaponShopOptionUI & rhs)
	: CUI(rhs)
{
}

HRESULT CWeaponShopOptionUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CWeaponShopOptionUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);
	m_bIsActive = false;
	
	m_pCollider->Set_Radius(_v3{ 0.5f, 0.07f, 0.1f });
	m_pCollider->Set_Dynamic(true);
	m_pCollider->Set_Type(COL_SPHERE);
	m_pCollider->Set_CenterPos(m_pTransformCom->Get_Pos());
	
	return NOERROR;
}

_int CWeaponShopOptionUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	if (m_bIsDead)
		return DEAD_OBJ;

	if (!m_bIsActive)
		return NOERROR;

	m_pRendererCom->Add_RenderList(RENDER_3DUI, this);
	
	_v3 vWorldPos;
	memcpy(vWorldPos, &m_pTransformCom->Get_WorldMat()._41, sizeof(_v3));
	Compute_ViewZ(&vWorldPos);

	m_pCollider->Update(m_pTransformCom->Get_Pos());

	if (MENU_NONE == m_eMenu)
	{
		switch (m_eOption)
		{
		case Client::CWeaponShopOptionUI::OPTION_UPGRADE:
			m_iIndex = 2;
			break;
		case Client::CWeaponShopOptionUI::OPTION_BUY:
			m_iIndex = 3;
			break;
		case Client::CWeaponShopOptionUI::OPTION_SELL:
			m_iIndex = 4;
			break;
		case Client::CWeaponShopOptionUI::OPTION_BYEBYE:
			m_iIndex = 5;
			break;
		case Client::CWeaponShopOptionUI::OPTION_END:
			break;
		}
	}
	
	if (MENU_BUY == m_eMenu)
	{
		switch (m_eBuyOption)
		{
		case Client::CWeaponShopOptionUI::OPTION_BUY_WEAPON:
			m_iIndex = 8;
			break;
		case Client::CWeaponShopOptionUI::OPTION_BUY_DRAINWEAPON:
			m_iIndex = 9;
			break;
		case Client::CWeaponShopOptionUI::OPTION_BUY_END:
			break;
		}
	}

	if (MENU_SELL == m_eMenu)
	{
		switch (m_eBuyOption)
		{
		case Client::CWeaponShopOptionUI::OPTION_BUY_WEAPON:
			m_iIndex = 10;
			break;
		case Client::CWeaponShopOptionUI::OPTION_BUY_DRAINWEAPON:
			m_iIndex = 11;
			break;
		case Client::CWeaponShopOptionUI::OPTION_BUY_ITEM:
			m_iIndex = 12;
			break;
		case Client::CWeaponShopOptionUI::OPTION_BUY_END:
			break;
		}
	}

	if (MENU_UPGRADE == m_eMenu)
	{
		switch (m_eBuyOption)
		{
		case Client::CWeaponShopOptionUI::OPTION_BUY_WEAPON:
			m_iIndex = 13;
			break;
		case Client::CWeaponShopOptionUI::OPTION_BUY_DRAINWEAPON:
			m_iIndex = 14;
			break;
		}
	}
	
	return NO_EVENT;
}

_int CWeaponShopOptionUI::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	m_matView = g_pManagement->Get_Transform(D3DTS_VIEW);
	m_matProj = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	return NO_EVENT;
}

HRESULT CWeaponShopOptionUI::Render_GameObject()
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
		if (FAILED(SetUp_ConstantTable(iIndex)))
			return E_FAIL;

		m_pShaderCom->Begin_Shader();
		m_pShaderCom->Begin_Pass(1);

		m_pBufferCom->Render_VIBuffer();
		m_pShaderCom->End_Pass();
		m_pShaderCom->End_Shader();
	}
	else
	{
		LOOP(2)
		{
			(i == 0) ? (iIndex = 1) : (iIndex = m_iIndex);

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

HRESULT CWeaponShopOptionUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_WeaponShopUI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_3dUI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	// for.Com_Collider
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	

	return NOERROR;
}

HRESULT CWeaponShopOptionUI::SetUp_ConstantTable(_uint iIndex)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, _uint(iIndex))))
		return E_FAIL;

	return NOERROR;
}

CWeaponShopOptionUI * CWeaponShopOptionUI::Create(_Device pGraphic_Device)
{
	CWeaponShopOptionUI* pInstance = new CWeaponShopOptionUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CWeaponShopOptionUI::Clone_GameObject(void * pArg)
{
	CWeaponShopOptionUI* pInstance = new CWeaponShopOptionUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CWeaponShopOptionUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pCollider);

	CUI::Free();
}
