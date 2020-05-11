#include "stdafx.h"
#include "..\Headers\Weapon_Inven.h"


CWeapon_Inven::CWeapon_Inven(_Device pDevice)
	: CUI(pDevice)
{
}

CWeapon_Inven::CWeapon_Inven(const CWeapon_Inven & rhs)
	: CUI(rhs)
{
}

HRESULT CWeapon_Inven::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CWeapon_Inven::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	m_fPosX = 229.5f;
	m_fPosY = 325.5f;
	m_fSizeX = 280.f;
	m_fSizeY = 471.f;
	m_fViewZ = 4.f;

	m_bIsActive = false;

	// Slot Create
	CUI::UI_DESC* pDesc = nullptr;
	CWeapon_Slot* pSlot = nullptr;
	
	if (FAILED(SetUp_WeaponData()))
		return E_FAIL;
	LOOP(2)
	{
		m_UseWeaponParam[i] = m_tWeaponParam[WPN_DATA_End];
	}
	
	Add_Weapon(m_tWeaponParam[Wpn_Gun_Slave]);
	Add_Weapon(m_tWeaponParam[Wpn_Hammer]);
	Add_Weapon(m_tWeaponParam[Wpn_LSword_Military]);
	Add_Weapon(m_tWeaponParam[Wpn_Halberd]);
	return NOERROR;
}

_int CWeapon_Inven::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);


	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.0f);

	Click_Inven();
	
	for (auto& pWeaponSlot : m_vecWeaponSlot)
	{
		pWeaponSlot->Set_Active(m_bIsActive);
		pWeaponSlot->Set_ViewZ(m_fViewZ - 0.1f);
	}

	
	return NO_EVENT;
}

_int CWeapon_Inven::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CWeapon_Inven::Render_GameObject()
{
	if (!m_bIsActive)
		return NOERROR;

	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);

	m_matOldView = g_pManagement->Get_Transform(D3DTS_VIEW);
	m_matOldProj = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();

	m_pShaderCom->Begin_Pass(1);

	m_pBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();

	m_pShaderCom->End_Shader();

	g_pManagement->Set_Transform(D3DTS_VIEW, m_matOldView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matOldProj);

	return NOERROR;
}

HRESULT CWeapon_Inven::Add_Component()
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

HRESULT CWeapon_Inven::SetUp_ConstantTable()
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

void CWeapon_Inven::Click_Inven()
{
	if (!m_bIsActive)
		return;

	for (auto& pSlot : m_vecWeaponSlot)
	{
		if (pSlot->Pt_InRect())
		{
			if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB) &&
				!pSlot->Get_Select())
			{
				Regist_Weapon(pSlot);
			}
			if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_RB) &&
				pSlot->Get_Select())
			{
				UnRegist_Weapon(pSlot);
			}
		}
	}
}


void CWeapon_Inven::Regist_Weapon(CWeapon_Slot* pWeaponSlot)
{
	if (pWeaponSlot->Get_WeaponParam().iWeaponName == WPN_DATA_End)
		return;
	if (m_UseWeaponParam[0].iWeaponName == WPN_DATA_End)
	{
		m_UseWeaponParam[0] = pWeaponSlot->Get_WeaponParam();
		pWeaponSlot->Set_Select(true);
	}
	else if (m_UseWeaponParam[1].iWeaponName == WPN_DATA_End)
	{
		m_UseWeaponParam[1] = pWeaponSlot->Get_WeaponParam();
		pWeaponSlot->Set_Select(true);
	}
	else
		return;
}

void CWeapon_Inven::UnRegist_Weapon(CWeapon_Slot * pWeaponSlot)
{
	if (pWeaponSlot->Get_WeaponParam().iWeaponName == WPN_DATA_End)
		return;
	if (pWeaponSlot->Get_WeaponParam().iWeaponName == m_UseWeaponParam[0].iWeaponName)
	{
		m_UseWeaponParam[0] = m_tWeaponParam[WPN_DATA_End];
		pWeaponSlot->Set_Select(false);
	}
	else if (pWeaponSlot->Get_WeaponParam().iWeaponName == m_UseWeaponParam[1].iWeaponName)
	{
		m_UseWeaponParam[1] = m_tWeaponParam[WPN_DATA_End];
		pWeaponSlot->Set_Select(false);
	}
	else
		return;
}

