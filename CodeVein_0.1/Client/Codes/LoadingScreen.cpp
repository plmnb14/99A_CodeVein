#include "stdafx.h"
#include "..\Headers\LoadingScreen.h"
#include "LoadingBar.h"

CLoadingScreen::CLoadingScreen(_Device pDevice)
	: CUI(pDevice)
{
}

CLoadingScreen::CLoadingScreen(const CLoadingScreen & rhs)
	: CUI(rhs)
{
}

HRESULT CLoadingScreen::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CLoadingScreen::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	m_fPosX = WINCX * 0.5f;
	m_fPosY = WINCY * 0.5f;
	m_fSizeX = WINCX;
	m_fSizeY = WINCY;
	m_fViewZ = -1.f;
	
	m_iIndex = 1;
	m_fAlpha = 0.f;
	m_fFadeSpeed = 0.3f;
	return NOERROR;
}

_int CLoadingScreen::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);
	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	if (m_bIsActive)
	{
		if (m_fAlpha < 1.f)
		{
			m_fAlpha += _float(TimeDelta) * m_fFadeSpeed;
			m_bIsLoad = false;
		}
		else
		{
			m_bIsLoad = true;
		}
			
	}
	else
	{
		if (m_fAlpha > 0.f)
		{
			m_fAlpha -= _float(TimeDelta) * m_fFadeSpeed;
		}
	}
		
	return NO_EVENT;
}

_int CLoadingScreen::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CLoadingScreen::Render_GameObject()
{
	if (!m_bIsActive && m_fAlpha <= 0.f)
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

HRESULT CLoadingScreen::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"DefaultTex_LoadingScreen", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CLoadingScreen::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, m_iIndex)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	return NOERROR;
}

CLoadingScreen * CLoadingScreen::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLoadingScreen* pInstance = new CLoadingScreen(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("LoadingScreen Create Failed");
		Safe_Release(pInstance);
	}
		
	return pInstance;
}

CGameObject * CLoadingScreen::Clone_GameObject(void * pArg)
{
	CLoadingScreen* pInstance = new CLoadingScreen(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("LoadingScreen Clone Failed");
		Safe_Release(pInstance);
	}
		
	return pInstance;
}

void CLoadingScreen::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
