#include "stdafx.h"
#include "..\Headers\Info_Slot.h"
#include "PlayerFontUI.h"

CInfo_Slot::CInfo_Slot(_Device pDevice)
	: CUI(pDevice)
{
}

CInfo_Slot::CInfo_Slot(const CInfo_Slot & rhs)
	: CUI(rhs)
{
}

void CInfo_Slot::Set_Number(_uint iNumber)
{
	m_iNumber = iNumber;
}

HRESULT CInfo_Slot::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CInfo_Slot::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);

	m_bIsActive = false;

	m_pItemCntFont = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pItemCntFont->Set_UI_Pos(m_fPosX - m_fSizeX * 0.25f, m_fPosY + m_fSizeY * 0.25f);
	m_pItemCntFont->Set_UI_Size(10.4f, 20.f);
	m_pItemCntFont->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pItemCntFont, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	return NOERROR;
}

_int CInfo_Slot::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	switch (m_eInfo)
	{
	case CExpendables::Expend_MaximumUp:
		m_iIndex = 3;
		break;
	case CExpendables::Expend_Hp:
		m_iIndex = 4;
		break;
	case CExpendables::Expend_Return:
		m_iIndex = 5;
		break;
	case CExpendables::Expend_Blood:
		m_iIndex = 6;
		break;
	case CExpendables::Expend_Cheet:
		m_iIndex = 7;
		break;
	case CExpendables::Expend_SuperArmor:
		m_iIndex = 8;
		break;
	case CExpendables::EXPEND_END:
		break;
	}

	if (m_eInfo == CExpendables::EXPEND_END)
		m_pItemCntFont->Set_Active(false);
	else
		m_pItemCntFont->Set_Active(m_bIsActive);
	m_pItemCntFont->Update_NumberValue((_float)m_iNumber);
	m_pItemCntFont->Set_ViewZ(m_fViewZ - 0.1f);

	return NO_EVENT;
}

_int CInfo_Slot::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CInfo_Slot::Render_GameObject()
{
	if (!m_bIsActive || 
		m_eInfo == CExpendables::EXPEND_END)
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

HRESULT CInfo_Slot::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_Expendables", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CInfo_Slot::SetUp_ConstantTable()
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

CInfo_Slot * CInfo_Slot::Create(_Device pGraphic_Device)
{
	CInfo_Slot* pInstance = new CInfo_Slot(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CInfo_Slot::Clone_GameObject(void * pArg)
{
	CInfo_Slot* pInstance = new CInfo_Slot(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CInfo_Slot::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
