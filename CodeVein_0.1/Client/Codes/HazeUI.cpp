#include "stdafx.h"
#include "..\Headers\HazeUI.h"
#include "UI_Manager.h"

CHazeUI::CHazeUI(_Device pDevice)
	: CUI(pDevice)
{
}

CHazeUI::CHazeUI(const CHazeUI & rhs)
	: CUI(rhs)
{
}

HRESULT CHazeUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CHazeUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);
	
	SetUp_Default();
	m_uHazeCnt = 2000;
	return NOERROR;
}

_int CHazeUI::Update_GameObject(_double TimeDelta)
{
	if (g_eSceneID_Cur == SCENE_STAGE_BASE)
	{
		Set_Active(false);
		m_pHazeCntFont->Set_Active(false);
		return S_OK;
	}
	else
	{
		Set_Active(true);
		m_pHazeCntFont->Set_Active(true);
	}

	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	if (m_pHazeCntFont)
	{
		Calc_HazeFontPos();
		m_pHazeCntFont->Set_Active(m_bIsActive);
		m_pHazeCntFont->Update_NumberValue(_float(m_uHazeCnt));
	}	
	if (g_pInput_Device->Key_Pressing(DIK_J))
		m_uHazeCnt = m_uHazeCnt + 10;
	return NO_EVENT;
}

_int CHazeUI::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CHazeUI::Render_GameObject()
{
	if (!m_bIsActive)
		return NOERROR;

	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);
	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

	if (FAILED(SetUp_ConstantTable(0)))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(1);
	m_pBufferCom->Render_VIBuffer();
	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return NOERROR;
}

HRESULT CHazeUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_HazeUI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CHazeUI::SetUp_ConstantTable(_uint iIndex)
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

HRESULT CHazeUI::SetUp_Default()
{
	m_fPosX = 60.f;
	m_fPosY = 690.f;
	m_fSizeX = 64.f;
	m_fSizeY = 64.f;
	m_bIsActive = true;

	m_pHazeCntFont = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pHazeCntFont->Set_UI_Pos(m_fPosX + 110.f, m_fPosY);
	m_pHazeCntFont->Set_UI_Size(15.6f, 30.f);
	if (FAILED(g_pManagement->Add_GameOject_ToLayer_NoClone(m_pHazeCntFont, SCENE_MORTAL, L"Layer_PlayerUI", nullptr)))
		return E_FAIL;
	return NOERROR;
}

void CHazeUI::Calc_HazeFontPos()
{
	if (m_uHazeCnt >= 0 && m_uHazeCnt < 10)
	{
		m_pHazeCntFont->Set_UI_Pos(m_fPosX + 30.f, m_fPosY);
	}
	else if (m_uHazeCnt < 100)
	{
		m_pHazeCntFont->Set_UI_Pos(m_fPosX + 45.f, m_fPosY);
	}
	else if (m_uHazeCnt < 1000)
	{
		m_pHazeCntFont->Set_UI_Pos(m_fPosX + 60.f, m_fPosY);
	}
	else if (m_uHazeCnt < 10000)
	{
		m_pHazeCntFont->Set_UI_Pos(m_fPosX + 75.f, m_fPosY);
	}
	else if (m_uHazeCnt < 100000)
	{
		m_pHazeCntFont->Set_UI_Pos(m_fPosX + 90.f, m_fPosY);
	}
}

CHazeUI * CHazeUI::Create(_Device pGraphic_Device)
{
	CHazeUI* pInstance = new CHazeUI(pGraphic_Device);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed Creating HazeUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CHazeUI::Clone_GameObject(void * pArg)
{
	CHazeUI* pInstance = new CHazeUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned HazeUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHazeUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);

	CUI::Free();
}