HRESULT CWeapon_Inven::SetUp_WeaponData()
{
	//===========================================================================================
	// 한손검
	//===========================================================================================

	m_tWeaponParam[Wpn_SSword].iWeaponName = Wpn_SSword;
	m_tWeaponParam[Wpn_SSword].iWeaponType = WEAPON_SSword;
	m_tWeaponParam[Wpn_SSword].iReinforce = 0;
	m_tWeaponParam[Wpn_SSword].fDamage = 1000.f;
	m_tWeaponParam[Wpn_SSword].fRadius = 0.6f;
	m_tWeaponParam[Wpn_SSword].fTrail_Min = 0.6f;
	m_tWeaponParam[Wpn_SSword].fTrail_Max = 1.8f;
	m_tWeaponParam[Wpn_SSword].fCol_Height = 1.1f;

	m_tWeaponParam[Wpn_SSword_Black].iWeaponName = Wpn_SSword_Black;
	m_tWeaponParam[Wpn_SSword_Black].iWeaponType = WEAPON_SSword;
	m_tWeaponParam[Wpn_SSword_Black].iReinforce = 0;
	m_tWeaponParam[Wpn_SSword_Black].fDamage = 100.f;
	m_tWeaponParam[Wpn_SSword_Black].fRadius = 0.9f;
	m_tWeaponParam[Wpn_SSword_Black].fTrail_Min = 0.6f;
	m_tWeaponParam[Wpn_SSword_Black].fTrail_Max = 1.8f;
	m_tWeaponParam[Wpn_SSword_Black].fCol_Height = 0.6f;

	m_tWeaponParam[Wpn_SSword_Military].iWeaponName = Wpn_SSword_Military;
	m_tWeaponParam[Wpn_SSword_Military].iWeaponType = WEAPON_SSword;
	m_tWeaponParam[Wpn_SSword_Military].iReinforce = 0;
	m_tWeaponParam[Wpn_SSword_Military].fDamage = 100.f;
	m_tWeaponParam[Wpn_SSword_Military].fRadius = 0.6f;
	m_tWeaponParam[Wpn_SSword_Military].fTrail_Min = 0.6f;
	m_tWeaponParam[Wpn_SSword_Military].fTrail_Max = 1.8f;
	m_tWeaponParam[Wpn_SSword_Military].fCol_Height = 1.1f;

	m_tWeaponParam[Wpn_SSword_Slave].iWeaponName = Wpn_SSword_Slave;
	m_tWeaponParam[Wpn_SSword_Slave].iWeaponType = WEAPON_SSword;
	m_tWeaponParam[Wpn_SSword_Slave].iReinforce = 0;
	m_tWeaponParam[Wpn_SSword_Slave].fDamage = 100.f;
	m_tWeaponParam[Wpn_SSword_Slave].fRadius = 0.7f;
	m_tWeaponParam[Wpn_SSword_Slave].fTrail_Min = 0.6f;
	m_tWeaponParam[Wpn_SSword_Slave].fTrail_Max = 1.8f;
	m_tWeaponParam[Wpn_SSword_Slave].fCol_Height = 1.2f;

	//===========================================================================================
	// 대검
	//===========================================================================================

	m_tWeaponParam[Wpn_LSword].iWeaponName = Wpn_LSword;
	m_tWeaponParam[Wpn_LSword].iWeaponType = WEAPON_LSword;
	m_tWeaponParam[Wpn_LSword].iReinforce = 0;
	m_tWeaponParam[Wpn_LSword].fDamage = 155.f;
	m_tWeaponParam[Wpn_LSword].fRadius = 0.9f;
	m_tWeaponParam[Wpn_LSword].fTrail_Min = 0.8f;
	m_tWeaponParam[Wpn_LSword].fTrail_Max = 2.1f;
	m_tWeaponParam[Wpn_LSword].fCol_Height = 1.4f;

	m_tWeaponParam[Wpn_LSword_Black].iWeaponName = Wpn_LSword_Black;
	m_tWeaponParam[Wpn_LSword_Black].iWeaponType = WEAPON_LSword;
	m_tWeaponParam[Wpn_LSword_Black].iReinforce = 0;
	m_tWeaponParam[Wpn_LSword_Black].fDamage = 155.f;
	m_tWeaponParam[Wpn_LSword_Black].fRadius = 0.8f;
	m_tWeaponParam[Wpn_LSword_Black].fTrail_Min = 0.8f;
	m_tWeaponParam[Wpn_LSword_Black].fTrail_Max = 2.1f;
	m_tWeaponParam[Wpn_LSword_Black].fCol_Height = 1.3f;

	m_tWeaponParam[Wpn_LSword_Military].iWeaponName = Wpn_LSword_Military;
	m_tWeaponParam[Wpn_LSword_Military].iWeaponType = WEAPON_LSword;
	m_tWeaponParam[Wpn_LSword_Military].iReinforce = 0;
	m_tWeaponParam[Wpn_LSword_Military].fDamage = 155.f;
	m_tWeaponParam[Wpn_LSword_Military].fRadius = 0.8f;
	m_tWeaponParam[Wpn_LSword_Military].fTrail_Min = 0.8f;
	m_tWeaponParam[Wpn_LSword_Military].fTrail_Max = 2.1f;
	m_tWeaponParam[Wpn_LSword_Military].fCol_Height = 1.5f;

	m_tWeaponParam[Wpn_LSword_Slave].iWeaponName = Wpn_LSword_Slave;
	m_tWeaponParam[Wpn_LSword_Slave].iWeaponType = WEAPON_LSword;
	m_tWeaponParam[Wpn_LSword_Slave].iReinforce = 0;
	m_tWeaponParam[Wpn_LSword_Slave].fDamage = 155.f;
	m_tWeaponParam[Wpn_LSword_Slave].fRadius = 0.8f;
	m_tWeaponParam[Wpn_LSword_Slave].fTrail_Min = 0.8f;
	m_tWeaponParam[Wpn_LSword_Slave].fTrail_Max = 2.1f;
	m_tWeaponParam[Wpn_LSword_Slave].fCol_Height = 1.5f;

	//===========================================================================================
	// 도끼창
	//===========================================================================================

	m_tWeaponParam[Wpn_Halberd].iWeaponName = Wpn_Halberd;
	m_tWeaponParam[Wpn_Halberd].iWeaponType = WEAPON_Halberd;
	m_tWeaponParam[Wpn_Halberd].iReinforce = 0;
	m_tWeaponParam[Wpn_Halberd].fDamage = 160.f;
	m_tWeaponParam[Wpn_Halberd].fRadius = 0.7f;
	m_tWeaponParam[Wpn_Halberd].fTrail_Min = 1.2f;
	m_tWeaponParam[Wpn_Halberd].fTrail_Max = 2.3f;
	m_tWeaponParam[Wpn_Halberd].fCol_Height = 1.6f;

	m_tWeaponParam[Wpn_Halberd_Black].iWeaponName = Wpn_Halberd_Black;
	m_tWeaponParam[Wpn_Halberd_Black].iWeaponType = WEAPON_Halberd;
	m_tWeaponParam[Wpn_Halberd_Black].iReinforce = 0;
	m_tWeaponParam[Wpn_Halberd_Black].fDamage = 160.f;
	m_tWeaponParam[Wpn_Halberd_Black].fRadius = 0.7f;
	m_tWeaponParam[Wpn_Halberd_Black].fTrail_Min = 1.2f;
	m_tWeaponParam[Wpn_Halberd_Black].fTrail_Max = 2.3f;
	m_tWeaponParam[Wpn_Halberd_Black].fCol_Height = 1.4f;

	m_tWeaponParam[Wpn_Halberd_Military].iWeaponName = Wpn_Halberd_Military;
	m_tWeaponParam[Wpn_Halberd_Military].iWeaponType = WEAPON_Halberd;
	m_tWeaponParam[Wpn_Halberd_Military].iReinforce = 0;
	m_tWeaponParam[Wpn_Halberd_Military].fDamage = 160.f;
	m_tWeaponParam[Wpn_Halberd_Military].fRadius = 0.7f;
	m_tWeaponParam[Wpn_Halberd_Military].fTrail_Min = 1.2f;
	m_tWeaponParam[Wpn_Halberd_Military].fTrail_Max = 2.3f;
	m_tWeaponParam[Wpn_Halberd_Military].fCol_Height = 1.4f;

	m_tWeaponParam[Wpn_Halberd_Slave].iWeaponName = Wpn_Halberd_Slave;
	m_tWeaponParam[Wpn_Halberd_Slave].iWeaponType = WEAPON_Halberd;
	m_tWeaponParam[Wpn_Halberd_Slave].iReinforce = 0;
	m_tWeaponParam[Wpn_Halberd_Slave].fDamage = 160.f;
	m_tWeaponParam[Wpn_Halberd_Slave].fRadius = 0.8f;
	m_tWeaponParam[Wpn_Halberd_Slave].fTrail_Min = 1.2f;
	m_tWeaponParam[Wpn_Halberd_Slave].fTrail_Max = 2.3f;
	m_tWeaponParam[Wpn_Halberd_Slave].fCol_Height = 1.3f;

	//===========================================================================================
	// 해머
	//===========================================================================================

	m_tWeaponParam[Wpn_Hammer].iWeaponName = Wpn_LSword;
	m_tWeaponParam[Wpn_Hammer].iWeaponType = WEAPON_Halberd;
	m_tWeaponParam[Wpn_Hammer].iReinforce = 0;
	m_tWeaponParam[Wpn_Hammer].fDamage = 155.f;
	m_tWeaponParam[Wpn_Hammer].fRadius = 0.7f;
	m_tWeaponParam[Wpn_Hammer].fTrail_Min = 0.75f;
	m_tWeaponParam[Wpn_Hammer].fTrail_Max = 1.5f;
	m_tWeaponParam[Wpn_Hammer].fCol_Height = 1.1f;

	m_tWeaponParam[Wpn_Hammer_Black].iWeaponName = Wpn_Hammer_Black;
	m_tWeaponParam[Wpn_Hammer_Black].iWeaponType = WEAPON_Halberd;
	m_tWeaponParam[Wpn_Hammer_Black].iReinforce = 0;
	m_tWeaponParam[Wpn_Hammer_Black].fDamage = 155.f;
	m_tWeaponParam[Wpn_Hammer_Black].fRadius = 0.75f;
	m_tWeaponParam[Wpn_Hammer_Black].fTrail_Min = 0.75f;
	m_tWeaponParam[Wpn_Hammer_Black].fTrail_Max = 1.5f;
	m_tWeaponParam[Wpn_Hammer_Black].fCol_Height = 1.0f;

	m_tWeaponParam[Wpn_Hammer_Military].iWeaponName = Wpn_Hammer_Military;
	m_tWeaponParam[Wpn_Hammer_Military].iWeaponType = WEAPON_Halberd;
	m_tWeaponParam[Wpn_Hammer_Military].iReinforce = 0;
	m_tWeaponParam[Wpn_Hammer_Military].fDamage = 155.f;
	m_tWeaponParam[Wpn_Hammer_Military].fRadius = 1.1f;
	m_tWeaponParam[Wpn_Hammer_Military].fTrail_Min = 0.75f;
	m_tWeaponParam[Wpn_Hammer_Military].fTrail_Max = 1.5f;
	m_tWeaponParam[Wpn_Hammer_Military].fCol_Height = 1.9f;

	m_tWeaponParam[Wpn_Hammer_Slave].iWeaponName = Wpn_Hammer_Slave;
	m_tWeaponParam[Wpn_Hammer_Slave].iWeaponType = WEAPON_Halberd;
	m_tWeaponParam[Wpn_Hammer_Slave].iReinforce = 0;
	m_tWeaponParam[Wpn_Hammer_Slave].fDamage = 155.f;
	m_tWeaponParam[Wpn_Hammer_Slave].fRadius = 0.7f;
	m_tWeaponParam[Wpn_Hammer_Slave].fTrail_Min = 0.75f;
	m_tWeaponParam[Wpn_Hammer_Slave].fTrail_Max = 1.5f;
	m_tWeaponParam[Wpn_Hammer_Slave].fCol_Height = 0.9f;

	//===========================================================================================
	// 총검
	//===========================================================================================

	m_tWeaponParam[Wpn_Gun].iWeaponName = Wpn_Gun;
	m_tWeaponParam[Wpn_Gun].iWeaponType = WEAPON_Gun;
	m_tWeaponParam[Wpn_Gun].iReinforce = 0;
	m_tWeaponParam[Wpn_Gun].fDamage = 70.f;
	m_tWeaponParam[Wpn_Gun].fRadius = 0.5f;
	m_tWeaponParam[Wpn_Gun].fTrail_Min = 0.75f;
	m_tWeaponParam[Wpn_Gun].fTrail_Max = 1.5f;
	m_tWeaponParam[Wpn_Gun].fCol_Height = 1.0f;

	m_tWeaponParam[Wpn_Gun_Black].iWeaponName = Wpn_Gun_Black;
	m_tWeaponParam[Wpn_Gun_Black].iWeaponType = WEAPON_Gun;
	m_tWeaponParam[Wpn_Gun_Black].iReinforce = 0;
	m_tWeaponParam[Wpn_Gun_Black].fDamage = 70.f;
	m_tWeaponParam[Wpn_Gun_Black].fRadius = 0.5f;
	m_tWeaponParam[Wpn_Gun_Black].fTrail_Min = 0.75f;
	m_tWeaponParam[Wpn_Gun_Black].fTrail_Max = 1.5f;
	m_tWeaponParam[Wpn_Gun_Black].fCol_Height = 1.1f;

	m_tWeaponParam[Wpn_Gun_Military].iWeaponName = Wpn_Gun_Military;
	m_tWeaponParam[Wpn_Gun_Military].iWeaponType = WEAPON_Gun;
	m_tWeaponParam[Wpn_Gun_Military].iReinforce = 0;
	m_tWeaponParam[Wpn_Gun_Military].fDamage = 70.f;
	m_tWeaponParam[Wpn_Gun_Military].fRadius = 0.5f;
	m_tWeaponParam[Wpn_Gun_Military].fTrail_Min = 0.75f;
	m_tWeaponParam[Wpn_Gun_Military].fTrail_Max = 1.5f;
	m_tWeaponParam[Wpn_Gun_Military].fCol_Height = 1.1f;

	m_tWeaponParam[Wpn_Gun_Slave].iWeaponName = Wpn_Gun_Slave;
	m_tWeaponParam[Wpn_Gun_Slave].iWeaponType = WEAPON_Gun;
	m_tWeaponParam[Wpn_Gun_Slave].iReinforce = 0;
	m_tWeaponParam[Wpn_Gun_Slave].fDamage = 70.f;
	m_tWeaponParam[Wpn_Gun_Slave].fRadius = 0.5f;
	m_tWeaponParam[Wpn_Gun_Slave].fTrail_Min = 0.75f;
	m_tWeaponParam[Wpn_Gun_Slave].fTrail_Max = 1.5f;
	m_tWeaponParam[Wpn_Gun_Slave].fCol_Height = 1.1f;

	//===========================================================================================
	// 방패
	//===========================================================================================

	m_tWeaponParam[WPN_Shield_Normal].iWeaponName = WPN_Shield_Normal;
	m_tWeaponParam[WPN_Shield_Normal].iWeaponType = WEAPON_Shield;
	m_tWeaponParam[WPN_Shield_Normal].iReinforce = 0;
	m_tWeaponParam[WPN_Shield_Normal].fDamage = 25.f;
	m_tWeaponParam[WPN_Shield_Normal].fRadius = 0.7f;
	m_tWeaponParam[WPN_Shield_Normal].fTrail_Min = 0.f;
	m_tWeaponParam[WPN_Shield_Normal].fTrail_Max = 1.f;
	m_tWeaponParam[WPN_Shield_Normal].fCol_Height = 0.f;

	//===========================================================================================

	m_tWeaponParam[WPN_QueenLance].iWeaponName = WPN_QueenLance;
	m_tWeaponParam[WPN_QueenLance].iWeaponType = WEAPON_SSword;
	m_tWeaponParam[WPN_QueenLance].iReinforce = 0;
	m_tWeaponParam[WPN_QueenLance].fDamage = 25.f;
	m_tWeaponParam[WPN_QueenLance].fRadius = 1.4f;
	m_tWeaponParam[WPN_QueenLance].fTrail_Min = 0.3f;
	m_tWeaponParam[WPN_QueenLance].fTrail_Max = 3.4f;
	m_tWeaponParam[WPN_QueenLance].fCol_Height = 1.7f;

	m_tWeaponParam[WPN_QueenShield].iWeaponName = WPN_QueenShield;
	m_tWeaponParam[WPN_QueenShield].iWeaponType = WEAPON_Shield;
	m_tWeaponParam[WPN_QueenShield].iReinforce = 0;
	m_tWeaponParam[WPN_QueenShield].fDamage = 25.f;
	m_tWeaponParam[WPN_QueenShield].fRadius = 0.8f;
	m_tWeaponParam[WPN_QueenShield].fTrail_Min = 0.f;
	m_tWeaponParam[WPN_QueenShield].fTrail_Max = 1.f;
	m_tWeaponParam[WPN_QueenShield].fCol_Height = 0.f;

	//===========================================================================================

	m_tWeaponParam[WPN_FrostBlood_IceGirl].iWeaponName = WPN_FrostBlood_IceGirl;
	m_tWeaponParam[WPN_FrostBlood_IceGirl].iWeaponType = WEAPON_SSword;
	m_tWeaponParam[WPN_FrostBlood_IceGirl].iReinforce = 0;
	m_tWeaponParam[WPN_FrostBlood_IceGirl].fDamage = 25.f;
	m_tWeaponParam[WPN_FrostBlood_IceGirl].fRadius = 0.7f;
	m_tWeaponParam[WPN_FrostBlood_IceGirl].fTrail_Min = 0.3f;
	m_tWeaponParam[WPN_FrostBlood_IceGirl].fTrail_Max = 1.6f;
	m_tWeaponParam[WPN_FrostBlood_IceGirl].fCol_Height = 0.9f;

	//===========================================================================================

	m_tWeaponParam[WPN_DeerKingShield].iWeaponName = WPN_DeerKingShield;
	m_tWeaponParam[WPN_DeerKingShield].iWeaponType = WEAPON_Shield;
	m_tWeaponParam[WPN_DeerKingShield].iReinforce = 0;
	m_tWeaponParam[WPN_DeerKingShield].fDamage = 25.f;
	m_tWeaponParam[WPN_DeerKingShield].fRadius = 1.4f;
	m_tWeaponParam[WPN_DeerKingShield].fTrail_Min = 0.f;
	m_tWeaponParam[WPN_DeerKingShield].fTrail_Max = 1.f;
	m_tWeaponParam[WPN_DeerKingShield].fCol_Height = 0.f;

	//===========================================================================================

	m_tWeaponParam[WPN_DATA_End].iWeaponName = WPN_DATA_End;
	m_tWeaponParam[WPN_DATA_End].iWeaponType = WEAPON_End;
	m_tWeaponParam[WPN_DATA_End].iReinforce = 0;
	m_tWeaponParam[WPN_DATA_End].fDamage = 0.f;
	m_tWeaponParam[WPN_DATA_End].fRadius = 0.f;
	m_tWeaponParam[WPN_DATA_End].fTrail_Min = 0.f;
	m_tWeaponParam[WPN_DATA_End].fTrail_Max = 0.f;
	m_tWeaponParam[WPN_DATA_End].fCol_Height = 0.f;

	return S_OK;
}

