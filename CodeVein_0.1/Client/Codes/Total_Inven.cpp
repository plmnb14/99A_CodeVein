#include "stdafx.h"
#include "..\Headers\Total_Inven.h"
#include "Inventory_Icon.h"

CTotal_Inven::CTotal_Inven(_Device pDevice)
	: CUI(pDevice)
{
}

CTotal_Inven::CTotal_Inven(const CTotal_Inven & rhs)
	: CUI(rhs)
{
}

HRESULT CTotal_Inven::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CTotal_Inven::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	m_fPosX = WINCX * 0.3f;
	m_fPosY = 200.f;
	m_fSizeX = 543.f;
	m_fSizeY = 502.f;
	m_fViewZ = 5.f;

	m_bIsActive = false;

	//SetUp_Default();

	return NOERROR;
}

_int CTotal_Inven::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.0f);

	Click_Icon();

	//m_pIcon->Set_Active(m_bIsActive);
	
	return NO_EVENT;
}

_int CTotal_Inven::Late_Update_GameObject(_double TimeDelta)
{
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);

	m_matWorld._11 = m_fSizeX;
	m_matWorld._22 = m_fSizeY;
	m_matWorld._33 = 1.f;
	m_matWorld._41 = m_fPosX - WINCX * 0.5f;
	m_matWorld._42 = -m_fPosY + WINCY * 0.5f;
	m_matWorld._42 = 1.f;

	return NO_EVENT;
}

HRESULT CTotal_Inven::Render_GameObject()
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

HRESULT CTotal_Inven::Add_Component()
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

HRESULT CTotal_Inven::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, 3)))
		return E_FAIL;

	return NOERROR;
}

void CTotal_Inven::SetUp_Default()
{
	CUI::UI_DESC* pDesc = nullptr;
	
	pDesc = new CUI::UI_DESC;
	pDesc->fPosX = m_fPosX - 300.f;
	pDesc->fPosY = m_fPosY - 100.f;
	pDesc->fSizeX = 45.f;
	pDesc->fSizeY = 55.f;
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_InvenIcon", SCENE_STAGE, L"Layer_InvenIcon", pDesc);
	m_pIcon = static_cast<CInventory_Icon*>(g_pManagement->Get_GameObjectBack(L"Layer_InvenIcon", SCENE_STAGE));
	m_pIcon->Set_Type(CInventory_Icon::ICON_ALL);
}

void CTotal_Inven::Click_Icon()
{
	if (!m_bIsActive)
		return;

	/*if (m_pIcon->Pt_InRect() &&
		g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
	{
		m_bIsDetail = true;
	}*/
	
}

CTotal_Inven * CTotal_Inven::Create(_Device pGraphic_Device)
{
	CTotal_Inven* pInstance = new CTotal_Inven(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CTotal_Inven::Clone_GameObject(void * pArg)
{
	CTotal_Inven* pInstance = new CTotal_Inven(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CTotal_Inven::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	CUI::Free();
}
