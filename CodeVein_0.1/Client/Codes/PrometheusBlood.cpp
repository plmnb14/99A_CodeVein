#include "stdafx.h"
#include "..\Headers\PrometheusBlood.h"
#include "UI_Manager.h"


CPrometheusBlood::CPrometheusBlood(_Device pDevice)
	: CUI(pDevice)
{
}

CPrometheusBlood::CPrometheusBlood(const CPrometheusBlood & rhs)
	: CUI(rhs)
{
}

HRESULT CPrometheusBlood::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CPrometheusBlood::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	m_fPosX = WINCX * 0.3f;
	m_fPosY = WINCY * 0.5f;
	m_fSizeX = 280.f;
	m_fSizeY = 471.f;
	m_fViewZ = 4.f;

	SetUp_Default();

	Add_PrometheusBlood(Circulating_Pulse);
	Add_PrometheusBlood(Severing_Abyss);
	Add_PrometheusBlood(Phantom_Assault);
	Add_PrometheusBlood(Shadow_Assault);

	return NOERROR;
}

_int CPrometheusBlood::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);
	m_pRendererCom->Add_RenderList(RENDER_UI, this);
	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.0f);

	SetUp_SlotPos();
	Click_Icon();

	m_pExitIcon->Set_Active(m_bIsActive);
	return NO_EVENT;
}

_int CPrometheusBlood::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CPrometheusBlood::Render_GameObject()
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

HRESULT CPrometheusBlood::Add_Component()
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

HRESULT CPrometheusBlood::SetUp_ConstantTable()
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

void CPrometheusBlood::SetUp_Default()
{
	m_pExitIcon = static_cast<CInventory_Icon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_InvenIcon", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pExitIcon, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	m_pExitIcon->Set_UI_Pos(m_fPosX + 120.f, m_fPosY - 203.f);
	m_pExitIcon->Set_UI_Size(35.f, 45.f);
	m_pExitIcon->Set_Type(CInventory_Icon::ICON_EXIT);
}

void CPrometheusBlood::SetUp_SlotPos()
{
	_uint iIdx = 0;
	for (auto& iter : m_vecSlot)
	{
		iter->Set_UI_Pos(m_fPosX - 100.f + _float(iIdx) * 50.f, m_fPosY - 100.f);
		iter->Set_Active(m_bIsActive);
		iIdx++;
	}
}

void CPrometheusBlood::Click_Icon()
{
	if (!m_bIsActive)
		return;
	if (m_pExitIcon->Pt_InRect() && g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
	{
		m_bIsActive = false;
		CUI_Manager::Get_Instance()->Get_Total_Inven()->Set_Active(true);
	}
}

void CPrometheusBlood::Add_PrometheusBlood(Skill_ID eSkill_ID)
{
	CSkillSlot* pSlot = static_cast<CSkillSlot*>(g_pManagement->Clone_GameObject_Return(L"GameObject_SkillSlot", nullptr));
	pSlot->Set_SkillID(eSkill_ID);
	pSlot->Set_UI_Size(50.f, 50.f);
	pSlot->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pSlot, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	m_vecSlot.push_back(pSlot);
}

CPrometheusBlood * CPrometheusBlood::Create(_Device pGraphic_Device)
{
	CPrometheusBlood* pInstance = new CPrometheusBlood(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CPrometheusBlood::Clone_GameObject(void * pArg)
{
	CPrometheusBlood* pInstance = new CPrometheusBlood(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CPrometheusBlood::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}