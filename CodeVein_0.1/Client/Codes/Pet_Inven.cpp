#include "stdafx.h"
#include "..\Headers\Pet_Inven.h"
#include "UI_Manager.h"

CPet_Inven::CPet_Inven(_Device pDevice)
	: CUI(pDevice)
{
}

CPet_Inven::CPet_Inven(const CPet_Inven & rhs)
	: CUI(rhs)
{
}

HRESULT CPet_Inven::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CPet_Inven::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);

	SetUp_Default();
	Add_Pet(CPet::PET_DEERKING);
	Add_Pet(CPet::PET_POISONBUTTERFLY);
	Add_Pet(CPet::PET_POISONBUTTERFLY);
	return S_OK;
}

_int CPet_Inven::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pRenderer->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.0f);

	_uint iIdx = 0;
	for (auto& vector_iter : m_vecPetSlot)
	{
		vector_iter->Set_UI_Pos(m_fPosX - 100.f + 50.f * (iIdx % 5), m_fPosY - 150.f + 50.f * (iIdx / 5));
		vector_iter->Set_Active(m_bIsActive);
		iIdx++;
	}

	m_pExitIcon->Set_Active(m_bIsActive);

	Click_Inven();

	return NO_EVENT;
}

_int CPet_Inven::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CPet_Inven::Render_GameObject()
{
	if (false == m_bIsActive)
		return S_OK;

	IF_NULL_VALUE_RETURN(m_pShader, E_FAIL);
	IF_NULL_VALUE_RETURN(m_pBuffer, E_FAIL);

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);
	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShader->Begin_Shader();

	m_pShader->Begin_Pass(1);

	m_pBuffer->Render_VIBuffer();

	m_pShader->End_Pass();

	m_pShader->End_Shader();

	return S_OK;
}

void CPet_Inven::Click_Inven()
{
	if (!m_bIsActive)
		return;

	if (m_pExitIcon->Pt_InRect() && g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
	{
		m_bIsActive = false;
		CUI_Manager::Get_Instance()->Get_Total_Inven()->Set_Active(true);
		CUI_Manager::Get_Instance()->Get_StatusUI()->Set_Active(true);
	}

	if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
	{
		for (auto& iter : m_vecPetSlot)
		{
			if (iter->Pt_InRect())
			{
				Reset_SlotSelect();
				iter->Set_Select(true);
			}
		}
	}
	
}

void CPet_Inven::Reset_SlotSelect()
{
	for (auto& iter : m_vecPetSlot)
	{
		iter->Set_Select(false);
	}
}

HRESULT CPet_Inven::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransform)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_MenuWindow", L"Com_Texture", (CComponent**)&m_pTexture)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBuffer)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPet_Inven::SetUp_Default()
{
	m_fPosX = 229.5f;
	m_fPosY = 325.5f;
	m_fSizeX = 280.f;
	m_fSizeY = 471.f;
	m_fViewZ = 4.f;

	m_pExitIcon = static_cast<CInventory_Icon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_InvenIcon", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pExitIcon, SCENE_MORTAL, L"Layer_PetUI", nullptr);
	m_pExitIcon->Set_UI_Pos(m_fPosX + 120.f, m_fPosY - 203.f);
	m_pExitIcon->Set_UI_Size(40.f, 40.f);
	m_pExitIcon->Set_Type(CInventory_Icon::ICON_EXIT);

	return S_OK;
}

HRESULT CPet_Inven::SetUp_ConstantTable()
{
	IF_NULL_VALUE_RETURN(m_pShader, E_FAIL);

	if (FAILED(m_pShader->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pTexture->SetUp_OnShader("g_DiffuseTexture", m_pShader, 2)))
		return E_FAIL;

	return S_OK;
}

void CPet_Inven::Add_Pet(CPet::PET_TYPE ePetType)
{
	CPet_Slot* pPetSlot = static_cast<CPet_Slot*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PetSlot", nullptr));
	pPetSlot->Set_PetType(ePetType);
	pPetSlot->Set_PetLevel(1); // 레벨 1부터 시작
	pPetSlot->Set_UI_Size(40.f, 40.f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pPetSlot, SCENE_MORTAL, L"Layer_PetUI", nullptr);
	m_vecPetSlot.push_back(pPetSlot);
}

CPet_Inven* CPet_Inven::Create(_Device pGraphic_Device)
{
	CPet_Inven* pInstance = new CPet_Inven(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CPet_Inven");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPet_Inven::Clone_GameObject(void * pArg)
{
	CPet_Inven* pInstance = new CPet_Inven(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CPet_Inven");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPet_Inven::Free()
{
	Safe_Release(m_pTransform);
	Safe_Release(m_pBuffer);
	Safe_Release(m_pTexture);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);

	CUI::Free();
}
