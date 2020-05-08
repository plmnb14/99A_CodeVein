#include "stdafx.h"
#include "..\Headers\Pet_Slot.h"

CPet_Slot::CPet_Slot(_Device pDevice)
	:CUI(pDevice)
{
}

CPet_Slot::CPet_Slot(const CPet_Slot & rhs)
	:CUI(rhs)
{
}

HRESULT CPet_Slot::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CPet_Slot::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);

	m_bIsActive = false;

	return S_OK;
}

_int CPet_Slot::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pRenderer->Add_RenderList(RENDER_UI, this);
	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.0f);

	m_bIsCollMouse = Pt_InRect();
	
	switch (m_ePetType)
	{
	case CPet::PET_POISONBUTTERFLY:
		m_iIndex = 3;
		break;
	case CPet::PET_DEERKING:
		m_iIndex = 4;
		break;
	}
	return NO_EVENT;
}

_int CPet_Slot::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CPet_Slot::Render_GameObject()
{
	if (!m_bIsActive)
		return S_OK;

	IF_NULL_VALUE_RETURN(m_pShader, E_FAIL);
	IF_NULL_VALUE_RETURN(m_pBuffer, E_FAIL);

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);

	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

	_uint iIndex = 0;

	if (!m_bIsSelect)
	{
		if (!m_bIsCollMouse)
		{
			LOOP(2)
			{
				if (0 == i)
					iIndex = 0;
				else if (1 == i)
					iIndex = m_iIndex;
				if (FAILED(SetUp_ConstantTable(iIndex)))
					return E_FAIL;
				m_pShader->Begin_Shader();
				m_pShader->Begin_Pass(1);
				m_pBuffer->Render_VIBuffer();
				m_pShader->End_Pass();
				m_pShader->End_Shader();
			}
		}
		else
		{
			LOOP(3)
			{
				if (0 == i)
					iIndex = 0;
				else if (1 == i)
					iIndex = m_iIndex;
				else if (2 == i)
					iIndex = 2;
				if (FAILED(SetUp_ConstantTable(iIndex)))
					return E_FAIL;
				m_pShader->Begin_Shader();
				m_pShader->Begin_Pass(1);
				m_pBuffer->Render_VIBuffer();
				m_pShader->End_Pass();
				m_pShader->End_Shader();
			}
		}
	}
	else
	{
		if (!m_bIsCollMouse)
		{
			LOOP(3)
			{
				if (0 == i)
					iIndex = 0;
				else if (1 == i)
					iIndex = m_iIndex;
				else if (2 == i)
					iIndex = 1;
				if (FAILED(SetUp_ConstantTable(iIndex)))
					return E_FAIL;
				m_pShader->Begin_Shader();
				m_pShader->Begin_Pass(1);
				m_pBuffer->Render_VIBuffer();
				m_pShader->End_Pass();
				m_pShader->End_Shader();
			}
		}
		else
		{
			LOOP(4)
			{
				if (0 == i)
					iIndex = 0;
				else if (1 == i)
					iIndex = m_iIndex;
				else if (2 == i)
					iIndex = 1;
				else if (3 == i)
					iIndex = 2;
				if (FAILED(SetUp_ConstantTable(iIndex)))
					return E_FAIL;
				m_pShader->Begin_Shader();
				m_pShader->Begin_Pass(1);
				m_pBuffer->Render_VIBuffer();
				m_pShader->End_Pass();
				m_pShader->End_Shader();
			}
		}
	}
	
	

	return S_OK;
}


_bool CPet_Slot::Pt_InRect()
{
	return g_pInput_Device->MousePt_InRect(m_fPosX, m_fPosY, m_fSizeX, m_fSizeY, g_hWnd);
}

HRESULT CPet_Slot::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransform)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_PetIcon", L"Com_Texture", (CComponent**)&m_pTexture)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBuffer)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPet_Slot::SetUp_ConstantTable(_uint iIndex)
{
	IF_NULL_VALUE_RETURN(m_pShader, E_FAIL);

	if (FAILED(m_pShader->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Value("g_matView", &m_matView, sizeof(_mat))))

		return E_FAIL;
	if (FAILED(m_pShader->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pTexture->SetUp_OnShader("g_DiffuseTexture", m_pShader, iIndex)))
		return E_FAIL;

	return S_OK;
}

CPet_Slot* CPet_Slot::Create(_Device pGraphic_Device)
{
	CPet_Slot* pInstance = new CPet_Slot(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CPet_Slot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPet_Slot::Clone_GameObject(void * pArg)
{
	CPet_Slot* pInstance = new CPet_Slot(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CPet_Slot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPet_Slot::Free()
{
	Safe_Release(m_pTransform);
	Safe_Release(m_pBuffer);
	Safe_Release(m_pShader);
	Safe_Release(m_pTexture);
	Safe_Release(m_pRenderer);

	CUI::Free();
}
