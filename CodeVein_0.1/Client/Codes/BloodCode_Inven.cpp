#include "stdafx.h"
#include "..\Headers\BloodCode_Inven.h"
#include "UI_Manager.h"

CBloodCode_Inven::CBloodCode_Inven(_Device pDevice)
	: CUI(pDevice)
{
}

CBloodCode_Inven::CBloodCode_Inven(const CBloodCode_Inven & rhs)
	: CUI(rhs)
{
}

HRESULT CBloodCode_Inven::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CBloodCode_Inven::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	SetUp_Default();
	
	Add_BloodCode(BloodCode_Fighter);
	Add_BloodCode(BloodCode_Prometheus);
	Add_BloodCode(BloodCode_Artemis);
	Add_BloodCode(BloodCode_DarkKnight);
	Add_BloodCode(BloodCode_Queen);
	
	return NOERROR;
}

_int CBloodCode_Inven::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);
	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.0f);

	for (_uint i = 0; i < m_vecBloodCodeUI.size(); ++i)
	{
		m_vecBloodCodeUI[i]->Set_Active(m_bIsActive);
		m_vecBloodCodeUI[i]->Set_ViewZ(m_fViewZ - 0.1f);
		m_vecBloodCodeUI[i]->Set_UI_Pos(m_fPosX - 103.f + 70.f * (i % 4), m_fPosY - 130.f + 52.5f * (i / 4));
	}

	m_pExitIcon->Set_Active(m_bIsActive);

	Click_Icon();

	return NO_EVENT;
}

_int CBloodCode_Inven::Late_Update_GameObject(_double TimeDelta)
{
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);

	m_matWorld._11 = m_fSizeX;
	m_matWorld._22 = m_fSizeY;
	m_matWorld._33 = 1.f;
	m_matWorld._41 = m_fPosX - WINCX * 0.5f;
	m_matWorld._42 = -m_fPosY + WINCY * 0.5f;
	m_matWorld._42 = 1.f;

	return NO_EVENT;
}

HRESULT CBloodCode_Inven::Render_GameObject()
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

HRESULT CBloodCode_Inven::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_MenuWindow", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CBloodCode_Inven::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, 2)))
		return E_FAIL;

	return NOERROR;
}

void CBloodCode_Inven::SetUp_Default()
{
	m_fPosX = WINCX * 0.3f;
	m_fPosY = WINCY * 0.5f;
	m_fSizeX = 280.f;
	m_fSizeY = 471.f;
	m_fViewZ = 3.f;
	m_bIsActive = false;

	m_pExitIcon = static_cast<CInventory_Icon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_InvenIcon", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pExitIcon, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	m_pExitIcon->Set_UI_Pos(m_fPosX + 120.f, m_fPosY - 203.f);
	m_pExitIcon->Set_UI_Size(35.f, 45.f);
	m_pExitIcon->Set_Type(CInventory_Icon::ICON_EXIT);
}

void CBloodCode_Inven::Click_Icon()
{
	if (!m_bIsActive)
		return;

	if (m_pExitIcon->Pt_InRect() &&
		g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
	{
		m_bIsActive = false;
		CUI_Manager::Get_Instance()->Get_Total_Inven()->Set_Active(true);
	}

	if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
	{
		for (auto& iter : m_vecBloodCodeUI)
		{
			if (iter->Pt_InRect())
			{
				Reset_Select();
				iter->Set_Select(true);
				// total inven에 블러드코드 타입 전달
				CUI_Manager::Get_Instance()->Get_Total_Inven()->Set_PlayerBloodCodeType(iter->Get_Type());
			}
		}
	}
}

void CBloodCode_Inven::Reset_Select()
{
	for (auto& iter : m_vecBloodCodeUI)
		iter->Set_Select(false);
}

void CBloodCode_Inven::Add_BloodCode(BloodCode_ID eID)
{
	CBloodCodeUI* pInstance = nullptr;

	pInstance = static_cast<CBloodCodeUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_BloodCodeUI", nullptr));
	pInstance->Set_Type(eID);
	pInstance->Set_UI_Size(70.f, 52.5f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	m_vecBloodCodeUI.push_back(pInstance);
}

CBloodCode_Inven * CBloodCode_Inven::Create(_Device pGraphic_Device)
{
	CBloodCode_Inven* pInstance = new CBloodCode_Inven(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CBloodCode_Inven::Clone_GameObject(void * pArg)
{
	CBloodCode_Inven* pInstance = new CBloodCode_Inven(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CBloodCode_Inven::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	CUI::Free();
}
