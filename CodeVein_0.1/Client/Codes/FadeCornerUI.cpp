#include "stdafx.h"
#include "FadeCornerUI.h"

CFadeCornerUI::CFadeCornerUI(_Device pGraphic_Device)
	: CUI(pGraphic_Device)
{
}

CFadeCornerUI::CFadeCornerUI(const CFadeCornerUI & rhs)
	: CUI(rhs)
{
}

HRESULT CFadeCornerUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CFadeCornerUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);

	m_fPosX = 640;
	m_fPosY = 360.f;
	m_fSizeX = 1280.f;
	m_fSizeY = 720.f;

	m_bEnable = false;
	return NOERROR;
}

_int CFadeCornerUI::Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	if (m_bOnLifeTime)
	{
		m_fLifeTime_Cur -= (_float)TimeDelta;
		m_fAlphaPercent = m_fLifeTime_Cur / m_fLifeTime_Max;

		if (m_fLifeTime_Cur <= 0.f || m_fAlphaPercent <= 0.f)
		{
			m_fLifeTime_Max = 0.f;
			m_fLifeTime_Cur = 0.f;
			m_fAlphaPercent = 0.f;
			m_bOnLifeTime = false;
			m_bEnable = false;
		}
	}

	CGameObject::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);

	m_matWorld._11 = m_fSizeX;
	m_matWorld._22 = m_fSizeY;
	m_matWorld._33 = 0.1f;
	m_matWorld._41 = m_fPosX - WINCX * 0.5f;
	m_matWorld._42 = -m_fPosY + WINCY * 0.5f;

	return NO_EVENT;
}

_int CFadeCornerUI::Late_Update_GameObject(_double TimeDelta)
{
	return NO_EVENT;
}

HRESULT CFadeCornerUI::Render_GameObject()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);

	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);


	if (FAILED(SetUp_ConstantTable(2)))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();

	m_pShaderCom->Begin_Pass(13);

	m_pBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();

	m_pShaderCom->End_Shader();

	return NOERROR;
}

HRESULT CFadeCornerUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_Black_Corner", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CFadeCornerUI::SetUp_ConstantTable(_uint iIndex)
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
	if (FAILED(m_pShaderCom->Set_Value("g_fAlpha", &m_fAlphaPercent, sizeof(_float))))
		return E_FAIL;

	return NOERROR;
}

CFadeCornerUI * CFadeCornerUI::Create(_Device pGraphic_Device)
{
	CFadeCornerUI* pInstance = new CFadeCornerUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("CFadeCornerUI Creating Fail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFadeCornerUI::Clone_GameObject(void * pArg)
{
	CFadeCornerUI* pInstance = new CFadeCornerUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CFadeCornerUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFadeCornerUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
