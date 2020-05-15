#include "stdafx.h"
#include "..\Headers\WeaponUpgradeUI.h"
#include "UI_Manager.h"
#include "Total_Inven.h"
#include "CollisionMgr.h"

CWeaponUpgradeUI::CWeaponUpgradeUI(_Device pDevice)
	: CUI(pDevice)
{
}

CWeaponUpgradeUI::CWeaponUpgradeUI(const CWeaponUpgradeUI & rhs)
	: CUI(rhs)
{
}

void CWeaponUpgradeUI::Set_Active(_bool bIsActive)
{
	m_bIsActive = bIsActive;
}

void CWeaponUpgradeUI::Set_WeaponDescType(WEAPON_ALL_DATA eType)
{
	m_eWeaponDesc = eType;

	m_iTexIdx = m_eWeaponDesc;

	Change_Texture(L"Tex_Item_Name_Weapon");
}

void CWeaponUpgradeUI::Set_ArmorDescType(ARMOR_All_DATA eType)
{
	m_eArmorDesc = eType;

	m_iTexIdx = m_eArmorDesc;

	Change_Texture(L"Tex_Item_Name_Armor");
}

void CWeaponUpgradeUI::Set_WeaponMoveType(MOVE_TYPE eType)
{
	m_eMoveType = eType;

	switch (m_eMoveType)
	{
	case Client::CWeaponBuyUI::MOVE_STEP:
		m_iTexIdx = 8;
		break;
	case Client::CWeaponBuyUI::MOVE_ROLLING:
		m_iTexIdx = 7;
		break;
	case Client::CWeaponBuyUI::MOVE_HEAVYROLLING:
		m_iTexIdx = 9;
		break;
	}
}

HRESULT CWeaponUpgradeUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CWeaponUpgradeUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	m_fPosX = 650.f;
	m_fPosY = 350.f;
	m_fSizeX = 1920.f * 0.7f;
	m_fSizeY = 1080.f * 0.7f;

	m_bIsActive = false;
	
	m_iTexIdx = 0;

	SetUp_Default();

	return NOERROR;
}

_int CWeaponUpgradeUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);
	
	if (MOVE_END == m_eMoveType && WpnAll_END == m_eWeaponDesc)
	{
		switch (m_eUpgradeType)
		{
		case Client::CWeaponUpgradeUI::UPGRADE_WEAPON:
			Check_ItemOption_Weapon();
			break;
		case Client::CWeaponUpgradeUI::UPGRADE_ARMOR:
			Check_ItemOption_Armor();
			break;
		}

		Check_Option();
		Check_WeaponName();
		Check_MoveType();
		Check_LateInit();

		if (m_pFontReinforceHeader)m_pFontReinforceHeader->Set_Active(m_bIsActive);
		if (m_pFontReinforceDesc)m_pFontReinforceDesc->Set_Active(m_bIsActive);
		if (m_pFontReinforceAfterDesc)m_pFontReinforceAfterDesc->Set_Active(m_bIsActive);
		if (m_pFontPlusDamageDesc)m_pFontPlusDamageDesc->Set_Active(m_bIsActive);
		if (m_pFontPlusDamageAfterDesc)m_pFontPlusDamageAfterDesc->Set_Active(m_bIsActive);
		if (m_pFontDamageDesc)m_pFontDamageDesc->Set_Active(m_bIsActive);
		if (m_pFontDamageAfterDesc)m_pFontDamageAfterDesc->Set_Active(m_bIsActive);
		if (m_pFontPlusHPAfterDesc)m_pFontPlusHPAfterDesc->Set_Active(m_bIsActive);
		if (m_pFontPlusHPDesc)m_pFontPlusHPDesc->Set_Active(m_bIsActive);
		if (m_pFontTotalHPDesc)m_pFontTotalHPDesc->Set_Active(m_bIsActive);
		if (m_pFontTotalHPAfterDesc)m_pFontTotalHPAfterDesc->Set_Active(m_bIsActive);
		if (m_pWeaponNameUI)m_pWeaponNameUI->Set_Active(m_bIsActive);
		
		if (m_pFontRequireHaze)m_pFontRequireHaze->Set_Active(m_bIsActive);
		if (m_pFontMyHaze)m_pFontMyHaze->Set_Active(m_bIsActive);
		
		if (m_pFontRequireMatCnt_0)m_pFontRequireMatCnt_0->Set_Active(m_bIsActive);
		if (m_pFontRequireMatCnt_1)m_pFontRequireMatCnt_1->Set_Active(m_bIsActive);
		if (m_pFontRequireMatCnt_2)m_pFontRequireMatCnt_2->Set_Active(m_bIsActive);
		
		if (m_pFontMyMatCnt_0)m_pFontMyMatCnt_0->Set_Active(m_bIsActive);
		if (m_pFontMyMatCnt_1)m_pFontMyMatCnt_1->Set_Active(m_bIsActive);
		if (m_pFontMyMatCnt_2)m_pFontMyMatCnt_2->Set_Active(m_bIsActive);
		
		if (m_pFontSlash_0)m_pFontSlash_0->Set_Active(m_bIsActive);
		if (m_pFontSlash_1)m_pFontSlash_1->Set_Active(m_bIsActive);
		if (m_pFontSlash_2)m_pFontSlash_2->Set_Active(m_bIsActive);

		m_fViewZ = 0.2f;

		switch (m_eUpgradeType)
		{
		case Client::CWeaponUpgradeUI::UPGRADE_WEAPON:
			if (m_pWeaponMoveTypeUI)m_pWeaponMoveTypeUI->Set_Active(m_bIsActive);
			m_iTexIdx = 0;
			break;
		case Client::CWeaponUpgradeUI::UPGRADE_ARMOR:
		{
			m_pFontDamageDesc->Set_UI_Pos(WINCX * 0.25f, WINCY * 0.5825f);
			m_pFontDamageAfterDesc->Set_UI_Pos(WINCX * 0.32f, WINCY * 0.5825f);
			m_pFontPlusHPDesc->Set_UI_Pos(WINCX * 0.25f, WINCY * 0.43f);
			m_pFontPlusHPAfterDesc->Set_UI_Pos(WINCX * 0.32f, WINCY * 0.43f);
			m_pFontTotalHPDesc->Set_UI_Pos(WINCX * 0.25f, WINCY * 0.718f);
			m_pFontTotalHPAfterDesc->Set_UI_Pos(WINCX * 0.32f, WINCY * 0.718f);
			break;
		}
		}
	}

	if (WpnAll_END == m_eWeaponDesc &&
		MOVE_END == m_eMoveType &&
		m_bLateInit)
	{

		if (m_pWeaponNameUI)m_pWeaponNameUI->Set_UI_Pos(WINCX * 0.348f, WINCY * 0.13f);
		if (m_pWeaponNameUI)m_pWeaponNameUI->Set_UI_Size(450.0f * 1.5f, 120.f* 1.5f);

		if (m_pWeaponMoveTypeUI)m_pWeaponMoveTypeUI->Set_UI_Pos(WINCX * 0.27f, WINCY * 0.715f);
		if (m_pWeaponMoveTypeUI)m_pWeaponMoveTypeUI->Set_UI_Size(210.0f, 60.f);

		LOOP(2)
		{
			TARGET_TO_TRANS(m_vecOption[i])->Set_Angle(m_pTransformCom->Get_Angle());
			TARGET_TO_TRANS(m_vecOption[i])->Set_Scale(_v3(1.f, 1.f, 1.f));
			TARGET_TO_TRANS(m_vecOption[i])->Set_At(m_pTransformCom->Get_At());

			_v3 vPos;
			if (0 == i)
				m_vecOption[i]->Set_UI_Pos(Get_UI_Pos().x, Get_UI_Pos().y + 265.f);
			else if (1 == i)
				m_vecOption[i]->Set_UI_Pos(Get_UI_Pos().x, Get_UI_Pos().y + 305.f);

			m_vecOption[i]->Set_UI_Size(126, 60);

			TARGET_TO_TRANS(m_vecOption[i])->Set_Pos(vPos);
			m_vecOption[i]->Set_ButtonType(CWeaponUpgradeOptionUI::BUTTON_TYPE(i));
			m_vecOption[i]->Set_Active(m_bIsActive);
		}
	}

	if (!m_bIsActive)
		return S_OK;

	switch (m_eUpgradeType)
	{
	case Client::CWeaponUpgradeUI::UPGRADE_WEAPON:
		m_iTexIdx = 0;
		break;
	case Client::CWeaponUpgradeUI::UPGRADE_ARMOR:
		m_iTexIdx = 1;
		break;
	}

	Click_Option();

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);
	return NO_EVENT;
}