void CWeapon_Inven::Add_Weapon(WPN_PARAM tAddWpnParam)
{
	CWeapon_Slot* pSlot = static_cast<CWeapon_Slot*>(g_pManagement->Clone_GameObject_Return(L"GameObject_WeaponSlot", nullptr));
	pSlot->Set_UI_Size(50.f, 50.f);
	pSlot->Set_WeaponParam(tAddWpnParam);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pSlot, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);	
	m_vecWeaponSlot.push_back(pSlot);

	// 슬롯 생성시 위치 조정
	for (_uint i = 0; i < m_vecWeaponSlot.size(); ++i)
	{
		m_vecWeaponSlot[i]->Set_Active(m_bIsActive);
		m_vecWeaponSlot[i]->Set_ViewZ(m_fViewZ - 0.1f);
		m_vecWeaponSlot[i]->Set_UI_Pos(m_fPosX - 103.f + 52.f * (i % 5), m_fPosY - 140.f + 52.f * (i / 5));
	}
}


CWeapon_Inven * CWeapon_Inven::Create(_Device pGraphic_Device)
{
	CWeapon_Inven* pInstance = new CWeapon_Inven(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CWeapon_Inven::Clone_GameObject(void * pArg)
{
	CWeapon_Inven* pInstance = new CWeapon_Inven(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CWeapon_Inven::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	CUI::Free();
}
