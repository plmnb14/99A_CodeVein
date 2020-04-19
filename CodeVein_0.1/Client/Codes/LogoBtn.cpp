#include "stdafx.h"
#include "..\Headers\LogoBtn.h"
#include "CursorEffect.h"

CLogoBtn::CLogoBtn(_Device pGraphic_Device)
	: CUI(pGraphic_Device)
{
}

CLogoBtn::CLogoBtn(const CLogoBtn & rhs)
	: CUI(rhs)
{
}

HRESULT CLogoBtn::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CLogoBtn::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);

	m_fPosX = WINCX * 0.5f;
	m_fPosY = 470.f;
	m_fSizeX = 237.f;
	m_fSizeY = 44.f;
	
	m_fViewZ = 0.1f;
	m_fAlpha = 0.f;
	m_bIsActive = false;
	if (FAILED(SetUp_CursorEffect()))
		return E_FAIL;
	
	return NOERROR;
}

_int CLogoBtn::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	m_bIsColl = Coll_Mouse();

	m_pCursorEffect->Set_Active(m_bIsColl && m_bIsActive);
	
	if (m_bIsActive)
	{
		if (m_fAlpha < 1.f)
			m_fAlpha += 0.2f * (_float)TimeDelta;
	}

	return NO_EVENT;
}

_int CLogoBtn::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CLogoBtn::Render_GameObject()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);

	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

	if (FAILED(SetUp_ConstantTable(3)))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();

	m_pShaderCom->Begin_Pass(5);

	m_pBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();

	m_pShaderCom->End_Shader();
	
	return NOERROR;
}

HRESULT CLogoBtn::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"DefaultTex_LogoButton", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	

	return NOERROR;
}

HRESULT CLogoBtn::SetUp_ConstantTable(_uint iIndex)
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
	if (FAILED(m_pShaderCom->Set_Value("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;
	return NOERROR;
}

HRESULT CLogoBtn::SetUp_CursorEffect()
{
	UI_DESC* pDesc = new UI_DESC;
	pDesc->fPosX = m_fPosX;
	pDesc->fPosY = m_fPosY + 5.f;
	pDesc->fSizeX = m_fSizeX;
	pDesc->fSizeY = m_fSizeY;
	pDesc->iIndex = 0;
	if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_CursorEffect", SCENE_LOGO, L"Layer_CursorEffect", pDesc)))
		return E_FAIL;
	m_pCursorEffect = static_cast<CCursorEffect*>(g_pManagement->Get_GameObjectBack(L"Layer_CursorEffect", SCENE_LOGO));
	if (nullptr == m_pCursorEffect)
		return E_FAIL;

	m_pCursorEffect->Set_Active(false);
	m_pCursorEffect->Set_ViewZ(m_fViewZ + 0.1f);
	return NOERROR;
}


_bool CLogoBtn::Coll_Mouse()
{
	POINT pt = {};

	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	_float fMouseX = static_cast<float>(pt.x);
	_float fMouseY = static_cast<float>(pt.y);

	if (fMouseX < (m_fPosX - m_fSizeX * 0.5f) ||
		fMouseX >(m_fPosX + m_fSizeX * 0.5f) ||
		fMouseY < (m_fPosY - m_fSizeY * 0.5f) ||
		fMouseY >(m_fPosY + m_fSizeY * 0.5f))
		return false;

	return true;
}

CLogoBtn * CLogoBtn::Create(_Device pGraphic_Device)
{
	CLogoBtn* pInstance = new CLogoBtn(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("LogoBtn Creating Fail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLogoBtn::Clone_GameObject(void * pArg)
{
	CLogoBtn* pInstance = new CLogoBtn(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned LogoBtn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLogoBtn::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
