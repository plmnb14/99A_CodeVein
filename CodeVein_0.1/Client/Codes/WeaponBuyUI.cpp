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

	//if (m_pFontDamage)m_pFontDamage->Set_Active(bIsActive);
	//if (m_pMyHazeCnt)m_pMyHazeCnt->Set_Active(bIsActive);
	//if (m_pPriceHazeCnt)m_pPriceHazeCnt->Set_Active(bIsActive);
	//if (m_pWeaponMoveTypeUI)m_pWeaponMoveTypeUI->Set_Active(bIsActive);
	//if (m_pWeaponDescUI)m_pWeaponDescUI->Set_Active(bIsActive);
	//if (m_pFontReinforce)m_pFontReinforce->Set_Active(bIsActive);
	//if (m_pFontPlusOption_0)m_pFontPlusOption_0->Set_Active(bIsActive);
	//if (m_pFontPlusOption_1)m_pFontPlusOption_1->Set_Active(bIsActive);
	//if (m_pFontHP)m_pFontHP->Set_Active(bIsActive);

	if (m_pStatusUI)m_pStatusUI->Set_Active(bIsActive);
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

void CWeaponBuyUI::Set_ArmorDescType(ARMOR_All_DATA eType)
{
	m_eArmorDesc = eType;

	m_iTexIndex = m_eArmorDesc;

	Change_Texture(L"Tex_Item_Desc_Armor");
}

void CWeaponBuyUI::Set_ShopType(SHOP_OPTION eType)
{
	m_eType = eType;

	switch (m_eType)
	{
	case Client::CWeaponBuyUI::SHOP_WEAPON_BUY:
		m_iTexIndex = 0;
		break;
	case Client::CWeaponBuyUI::SHOP_WEAPON_SELL:
		m_iTexIndex = 11;
		break;
	case Client::CWeaponBuyUI::SHOP_ARMOR_BUY:
		m_iTexIndex = 12;
		break;
	case Client::CWeaponBuyUI::SHOP_ARMOR_SELL:
		m_iTexIndex = 13;
		break;
	case Client::CWeaponBuyUI::SHOP_WEAPON_UPGRADE:
		m_iTexIndex = 0;
		break;
	case Client::CWeaponBuyUI::SHOP_ARMOR_UPGRADE:
		m_iTexIndex = 12;
		break; 
	}
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

	m_iTexIndex = 0;

	m_bIsActive = false;

	return NOERROR;
}

