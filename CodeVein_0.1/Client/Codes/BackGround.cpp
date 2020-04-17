#include "stdafx.h"
#include "..\Headers\BackGround.h"

CBackGround::CBackGround(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI(pGraphic_Device)
{

}

CBackGround::CBackGround(const CBackGround & rhs)
	: CUI(rhs)
{

}

HRESULT CBackGround::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CBackGround::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);

	m_fPosX = WINCX * 0.5f;
	m_fPosY = WINCY * 0.5f;
	m_fSizeX = WINCX;
	m_fSizeY = WINCY;
	m_iIndex = 3;
	m_fViewZ = 10.f;
	return NOERROR;
}

_int CBackGround::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	m_fSpeed += -0.02f * _float(TimeDelta);

	return NO_EVENT;
}

_int CBackGround::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CBackGround::Render_GameObject()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);

	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

	_uint iTexNum = 0;
	_uint iPass = 0;
	
	LOOP(2)
	{
		if (1 == i)
		{
			iTexNum = 1;
			iPass = 1;
		}
		else
		{
			iTexNum = m_iIndex;
			iPass = 7;
		}
		if (FAILED(SetUp_ConstantTable(iTexNum)))
			return E_FAIL;

		m_pShaderCom->Begin_Shader();

		m_pShaderCom->Begin_Pass(iPass);

		m_pBufferCom->Render_VIBuffer();

		m_pShaderCom->End_Pass();

		m_pShaderCom->End_Shader();
	}
	
	return NOERROR;
}

HRESULT CBackGround::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_LogoBack", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;
	
	return NOERROR;
}

HRESULT CBackGround::SetUp_ConstantTable(_uint iIndex)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (iIndex == 1)
	{
		if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))

			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
			return E_FAIL;

		if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, iIndex)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_fSpeed", &m_fSpeed, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, iIndex)))
			return E_FAIL;
	}

	return NOERROR;
}

CBackGround * CBackGround::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBackGround*	pInstance = new CBackGround(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject * CBackGround::Clone_GameObject(void * pArg)
{
	CBackGround*	pInstance = new CBackGround(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBackGround::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