_int CWeaponUpgradeUI::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CWeaponUpgradeUI::Render_GameObject()
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


void CWeaponUpgradeUI::Click_Option()
{
	if (!m_bIsActive)
		return;
	
	_int iIdx = 0;
	
	for (auto& iter : m_vecOption)
	{
		if (iter->Pt_InRect())
		{
			Reset_Option();
			iter->Set_Select(true);
	
			if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
			{
				if (0 == iIdx)
				{
					switch (m_eUpgradeType)
					{
					case Client::CWeaponUpgradeUI::UPGRADE_WEAPON:
						Upgrade_Weapon();
						break;
					case Client::CWeaponUpgradeUI::UPGRADE_ARMOR:
						Upgrade_Armor();
						break;
					}
				}
				else if (1 == iIdx)
				{
					Set_Active(false);
				}
			}
		}
		else
		{
			iter->Set_Select(false);
		}
	
		++iIdx;
	}
}

void CWeaponUpgradeUI::Reset_Option()
{
	for (auto& iter : m_vecOption)
	{
		iter->Set_Select(false);
	}
}

void CWeaponUpgradeUI::Upgrade_Weapon()
{
	CWeapon_Slot* pSlot = m_pInven->Get_SelectedSlot_Weapon();
	_int iReinForce = pSlot->Get_WeaponParam().iReinforce;
	_int iPrice = (_int)m_pInven->Get_UpgradePrice(iReinForce);
	_int iMyHaze = (_int)CUI_Manager::Get_Instance()->Get_HazeUI()->Get_Haze_Cnt();

	if (iPrice > iMyHaze)
		return;

	vector<CMaterial_Slot*>* pInvenMaterial = CUI_Manager::Get_Instance()->Get_Material_Inven()->Get_VecMaterialSlot();

	_int iSteel				= Get_MyMaterial(CMaterial::MATERIAL_TYPE::Queen_Steel);
	_int iTitanium			= Get_MyMaterial(CMaterial::MATERIAL_TYPE::Queen_Titanium);
	_int iTungsten			= Get_MyMaterial(CMaterial::MATERIAL_TYPE::Queen_Tungsten);
	_int iRequireSteel		= Get_RequireMaterial(CMaterial::MATERIAL_TYPE::Queen_Steel, iReinForce);
	_int iRequireTitanium	= Get_RequireMaterial(CMaterial::MATERIAL_TYPE::Queen_Titanium, iReinForce);
	_int iRequireTungsten	= Get_RequireMaterial(CMaterial::MATERIAL_TYPE::Queen_Tungsten, iReinForce);

	if(iSteel < iRequireSteel)
		return;
	if (iTitanium < iRequireTitanium)
		return;
	if (iTungsten < iRequireTungsten)
		return;

	// =======================================================================
	// Can Upgrade
	CUI_Manager::Get_Instance()->Get_HazeUI()->Accumulate_Haze(-iPrice);

	for (auto& iter : *pInvenMaterial)
	{
		if (CMaterial::MATERIAL_TYPE::Queen_Steel == iter->Get_Type())
		{
			CUI_Manager::Get_Instance()->Get_Material_Inven()->Sell_Material(iRequireSteel);
		}
		else if (CMaterial::MATERIAL_TYPE::Queen_Titanium == iter->Get_Type())
		{
			CUI_Manager::Get_Instance()->Get_Material_Inven()->Sell_Material(iRequireTitanium);
		}
		else if (CMaterial::MATERIAL_TYPE::Queen_Tungsten == iter->Get_Type())
		{
			CUI_Manager::Get_Instance()->Get_Material_Inven()->Sell_Material(iRequireTungsten);
		}
	}

	if (false == Get_UpgradeSuccess(iReinForce))
	{
		m_pUpgradeResultPopup->Set_Active(true);
		m_pUpgradeResultPopup->Set_Inven(m_pInven);
		m_pUpgradeResultPopup->Set_PopupType(CWeaponUpgradeSuccessPopupUI::POPUP_FAILED);
		return;
	}

	// =======================================================================
	// Success
	WPN_PARAM tParam = pSlot->Get_WeaponParam();
	tParam.iReinforce += 1;
	tParam.fPlusDamage = Get_PlusDamage(tParam.fDamage, tParam.iReinforce);
	pSlot->Set_WeaponParam(tParam);
	m_pInven->Upgrade_Weapon(tParam);

	m_pUpgradeResultPopup->Set_Active(true);
	m_pUpgradeResultPopup->Set_Inven(m_pInven);
	m_pUpgradeResultPopup->Set_PopupType(CWeaponUpgradeSuccessPopupUI::POPUP_SUCCESS);
}

