#include "stdafx.h"
#include "..\Headers\ExplainArmorUI.h"


CExplainArmorUI::CExplainArmorUI(_Device pDevice)
	: CUI(pDevice)
{
}

CExplainArmorUI::CExplainArmorUI(const CExplainArmorUI & rhs)
	: CUI(rhs)
{
}

HRESULT CExplainArmorUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CExplainArmorUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);
	return NOERROR;
}

_int CExplainArmorUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.0f);

	//	ArmorAll_Drape_DarkNightHook,		// 암야의 낭아
	//	ArmorAll_Gauntlet_DarkNightHook,	// 암야의 갈고리
	//	ArmorAll_Gauntlet_MangSikHook,		// 망식의 갈고리
	//	ArmorAll_LongCoat_DarkNightSpear,	// 암야의 돌격창
	//	ArmorAll_LongCoat_WhiteSilver,		// 백은의 의장
	//	ArmorAll_LongCoat_QueenKiller,		// 퀸 킬러(여왕 토벌대 아장)_가시형
	//	ArmorAll_Muffler_DarkNightSpike,	// 암야의 가시
	//	ArmorAll_Muffler_WhiteGraze,		// 화이트 그레이즈
	//
	//	ArmorAll_END

	switch (m_tArmorParam.iArmorName)
	{
	case ArmorAll_Drape_DarkNightHook:
	{
		m_iInfoIdx = 1;
		m_iArmorIconIdx = 9;
	}
		break;
	case ArmorAll_Gauntlet_DarkNightHook:
	{
		m_iInfoIdx = 2;
		m_iArmorIconIdx = 10;
	}
		break;
	case ArmorAll_Gauntlet_MangSikHook:
	{
		m_iInfoIdx = 3;
		m_iArmorIconIdx = 11;
	}	
		break;
	case ArmorAll_LongCoat_DarkNightSpear:
	{
		m_iInfoIdx = 4;
		m_iArmorIconIdx = 12;
	}	
		break;
	case ArmorAll_LongCoat_WhiteSilver:
	{
		m_iInfoIdx = 5;
		m_iArmorIconIdx = 13;
	}
	break;
	case ArmorAll_LongCoat_QueenKiller:
	{
		m_iInfoIdx = 6;
		m_iArmorIconIdx = 14;
	}
	break;
	case ArmorAll_Muffler_DarkNightSpike:
	{
		m_iInfoIdx = 7;
		m_iArmorIconIdx = 15;
	}
	break;
	case ArmorAll_Muffler_WhiteGraze:
	{
		m_iInfoIdx = 8;
		m_iArmorIconIdx = 16;
	}
	break;
	}

	if (!m_bIsActive)
	{
		m_tArmorParam.iArmorType = ARMOR_End;
		m_tArmorParam.fDef = 0.f;
		m_tArmorParam.iPrice = 0;
	}
		

	return NO_EVENT;
}

_int CExplainArmorUI::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CExplainArmorUI::Render_GameObject()
{
	if (!m_bIsActive || m_tArmorParam.iArmorType == ARMOR_End)
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
			m_fSizeX = 720.f;
			m_fSizeY = 720.f;
			m_fPosX = WINCX * 0.5f + 30.f;
			m_fPosY = WINCY * 0.5f - 30.f;
		}
		else if (1 == i)
		{
			m_fSizeX = 400.f;
			m_fSizeY = 225.f;
			m_fPosX = WINCX * 0.5f + 70.f;
			m_fPosY = WINCY * 0.5f - 245.f;
			iIndex = m_iInfoIdx;
		}
		else if (2 == i)
		{
			m_fSizeX = 80.f;
			m_fSizeY = 80.f;
			m_fPosX = WINCX * 0.5f - 135.f;
			m_fPosY = WINCY * 0.5f - 238.f;
			iIndex = m_iArmorIconIdx;
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

HRESULT CExplainArmorUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_ExplainArmorUI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CExplainArmorUI::SetUp_ConstantTable(_uint iIndex)
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

CExplainArmorUI * CExplainArmorUI::Create(_Device pGraphic_Device)
{
	CExplainArmorUI* pInstance = new CExplainArmorUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CExplainArmorUI::Clone_GameObject(void * pArg)
{
	CExplainArmorUI* pInstance = new CExplainArmorUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);
	return pInstance;
}

void CExplainArmorUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	CUI::Free();
}
