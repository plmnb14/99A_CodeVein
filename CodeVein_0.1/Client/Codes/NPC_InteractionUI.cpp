#include "stdafx.h"
#include "..\Headers\NPC_InteractionUI.h"
#include "UI_Manager.h"

CNPC_InteractionUI::CNPC_InteractionUI(_Device pDevice)
	: CUI(pDevice)
{
}

CNPC_InteractionUI::CNPC_InteractionUI(const CNPC_InteractionUI & rhs)
	: CUI(rhs)
{
}

void CNPC_InteractionUI::Set_Alpha(_float fAlpha)
{
	m_fAlpha = fAlpha;
}

void CNPC_InteractionUI::Set_Active(_bool bIsActive)
{
	m_bIsActive = bIsActive;

	m_bInteraction = false;
	m_fAlpha = 1.f;
}

HRESULT CNPC_InteractionUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CNPC_InteractionUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	m_fPosX = WINCX * 0.5f;
	m_fPosY = WINCY - 100.f; // WINCY * 0.5f;
	m_fSizeX = 269.f;
	m_fSizeY = 82.f;

	m_bIsActive = false;
	
	m_iTexIdx = 0;

	SetUp_Default();

	return NOERROR;
}

_int CNPC_InteractionUI::Update_GameObject(_double TimeDelta)
{
	if (!m_bIsActive)
		return S_OK;

	m_fPosX = WINCX * 0.5f;
	m_fPosY = WINCY * 0.8f;

	CUI::Update_GameObject(TimeDelta);

	if (m_bInteraction)
		m_iTexIdx = 1;
	else
		m_iTexIdx = 0;

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);
	return NO_EVENT;
}

_int CNPC_InteractionUI::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CNPC_InteractionUI::Render_GameObject()
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

	m_pShaderCom->Begin_Pass(5);

	m_pBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();

	m_pShaderCom->End_Shader();

	return NOERROR;
}


HRESULT CNPC_InteractionUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_NPC_InteractionButton", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CNPC_InteractionUI::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, m_iTexIdx)))
		return E_FAIL;

	return NOERROR;
}

void CNPC_InteractionUI::Change_Texture(const _tchar * _Name)
{
	auto& iter = m_pmapComponents.find(L"Com_Texture");

	Safe_Release(m_pTextureCom);
	Safe_Release(iter->second);

	iter->second = m_pTextureCom = static_cast<CTexture*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, _Name));
	Safe_AddRef(iter->second);
}

void CNPC_InteractionUI::SetUp_Default()
{

}

CNPC_InteractionUI * CNPC_InteractionUI::Create(_Device pGraphic_Device)
{
	CNPC_InteractionUI* pInstance = new CNPC_InteractionUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CNPC_InteractionUI::Clone_GameObject(void * pArg)
{
	CNPC_InteractionUI* pInstance = new CNPC_InteractionUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CNPC_InteractionUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
