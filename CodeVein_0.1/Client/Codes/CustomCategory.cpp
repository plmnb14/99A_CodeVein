#include "stdafx.h"
#include "..\Headers\CustomCategory.h"
#include "UI_Manager.h"
#include "CustomInven.h"


CCustomCategory::CCustomCategory(_Device pDevice)
	: CUI(pDevice)
{
}

CCustomCategory::CCustomCategory(const CCustomCategory & rhs)
	: CUI(rhs)
{
}

void CCustomCategory::Set_Active(_bool bIsActive)
{
	m_bIsActive = bIsActive;

	Reset_Option();
}

HRESULT CCustomCategory::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CCustomCategory::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	m_fSizeX = 256.f;
	m_fSizeY = 60.f;
	m_fPosX = WINCX * 0.2f;
	m_fPosY = WINCY * 0.1f;

	m_bIsActive = false;
	
	m_iTexIdx = 0;

	SetUp_Default();

	return NOERROR;
}

_int CCustomCategory::Update_GameObject(_double TimeDelta)
{
	if (g_pInput_Device->Key_Down(DIK_T))
	{
		Set_Active(!Get_Active());
		m_pCustomInven->Set_Active(!Get_Active());
	}

	CUI::Update_GameObject(TimeDelta);
	m_fPosX = WINCX * 0.1f;

	LOOP(4)
	{
		TARGET_TO_TRANS(m_vecOption[i])->Set_Scale(_v3(1.f, 1.f, 1.f));
		TARGET_TO_TRANS(m_vecOption[i])->Set_At(m_pTransformCom->Get_At());

		m_vecOption[i]->Set_UI_Pos(Get_UI_Pos().x, Get_UI_Pos().y + 76.f + (i * 40.f));
		m_vecOption[i]->Set_UI_Size(256.f, 60.f);
		m_vecOption[i]->Set_ViewZ(m_fViewZ - 0.01f);
		m_vecOption[i]->Set_UI_Index(i + 1);
		m_vecOption[i]->Set_Active(m_bIsActive);
	}

	if (!m_bIsActive)
		return S_OK;

	Click_Option();

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);
	return NO_EVENT;
}

_int CCustomCategory::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CCustomCategory::Render_GameObject()
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


void CCustomCategory::Click_Option()
{
	if (!m_bIsActive)
		return;
	
	_int iIdx = 0;
	
	for (auto& iter : m_vecOption)
	{
		if (iter->Get_IsColl())
		{
			Reset_Option();
			iter->Set_Select(true);
	
			if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
			{
				m_pCustomInven->Set_ActiveSlot(CCustomInven::INVEN_TYPE(iIdx));
			}
		}
		else
		{
			iter->Set_Select(false);
		}
	
		++iIdx;
	}
}

void CCustomCategory::Reset_Option()
{
	for (auto& iter : m_vecOption)
	{
		iter->Set_Select(false);
	}
}

HRESULT CCustomCategory::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"DefaultTex_Custom_UI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CCustomCategory::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, m_iTexIdx)))
		return E_FAIL;

	return NOERROR;
}

void CCustomCategory::Change_Texture(const _tchar * _Name)
{
	auto& iter = m_pmapComponents.find(L"Com_Texture");

	Safe_Release(m_pTextureCom);
	Safe_Release(iter->second);

	iter->second = m_pTextureCom = static_cast<CTexture*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, _Name));
	Safe_AddRef(iter->second);
}

void CCustomCategory::SetUp_Default()
{
	CCustomCategoryOption* pInstance = nullptr;
	LOOP(4)
	{
		pInstance = static_cast<CCustomCategoryOption*>(g_pManagement->Clone_GameObject_Return(L"GameObject_CustomCategoryOption", nullptr));
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_PREPARE_ALL, L"Layer_CustomUI", nullptr);
		m_vecOption.push_back(pInstance);
	}

	m_pCustomInven = static_cast<CCustomInven*>(g_pManagement->Clone_GameObject_Return(L"GameObject_CustomInven", nullptr));
	m_pCustomInven->Set_Active(false);
	m_pCustomInven->Set_ActiveSlot(CCustomInven::TYPE_HAIR);

	if (FAILED(g_pManagement->Add_GameOject_ToLayer_NoClone(m_pCustomInven, SCENE_STAGE_BASE, L"Layer_Custom", nullptr)))
		return;

}

CCustomCategory * CCustomCategory::Create(_Device pGraphic_Device)
{
	CCustomCategory* pInstance = new CCustomCategory(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CCustomCategory::Clone_GameObject(void * pArg)
{
	CCustomCategory* pInstance = new CCustomCategory(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CCustomCategory::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