void CWeaponUpgradeUI::Upgrade_Armor()
{
	CArmor_Slot* pSlot = m_pInven->Get_SelectedSlot_Armor();
	_int iReinForce = pSlot->Get_ArmorParam().iReinforce;
	_int iPrice = (_int)m_pInven->Get_UpgradePrice(iReinForce);
	_int iMyHaze = (_int)CUI_Manager::Get_Instance()->Get_HazeUI()->Get_Haze_Cnt();

	if (iPrice > iMyHaze)
		return;

	vector<CMaterial_Slot*>* pInvenMaterial = CUI_Manager::Get_Instance()->Get_Material_Inven()->Get_VecMaterialSlot();

	_int iSteel = Get_MyMaterial(CMaterial::MATERIAL_TYPE::Queen_Steel);
	_int iTitanium = Get_MyMaterial(CMaterial::MATERIAL_TYPE::Queen_Titanium);
	_int iTungsten = Get_MyMaterial(CMaterial::MATERIAL_TYPE::Queen_Tungsten);
	_int iRequireSteel = Get_RequireMaterial(CMaterial::MATERIAL_TYPE::Queen_Steel, iReinForce);
	_int iRequireTitanium = Get_RequireMaterial(CMaterial::MATERIAL_TYPE::Queen_Titanium, iReinForce);
	_int iRequireTungsten = Get_RequireMaterial(CMaterial::MATERIAL_TYPE::Queen_Tungsten, iReinForce);

	if (iSteel < iRequireSteel)
		return;
	if (iTitanium < iRequireTitanium)
		return;
	if (iTungsten < iRequireTungsten)
		return;

	// =======================================================================
	// Can Upgrade
	CUI_Manager::Get_Instance()->Get_HazeUI()->Accumulate_Haze(-iPrice);
	
	for (auto& iter : *pInvenMaterial)
	{
		if (CMaterial::MATERIAL_TYPE::Queen_Steel == iter->Get_Type())
		{
			CUI_Manager::Get_Instance()->Get_Material_Inven()->Sell_Material(iRequireSteel);
		}
		else if (CMaterial::MATERIAL_TYPE::Queen_Titanium == iter->Get_Type())
		{
			CUI_Manager::Get_Instance()->Get_Material_Inven()->Sell_Material(iRequireTitanium);
		}
		else if (CMaterial::MATERIAL_TYPE::Queen_Tungsten == iter->Get_Type())
		{
			CUI_Manager::Get_Instance()->Get_Material_Inven()->Sell_Material(iRequireTungsten);
		}
	}

	if (false == Get_UpgradeSuccess(iReinForce))
	{
		m_pUpgradeResultPopup->Set_Active(true);
		m_pUpgradeResultPopup->Set_Inven(m_pInven);
		m_pUpgradeResultPopup->Set_PopupType(CWeaponUpgradeSuccessPopupUI::POPUP_FAILED);
		return;
	}

	// =======================================================================
	// Success

	ARMOR_PARAM tParam = pSlot->Get_ArmorParam();
	tParam.iReinforce += 1;
	tParam.fPlusDef = Get_PlusDamage(tParam.fDef, tParam.iReinforce);
	pSlot->Set_ArmorParam(tParam);
	m_pInven->Upgrade_Armor(tParam);

	m_pUpgradeResultPopup->Set_Active(true);
	m_pUpgradeResultPopup->Set_Inven(m_pInven);
	m_pUpgradeResultPopup->Set_PopupType(CWeaponUpgradeSuccessPopupUI::POPUP_SUCCESS);
}

