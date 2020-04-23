#include "stdafx.h"
#include "..\Headers\StatusUI.h"
#include "ConditionUI.h"
#include "ExpUI.h"
#include "UI_Manager.h"
#include "Total_Inven.h"

CStatusUI::CStatusUI(_Device pDevice)
	: CUI(pDevice)
{
}

CStatusUI::CStatusUI(const CStatusUI & rhs)
	: CUI(rhs)
{
}

HRESULT CStatusUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CStatusUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	m_fPosX = 900.f;
	m_fPosY = 300.f;
	m_fSizeX = 512.f;
	m_fSizeY = 512.f;
	m_fViewZ = 1.f;

	m_bIsActive = false;

	SetUp_Default();

	return NOERROR;
}

_int CStatusUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);
	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	for (auto& iter : m_vecConditionUI)
	{
		iter->Set_Active(m_bIsActive);
	}

	if (m_pExpUI)
		m_pExpUI->Set_Active(m_bIsActive);

	if (m_pExitUI)
		m_pExitUI->Set_Active(m_bIsActive);

	Exit_This_UI();

	return NO_EVENT;
}

_int CStatusUI::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CStatusUI::Render_GameObject()
{
	if (!m_bIsActive)
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

HRESULT CStatusUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_StatusUI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CStatusUI::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, 6)))
		return E_FAIL;

	return NOERROR;
}

void CStatusUI::SetUp_Default()
{
	CConditionUI* pInstance = nullptr;
	LOOP(5)
	{
		pInstance = static_cast<CConditionUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_ConditionUI", nullptr));
		pInstance->Set_UI_Pos(m_fPosX - 30.f, m_fPosY + 65.f * _float(i) - 100.f);
		pInstance->Set_UI_Size(325.f, 53.f);
		pInstance->Set_ViewZ(m_fViewZ - 0.1f);
		pInstance->Set_ConditionType(CConditionUI::CONDITION_TYPE(i));
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
		m_vecConditionUI.push_back(pInstance);
	}

	m_pExpUI = static_cast<CExpUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_ExpUI", nullptr));
	m_pExpUI->Set_UI_Pos(m_fPosX, m_fPosY - 180.f);
	m_pExpUI->Set_UI_Size(100.f, 100.f);
	m_pExpUI->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pExpUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	// 나가기 버튼
	m_pExitUI = static_cast<CButton_UI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_ButtonUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pExitUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	m_pExitUI->Set_UI_Pos(m_fPosX - 180.f, m_fPosY - 190.f);
	m_pExitUI->Set_UI_Size(30.f, 30.f);
	m_pExitUI->Set_UI_Index(8);
}

void CStatusUI::Exit_This_UI()
{
	if (!m_pExitUI->Get_Active())
		return;

	if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB) &&
		g_pInput_Device->MousePt_InRect(m_pExitUI->Get_UI_Pos().x, m_pExitUI->Get_UI_Pos().y, m_pExitUI->Get_UI_Size().x, m_pExitUI->Get_UI_Size().y, g_hWnd))
	{
		CUI_Manager::Get_Instance()->Get_Total_Inven()->Set_Active(true);
		m_bIsActive = false;
	}
}

CStatusUI * CStatusUI::Create(_Device pGraphic_Device)
{
	CStatusUI* pInstance = new CStatusUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CStatusUI::Clone_GameObject(void * pArg)
{
	CStatusUI* pInstance = new CStatusUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CStatusUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
