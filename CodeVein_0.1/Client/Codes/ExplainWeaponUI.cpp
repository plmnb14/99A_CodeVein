#include "stdafx.h"
#include "..\Headers\ExplainWeaponUI.h"


CExplainWeaponUI::CExplainWeaponUI(_Device pDevice)
	: CUI(pDevice)
{
}

CExplainWeaponUI::CExplainWeaponUI(const CExplainWeaponUI & rhs)
	: CUI(rhs)
{
}

HRESULT CExplainWeaponUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CExplainWeaponUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	m_tWeaponParam.iWeaponName = WPN_DATA_End;
	m_tWeaponParam.iWeaponType = WEAPON_End;
	m_tWeaponParam.iReinforce = 0;
	m_tWeaponParam.fDamage = 0.f;
	m_tWeaponParam.fRadius = 0.f;
	m_tWeaponParam.fTrail_Min = 0.f;
	m_tWeaponParam.fTrail_Max = 0.f;
	m_tWeaponParam.fCol_Height = 0.f;

	return NOERROR;
}

_int CExplainWeaponUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);
	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.0f);

	switch (m_tWeaponParam.iWeaponName)
	{
	case Wpn_SSword:
	{
		m_iInfoIdx = 1;
		m_iWeaponIconIdx = 21;
	}
		break;
	case Wpn_SSword_Black:
	{
		m_iInfoIdx = 2;
		m_iWeaponIconIdx = 22;
	}
		break;
	case Wpn_SSword_Military:
	{
		m_iInfoIdx = 3;
		m_iWeaponIconIdx = 23;
	}
		break;
	case Wpn_SSword_Slave:
	{
		m_iInfoIdx = 4;
		m_iWeaponIconIdx = 24;
	}
		break;
	case Wpn_LSword:
	{
		m_iInfoIdx = 5;
		m_iWeaponIconIdx = 25;
	}
		break;
	case Wpn_LSword_Black:
	{
		m_iInfoIdx = 6;
		m_iWeaponIconIdx = 26;
	}
		break;
	case Wpn_LSword_Military:
	{
		m_iInfoIdx = 7;
		m_iWeaponIconIdx = 27;
	}
		break;
	case Wpn_LSword_Slave:
	{
		m_iInfoIdx = 8;
		m_iWeaponIconIdx = 28;
	}
		break;
	case Wpn_Halberd:
	{
		m_iInfoIdx = 9;
		m_iWeaponIconIdx = 29;
	}
		break;
	case Wpn_Halberd_Black:
	{
		m_iInfoIdx = 10;
		m_iWeaponIconIdx = 30;
	}
		break;
	case Wpn_Halberd_Military:
	{
		m_iInfoIdx = 11;
		m_iWeaponIconIdx = 31;
	}
		break;
	case Wpn_Halberd_Slave:
	{
		m_iInfoIdx = 12;
		m_iWeaponIconIdx = 32;
	}
		break;
	case Wpn_Hammer:
	{
		m_iInfoIdx = 13;
		m_iWeaponIconIdx = 33;
	}
		break;
	case Wpn_Hammer_Black:
	{
		m_iInfoIdx = 14;
		m_iWeaponIconIdx = 34;
	}
		break;
	case Wpn_Hammer_Military:
	{
		m_iInfoIdx = 15;
		m_iWeaponIconIdx = 35;
	}
		break;
	case Wpn_Hammer_Slave:
	{
		m_iInfoIdx = 16;
		m_iWeaponIconIdx = 36;
	}
		break;
	case Wpn_Gun:
	{
		m_iInfoIdx = 17;
		m_iWeaponIconIdx = 37;
	}
		break;
	case Wpn_Gun_Black:
	{
		m_iInfoIdx = 18;
		m_iWeaponIconIdx = 38;
	}
		break;
	case Wpn_Gun_Military:
	{
		m_iInfoIdx = 19;
		m_iWeaponIconIdx = 39;
	}
		break;
	case Wpn_Gun_Slave:
	{
		m_iInfoIdx = 20;
		m_iWeaponIconIdx = 40;
	}
		break;
	default:
	{
		m_iInfoIdx = 41;
		m_iWeaponIconIdx = 41;
	}
		break;
	}

	if (!m_bIsActive)
	{
		m_tWeaponParam.iWeaponName = WPN_DATA_End;
		m_tWeaponParam.iWeaponType = WEAPON_End;
		m_tWeaponParam.iReinforce = 0;
		m_tWeaponParam.fDamage = 0.f;
		m_tWeaponParam.fRadius = 0.f;
		m_tWeaponParam.fTrail_Min = 0.f;
		m_tWeaponParam.fTrail_Max = 0.f;
		m_tWeaponParam.fCol_Height = 0.f;
	}

	return NO_EVENT;
}

_int CExplainWeaponUI::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CExplainWeaponUI::Render_GameObject()
{
	if (!m_bIsActive || m_tWeaponParam.iWeaponName == WPN_DATA_End)
		return NOERROR;

	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);

	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

	_uint iIndex = 0;
	LOOP(3)
	{
		if (0 == i)
		{
			iIndex = 0;
			m_fSizeX = 400.f;
			m_fSizeY = 400.f;
			m_fPosX = WINCX * 0.5f;
			m_fPosY = WINCY * 0.5f - 100.f;
		}
		else if (1 == i)
		{
			m_fSizeX = 400.f;
			m_fSizeY = 225.f;
			m_fPosX = WINCX * 0.5f + 35.f;
			m_fPosY = WINCY * 0.5f - 220.f;
			iIndex = m_iInfoIdx;
		}
		else if (2 == i)
		{
			m_fSizeX = 80.f;
			m_fSizeY = 80.f;
			m_fPosX = WINCX * 0.5f - 135.f;
			m_fPosY = WINCY * 0.5f - 225.f;
			iIndex = m_iWeaponIconIdx;
		}

		m_matWorld._11 = m_fSizeX;
		m_matWorld._22 = m_fSizeY;
		m_matWorld._33 = 1.f;
		m_matWorld._41 = m_fPosX - WINCX * 0.5f;
		m_matWorld._42 = -m_fPosY + WINCY * 0.5f;

		if (FAILED(SetUp_ConstantTable(iIndex)))
			return E_FAIL;

		m_pShaderCom->Begin_Shader();
		m_pShaderCom->Begin_Pass(1);
		m_pBufferCom->Render_VIBuffer();
		m_pShaderCom->End_Pass();
		m_pShaderCom->End_Shader();
	}


	return NOERROR;
}

HRESULT CExplainWeaponUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_ExplainWeaponUI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CExplainWeaponUI::SetUp_ConstantTable(_uint iIndex)
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

CExplainWeaponUI * CExplainWeaponUI::Create(_Device pGraphic_Device)
{
	CExplainWeaponUI* pInstance = new CExplainWeaponUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CExplainWeaponUI::Clone_GameObject(void * pArg)
{
	CExplainWeaponUI* pInstance = new CExplainWeaponUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CExplainWeaponUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	CUI::Free();
}