_int CWeaponUpgradeUI::Get_MyMaterial(CMaterial::MATERIAL_TYPE eType)
{
	_int iSteel = 0;
	_int iTitanium = 0;
	_int iTungsten = 0;

	vector<CMaterial_Slot*>* pInvenMaterial = CUI_Manager::Get_Instance()->Get_Material_Inven()->Get_VecMaterialSlot();
	for (auto& iter : *pInvenMaterial)
	{
		if (CMaterial::MATERIAL_TYPE::Queen_Steel == iter->Get_Type())
		{
			iSteel += iter->Get_Size();
		}
		else if (CMaterial::MATERIAL_TYPE::Queen_Titanium == iter->Get_Type())
		{
			iTitanium += iter->Get_Size();
		}
		else if (CMaterial::MATERIAL_TYPE::Queen_Tungsten == iter->Get_Type())
		{
			iTungsten += iter->Get_Size();
		}
	}

	switch (eType)
	{
	case Client::CMaterial::Queen_Steel:
		return iSteel;
	case Client::CMaterial::Queen_Titanium:
		return iTitanium;
	case Client::CMaterial::Queen_Tungsten:
		return iTungsten;
	}

	return -1;
}

_int CWeaponUpgradeUI::Get_RequireMaterial(CMaterial::MATERIAL_TYPE eType, _int iReinforce)
{
	switch (eType)
	{
	case Client::CMaterial::Queen_Steel:
		return iReinforce * 3 + 1;
	case Client::CMaterial::Queen_Titanium:
		return iReinforce * 2 + 1;
	case Client::CMaterial::Queen_Tungsten:
		return _int(iReinforce * 0.4f);
	}

	return -1;
}

_float CWeaponUpgradeUI::Get_PlusDamage(_float fDamage, _int iReinforce)
{
	return fDamage + (iReinforce * 1.5f) * (fDamage * 0.15f);
}

_bool CWeaponUpgradeUI::Get_UpgradeSuccess(_int iReinforce)
{
	_int iSuccessPercentage = 100;
	for (_int i = 0; i < iReinforce; i++)
		iSuccessPercentage = _int(iSuccessPercentage * 0.5f);

	_int iRandom = CCalculater::Random_Num(1, 100);

	cout << iSuccessPercentage << endl;
	
	if (iRandom < iSuccessPercentage)
		return true;
	else
		return false;

}

HRESULT CWeaponUpgradeUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_WeaponShop_Upgrade_UI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CWeaponUpgradeUI::SetUp_ConstantTable()
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

void CWeaponUpgradeUI::Change_Texture(const _tchar * _Name)
{
	auto& iter = m_pmapComponents.find(L"Com_Texture");

	Safe_Release(m_pTextureCom);
	Safe_Release(iter->second);

	iter->second = m_pTextureCom = static_cast<CTexture*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, _Name));
	Safe_AddRef(iter->second);
}

