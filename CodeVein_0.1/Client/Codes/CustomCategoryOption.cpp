#include "stdafx.h"
#include "..\Headers\CustomCategoryOption.h"
#include "CursorEffect.h"

CCustomCategoryOption::CCustomCategoryOption(_Device pGraphic_Device)
	: CUI(pGraphic_Device)
{
}

CCustomCategoryOption::CCustomCategoryOption(const CCustomCategoryOption & rhs)
	: CUI(rhs)
{
}

HRESULT CCustomCategoryOption::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CCustomCategoryOption::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);

	m_fPosX = WINCX * 0.5f;
	m_fPosY = 470.f;
	m_fSizeX = 237.f;
	m_fSizeY = 44.f;
	
	m_fViewZ = 0.1f;
	m_bIsActive = false;
	
	return NOERROR;
}

_int CCustomCategoryOption::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);
	m_bIsColl = Coll_Mouse();

	return NO_EVENT;
}

_int CCustomCategoryOption::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CCustomCategoryOption::Render_GameObject()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);

	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

	if (FAILED(SetUp_ConstantTable(m_iIndex)))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();

	m_pShaderCom->Begin_Pass(1);

	m_pBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();

	m_pShaderCom->End_Shader();
	
	return NOERROR;
}

HRESULT CCustomCategoryOption::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"DefaultTex_Custom_UI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	

	return NOERROR;
}

HRESULT CCustomCategoryOption::SetUp_ConstantTable(_uint iIndex)
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

_bool CCustomCategoryOption::Coll_Mouse()
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

CCustomCategoryOption * CCustomCategoryOption::Create(_Device pGraphic_Device)
{
	CCustomCategoryOption* pInstance = new CCustomCategoryOption(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("CCustomCategoryOption Creating Fail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCustomCategoryOption::Clone_GameObject(void * pArg)
{
	CCustomCategoryOption* pInstance = new CCustomCategoryOption(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned LogoBtn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCustomCategoryOption::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
