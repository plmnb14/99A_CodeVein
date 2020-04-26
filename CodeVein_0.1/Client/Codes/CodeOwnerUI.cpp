#include "stdafx.h"
#include "..\Headers\CodeOwnerUI.h"


CCodeOwnerUI::CCodeOwnerUI(_Device pDevice)
	: CUI(pDevice)
{
}

CCodeOwnerUI::CCodeOwnerUI(const CCodeOwnerUI & rhs)
	: CUI(rhs)
{
}

HRESULT CCodeOwnerUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CCodeOwnerUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	m_eBloodCode = BloodCode_Prometheus;
	m_bIsSelect = true;

	m_fAlpha = 1.f;
	return NOERROR;
}

_int CCodeOwnerUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	

	Compute_ViewZ(&m_pTransformCom->Get_Pos());

	switch (m_eBloodCode)
	{
	case BloodCode_Fighter:
	{
		if (!m_bIsSelect)
		{
			m_iIndex = 0;
		}
		else
		{
			m_iIndex = 1;
		}
	}
	break;
	case BloodCode_Caster:
	{
		if (!m_bIsSelect)
		{
			m_iIndex = 2;
		}
		else
		{
			m_iIndex = 3;
		}
	}
	break;

	case BloodCode_Berserker:
	{
		if (!m_bIsSelect)
		{
			m_iIndex = 4;
		}
		else
		{
			m_iIndex = 5;
		}
	}
	break;

	case BloodCode_Prometheus:
	{
		if (!m_bIsSelect)
		{
			m_iIndex = 6;
		}
		else
		{
			m_iIndex = 7;
		}
	}
	break;

	case BloodCode_Eos:
	{
		if (!m_bIsSelect)
		{
			m_iIndex = 8;
		}
		else
		{
			m_iIndex = 9;
		}
	}
	break;

	default:
		
		break;
	}

	return NO_EVENT;
}

_int CCodeOwnerUI::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	m_matView = g_pManagement->Get_Transform(D3DTS_VIEW);
	m_matProj = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	return NO_EVENT;
}

HRESULT CCodeOwnerUI::Render_GameObject()
{
	if (!m_bIsActive)
		return NOERROR;

	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;


	if (FAILED(SetUp_ConstantTable(0)))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(8);

	m_pBufferCom->Render_VIBuffer();
	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();
	


	return NOERROR;
}

HRESULT CCodeOwnerUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_BloodCodeOwner", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CCodeOwnerUI::SetUp_ConstantTable(_uint iIndex)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
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

CCodeOwnerUI * CCodeOwnerUI::Create(_Device pGraphic_Device)
{
	CCodeOwnerUI* pInstance = new CCodeOwnerUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CCodeOwnerUI::Clone_GameObject(void * pArg)
{
	CCodeOwnerUI* pInstance = new CCodeOwnerUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CCodeOwnerUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