void CWeaponUpgradeUI::SetUp_Default()
{
	_float fMul = 0.5f;
	_float fPosX_1 = WINCX * 0.25f;
	_float fPosX_2 = WINCX * 0.32f;
	_float fPosY_1 = WINCY * 0.36f;
	_float fPosY_2 = WINCY * 0.395f;
	_float fPosY_3 = WINCY * 0.55f;

	m_pFontReinforceHeader = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pFontReinforceHeader->Set_UI_Pos(WINCX * 0.55f, WINCY * 0.13f);
	m_pFontReinforceHeader->Set_UI_Size(20.4f, 35.f);
	m_pFontReinforceHeader->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFontReinforceHeader, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pFontReinforceDesc = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pFontReinforceDesc->Set_UI_Pos(fPosX_1, fPosY_1);
	m_pFontReinforceDesc->Set_UI_Size(20.4f * fMul, 35.f * fMul);
	m_pFontReinforceDesc->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFontReinforceDesc, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pFontReinforceAfterDesc = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pFontReinforceAfterDesc->Set_UI_Pos(fPosX_2, fPosY_1);
	m_pFontReinforceAfterDesc->Set_UI_Size(20.4f * fMul, 35.f * fMul);
	m_pFontReinforceAfterDesc->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFontReinforceAfterDesc, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pFontPlusDamageDesc = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pFontPlusDamageDesc->Set_UI_Pos(fPosX_1, fPosY_2);
	m_pFontPlusDamageDesc->Set_UI_Size(20.4f * fMul, 35.f * fMul);
	m_pFontPlusDamageDesc->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFontPlusDamageDesc, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pFontPlusDamageAfterDesc = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pFontPlusDamageAfterDesc->Set_UI_Pos(fPosX_2, fPosY_2);
	m_pFontPlusDamageAfterDesc->Set_UI_Size(20.4f * fMul, 35.f * fMul);
	m_pFontPlusDamageAfterDesc->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFontPlusDamageAfterDesc, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pFontPlusHPDesc = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pFontPlusHPDesc->Set_UI_Pos(fPosX_2, fPosY_2);
	m_pFontPlusHPDesc->Set_UI_Size(20.4f * fMul, 35.f * fMul);
	m_pFontPlusHPDesc->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFontPlusHPDesc, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pFontPlusHPAfterDesc = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pFontPlusHPAfterDesc->Set_UI_Pos(fPosX_2, fPosY_2);
	m_pFontPlusHPAfterDesc->Set_UI_Size(20.4f * fMul, 35.f * fMul);
	m_pFontPlusHPAfterDesc->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFontPlusHPAfterDesc, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pFontTotalHPDesc = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pFontTotalHPDesc->Set_UI_Pos(fPosX_2, fPosY_2);
	m_pFontTotalHPDesc->Set_UI_Size(20.4f * fMul, 35.f * fMul);
	m_pFontTotalHPDesc->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFontTotalHPDesc, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pFontTotalHPAfterDesc = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pFontTotalHPAfterDesc->Set_UI_Pos(fPosX_2, fPosY_2);
	m_pFontTotalHPAfterDesc->Set_UI_Size(20.4f * fMul, 35.f * fMul);
	m_pFontTotalHPAfterDesc->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFontTotalHPAfterDesc, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pFontDamageDesc = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pFontDamageDesc->Set_UI_Pos(fPosX_1, fPosY_3);
	m_pFontDamageDesc->Set_UI_Size(20.4f * fMul, 35.f * fMul);
	m_pFontDamageDesc->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFontDamageDesc, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pFontDamageAfterDesc = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pFontDamageAfterDesc->Set_UI_Pos(fPosX_2, fPosY_3);
	m_pFontDamageAfterDesc->Set_UI_Size(20.4f * fMul, 35.f * fMul);
	m_pFontDamageAfterDesc->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFontDamageAfterDesc, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pFontRequireHaze = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pFontRequireHaze->Set_UI_Pos(WINCX * 0.9f, WINCY * 0.11f);
	m_pFontRequireHaze->Set_UI_Size(20.4f * fMul, 35.f * fMul);
	m_pFontRequireHaze->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFontRequireHaze, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pFontMyHaze = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pFontMyHaze->Set_UI_Pos(WINCX * 0.9f, WINCY * 0.15f);
	m_pFontMyHaze->Set_UI_Size(20.4f * fMul, 35.f * fMul);
	m_pFontMyHaze->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFontMyHaze, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	
	fPosX_1 = WINCX * 0.75f;
	fPosX_2 = WINCX * 0.82f;
	fPosY_1 = WINCY * 0.43f;
	fPosY_2 = WINCY * 0.51f;
	fPosY_3 = WINCY * 0.59f;
	
	m_pFontRequireMatCnt_0 = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pFontRequireMatCnt_0->Set_UI_Pos(fPosX_2, fPosY_1);
	m_pFontRequireMatCnt_0->Set_UI_Size(20.4f * fMul, 35.f * fMul);
	m_pFontRequireMatCnt_0->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFontRequireMatCnt_0, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pFontRequireMatCnt_1 = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pFontRequireMatCnt_1->Set_UI_Pos(fPosX_2, fPosY_2);
	m_pFontRequireMatCnt_1->Set_UI_Size(20.4f * fMul, 35.f * fMul);
	m_pFontRequireMatCnt_1->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFontRequireMatCnt_1, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pFontRequireMatCnt_2 = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pFontRequireMatCnt_2->Set_UI_Pos(fPosX_2, fPosY_3);
	m_pFontRequireMatCnt_2->Set_UI_Size(20.4f * fMul, 35.f * fMul);
	m_pFontRequireMatCnt_2->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFontRequireMatCnt_2, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pFontMyMatCnt_0 = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pFontMyMatCnt_0->Set_UI_Pos(fPosX_1, fPosY_1);
	m_pFontMyMatCnt_0->Set_UI_Size(20.4f * fMul, 35.f * fMul);
	m_pFontMyMatCnt_0->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFontMyMatCnt_0, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pFontMyMatCnt_1 = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pFontMyMatCnt_1->Set_UI_Pos(fPosX_1, fPosY_2);
	m_pFontMyMatCnt_1->Set_UI_Size(20.4f * fMul, 35.f * fMul);
	m_pFontMyMatCnt_1->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFontMyMatCnt_1, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pFontMyMatCnt_2 = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pFontMyMatCnt_2->Set_UI_Pos(fPosX_1, fPosY_3);
	m_pFontMyMatCnt_2->Set_UI_Size(20.4f * fMul, 35.f * fMul);
	m_pFontMyMatCnt_2->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFontMyMatCnt_2, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pFontSlash_0 = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pFontSlash_0->Set_UI_Pos(fPosX_1 + 40.f, fPosY_1);
	m_pFontSlash_0->Set_UI_Size(20.4f * fMul, 35.f * fMul);
	m_pFontSlash_0->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFontSlash_0, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	
	m_pFontSlash_1 = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pFontSlash_1->Set_UI_Pos(fPosX_1 + 40.f, fPosY_2);
	m_pFontSlash_1->Set_UI_Size(20.4f * fMul, 35.f * fMul);
	m_pFontSlash_1->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFontSlash_1, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pFontSlash_2 = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pFontSlash_2->Set_UI_Pos(fPosX_1 + 40.f, fPosY_3);
	m_pFontSlash_2->Set_UI_Size(20.4f * fMul, 35.f * fMul);
	m_pFontSlash_2->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFontSlash_2, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pUpgradeResultPopup = static_cast<CWeaponUpgradeSuccessPopupUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon_UpgradeSuccessPopup", nullptr));
	m_pUpgradeResultPopup->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pUpgradeResultPopup, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
}

