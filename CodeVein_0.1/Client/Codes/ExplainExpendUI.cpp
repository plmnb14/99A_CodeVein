#include "stdafx.h"
#include "..\Headers\ExplainExpendUI.h"
#include "UI_Manager.h"


CExplainExpendUI::CExplainExpendUI(_Device pDevice)
	: CUI(pDevice)
{
}

CExplainExpendUI::CExplainExpendUI(const CExplainExpendUI & rhs)
	: CUI(rhs)
{
}

HRESULT CExplainExpendUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CExplainExpendUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	SetUp_Default();
	

	return NOERROR;
}

_int CExplainExpendUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);
	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.0f);

	_int i = 0;
	if (m_bIsActive)
	{
		if (m_pFontCurHave)
		{
			if (m_eType != CExpendables::EXPEND_END)
				m_pFontCurHave->Set_Active(true);
			else
				m_pFontCurHave->Set_Active(false);

			m_pFontCurHave->Update_NumberValue(_float(m_iCurHaveCnt));
			m_pFontCurHave->Set_UI_Pos(m_fPosX - 175.f, m_fPosY - 3.f);
			m_pFontCurHave->Set_UI_Size(10.4f, 20.f);
		}
		if (m_pFontMaximum)
		{
			if (m_eType != CExpendables::EXPEND_END)
				m_pFontMaximum->Set_Active(true);
			else
				m_pFontMaximum->Set_Active(false);

			m_pFontMaximum->Update_NumberValue(_float(m_iMaximumCnt));
			m_pFontMaximum->Set_UI_Pos(m_fPosX - 145.f, m_fPosY - 3.f);
			m_pFontMaximum->Set_UI_Size(10.4f, 20.f);
		}
	}
	else
	{
		m_pFontCurHave->Set_Active(false);
		m_pFontMaximum->Set_Active(false);
		m_eType = CExpendables::EXPEND_END;
	}
		

	return NO_EVENT;
}

_int CExplainExpendUI::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CExplainExpendUI::Render_GameObject()
{
	if (!m_bIsActive || m_eType == CExpendables::EXPEND_END)
		return NOERROR;

	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);
	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

	SetUp_TexIndex();

	if (FAILED(SetUp_ConstantTable(m_iIndex)))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(1);
	m_pBufferCom->Render_VIBuffer();
	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return NOERROR;
}

HRESULT CExplainExpendUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_ExplainExpendUI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CExplainExpendUI::SetUp_ConstantTable(_uint iIndex)
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

void CExplainExpendUI::SetUp_Default()
{
	m_pFontMaximum = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pFontMaximum->Set_UI_Pos(m_fPosX - 145.f, m_fPosY - 3.f);
	m_pFontMaximum->Set_UI_Size(10.4f, 20.f);
	m_pFontMaximum->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFontMaximum, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pFontCurHave = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pFontCurHave->Set_UI_Pos(m_fPosX - 175.f, m_fPosY - 3.f);
	m_pFontCurHave->Set_UI_Size(10.4f, 20.f);
	m_pFontCurHave->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFontCurHave, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
}

void CExplainExpendUI::SetUp_TexIndex()
{
	//	Expend_Hp,				// 체력 아이템
	//	Expend_Return,			// 귀환 아이템
	//	Expend_Blood,			// 영혈 아이템
	//	Expend_Cheet,			// 치트 아이템
	//	Expend_SuperArmor,		// 슈퍼 아머
	//	EXPEND_END

	switch (m_eType)
	{
	case CExpendables::Expend_MaximumUp:
		m_iIndex = 0;
		break;
	case CExpendables::Expend_Hp:
		m_iIndex = 1;
		break;
	case CExpendables::Expend_Return:
		m_iIndex = 2;
		break;
	case CExpendables::Expend_Blood:
		m_iIndex = 3;
		break;
	case CExpendables::Expend_Cheet:
		m_iIndex = 4;
		break;
	case CExpendables::Expend_SuperArmor:
		m_iIndex = 5;
		break;
	}
}


CExplainExpendUI * CExplainExpendUI::Create(_Device pGraphic_Device)
{
	CExplainExpendUI* pInstance = new CExplainExpendUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CExplainExpendUI::Clone_GameObject(void * pArg)
{
	CExplainExpendUI* pInstance = new CExplainExpendUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CExplainExpendUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	CUI::Free();
}
