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

void CWeaponBuyUI::Set_Active(_bool bIsActive)
{
	m_bIsActive = bIsActive;

	if (MOVE_END == m_eMoveType && WpnAll_END == m_eWeaponDesc)
	{
		m_pFontDamage->Set_Active(bIsActive);
		m_pMyHazeCnt->Set_Active(bIsActive);
		m_pPriceHazeCnt->Set_Active(bIsActive);
		m_pWeaponMoveTypeUI->Set_Active(bIsActive);
		m_pWeaponDescUI->Set_Active(bIsActive);
		m_pStatusUI->Set_Active(bIsActive);
	}
}

void CWeaponBuyUI::Set_WeaponMoveType(MOVE_TYPE eType)
{
	m_eMoveType = eType;

	switch (m_eMoveType)
	{
	case Client::CWeaponBuyUI::MOVE_STEP:
		m_iTexIndex = 8;
		break;
	case Client::CWeaponBuyUI::MOVE_ROLLING:
		m_iTexIndex = 7;
		break;
	case Client::CWeaponBuyUI::MOVE_HEAVYROLLING:
		m_iTexIndex = 9;
		break;
	}
}

void CWeaponBuyUI::Set_WeaponDescType(WEAPON_ALL_DATA eType)
{
	m_eWeaponDesc = eType;

	m_iTexIndex = m_eWeaponDesc;

	Change_Texture(L"Tex_Item_Desc_Weapon");
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

	return NOERROR;
}

_int CWeaponBuyUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	if (MOVE_END == m_eMoveType && WpnAll_END == m_eWeaponDesc)
	{
		Check_LateInit();
		Check_MoveType();
		Check_Desc();
		Check_ItemOption();
	}

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

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
	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, m_iTexIndex)))
		return E_FAIL;

	return NOERROR;
}

void CWeaponBuyUI::Change_Texture(const _tchar * _Name)
{
	auto& iter = m_pmapComponents.find(L"Com_Texture");

	Safe_Release(m_pTextureCom);
	Safe_Release(iter->second);

	iter->second = m_pTextureCom = static_cast<CTexture*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, _Name));
	Safe_AddRef(iter->second);
}