void CWeaponUpgradeUI::Check_LateInit()
{
	if (m_bLateInit)
		return;
	m_bLateInit = true;

	SetUp_Default();
}

void CWeaponUpgradeUI::Check_ItemOption_Weapon()
{
	if (!m_bIsActive)
		return;

	CWeapon_Slot* pWeaponSlot = m_pInven->Get_HoverSlot_Weapon();
	

	if (!pWeaponSlot || m_pInven->Get_PopupOn())
		pWeaponSlot = m_pInven->Get_SelectedSlot_Weapon();

	if (!pWeaponSlot)
		return;

	WPN_PARAM tParam = pWeaponSlot->Get_WeaponParam();

	//==============================================================================================================
	// Reinforce
	if (!m_pFontReinforceHeader)
		return;
	m_pFontReinforceHeader->Update_NumberValue((_float)tParam.iReinforce + 1);
	m_pFontReinforceHeader->Set_Active(true);

	if (!m_pFontReinforceDesc)
		return;
	m_pFontReinforceDesc->Update_NumberValue((_float)tParam.iReinforce);
	m_pFontReinforceDesc->Set_Active(true);

	if (!m_pFontReinforceAfterDesc)
		return;
	m_pFontReinforceAfterDesc->Update_NumberValue((_float)tParam.iReinforce + 1);
	m_pFontReinforceAfterDesc->Set_Active(true);
	//==============================================================================================================
	// Damage 
	if (!m_pFontPlusDamageDesc)
		return;
	m_pFontPlusDamageDesc->Update_NumberValue(tParam.fPlusDamage);
	m_pFontPlusDamageDesc->Set_Active(true);

	if (!m_pFontPlusDamageAfterDesc)
		return;
	_float fPlus =  Get_PlusDamage(tParam.fDamage, tParam.iReinforce + 1);
	m_pFontPlusDamageAfterDesc->Update_NumberValue(fPlus);
	m_pFontPlusDamageAfterDesc->Set_Active(true);

	if (!m_pFontDamageDesc)
		return;
	m_pFontDamageDesc->Update_NumberValue(tParam.fDamage + tParam.fPlusDamage);
	m_pFontDamageDesc->Set_Active(true);

	if (!m_pFontDamageAfterDesc)
		return;
	m_pFontDamageAfterDesc->Update_NumberValue(tParam.fDamage + fPlus);
	m_pFontDamageAfterDesc->Set_Active(true);
	//==============================================================================================================
	// HP 
	if (!m_pFontPlusHPDesc)
		return;
	m_pFontPlusHPDesc->Set_Active(false);
	if (!m_pFontPlusHPAfterDesc)
		return;
	m_pFontPlusHPAfterDesc->Set_Active(false);

	if (!m_pFontTotalHPDesc)
		return;
	m_pFontTotalHPDesc->Set_Active(false);
	if (!m_pFontTotalHPAfterDesc)
		return;
	m_pFontTotalHPAfterDesc->Set_Active(false);
	//==============================================================================================================
	// Haze
	if (!m_pFontRequireHaze)
		return;
	
	m_pFontRequireHaze->Update_NumberValue(m_pInven->Get_UpgradePrice(tParam.iReinforce));
	m_pFontRequireHaze->Set_Active(true);

	_int iHazeCnt = _int(CUI_Manager::Get_Instance()->Get_HazeUI()->Get_Haze_Cnt());
	if (!m_pFontMyHaze)
		return;
	m_pFontMyHaze->Update_NumberValue(_float(iHazeCnt));
	m_pFontMyHaze->Set_Active(true);
	//==============================================================================================================
	// Material
	if (!m_pFontRequireMatCnt_0)
		return;
	m_pFontRequireMatCnt_0->Update_NumberValue((_float)Get_RequireMaterial(CMaterial::MATERIAL_TYPE::Queen_Steel, tParam.iReinforce));
	m_pFontRequireMatCnt_0->Set_Active(true);

	if (!m_pFontRequireMatCnt_1)
		return;
	m_pFontRequireMatCnt_1->Update_NumberValue((_float)Get_RequireMaterial(CMaterial::MATERIAL_TYPE::Queen_Titanium, tParam.iReinforce));
	m_pFontRequireMatCnt_1->Set_Active(true);

	if (!m_pFontRequireMatCnt_2)
		return;
	m_pFontRequireMatCnt_2->Update_NumberValue((_float)Get_RequireMaterial(CMaterial::MATERIAL_TYPE::Queen_Tungsten, tParam.iReinforce));
	m_pFontRequireMatCnt_2->Set_Active(true);

	_int iSteel = Get_MyMaterial(CMaterial::MATERIAL_TYPE::Queen_Steel);
	_int iTitanium = Get_MyMaterial(CMaterial::MATERIAL_TYPE::Queen_Titanium);
	_int iTungsten = Get_MyMaterial(CMaterial::MATERIAL_TYPE::Queen_Tungsten);

	if (!m_pFontMyMatCnt_0)
		return;
	m_pFontMyMatCnt_0->Update_NumberValue((_float)iSteel);
	m_pFontMyMatCnt_0->Set_Active(true);

	if (!m_pFontMyMatCnt_1)
		return;
	m_pFontMyMatCnt_1->Update_NumberValue((_float)iTitanium);
	m_pFontMyMatCnt_1->Set_Active(true);

	if (!m_pFontMyMatCnt_2)
		return;
	m_pFontMyMatCnt_2->Update_NumberValue((_float)iTungsten);
	m_pFontMyMatCnt_2->Set_Active(true);

	if (!m_pFontSlash_0)
		return;
	m_pFontSlash_0->Set_Slash(true);
	m_pFontSlash_0->Set_Active(true);
	if (!m_pFontSlash_1)
		return;
	m_pFontSlash_1->Set_Slash(true);
	m_pFontSlash_1->Set_Active(true);
	if (!m_pFontSlash_2)
		return;
	m_pFontSlash_2->Set_Slash(true);
	m_pFontSlash_2->Set_Active(true);
	//==============================================================================================================
	// Name
	if (!m_pWeaponNameUI)
		return;
	WEAPON_ALL_DATA eAllDate = WEAPON_ALL_DATA(tParam.iWeaponName_InShop);
	m_pWeaponNameUI->Set_WeaponDescType(eAllDate);
	m_pWeaponNameUI->Set_Active(m_bIsActive);
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
	}
	m_pWeaponMoveTypeUI->Set_Active(m_bIsActive);

}

