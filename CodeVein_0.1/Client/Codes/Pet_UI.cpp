#include "stdafx.h"
#include "..\Headers\Pet_UI.h"

CPet_UI::CPet_UI(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CUI(pGraphic_Device)
{
}

CPet_UI::CPet_UI(const CPet_UI & rhs)
	:CUI(rhs)
{
}

HRESULT CPet_UI::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CPet_UI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	m_fPosX = 900.f;
	m_fPosY = 300.f;
	m_fSizeX = 512.f;
	m_fSizeY = 512.f;
	m_fViewZ = 1.f;

	m_bIsActive = false;

	SetUp_Default();

	return S_OK;
}

_int CPet_UI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);
	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	//for (auto& iter : m_vecConditionUI)
	//{
	//	iter->Set_Active(m_bIsActive);
	//}

	Close_UI();

	return NO_EVENT;
}

_int CPet_UI::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CPet_UI::Render_GameObject()
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

	return S_OK;
}

void CPet_UI::Close_UI()
{
	//if (!m_pExitUI->Get_Active())
	//	return;

	//if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB) &&
	//	g_pInput_Device->MousePt_InRect(m_pExitUI->Get_UI_Pos().x, m_pExitUI->Get_UI_Pos().y, m_pExitUI->Get_UI_Size().x, m_pExitUI->Get_UI_Size().y, g_hWnd))
	//{
	//	CUI_Manager::Get_Instance()->Get_Total_Inven()->Set_Active(true);
	//	m_bIsActive = false;
	//}

	return;
}

HRESULT CPet_UI::Add_Component()
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

	return S_OK;
}

HRESULT CPet_UI::SetUp_Default()
{

	return S_OK;
}

HRESULT CPet_UI::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, 0)))
		return E_FAIL;

	return S_OK;
}

CPet_UI * CPet_UI::Create(_Device pGraphic_Device)
{
	CPet_UI* pInstance = new CPet_UI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CPet_UI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPet_UI::Clone_GameObject(void * pArg)
{
	CPet_UI* pInstance = new CPet_UI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CPet_UI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPet_UI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	CUI::Free();

	return;
}