_int CWeaponBuyUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	if (MOVE_END == m_eMoveType &&
		WpnAll_END == m_eWeaponDesc &&
		ArmorAll_END == m_eArmorDesc)
	{
		Check_LateInit();
		Check_MoveType();
		Check_Desc();
		Set_NoneSelect();

		if(SHOP_WEAPON_BUY == m_eType ||
			SHOP_WEAPON_SELL == m_eType ||
			SHOP_WEAPON_UPGRADE == m_eType)
			Check_ItemOption_Weapon();
		if (SHOP_ARMOR_BUY == m_eType ||
			SHOP_ARMOR_SELL == m_eType ||
			SHOP_ARMOR_UPGRADE == m_eType)
			Check_ItemOption_Armor();
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
	if(m_bIsDead)
		return NOERROR;

	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	if (!m_bIsActive)
		return NOERROR;
	if (-1 == m_iTexIndex)
		return NOERROR;

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
	m_pFontDamage->Set_UI_Pos(WINCX * 0.46f, WINCY * 0.58f);
	m_pFontDamage->Set_UI_Size(10.4f, 20.f);
	m_pFontDamage->Set_ViewZ(m_fViewZ - 0.05f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFontDamage, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	_float fHazeX = WINCX * 0.5f;
	m_pMyHazeCnt = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pMyHazeCnt->Set_UI_Pos(fHazeX, WINCY * 0.24f);
	m_pMyHazeCnt->Set_UI_Size(10.4f, 20.f);
	m_pMyHazeCnt->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pMyHazeCnt, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pPriceHazeCnt = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pPriceHazeCnt->Set_UI_Pos(fHazeX, WINCY * 0.20f);
	m_pPriceHazeCnt->Set_UI_Size(10.4f, 20.f);
	m_pPriceHazeCnt->Set_ViewZ(m_fViewZ - 0.05f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pPriceHazeCnt, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pFontReinforce = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pFontReinforce->Set_UI_Pos(WINCX * 0.47f, WINCY * 0.35f);
	m_pFontReinforce->Set_UI_Size(10.4f, 20.f);
	m_pFontReinforce->Set_ViewZ(m_fViewZ - 0.05f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFontReinforce, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pFontPlusOption_0 = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pFontPlusOption_0->Set_UI_Pos(WINCX * 0.47f, WINCY * 0.37f);
	m_pFontPlusOption_0->Set_UI_Size(10.4f, 20.f);
	m_pFontPlusOption_0->Set_ViewZ(m_fViewZ - 0.05f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFontPlusOption_0, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pFontPlusOption_1 = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pFontPlusOption_1->Set_UI_Pos(WINCX * 0.47f, WINCY * 0.39f);
	m_pFontPlusOption_1->Set_UI_Size(10.4f, 20.f);
	m_pFontPlusOption_1->Set_ViewZ(m_fViewZ - 0.05f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFontPlusOption_1, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pFontHP = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pFontHP->Set_UI_Pos(WINCX * 0.47f, WINCY * 0.79f);
	m_pFontHP->Set_UI_Size(10.4f, 20.f);
	m_pFontHP->Set_ViewZ(m_fViewZ - 0.05f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFontHP, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pShopItemIcon = static_cast<CShopItemIcon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_ShopItemIcon", nullptr));
	m_pShopItemIcon->Set_UI_Pos(WINCX * 0.385f, WINCY * 0.10f);
	m_pShopItemIcon->Set_UI_Size(100.f, 100.f);
	m_pShopItemIcon->Set_ViewZ(m_fViewZ - 0.05f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pShopItemIcon, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pStatusUI = CUI_Manager::Get_Instance()->Get_StatusUI();
}

void CWeaponBuyUI::Check_ItemOption_Weapon()
{
	if (SHOP_WEAPON_UPGRADE == m_eType)
	{
		m_pInven = m_pParent->Get_InvenUpgradeWeapon();

		if (!m_pInven)
			return;

		if (m_pInven->Get_CheckCloseUpgradePopup())
		{
			this->Set_Active(true);
			m_pInven->Set_CheckCloseUpgradePopup();
		}
	}

	if (!m_pParent)
		return;
	if (!m_bIsActive)
		return;

	switch (m_eType)
	{
	case Client::CWeaponBuyUI::SHOP_WEAPON_BUY:
		m_pInven = m_pParent->Get_InvenBuyWeapon();
		break;
	case Client::CWeaponBuyUI::SHOP_WEAPON_SELL:
		m_pInven = m_pParent->Get_InvenSellWeapon();
		break;
	case Client::CWeaponBuyUI::SHOP_ARMOR_BUY:
		m_pInven = m_pParent->Get_InvenBuyArmor();
		break;
	case Client::CWeaponBuyUI::SHOP_ARMOR_SELL:
		m_pInven = m_pParent->Get_InvenSellArmor();
		break;
	case Client::CWeaponBuyUI::SHOP_ITEM_BUY:
		break;
	case Client::CWeaponBuyUI::SHOP_ITEM_SELL:
		break;
	case Client::CWeaponBuyUI::SHOP_WEAPON_UPGRADE:
	{
		m_pInven = m_pParent->Get_InvenUpgradeWeapon();

		if (!m_pInven)
			return;
		this->Set_Active(!m_pInven->Get_PopupOn());
		m_pStatusUI->Set_Active(!m_pInven->Get_PopupOn());
		break;
	}
	}

	if (SHOP_WEAPON_BUY == m_eType ||
		SHOP_WEAPON_SELL == m_eType ||
		SHOP_WEAPON_UPGRADE == m_eType)
	{
		m_pFontReinforce->Set_UI_Pos(WINCX * 0.46f, WINCY * 0.39f);
		m_pFontPlusOption_0->Set_UI_Pos(WINCX * 0.46f, WINCY * 0.425f);
		m_pFontDamage->Set_UI_Pos(WINCX * 0.46f, WINCY * 0.58f);
	}

	if (!m_pInven)
		return;

	CWeapon_Slot* pWeaponSlot = m_pInven->Get_HoverSlot_Weapon();

	if (m_pInven->Get_PopupOn())
		pWeaponSlot = m_pInven->Get_SelectedSlot_Weapon();

	if (!pWeaponSlot || pWeaponSlot->Get_Dead())
		return;

	WPN_PARAM tParam = pWeaponSlot->Get_WeaponParam();

	//==============================================================================================================
	// MyHaze
	_int iHazeCnt = _int(CUI_Manager::Get_Instance()->Get_HazeUI()->Get_Haze_Cnt());
	if (!m_pMyHazeCnt)
		return;
	m_pMyHazeCnt->Update_NumberValue((_float)iHazeCnt);
	m_pMyHazeCnt->Set_Active(m_bIsActive);
	//==============================================================================================================
	// Damage
	if (!m_pFontDamage)
		return;
	m_pFontDamage->Update_NumberValue(tParam.fDamage + tParam.fPlusDamage);
	m_pFontDamage->Set_Active(m_bIsActive);
	//==============================================================================================================
	// HazePrice
	if (!m_pPriceHazeCnt)
		return;

	_int iPrice = tParam.iPrice;
	if (SHOP_ARMOR_UPGRADE == m_eType)
		iPrice = (_int)m_pInven->Get_UpgradePrice(tParam.iReinforce);

	m_pPriceHazeCnt->Update_NumberValue((_float)iPrice);
	m_pPriceHazeCnt->Set_Active(m_bIsActive);
	//==============================================================================================================
	// Reinforce
	if (!m_pFontReinforce)
		return;
	m_pFontReinforce->Update_NumberValue((_float)tParam.iReinforce);
	m_pFontReinforce->Set_Active(m_bIsActive);

	if (!m_pFontPlusOption_0)
		return;
	m_pFontPlusOption_0->Update_NumberValue((_float)tParam.fPlusDamage);
	m_pFontPlusOption_0->Set_Active(m_bIsActive);

	// Option_1 false
	if (!m_pFontPlusOption_1)
		return;
	m_pFontPlusOption_1->Set_Active(false);

	//==============================================================================================================
	// HP - false
	if (!m_pFontHP)
		return;
	m_pFontHP->Set_Active(false);
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
	m_pWeaponMoveTypeUI->Set_Active(m_bIsActive);
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
	m_pWeaponDescUI->Set_Active(m_bIsActive);

	//==============================================================================================================
	// Icon
	if (!m_pShopItemIcon)
		return;
	if (WpnAll_END == eAllDate)
	{
		m_pShopItemIcon->Set_Active(false);
		return;
	}
	m_pShopItemIcon->Set_WeaponDescType(eAllDate);
	m_pShopItemIcon->Set_Active(m_bIsActive);	
	//=======================================================
}

void CWeaponBuyUI::Check_ItemOption_Armor()
{
	if (SHOP_ARMOR_UPGRADE == m_eType)
	{
		m_pInven = m_pParent->Get_InvenUpgradeArmor();

		if (!m_pInven)
			return;

		if (m_pInven->Get_CheckCloseUpgradePopup())
		{
			this->Set_Active(true);
			m_pInven->Set_CheckCloseUpgradePopup();
		}
	}

	if (!m_pParent)
		return;
	if (!m_bIsActive)
		return;

	switch (m_eType)
	{
	case Client::CWeaponBuyUI::SHOP_WEAPON_BUY:
		m_pInven = m_pParent->Get_InvenBuyWeapon();
		break;
	case Client::CWeaponBuyUI::SHOP_WEAPON_SELL:
		m_pInven = m_pParent->Get_InvenSellWeapon();
		break;
	case Client::CWeaponBuyUI::SHOP_ARMOR_BUY:
		m_pInven = m_pParent->Get_InvenBuyArmor();
		break;
	case Client::CWeaponBuyUI::SHOP_ARMOR_SELL:
		m_pInven = m_pParent->Get_InvenSellArmor();
		break;
	case Client::CWeaponBuyUI::SHOP_ARMOR_UPGRADE:
	{
		m_pInven = m_pParent->Get_InvenUpgradeArmor();

		if (!m_pInven)
			return;
		this->Set_Active(!m_pInven->Get_PopupOn());
		m_pStatusUI->Set_Active(!m_pInven->Get_PopupOn());
		break;
	}
	}

	if (SHOP_ARMOR_BUY == m_eType ||
		SHOP_ARMOR_SELL == m_eType||
		SHOP_ARMOR_UPGRADE == m_eType)
	{
		m_pFontDamage->Set_UI_Pos(WINCX * 0.46f, WINCY * 0.64f);

		m_pFontReinforce->Set_UI_Pos(WINCX * 0.46f, WINCY * 0.39f);

		m_pFontPlusOption_0->Set_UI_Pos(WINCX * 0.46f, WINCY * 0.4225f);

		m_pFontPlusOption_1->Set_UI_Pos(WINCX * 0.46f, WINCY * 0.455f);

		m_pFontHP->Set_UI_Pos(WINCX * 0.46f, WINCY * 0.78f);
	}

	if (!m_pInven)
		return;

	CArmor_Slot* pArmorSlot = m_pInven->Get_HoverSlot_Armor();

	if (m_pInven->Get_PopupOn())
		pArmorSlot = m_pInven->Get_SelectedSlot_Armor();

	if (!pArmorSlot || pArmorSlot->Get_Dead())
		return;

	ARMOR_PARAM tParam = pArmorSlot->Get_ArmorParam();

	//==============================================================================================================
	// MyHaze
	_int iHazeCnt = _int(CUI_Manager::Get_Instance()->Get_HazeUI()->Get_Haze_Cnt());
	if (!m_pMyHazeCnt)
		return;
	m_pMyHazeCnt->Update_NumberValue((_float)iHazeCnt);
	m_pMyHazeCnt->Set_Active(m_bIsActive);
	//==============================================================================================================
	// Def
	if (!m_pFontDamage)
		return;
	m_pFontDamage->Update_NumberValue((_float)tParam.fDef);
	m_pFontDamage->Set_Active(m_bIsActive);
	//==============================================================================================================
	// HazePrice
	if (!m_pPriceHazeCnt)
		return;

	_int iPrice = tParam.iPrice;
	if (SHOP_ARMOR_UPGRADE == m_eType)
		iPrice = (_int)m_pInven->Get_UpgradePrice(tParam.iReinforce);

	m_pPriceHazeCnt->Update_NumberValue((_float)iPrice);
	m_pPriceHazeCnt->Set_Active(m_bIsActive);
	//==============================================================================================================
	// Reinforce
	if (!m_pFontReinforce)
		return;
	m_pFontReinforce->Update_NumberValue((_float)tParam.iReinforce);
	m_pFontReinforce->Set_Active(m_bIsActive);

	if (!m_pFontPlusOption_0)
		return;
	m_pFontPlusOption_0->Update_NumberValue((_float)tParam.fPlusDef);
	m_pFontPlusOption_0->Set_Active(m_bIsActive);

	if (!m_pFontPlusOption_1)
		return;
	m_pFontPlusOption_1->Update_NumberValue((_float)tParam.fPlusHP);
	m_pFontPlusOption_1->Set_Active(m_bIsActive);
	//==============================================================================================================
	// HP
	if (!m_pFontHP)
		return;
	m_pFontHP->Update_NumberValue(tParam.fHP + tParam.fPlusHP);
	m_pFontHP->Set_Active(m_bIsActive);
	//==============================================================================================================
	// MoveType - false
	if (!m_pWeaponMoveTypeUI)
		return;
	m_pWeaponMoveTypeUI->Set_Active(false);
	//==============================================================================================================
	// Desc
	if (!m_pWeaponDescUI)
		return;
	ARMOR_All_DATA eAllDate = ARMOR_All_DATA(tParam.iArmorName);
	if (ArmorAll_END == eAllDate)
	{
		m_pWeaponDescUI->Set_Active(false);
		return;
	}
	m_pWeaponDescUI->Set_ArmorDescType(eAllDate);
	m_pWeaponDescUI->Set_Active(m_bIsActive);
	//==============================================================================================================
	// Icon
	if (!m_pShopItemIcon)
		return;
	if (ArmorAll_END == eAllDate)
	{
		m_pShopItemIcon->Set_Active(false);
		return;
	}
	m_pShopItemIcon->Set_ArmorDescType(eAllDate);
	m_pShopItemIcon->Set_Active(m_bIsActive);

}

void CWeaponBuyUI::Set_NoneSelect()
{
	m_pMyHazeCnt->Set_Active(false);
	m_pFontDamage->Set_Active(false);
	m_pPriceHazeCnt->Set_Active(false);
	m_pFontReinforce->Set_Active(false);
	m_pFontPlusOption_0->Set_Active(false);
	m_pFontPlusOption_1->Set_Active(false);
	m_pFontHP->Set_Active(false);
	m_pWeaponMoveTypeUI->Set_Active(false);
	m_pWeaponDescUI->Set_Active(false);
	m_pShopItemIcon->Set_Active(false);
}

void CWeaponBuyUI::Check_LateInit()
{
	if (m_bLateInit)
		return;
	m_bLateInit = true;

	SetUp_Default();
}

void CWeaponBuyUI::Check_MoveType()
{
	if (MOVE_END == m_eMoveType && !m_pWeaponMoveTypeUI)
	{
		m_pWeaponMoveTypeUI = static_cast<CWeaponBuyUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_WeaponBuyUI", nullptr));
		m_pWeaponMoveTypeUI->Set_UI_Pos(WINCX * 0.448f, WINCY * 0.710f);
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
		m_pWeaponDescUI->Set_UI_Pos(WINCX * 0.549f, WINCY * 0.079f);
		m_pWeaponDescUI->Set_UI_Size(455.0f, 255.0f);
		m_pWeaponDescUI->Set_ViewZ(m_fViewZ - 0.05f);
		m_pWeaponDescUI->Set_WeaponDescType(WEAPON_ALL_DATA::WpnAll_Gun_Bayonet);
		m_pWeaponDescUI->Set_ArmorDescType(ARMOR_All_DATA::ArmorAll_Gauntlet_DarkNightHook);
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
