#include "stdafx.h"
#include "..\Headers\Expend_InfoUI.h"
#include "UI_Manager.h"

CExpend_InfoUI::CExpend_InfoUI(_Device pDevice)
	: CUI(pDevice)
{
}

CExpend_InfoUI::CExpend_InfoUI(const CExpend_InfoUI & rhs)
	: CUI(rhs)
{
}

HRESULT CExpend_InfoUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CExpend_InfoUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	m_fPosX = WINCX * 0.5f;
	m_fPosY = WINCY * 0.5f;
	m_fSizeX = 1280.f;
	m_fSizeY = 700.f;

	m_pCostFont = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	if (FAILED(g_pManagement->Add_GameOject_ToLayer_NoClone(m_pCostFont, SCENE_MORTAL, L"Layer_PlayerUI", nullptr)))
		return E_FAIL;
	m_pCostFont->Set_UI_Size(15.f, 30.f);
	m_pCostFont->Set_ViewZ(m_fViewZ - 0.1f);

	m_pMyHazeFont = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	if (FAILED(g_pManagement->Add_GameOject_ToLayer_NoClone(m_pMyHazeFont, SCENE_MORTAL, L"Layer_PlayerUI", nullptr)))
		return E_FAIL;
	m_pMyHazeFont->Set_UI_Size(15.f, 30.f);
	m_pMyHazeFont->Set_ViewZ(m_fViewZ - 0.1f);

	return NOERROR;
}

_int CExpend_InfoUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	SetUp_Cost();

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

	
	m_pCostFont->Set_Active(m_bIsActive);
	m_pMyHazeFont->Set_Active(m_bIsActive);
	
	
	m_pCostFont->Update_NumberValue(_float(m_iCost));
	m_pCostFont->Set_UI_Pos(630.f, 580.f);

	m_pMyHazeFont->Update_NumberValue(_float(CUI_Manager::Get_Instance()->Get_HazeUI()->Get_Haze_Cnt()));
	m_pMyHazeFont->Set_UI_Pos(630.f, 600.f);


	//if (!m_bIsActive)
	//	m_eType = CExpendables::EXPEND_END;

	return NO_EVENT;
}

_int CExpend_InfoUI::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CExpend_InfoUI::Render_GameObject()
{
	if (!m_bIsActive || CExpendables::EXPEND_END == m_eType)
		return NOERROR;

	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);
	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(1);
	m_pBufferCom->Render_VIBuffer();
	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return NOERROR;
}

HRESULT CExpend_InfoUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_ExpendOption", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CExpend_InfoUI::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, m_iIndex)))
		return E_FAIL;

	return NOERROR;
}

void CExpend_InfoUI::SetUp_Cost()
{
	switch (m_eType)
	{
	case CExpendables::Expend_MaximumUp:
	{
		m_iCost = 50;
	}
	break;
	case CExpendables::Expend_Hp:
	{
		m_iCost = 30;
	}
	break;
	case CExpendables::Expend_Return:
	{
		m_iCost = 100;
	}
	break;
	case CExpendables::Expend_Blood:
	{
		m_iCost = 40;
	}
	break;
	case CExpendables::Expend_Cheet:
	{
		m_iCost = 80;
	}
	break;
	case CExpendables::Expend_SuperArmor:
	{
		m_iCost = 200;
	}
	break;
	}
}

CExpend_InfoUI * CExpend_InfoUI::Create(_Device pGraphic_Device)
{
	CExpend_InfoUI* pInstance = new CExpend_InfoUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CExpend_InfoUI::Clone_GameObject(void * pArg)
{
	CExpend_InfoUI* pInstance = new CExpend_InfoUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CExpend_InfoUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