void CWeaponUpgradeUI::Check_ItemOption_Armor()
{
	if (!m_bIsActive)
		return;

	CArmor_Slot* pArmorSlot = m_pInven->Get_SelectedSlot_Armor();

	if (!pArmorSlot || m_pInven->Get_PopupOn())
		pArmorSlot = m_pInven->Get_SelectedSlot_Armor();

	if (!pArmorSlot)
		return;

	ARMOR_PARAM tParam = pArmorSlot->Get_ArmorParam();

	//==============================================================================================================
	// Reinforce
	if (!m_pFontReinforceHeader)
		return;
	m_pFontReinforceHeader->Update_NumberValue((_float)tParam.iReinforce + 1);
	m_pFontReinforceHeader->Set_Active(true);

	if (!m_pFontReinforceDesc)
		return;
	m_pFontReinforceDesc->Update_NumberValue((_float)tParam.iReinforce);
	m_pFontReinforceDesc->Set_Active(true);

	if (!m_pFontReinforceAfterDesc)
		return;
	m_pFontReinforceAfterDesc->Update_NumberValue((_float)tParam.iReinforce + 1);
	m_pFontReinforceAfterDesc->Set_Active(true);
	//==============================================================================================================
	// Damage 
	if (!m_pFontPlusDamageDesc)
		return;
	m_pFontPlusDamageDesc->Update_NumberValue((_float)tParam.fPlusDef);
	m_pFontPlusDamageDesc->Set_Active(true);

	if (!m_pFontPlusDamageAfterDesc)
		return;
	_float fPlus = Get_PlusDamage(tParam.fPlusDef, tParam.iReinforce + 1);
	m_pFontPlusDamageAfterDesc->Update_NumberValue(fPlus);
	m_pFontPlusDamageAfterDesc->Set_Active(true);

	if (!m_pFontDamageDesc)
		return;
	m_pFontDamageDesc->Update_NumberValue((_float)tParam.fDef + tParam.fPlusDef);
	m_pFontDamageDesc->Set_Active(true);

	if (!m_pFontDamageAfterDesc)
		return;
	m_pFontDamageAfterDesc->Update_NumberValue(tParam.fDef + fPlus);
	m_pFontDamageAfterDesc->Set_Active(true);
	//==============================================================================================================
	// HP 
	if (!m_pFontPlusHPDesc)
		return;

	CGameObject* pPlayer = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL);
	if (!pPlayer)
		return;
	_float fPlayerHP = pPlayer->Get_Target_Hp();

	fPlus = Get_PlusDamage(tParam.fPlusHP, tParam.iReinforce + 1);

	m_pFontPlusHPDesc->Update_NumberValue((_float)tParam.fPlusHP);
	m_pFontPlusHPDesc->Set_Active(true);
	if (!m_pFontPlusHPAfterDesc)
		return;
	m_pFontPlusHPAfterDesc->Update_NumberValue(fPlus);
	m_pFontPlusHPAfterDesc->Set_Active(true);

	if (!m_pFontTotalHPDesc)
		return;
	m_pFontTotalHPDesc->Update_NumberValue(fPlayerHP + tParam.fHP + tParam.fPlusHP);
	m_pFontTotalHPDesc->Set_Active(true);

	if (!m_pFontTotalHPAfterDesc)
		return;
	m_pFontTotalHPAfterDesc->Update_NumberValue(fPlayerHP + tParam.fHP + fPlus);
	m_pFontTotalHPAfterDesc->Set_Active(true);
	//==============================================================================================================
	// Haze
	if (!m_pFontRequireHaze)
		return;
	m_pFontRequireHaze->Update_NumberValue(m_pInven->Get_UpgradePrice(tParam.iReinforce));
	m_pFontRequireHaze->Set_Active(true);

	_int iHazeCnt = _int(CUI_Manager::Get_Instance()->Get_HazeUI()->Get_Haze_Cnt());
	if (!m_pFontMyHaze)
		return;
	m_pFontMyHaze->Update_NumberValue((_float)iHazeCnt);
	m_pFontMyHaze->Set_Active(true);
	//==============================================================================================================
	// Material
	if (!m_pFontRequireMatCnt_0)
		return;
	m_pFontRequireMatCnt_0->Update_NumberValue((_float)Get_RequireMaterial(CMaterial::MATERIAL_TYPE::Queen_Steel, tParam.iReinforce));
	m_pFontRequireMatCnt_0->Set_Active(true);

	if (!m_pFontRequireMatCnt_1)
		return;
	m_pFontRequireMatCnt_1->Update_NumberValue((_float)Get_RequireMaterial(CMaterial::MATERIAL_TYPE::Queen_Titanium, tParam.iReinforce));
	m_pFontRequireMatCnt_1->Set_Active(true);

	if (!m_pFontRequireMatCnt_2)
		return;
	m_pFontRequireMatCnt_2->Update_NumberValue((_float)Get_RequireMaterial(CMaterial::MATERIAL_TYPE::Queen_Tungsten, tParam.iReinforce));
	m_pFontRequireMatCnt_2->Set_Active(true);

	_int iSteel = Get_MyMaterial(CMaterial::MATERIAL_TYPE::Queen_Steel);
	_int iTitanium = Get_MyMaterial(CMaterial::MATERIAL_TYPE::Queen_Titanium);
	_int iTungsten = Get_MyMaterial(CMaterial::MATERIAL_TYPE::Queen_Tungsten);

	if (!m_pFontMyMatCnt_0)
		return;
	m_pFontMyMatCnt_0->Update_NumberValue((_float)iSteel);
	m_pFontMyMatCnt_0->Set_Active(true);

	if (!m_pFontMyMatCnt_1)
		return;
	m_pFontMyMatCnt_1->Update_NumberValue((_float)iTitanium);
	m_pFontMyMatCnt_1->Set_Active(true);

	if (!m_pFontMyMatCnt_2)
		return;
	m_pFontMyMatCnt_2->Update_NumberValue((_float)iTungsten);
	m_pFontMyMatCnt_2->Set_Active(true);

	if (!m_pFontSlash_0)
		return;
	m_pFontSlash_0->Set_Slash(true);
	m_pFontSlash_0->Set_Active(true);
	if (!m_pFontSlash_1)
		return;
	m_pFontSlash_1->Set_Slash(true);
	m_pFontSlash_1->Set_Active(true);
	if (!m_pFontSlash_2)
		return;
	m_pFontSlash_2->Set_Slash(true);
	m_pFontSlash_2->Set_Active(true);
	//==============================================================================================================
	// Name
	if (!m_pWeaponNameUI)
		return;
	ARMOR_All_DATA eAllDate = ARMOR_All_DATA(tParam.iArmorName);
	m_pWeaponNameUI->Set_ArmorDescType(eAllDate);
	m_pWeaponNameUI->Set_Active(m_bIsActive);
	//==============================================================================================================
	// MoveType
	if (!m_pWeaponMoveTypeUI)
		return;
	m_pWeaponMoveTypeUI->Set_Active(false);

}

