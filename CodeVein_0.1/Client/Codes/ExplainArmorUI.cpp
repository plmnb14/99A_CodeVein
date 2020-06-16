#include "stdafx.h"
#include "..\Headers\ExplainArmorUI.h"
#include "UI_Manager.h"


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

	SetUp_Default();
	return NOERROR;
}

_int CExplainArmorUI::Update_GameObject(_double TimeDelta)
{
	if (m_bIsActive && !m_bIsSubActive)
	{
		SetUp_SubUI_Active(true);
		m_bIsSubActive = true;
	}
	else if (!m_bIsActive && m_bIsSubActive)
	{
		SetUp_SubUI_Active(false);
		m_bIsSubActive = false;
		m_tArmorParam.iArmorName = ArmorAll_END;
	}

	if(m_bIsActive)
	{
		m_pReinForceFont->Update_NumberValue(_float(m_tArmorParam.iReinforce));
		m_pDefFont->Update_NumberValue(_float(m_tArmorParam.fDef));
		m_pPlusDefFont->Update_NumberValue(_float(m_tArmorParam.fPlusDef));
		m_pHPFont->Update_NumberValue(_float(m_tArmorParam.fHP));
		m_pPlusHPFont->Update_NumberValue(_float(m_tArmorParam.fPlusHP));

		m_pReinForceFont->Set_UI_Pos(630.f, 255.f);	
		m_pPlusDefFont->Set_UI_Pos(630.f, 279.f);	
		m_pDefFont->Set_UI_Pos(630.f, 425.f);
		m_pHPFont->Set_UI_Pos(630.f, 520.f);
		m_pPlusHPFont->Set_UI_Pos(630.f, 300.f);

		if(m_tArmorParam.iArmorName == ArmorAll_END)
			SetUp_SubUI_Active(false);
		else
			SetUp_SubUI_Active(true);
	}
	else if (!m_bIsActive || m_tArmorParam.iArmorName == ArmorAll_END)
	{
		SetUp_SubUI_Active(false);
	}

	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.0f);

	return NO_EVENT;
}

_int CExplainArmorUI::Late_Update_GameObject(_double TimeDelta)
{
	if (!m_bIsActive)
		return NO_EVENT;

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

	SetUp_TexIdx();

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

void CExplainArmorUI::SetUp_Default()
{
	m_pReinForceFont = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pReinForceFont->Set_UI_Size(15.f, 30.f);
	m_pReinForceFont->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pReinForceFont, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pDefFont = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pDefFont->Set_UI_Size(15.f, 30.f);
	m_pDefFont->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pDefFont, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pPlusDefFont = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pPlusDefFont->Set_UI_Size(15.f, 30.f);
	m_pPlusDefFont->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pPlusDefFont, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pHPFont = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pHPFont->Set_UI_Size(15.f, 30.f);
	m_pHPFont->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pHPFont, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pPlusHPFont = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pPlusHPFont->Set_UI_Size(15.f, 30.f);
	m_pPlusHPFont->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pPlusHPFont, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_tArmorParam.iArmorName = ARMOR_All_DATA::ArmorAll_END;
}

void CExplainArmorUI::SetUp_SubUI_Active(_bool bIsActive)
{
	m_pReinForceFont->Set_Active(bIsActive);
	m_pDefFont->Set_Active(bIsActive);
	m_pPlusDefFont->Set_Active(bIsActive);
	m_pHPFont->Set_Active(bIsActive);
	m_pPlusHPFont->Set_Active(bIsActive);
}

void CExplainArmorUI::SetUp_TexIdx()
{
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
	default:
	{
		m_iInfoIdx = 17;
		m_iArmorIconIdx = 17;
	}
	break;
	}
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
