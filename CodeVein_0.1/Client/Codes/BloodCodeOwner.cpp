#include "stdafx.h"
#include "..\Headers\BloodCodeOwner.h"


CBloodCodeOwner::CBloodCodeOwner(_Device pDevice)
	: CUI(pDevice)
{
}

CBloodCodeOwner::CBloodCodeOwner(const CBloodCodeOwner & rhs)
	: CUI(rhs)
{
}

HRESULT CBloodCodeOwner::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CBloodCodeOwner::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);
	
	return NOERROR;
}

_int CBloodCodeOwner::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_3DUI, this);

	_v3 vWorldPos;
	memcpy(vWorldPos, &m_pTransformCom->Get_WorldMat()._41, sizeof(_v3));
	Compute_ViewZ(&vWorldPos);

	Set_BloodCodeOwner();
	
	if (m_bIsActive &&
		1.f > m_fAlpha)
		m_fAlpha += _float(TimeDelta) * 1.2f;

	if (!m_bIsActive &&
		0.f < m_fAlpha)
		m_fAlpha -= _float(TimeDelta) * 1.2f;

	

	return NO_EVENT;
}

_int CBloodCodeOwner::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	m_matView = g_pManagement->Get_Transform(D3DTS_VIEW);
	m_matProj = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	return NO_EVENT;
}

HRESULT CBloodCodeOwner::Render_GameObject()
{
	if (!m_bIsActive && m_fAlpha <= 0.f)
		return NOERROR;

	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable(m_iIndex)))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(3);

	m_pBufferCom->Render_VIBuffer();
	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return NOERROR;
}

HRESULT CBloodCodeOwner::Add_Component()
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_3dUI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CBloodCodeOwner::SetUp_ConstantTable(_uint iIndex)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, _uint(iIndex))))
		return E_FAIL;

	return NOERROR;
}

void CBloodCodeOwner::Set_BloodCodeOwner()
{
	switch (m_eID)
	{
	case BloodCode_Artemis:
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
	case BloodCode_Assassin:
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
	case BloodCode_DarkKnight:
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
	case BloodCode_Queen:
	{
		if (!m_bIsSelect)
		{
			m_iIndex = 20;
		}
		else
		{
			m_iIndex = 21;
		}
	}
	break;
	case BloodCode_Berserker:
	{
		if (!m_bIsSelect)
		{
			m_iIndex = 16;
		}
		else
		{
			m_iIndex = 17;
		}
	}
	break;
	case BloodCode_Hephaestus:
	{
		if (!m_bIsSelect)
		{
			m_iIndex = 12;
		}
		else
		{
			m_iIndex = 13;
		}
	}
	break;
	case BloodCode_Fighter:
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
	case BloodCode_Heimdal:
	{
		if (!m_bIsSelect)
		{
			m_iIndex = 10;
		}
		else
		{
			m_iIndex = 11;
		}
	}
	break;
	case BloodCode_Hermes:
	{
		if (!m_bIsSelect)
		{
			m_iIndex = 14;
		}
		else
		{
			m_iIndex = 15;
		}
	}
	break;
	case BloodCode_Atlas:
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
			m_iIndex = 18;
		}
		else
		{
			m_iIndex = 19;
		}
	}
	break;
	case BloodCode_End:
	{
		m_iIndex = 22;
	}
	break;
	}
}

CBloodCodeOwner * CBloodCodeOwner::Create(_Device pGraphic_Device)
{
	CBloodCodeOwner* pInstance = new CBloodCodeOwner(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);
	return pInstance;
}

CGameObject * CBloodCodeOwner::Clone_GameObject(void * pArg)
{
	CBloodCodeOwner* pInstance = new CBloodCodeOwner(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CBloodCodeOwner::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