void CWeaponUpgradeUI::Check_WeaponName()
{
	if (WpnAll_END == m_eWeaponDesc && !m_pWeaponNameUI)
	{
		m_pWeaponNameUI = static_cast<CWeaponUpgradeUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon_Upgrade", nullptr));
		m_pWeaponNameUI->Set_UI_Pos(WINCX * 0.348f, WINCY * 0.13f);
		m_pWeaponNameUI->Set_UI_Size(450.0f * 1.5f, 120.f* 1.5f);
		m_pWeaponNameUI->Set_ViewZ(m_fViewZ - 0.05f);
		m_pWeaponNameUI->Set_WeaponDescType(WpnAll_Gun_Bayonet);
		m_pWeaponNameUI->Set_ArmorDescType(ArmorAll_Gauntlet_DarkNightHook);
		g_pManagement->Add_GameOject_ToLayer_NoClone(m_pWeaponNameUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	}
}

void CWeaponUpgradeUI::Check_MoveType()
{
	if (MOVE_END == m_eMoveType && !m_pWeaponMoveTypeUI)
	{
		m_pWeaponMoveTypeUI = static_cast<CWeaponUpgradeUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon_Upgrade", nullptr));
		m_pWeaponMoveTypeUI->Set_UI_Pos(WINCX * 0.25f, WINCY * 0.710f);
		m_pWeaponMoveTypeUI->Set_UI_Size(200.0f, 60.f);
		m_pWeaponMoveTypeUI->Set_ViewZ(m_fViewZ - 0.05f);
		m_pWeaponMoveTypeUI->Set_WeaponMoveType(MOVE_STEP);
		g_pManagement->Add_GameOject_ToLayer_NoClone(m_pWeaponMoveTypeUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	}
}

void CWeaponUpgradeUI::Check_Option()
{
	if (WpnAll_END == m_eWeaponDesc && !m_bLateInit)
	{
		m_bLateInit = true;
		CWeaponUpgradeOptionUI* pInstance = nullptr;
		LOOP(2)
		{
			pInstance = static_cast<CWeaponUpgradeOptionUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon_UpgradeOption", nullptr));
			g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
			m_vecOption.push_back(pInstance);
		}
	}
}

CWeaponUpgradeUI * CWeaponUpgradeUI::Create(_Device pGraphic_Device)
{
	CWeaponUpgradeUI* pInstance = new CWeaponUpgradeUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CWeaponUpgradeUI::Clone_GameObject(void * pArg)
{
	CWeaponUpgradeUI* pInstance = new CWeaponUpgradeUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CWeaponUpgradeUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
