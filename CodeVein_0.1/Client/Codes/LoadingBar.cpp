#include "stdafx.h"
#include "..\Headers\LoadingBar.h"


CLoadingBar::CLoadingBar(_Device pDevice)
	: CUI(pDevice)
{
}

CLoadingBar::CLoadingBar(const CLoadingBar & rhs)
	: CUI(rhs)
{
}

HRESULT CLoadingBar::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	
	return NOERROR;
}

HRESULT CLoadingBar::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);
	m_fPosX = WINCX * 0.9f;
	m_fPosY = WINCY * 0.9f;
	m_fSizeX = 100.f;
	m_fSizeY = 100.f;
	m_fViewZ = -2.f;

	return NOERROR;
}

_int CLoadingBar::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);
	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	if(!m_bIsFinish)
		m_dLoadingAni += 10.0 * TimeDelta;

	if (m_dLoadingAni >= 10.0)
		m_dLoadingAni = 0.0;

	return NO_EVENT;
}

_int CLoadingBar::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CLoadingBar::Render_GameObject()
{
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

	return NOERROR;
}

HRESULT CLoadingBar::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_LoadingBar", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CLoadingBar::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, _uint(m_dLoadingAni))))
		return E_FAIL;

	return NOERROR;
}

CLoadingBar * CLoadingBar::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLoadingBar* pInstance = new CLoadingBar(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("CLoadingBar Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLoadingBar::Clone_GameObject(void * pArg)
{
	CLoadingBar* pInstance = new CLoadingBar(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("CLoadingBar Clone Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoadingBar::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
