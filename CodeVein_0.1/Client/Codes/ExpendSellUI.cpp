#include "stdafx.h"
#include "..\Headers\ExpendSellUI.h"
#include "UI_Manager.h"

CExpendSellUI::CExpendSellUI(_Device pDevice)
	: CUI(pDevice)
{
}

CExpendSellUI::CExpendSellUI(const CExpendSellUI & rhs)
	: CUI(rhs)
{
}

HRESULT CExpendSellUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CExpendSellUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_fPosX = 229.5f;
	m_fPosY = 325.5f;
	m_fSizeX = 280.f;
	m_fSizeY = 471.f;
	m_fViewZ = 4.f;

	SetUp_SlotPos();
	return NOERROR;
}

_int CExpendSellUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	if (m_bIsActive && !m_bIsSubActive)
	{
		m_vecSlot = *CUI_Manager::Get_Instance()->Get_Expendables_Inven()->Get_VecExpendSlot();
		SetUp_SlotPos();
		SetUp_SubUI_Active(true);

		m_bIsSubActive = true;
	}
	else if (!m_bIsActive && m_bIsSubActive)
	{
		SetUp_SubUI_Active(false);
		m_bIsSubActive = false;
	}

	
	Click_SubUI();
	SetUp_SlotPos();

	return NO_EVENT;
}

_int CExpendSellUI::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CExpendSellUI::Render_GameObject()
{
	/*if (!m_bIsActive)
		return NOERROR;
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);
	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

	if (FAILED(SetUp_ConstantTable(2)))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(1);
	m_pBufferCom->Render_VIBuffer();
	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();*/

	return NOERROR;
}

HRESULT CExpendSellUI::Add_Component()
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

HRESULT CExpendSellUI::SetUp_ConstantTable(_uint iIndex)
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

void CExpendSellUI::SetUp_SubUI_Active(_bool bIsActive)
{
	for (auto& iter : m_vecSlot)
	{
		iter->Set_Active(bIsActive);
	}
}

void CExpendSellUI::SetUp_SlotPos()
{
	_uint iIdx = 0;
	for (auto& vector_iter : m_vecSlot)
	{
		vector_iter->Set_UI_Pos(m_fPosX - 100.f + 52.f * (iIdx % 5), m_fPosY - 150.f + 52.f * (iIdx / 5));
		vector_iter->Set_ViewZ(m_fViewZ - 0.1f);
		iIdx++;
	}
}

void CExpendSellUI::Click_SubUI()
{
	if (!m_bIsActive)
		return;

	for (auto& iter : m_vecSlot)
	{
		if (iter->Pt_InRect())
		{
			if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
			{
				CExpendables_Inven* pInven = CUI_Manager::Get_Instance()->Get_Expendables_Inven();
				pInven->Sell_Item(iter);

				
			}
		}
	}
	
	_uint iIdx = 0;
	for (auto& vector_iter : m_vecSlot)
	{
		vector_iter->Set_UI_Pos(m_fPosX - 100.f + 52.f * (iIdx % 5), m_fPosY - 150.f + 52.f * (iIdx / 5));
		vector_iter->Set_UI_Size(50.f, 50.f);
		vector_iter->Set_ViewZ(m_fViewZ - 0.1f);
		iIdx++;
	}
}

CExpendSellUI * CExpendSellUI::Create(_Device pGraphic_Device)
{
	CExpendSellUI* pInstance = new CExpendSellUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("GeneralSellUI Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CExpendSellUI::Clone_GameObject(void * pArg)
{
	CExpendSellUI* pInstance = new CExpendSellUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("GeneralSellUI Clone Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CExpendSellUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
