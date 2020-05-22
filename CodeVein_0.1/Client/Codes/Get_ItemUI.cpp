#include "stdafx.h"
#include "Get_ItemUI.h"
#include "..\Headers\Get_ItemUI.h"

#include "UI_Manager.h"

#include "PickUp_ItemUI.h"


CGet_ItemUI::CGet_ItemUI(_Device Graphic_Device)
	: CUI(Graphic_Device)
{
}

CGet_ItemUI::CGet_ItemUI(const CGet_ItemUI& rhs)
	: CUI(rhs)

{
}

HRESULT CGet_ItemUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CGet_ItemUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);

	m_fPosX = WINCX * 0.5f;
	m_fPosY = WINCY - 100.f;

	m_fSizeX = 269.f;
	m_fSizeY = 81.f;

	return S_OK;
}

_int CGet_ItemUI::Update_GameObject(_double TimeDelta)
{
	CGameObject::LateInit_GameObject();
	CUI::Update_GameObject(TimeDelta);

	SetUp_State(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	return S_OK;
}

_int CGet_ItemUI::Late_Update_GameObject(_double TimeDelta)
{
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);

	m_matWorld._11 = m_fSizeX;
	m_matWorld._22 = m_fSizeY;
	m_matWorld._33 = 1.f;
	m_matWorld._41 = m_fPosX - WINCX * 0.5f;
	m_matWorld._42 = -m_fPosY + WINCY * 0.5f;

	return S_OK;
}

HRESULT CGet_ItemUI::LateInit_GameObject()
{
	return S_OK;
}

HRESULT CGet_ItemUI::Render_GameObject()
{
	if (nullptr == m_pShaderCom || nullptr == m_pBufferCom)
		return E_FAIL;

	//if (false == m_bIsActive)
	//	return NOERROR;

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);

	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

	m_pShaderCom->Begin_Shader();

	m_pShaderCom->Begin_Pass(1);

	if (FAILED(SetUp_ConstantTable(m_iUINumber)))
		return E_FAIL;
		
	m_pShaderCom->Commit_Changes();
	m_pBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();

	m_pShaderCom->End_Shader();

	return S_OK;
}

HRESULT CGet_ItemUI::Add_Component()
{
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_Get_ItemUI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGet_ItemUI::SetUp_ConstantTable(_uint TextureIndex)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_fPosX", &m_fPosX, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_fSizeX", &m_fSizeX, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_fPercentage", &m_fPercentage, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, TextureIndex)))
		return E_FAIL;

	return S_OK;
}

void CGet_ItemUI::SetUp_State(_double TimeDelta)
{
	CUI_Manager*	pUIManager = CUI_Manager::Get_Instance();
	CItem_Manager*	pItemManager = CItem_Manager::Get_Instance();

	m_fPercentage = m_fNowItemBar_Size / m_fSizeX;

	if (g_pInput_Device->Key_Pressing(DIK_Z))
		m_iUINumber = 1;
	if (g_pInput_Device->Key_Up(DIK_Z))
	{
		m_iUINumber = 0;
		++m_iPickUp_ItemNumber;
		pUIManager->Set_CoundItem(m_iPickUp_ItemNumber);
		m_bShow_GetItemName = true;
		pItemManager->Set_PickUI(true);
	}
	else
		m_bShow_GetItemName = false;

	if (1 == m_iUINumber && true == m_bShow_GetItemName)
		m_iUINumber = 0;
}

CGet_ItemUI * CGet_ItemUI::Create(_Device pGraphic_Device)
{
	CGet_ItemUI* pInstance = new CGet_ItemUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CGet_ItemUI::Clone_GameObject(void * pArg)
{
	CGet_ItemUI* pInstance = new CGet_ItemUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CGet_ItemUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}