#include "stdafx.h"
#include "..\Headers\BloodCodeSlot.h"


CBloodCodeSlot::CBloodCodeSlot(_Device pDevice)
	: CUI(pDevice)
{
}

CBloodCodeSlot::CBloodCodeSlot(const CBloodCodeSlot & rhs)
	: CUI(rhs)
{
}

HRESULT CBloodCodeSlot::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CBloodCodeSlot::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	return NOERROR;
}

_int CBloodCodeSlot::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_ALPHA, this);

	_v3 vWorldPos;
	memcpy(vWorldPos, &m_pTransformCom->Get_WorldMat()._41, sizeof(_v3));
	Compute_ViewZ(&vWorldPos);

	SetUp_BloodCodeIdx();

	m_pCollider->Update(m_pTransformCom->Get_Pos());

	return NO_EVENT;
}

_int CBloodCodeSlot::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	m_matView = g_pManagement->Get_Transform(D3DTS_VIEW);
	m_matProj = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	return NO_EVENT;
}

HRESULT CBloodCodeSlot::Render_GameObject()
{
	if (!m_bIsActive)
		return NOERROR;

	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	_uint iIndex = 0;
	_uint iPass = 0;

	if (!m_bIsSelect)
	{
		LOOP(2)
		{
			(0 == i) ? (iIndex = 0) && (iPass = 1) : (iIndex = m_iIndex) && (iPass = 1);

			if (FAILED(SetUp_ConstantTable(iIndex)))
				return E_FAIL;

			m_pShaderCom->Begin_Shader();
			m_pShaderCom->Begin_Pass(iPass);

			m_pBufferCom->Render_VIBuffer();
			m_pShaderCom->End_Pass();
			m_pShaderCom->End_Shader();
		}
	}
	else
	{
		LOOP(3)
		{
			if (0 == i)
			{
				iIndex = 0;
				iPass = 0;
			}
			else if (1 == i)
			{
				iIndex = m_iIndex;
				iPass = 1;
			}
			else if (2 == i)
			{
				iIndex = 1;
				iPass = 1;
			}
			if (FAILED(SetUp_ConstantTable(iIndex)))
				return E_FAIL;

			m_pShaderCom->Begin_Shader();
			m_pShaderCom->Begin_Pass(iPass);

			m_pBufferCom->Render_VIBuffer();
			m_pShaderCom->End_Pass();
			m_pShaderCom->End_Shader();
		}
	}
	

	return NOERROR;
}

HRESULT CBloodCodeSlot::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_BloodCodeIcon", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_3dUI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	// for.Com_Collider
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	m_pCollider->Set_Radius(_v3{ 0.15f, 0.111f, 0.1f });
	m_pCollider->Set_Dynamic(true);
	m_pCollider->Set_Type(COL_SPHERE);
	m_pCollider->Set_CenterPos(m_pTransformCom->Get_Pos());

	return NOERROR;
}

HRESULT CBloodCodeSlot::SetUp_ConstantTable(_uint iIndex)
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

void CBloodCodeSlot::SetUp_Default()
{
}

void CBloodCodeSlot::SetUp_BloodCodeIdx()
{
	switch (m_eID)
	{
	case BloodCode_Artemis:
	{
		m_iIndex = 2;
	}
	break;
	case BloodCode_Assassin:
	{
		m_iIndex = 3;
	}
	break;
	case BloodCode_DarkKnight:
	{
		m_iIndex = 5;
	}
	break;
	case BloodCode_Queen:
	{
		m_iIndex = 12;
	}
	break;
	case BloodCode_Berserker:
	{
		m_iIndex = 10;
	}
	break;
	case BloodCode_Hephaestus:
	{
		m_iIndex = 8;
	}
	break;
	case BloodCode_Fighter:
	{
		m_iIndex = 6;
	}
	break;
	case BloodCode_Heimdal:
	{
		m_iIndex = 7;
	}
	break;
	case BloodCode_Hermes:
	{
		m_iIndex = 9;
	}
	break;
	case BloodCode_Atlas:
	{
		m_iIndex = 4;
	}
	break;
	case BloodCode_Prometheus:
	{
		m_iIndex = 11;
	}
	break;
	}
}

CBloodCodeSlot * CBloodCodeSlot::Create(_Device pGraphic_Device)
{
	CBloodCodeSlot* pInstance = new CBloodCodeSlot(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CBloodCodeSlot::Clone_GameObject(void * pArg)
{
	CBloodCodeSlot* pInstance = new CBloodCodeSlot(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CBloodCodeSlot::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pCollider);

	CUI::Free();
}
