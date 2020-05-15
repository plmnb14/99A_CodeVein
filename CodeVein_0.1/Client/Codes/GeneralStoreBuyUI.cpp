#include "stdafx.h"
#include "..\Headers\GeneralStoreBuyUI.h"
#include "UI_Manager.h"

CGeneralStoreBuyUI::CGeneralStoreBuyUI(_Device pDevice)
	: CUI(pDevice)
{
}

CGeneralStoreBuyUI::CGeneralStoreBuyUI(const CGeneralStoreBuyUI & rhs)
	: CUI(rhs)
{
}

HRESULT CGeneralStoreBuyUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CGeneralStoreBuyUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	SetUp_Default();
	
	return NOERROR;
}

_int CGeneralStoreBuyUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	Update_SubUI();
	Click_SubUI();
	
	return NO_EVENT;
}

_int CGeneralStoreBuyUI::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CGeneralStoreBuyUI::Render_GameObject()
{
	if (!m_bIsActive)
		return NOERROR;
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);
	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

	if (FAILED(SetUp_ConstantTable(m_iIndex)))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(1);
	m_pBufferCom->Render_VIBuffer();
	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return NOERROR;
}

HRESULT CGeneralStoreBuyUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_GeneralStoreBuyUI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CGeneralStoreBuyUI::SetUp_ConstantTable(_uint iIndex)
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

void CGeneralStoreBuyUI::SetUp_Default()
{
	m_iIndex = 0;

	m_fPosX = WINCX * 0.5f;
	m_fPosY = WINCY * 0.5f;
	m_fSizeX = 200.f;
	m_fSizeY = 100.f;
	m_fViewZ = 1.f;

	m_pCntMinusOption = static_cast<CBuyOptionUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_BuyOptionUI", nullptr));
	m_pCntMinusOption->Set_UI_Pos(m_fPosX - 85.f, m_fPosY);
	m_pCntMinusOption->Set_UI_Size(30.f, 30.f);
	m_pCntMinusOption->Set_ViewZ(m_fViewZ - 0.1f);
	m_pCntMinusOption->Set_UI_Index(1);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pCntMinusOption, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pCntPlusOption = static_cast<CBuyOptionUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_BuyOptionUI", nullptr));
	m_pCntPlusOption->Set_UI_Pos(m_fPosX + 85.f, m_fPosY);
	m_pCntPlusOption->Set_UI_Size(30.f, 30.f);
	m_pCntPlusOption->Set_ViewZ(m_fViewZ - 0.3f);
	m_pCntPlusOption->Set_UI_Index(2);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pCntPlusOption, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	
	m_pDecisionOption = static_cast<CBuyOptionUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_BuyOptionUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pDecisionOption, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	m_pDecisionOption->Set_UI_Pos(m_fPosX, m_fPosY + 40.f);
	m_pDecisionOption->Set_UI_Size(30.f, 30.f);
	m_pDecisionOption->Set_UI_Index(3);
	m_pDecisionOption->Set_ViewZ(m_fViewZ - 0.1f);

	m_pBuyCntFont = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBuyCntFont, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	m_pBuyCntFont->Set_UI_Pos(m_fPosX, m_fPosY);
	m_pBuyCntFont->Set_UI_Size(20.f, 40.8f);
	m_pBuyCntFont->Set_ViewZ(m_fViewZ - 0.1f);
}

void CGeneralStoreBuyUI::Update_SubUI()
{
	m_pCntMinusOption->Set_Active(m_bIsActive);
	m_pCntPlusOption->Set_Active(m_bIsActive);
	m_pDecisionOption->Set_Active(m_bIsActive);
	m_pBuyCntFont->Set_Active(m_bIsActive);

	m_pBuyCntFont->Update_NumberValue(m_iBuyCnt);
}

void CGeneralStoreBuyUI::Click_SubUI()
{
	if (!m_bIsActive)
		return;
	if (m_pCntMinusOption->Pt_InRect() && g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
	{
		if (m_iBuyCnt > 0)
			m_iBuyCnt--;
		else return;
	}
	else if (m_pCntPlusOption->Pt_InRect() && g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
	{
		if (m_iBuyCnt < 9)
			m_iBuyCnt++;
		else return;
	}
	else if (m_pDecisionOption->Pt_InRect() && g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
	{
		
	}
}

CGeneralStoreBuyUI * CGeneralStoreBuyUI::Create(_Device pGraphic_Device)
{
	CGeneralStoreBuyUI* pInstance = new CGeneralStoreBuyUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CGeneralStoreBuyUI::Clone_GameObject(void * pArg)
{
	CGeneralStoreBuyUI* pInstance = new CGeneralStoreBuyUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CGeneralStoreBuyUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
