#include "stdafx.h"
#include "..\Headers\WeaponBuyUI.h"
#include "UI_Manager.h"
#include "Total_Inven.h"

CWeaponBuyUI::CWeaponBuyUI(_Device pDevice)
	: CUI(pDevice)
{
}

CWeaponBuyUI::CWeaponBuyUI(const CWeaponBuyUI & rhs)
	: CUI(rhs)
{
}

HRESULT CWeaponBuyUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CWeaponBuyUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	m_fPosX = 650.f;
	m_fPosY = 350.f;
	m_fSizeX = 512.f * 1.5f;
	m_fSizeY = 512.f * 1.5f;
	m_fViewZ = 1.f;

	m_bIsActive = false;

	SetUp_Default();

	return NOERROR;
}

_int CWeaponBuyUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);
	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	//for (auto& iter : m_vecConditionUI)
	//{
	//	iter->Set_Active(m_bIsActive);
	//}
	//
	//if (m_pExpUI)
	//	m_pExpUI->Set_Active(m_bIsActive);

	return NO_EVENT;
}

_int CWeaponBuyUI::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CWeaponBuyUI::Render_GameObject()
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

HRESULT CWeaponBuyUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_Weapon_Buy_UI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CWeaponBuyUI::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, 0)))
		return E_FAIL;

	return NOERROR;
}

void CWeaponBuyUI::SetUp_Default()
{
	//CConditionUI* pInstance = nullptr;
	//LOOP(5)
	//{
	//	pInstance = static_cast<CConditionUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_ConditionUI", nullptr));
	//	pInstance->Set_UI_Pos(1080.f, 230.f + 43.f * _float(i));
	//	pInstance->Set_UI_Size(199.36f, 80.f);
	//	pInstance->Set_ViewZ(m_fViewZ - 0.1f);
	//	pInstance->Set_ConditionType(CConditionUI::CONDITION_TYPE(i));
	//	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	//	m_vecConditionUI.push_back(pInstance);
	//}
	//
	//m_pExpUI = static_cast<CExpUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_ExpUI", nullptr));
	//m_pExpUI->Set_UI_Pos(m_fPosX, m_fPosY - 120.f);
	//m_pExpUI->Set_UI_Size(70.f, 70.f);
	//m_pExpUI->Set_ViewZ(m_fViewZ - 0.2f);
	//g_pManagement->Add_GameOject_ToLayer_NoClone(m_pExpUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
}

CWeaponBuyUI * CWeaponBuyUI::Create(_Device pGraphic_Device)
{
	CWeaponBuyUI* pInstance = new CWeaponBuyUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CWeaponBuyUI::Clone_GameObject(void * pArg)
{
	CWeaponBuyUI* pInstance = new CWeaponBuyUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CWeaponBuyUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
