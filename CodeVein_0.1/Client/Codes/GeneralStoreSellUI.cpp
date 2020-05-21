#include "stdafx.h"
#include "..\Headers\GeneralStoreSellUI.h"
#include "UI_Manager.h"


CGeneralStoreSellUI::CGeneralStoreSellUI(_Device pDevice)
	: CUI(pDevice)
{
}

CGeneralStoreSellUI::CGeneralStoreSellUI(const CGeneralStoreSellUI & rhs)
	: CUI(rhs)
{
}

HRESULT CGeneralStoreSellUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CGeneralStoreSellUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	m_fPosX = WINCX * 0.5f;
	m_fPosY = WINCY * 0.5f;
	m_fSizeX = 1280.f;
	m_fSizeY = 700.f;

	SetUp_Default();

	return NOERROR;
}

_int CGeneralStoreSellUI::Update_GameObject(_double TimeDelta)
{
	if (m_bIsActive && !m_bIsSubActive)
	{
		m_pExpendSellUI->Set_Active(true);
		SetUp_SubUI_Active(true);
		m_bIsSubActive = true;
	}
	else if (!m_bIsActive && m_bIsSubActive)
	{
		m_pExpendSellUI->Set_Active(false);
		m_pMtrlSellUI->Set_Active(false);
		SetUp_SubUI_Active(false);
		m_bIsSubActive = false;
	}
	if (!m_bIsActive)
		return NO_EVENT;
	CUI::Update_GameObject(TimeDelta);
	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	Click_SubUI();
	
	return NO_EVENT;
}

_int CGeneralStoreSellUI::Late_Update_GameObject(_double TimeDelta)
{
	if (!m_bIsActive)
		return NO_EVENT;
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);

	m_matWorld._11 = m_fSizeX;
	m_matWorld._22 = m_fSizeY;
	m_matWorld._33 = 1.f;
	m_matWorld._41 = m_fPosX - WINCX * 0.5f;
	m_matWorld._42 = -m_fPosY + WINCY * 0.5f;

	return NO_EVENT;
}

HRESULT CGeneralStoreSellUI::Render_GameObject()
{
	if (!m_bIsActive)
		return NOERROR;
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);
	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);


	if (FAILED(SetUp_ConstantTable(1)))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(1);
	m_pBufferCom->Render_VIBuffer();
	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return NOERROR;
}

HRESULT CGeneralStoreSellUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_GeneralStoreUI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CGeneralStoreSellUI::SetUp_ConstantTable(_uint iIndex)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, iIndex)))
		return E_FAIL;

	return NOERROR;
}

void CGeneralStoreSellUI::SetUp_Default()
{
	LOOP(2)
	{
		m_pShopIcon[i] = static_cast<CInventory_Icon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_InvenIcon", nullptr));
		g_pManagement->Add_GameOject_ToLayer_NoClone(m_pShopIcon[i], SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
		m_pShopIcon[i]->Set_UI_Pos(80.f + 80.f * i, 130.f);
		m_pShopIcon[i]->Set_UI_Size(40.f, 40.f);
	}
	m_pShopIcon[0]->Set_Type(CInventory_Icon::ICON_TYPE::ICON_EXPEND);
	m_pShopIcon[1]->Set_Type(CInventory_Icon::ICON_TYPE::ICON_MTRL);

	// 소비템 판매
	m_pExpendSellUI = static_cast<CExpendSellCollectionUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_ExpendSellCollectionUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pExpendSellUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	// 재료 판매
	m_pMtrlSellUI = static_cast<CMaterialSellCollectionUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_MaterialSellCollectionUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pMtrlSellUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
}

void CGeneralStoreSellUI::Click_SubUI()
{
	if (!m_bIsActive)
		return;

	if (m_pShopIcon[0]->Pt_InRect() && g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
	{
		m_pExpendSellUI->Set_Active(true);
		m_pMtrlSellUI->Set_Active(false);
	}
	else if (m_pShopIcon[1]->Pt_InRect() && g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
	{
		m_pExpendSellUI->Set_Active(false);
		m_pMtrlSellUI->Set_Active(true);
	}

	if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_RB))
		m_bIsActive = false;
}

void CGeneralStoreSellUI::SetUp_SubUI_Active(_bool bIsActive)
{
	LOOP(2)
	{
		m_pShopIcon[i]->Set_Active(bIsActive);
	}

	
}

CGeneralStoreSellUI * CGeneralStoreSellUI::Create(_Device pGraphic_Device)
{
	CGeneralStoreSellUI* pInstance = new CGeneralStoreSellUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CGeneralStoreSellUI::Clone_GameObject(void * pArg)
{
	CGeneralStoreSellUI* pInstance = new CGeneralStoreSellUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CGeneralStoreSellUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