void CWeaponBuyUI::SetUp_Default()
{
	m_pFontDamage = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pFontDamage->Set_UI_Pos(WINCX * 0.47f, WINCY * 0.38f);
	m_pFontDamage->Set_UI_Size(10.4f, 20.f);
	m_pFontDamage->Set_ViewZ(m_fViewZ - 0.05f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFontDamage, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pMyHazeCnt = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pMyHazeCnt->Set_UI_Pos(WINCX * 0.47f, WINCY * 0.20f);
	m_pMyHazeCnt->Set_UI_Size(10.4f, 20.f);
	m_pMyHazeCnt->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pMyHazeCnt, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pPriceHazeCnt = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pPriceHazeCnt->Set_UI_Pos(WINCX * 0.47f, WINCY * 0.24f);
	m_pPriceHazeCnt->Set_UI_Size(10.4f, 20.f);
	m_pPriceHazeCnt->Set_ViewZ(m_fViewZ - 0.05f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pPriceHazeCnt, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pStatusUI = CUI_Manager::Get_Instance()->Get_StatusUI();
}

void CWeaponBuyUI::Check_ItemOption()
{
	if (!m_bIsActive)
		return;

	CWeapon_Slot* pWeaponSlot = m_pInven->Get_HoverSlot();

	if (!pWeaponSlot)
	{
		pWeaponSlot = m_pInven->Get_SelectedSlot();
		return;
	}

	WPN_PARAM tParam = pWeaponSlot->Get_WeaponParam();
	//tParam.iReinforce;
	
	//==============================================================================================================
	// Damage
	if (!m_pFontDamage)
		return;
	m_pFontDamage->Update_NumberValue((_float)tParam.fDamage);
	m_pFontDamage->Set_Active(true);
	//==============================================================================================================
	// MyHaze
	_int iHazeCnt = _int(CUI_Manager::Get_Instance()->Get_HazeUI()->Get_Haze_Cnt());
	if (!m_pMyHazeCnt)
		return;
	m_pMyHazeCnt->Update_NumberValue((_float)iHazeCnt);
	m_pMyHazeCnt->Set_Active(true);
	//==============================================================================================================
	// HazePrice
	if (!m_pPriceHazeCnt)
		return;
	m_pPriceHazeCnt->Update_NumberValue((_float)tParam.iPrice);
	m_pPriceHazeCnt->Set_Active(true);
	//==============================================================================================================
	// MoveType
	if (!m_pWeaponMoveTypeUI)
		return;
	WEAPON_STATE eWeaponType = WEAPON_STATE(tParam.iWeaponType);
	switch (eWeaponType)
	{
	case WEAPON_None:
		break;
	case WEAPON_SSword:
		m_pWeaponMoveTypeUI->Set_WeaponMoveType(MOVE_STEP);
		break;
	case WEAPON_LSword:
		m_pWeaponMoveTypeUI->Set_WeaponMoveType(MOVE_HEAVYROLLING);
		break;
	case WEAPON_Hammer:
		m_pWeaponMoveTypeUI->Set_WeaponMoveType(MOVE_HEAVYROLLING);
		break;
	case WEAPON_Halberd:
		m_pWeaponMoveTypeUI->Set_WeaponMoveType(MOVE_ROLLING);
		break;
	case WEAPON_Gun:
		m_pWeaponMoveTypeUI->Set_WeaponMoveType(MOVE_STEP);
		break;
	case WEAPON_Shield:
		break;
	case WEAPON_End:
		break;
	}
	m_pWeaponMoveTypeUI->Set_Active(true);
	//==============================================================================================================
	// Desc
	if (!m_pWeaponDescUI)
		return;
	WEAPON_ALL_DATA eAllDate = WEAPON_ALL_DATA(tParam.iWeaponName_InShop);
	if (WpnAll_END == eAllDate)
	{
		m_pWeaponDescUI->Set_Active(false);
		return;
	}
	m_pWeaponDescUI->Set_WeaponDescType(eAllDate);
	m_pWeaponDescUI->Set_Active(true);
	//=======================================================
}

void CWeaponBuyUI::Check_LateInit()
{
	if (m_pInven || !m_pParent)
		return;

	m_pInven = m_pParent->Get_InvenBuy();
	SetUp_Default();
}

void CWeaponBuyUI::Check_MoveType()
{
	if (MOVE_END == m_eMoveType && !m_pWeaponMoveTypeUI)
	{
		m_pWeaponMoveTypeUI = static_cast<CWeaponBuyUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_WeaponBuyUI", nullptr));
		m_pWeaponMoveTypeUI->Set_UI_Pos(WINCX * 0.48f, WINCY * 0.510f);
		m_pWeaponMoveTypeUI->Set_UI_Size(200.0f, 60.f);
		m_pWeaponMoveTypeUI->Set_ViewZ(m_fViewZ - 0.05f);
		m_pWeaponMoveTypeUI->Set_WeaponMoveType(MOVE_STEP);
		g_pManagement->Add_GameOject_ToLayer_NoClone(m_pWeaponMoveTypeUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	}
}

void CWeaponBuyUI::Check_Desc()
{
	if (WpnAll_END == m_eWeaponDesc && !m_pWeaponDescUI)
	{
		m_pWeaponDescUI = static_cast<CWeaponBuyUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_WeaponBuyUI", nullptr));
		m_pWeaponDescUI->Set_UI_Pos(WINCX * 0.55f, WINCY * 0.08f);
		m_pWeaponDescUI->Set_UI_Size(450.0f, 250.0f);
		m_pWeaponDescUI->Set_ViewZ(m_fViewZ - 0.05f);
		m_pWeaponDescUI->Set_WeaponDescType(WEAPON_ALL_DATA::WpnAll_Gun_Bayonet);
		g_pManagement->Add_GameOject_ToLayer_NoClone(m_pWeaponDescUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	}
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
