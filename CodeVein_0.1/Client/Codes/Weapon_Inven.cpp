#include "stdafx.h"
#include "..\Headers\Weapon_Inven.h"
#include "Weapon.h"
#include "Player.h"
#include "UI_Manager.h"

CWeapon_Inven::CWeapon_Inven(_Device pDevice)
	: CUI(pDevice)
{
	ZeroMemory(m_pRegistSlot, sizeof(CWeapon_Slot*) * 2);
}

CWeapon_Inven::CWeapon_Inven(const CWeapon_Inven & rhs)
	: CUI(rhs)
{
	ZeroMemory(m_pRegistSlot, sizeof(CWeapon_Slot*) * 2);
}

HRESULT CWeapon_Inven::Set_WeaponData_FromWeapon()
{
	m_pTempWeapon = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));
	if (!m_pTempWeapon)
		return E_FAIL;

	for (_int i = 0; i < WEAPON_DATA::WPN_DATA_End; i++)
		m_tWeaponParam[i] = m_pTempWeapon->Get_WeaponParam((WEAPON_DATA)i);

	for (auto& iter : m_vecWeaponSlot)
		iter->Set_WeaponParam(m_tWeaponParam[iter->Get_WeaponParam().iWeaponType]);

	return S_OK;
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
	
	
	SetUp_Default();

	LOOP(2)
	{
		ZeroMemory(&m_UseWeaponParam[i], sizeof(WPN_PARAM));
		m_UseWeaponParam[i].iWeaponName = WPN_DATA_End;
	}
	
	

	return NOERROR;
}

_int CWeapon_Inven::Update_GameObject(_double TimeDelta)
{
	Late_Init();

	if (m_bIsActive && !m_bIsSubActive)
	{
		SetUp_SubUI_Active(true);

		m_bIsSubActive = true;
	}
	else if (!m_bIsActive && m_bIsSubActive)
	{
		SetUp_SubUI_Active(false);
		m_bIsSubActive = false;
	}

	if (!m_bIsActive)
		return NO_EVENT;

	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.0f);


	Click_Inven();
	
	
	return NO_EVENT;
}

_int CWeapon_Inven::Late_Update_GameObject(_double TimeDelta)
{
	if (!m_bIsActive)
		return NO_EVENT;
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

	_uint iIdx = 0;
	for (auto& pSlot : m_vecWeaponSlot)
	{
		if (pSlot->Pt_InRect())
		{
			m_pExplainUI->Set_WeaponParam(pSlot->Get_WeaponParam());

			if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB) &&
				!pSlot->Get_Select())
			{
				CUI_Manager::Get_Instance()->Stop_Play_UISound(L"Slot_Regist.ogg", CSoundManager::CHANNELID::UI_Click, CSoundManager::Effect_Sound);
				Regist_Weapon(pSlot);			
			}
			/*if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_RB) &&
				pSlot->Get_Select())
			{
				UnRegist_Weapon(pSlot);
			}*/
			
		}
		iIdx++;
	}

	
	if (m_pExitIcon->Pt_InRect() && g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
	{
		m_bIsActive = false;
		CUI_Manager::Get_Instance()->Get_Total_Inven()->Set_Active(true);
	}
}


void CWeapon_Inven::Regist_Weapon(CWeapon_Slot* pWeaponSlot)
{
	/*if (pWeaponSlot->Get_WeaponParam().iWeaponName == WPN_DATA_End)
		return;

	if (m_UseWeaponParam[0].iWeaponName == WPN_DATA_End)
	{
		m_UseWeaponParam[0] = pWeaponSlot->Get_WeaponParam();
		pWeaponSlot->Set_Select(true);

		m_pPlayer->Set_WeaponSlot((CPlayer::ACTIVE_WEAPON_SLOT)0, (WEAPON_DATA)m_UseWeaponParam[0].iWeaponName);

		g_pSoundManager->Play_Sound(L"UI_CommonHover.wav", CSoundManager::WeaponInven_Regist_Slot01, CSoundManager::Effect_Sound);
	}
	else if (m_UseWeaponParam[1].iWeaponName == WPN_DATA_End)
	{
		m_UseWeaponParam[1] = pWeaponSlot->Get_WeaponParam();
		pWeaponSlot->Set_Select(true);

		m_pPlayer->Set_WeaponSlot((CPlayer::ACTIVE_WEAPON_SLOT)1, (WEAPON_DATA)m_UseWeaponParam[1].iWeaponName);

		g_pSoundManager->Play_Sound(L"UI_CommonHover.wav", CSoundManager::WeaponInven_Regist_Slot02, CSoundManager::Effect_Sound);
	}
	else
		return;*/

	if (!pWeaponSlot)
		return;
	
	pWeaponSlot->Set_Select(true);
	m_pRegistSlot[m_iRegistIndex] = pWeaponSlot;
	Reset_SlotSelect();
	CTotal_Inven* pTotal_Inven = CUI_Manager::Get_Instance()->Get_Total_Inven();

	pTotal_Inven->Set_WeaponParam(m_iRegistIndex, pWeaponSlot->Get_WeaponParam());
	m_pPlayer->Set_WeaponSlot((CPlayer::ACTIVE_WEAPON_SLOT)m_iRegistIndex, (WEAPON_DATA)pWeaponSlot->Get_WeaponParam().iWeaponName);
}

void CWeapon_Inven::UnRegist_Weapon(CWeapon_Slot * pWeaponSlot)
{
	if (pWeaponSlot->Get_WeaponParam().iWeaponName == WPN_DATA_End)
		return;

	// 한 개의 무기는 꼭 착용해야 함.
	if (1 >= m_vecWeaponSlot.size())
		return;
	_bool bIsNull = false;
	for (_int i = 0; i < 2; i++)
	{
		if (nullptr == m_vecWeaponSlot[i] || false == m_vecWeaponSlot[i]->Get_Select())
			bIsNull = true;
	}
	if (bIsNull)
		return;
	
	if (pWeaponSlot->Get_WeaponParam().iWeaponName == m_UseWeaponParam[0].iWeaponName)
	{
		m_UseWeaponParam[0].iWeaponName = WPN_DATA_End;
		m_UseWeaponParam[0].iWeaponType = WEAPON_End;
		m_UseWeaponParam[0].iPrice = 0;
		m_UseWeaponParam[0].iReinforce = 0;
		m_UseWeaponParam[0].fDamage = 0.f;
		m_UseWeaponParam[0].fPlusDamage = 0.f;
		m_UseWeaponParam[0].fRadius = 0.f;
		m_UseWeaponParam[0].fTrail_Min = 0.f;
		m_UseWeaponParam[0].fTrail_Max = 0.f;
		m_UseWeaponParam[0].fCol_Height = 0.f;
		pWeaponSlot->Set_Select(false);

		m_pPlayer->Set_WeaponSlot((CPlayer::ACTIVE_WEAPON_SLOT)0, WEAPON_DATA::WPN_DATA_End);

		CUI_Manager::Get_Instance()->Stop_Play_UISound(L"Slot_Regist.ogg", CSoundManager::CHANNELID::UI_Click, CSoundManager::Effect_Sound);
	}
	else if (pWeaponSlot->Get_WeaponParam().iWeaponName == m_UseWeaponParam[1].iWeaponName)
	{
		m_UseWeaponParam[1].iWeaponName = WPN_DATA_End;
		m_UseWeaponParam[1].iWeaponType = WEAPON_End;
		m_UseWeaponParam[1].iPrice = 0;
		m_UseWeaponParam[1].iReinforce = 0;
		m_UseWeaponParam[1].fDamage = 0.f;
		m_UseWeaponParam[1].fPlusDamage = 0.f;
		m_UseWeaponParam[1].fRadius = 0.f;
		m_UseWeaponParam[1].fTrail_Min = 0.f;
		m_UseWeaponParam[1].fTrail_Max = 0.f;
		m_UseWeaponParam[1].fCol_Height = 0.f;
		pWeaponSlot->Set_Select(false);

		m_pPlayer->Set_WeaponSlot((CPlayer::ACTIVE_WEAPON_SLOT)1, WEAPON_DATA::WPN_DATA_End);

		CUI_Manager::Get_Instance()->Stop_Play_UISound(L"Slot_Regist.ogg", CSoundManager::CHANNELID::UI_Click, CSoundManager::Effect_Sound);
	}
	else
		return;
}

// 디버깅용
HRESULT CWeapon_Inven::SetUp_WeaponData()
{
	//===========================================================================================
	// 한손검
	//===========================================================================================

	m_tWeaponParam[Wpn_SSword].iWeaponName = Wpn_SSword;
	m_tWeaponParam[Wpn_SSword].iWeaponName_InShop = WpnAll_SSword_Red;
	m_tWeaponParam[Wpn_SSword].iWeaponType = WEAPON_SSword;
	m_tWeaponParam[Wpn_SSword].iPrice = 100;
	m_tWeaponParam[Wpn_SSword].iReinforce = 0;
	m_tWeaponParam[Wpn_SSword].fDamage = 1000.f;
	m_tWeaponParam[Wpn_SSword].fPlusDamage = 100;
	m_tWeaponParam[Wpn_SSword].fRadius = 0.6f;
	m_tWeaponParam[Wpn_SSword].fTrail_Min = 0.6f;
	m_tWeaponParam[Wpn_SSword].fTrail_Max = 1.8f;
	m_tWeaponParam[Wpn_SSword].fCol_Height = 1.1f;

	m_tWeaponParam[Wpn_SSword_Black].iWeaponName = Wpn_SSword_Black;
	m_tWeaponParam[Wpn_SSword_Black].iWeaponName_InShop = WpnAll_SSword_Executioner;
	m_tWeaponParam[Wpn_SSword_Black].iWeaponType = WEAPON_SSword;
	m_tWeaponParam[Wpn_SSword_Black].iPrice = 100;
	m_tWeaponParam[Wpn_SSword_Black].iReinforce = 0;
	m_tWeaponParam[Wpn_SSword_Black].fDamage = 50.f;
	m_tWeaponParam[Wpn_SSword_Black].fPlusDamage = 100;
	m_tWeaponParam[Wpn_SSword_Black].fRadius = 0.9f;
	m_tWeaponParam[Wpn_SSword_Black].fTrail_Min = 0.6f;
	m_tWeaponParam[Wpn_SSword_Black].fTrail_Max = 1.8f;
	m_tWeaponParam[Wpn_SSword_Black].fCol_Height = 0.6f;

	m_tWeaponParam[Wpn_SSword_Military].iWeaponName = Wpn_SSword_Military;
	m_tWeaponParam[Wpn_SSword_Military].iWeaponName_InShop = WpnAll_SSword_Nakil;
	m_tWeaponParam[Wpn_SSword_Military].iWeaponType = WEAPON_SSword;
	m_tWeaponParam[Wpn_SSword_Military].iPrice = 100;
	m_tWeaponParam[Wpn_SSword_Military].iReinforce = 0;
	m_tWeaponParam[Wpn_SSword_Military].fDamage = 100.f;
	m_tWeaponParam[Wpn_SSword_Military].fPlusDamage = 50.f;
	m_tWeaponParam[Wpn_SSword_Military].fRadius = 0.6f;
	m_tWeaponParam[Wpn_SSword_Military].fTrail_Min = 0.6f;
	m_tWeaponParam[Wpn_SSword_Military].fTrail_Max = 1.8f;
	m_tWeaponParam[Wpn_SSword_Military].fCol_Height = 1.1f;

	m_tWeaponParam[Wpn_SSword_Slave].iWeaponName = Wpn_SSword_Slave;
	m_tWeaponParam[Wpn_SSword_Slave].iWeaponName_InShop = WpnAll_SSword_DarkBrodeSword;
	m_tWeaponParam[Wpn_SSword_Slave].iWeaponType = WEAPON_SSword;
	m_tWeaponParam[Wpn_SSword_Slave].iPrice = 100;
	m_tWeaponParam[Wpn_SSword_Slave].iReinforce = 0;
	m_tWeaponParam[Wpn_SSword_Slave].fDamage = 100.f;
	m_tWeaponParam[Wpn_SSword_Slave].fPlusDamage = 50.f;
	m_tWeaponParam[Wpn_SSword_Slave].fRadius = 0.7f;
	m_tWeaponParam[Wpn_SSword_Slave].fTrail_Min = 0.6f;
	m_tWeaponParam[Wpn_SSword_Slave].fTrail_Max = 1.8f;
	m_tWeaponParam[Wpn_SSword_Slave].fCol_Height = 1.2f;

	//===========================================================================================
	// 대검
	//===========================================================================================

	m_tWeaponParam[Wpn_LSword].iWeaponName = Wpn_LSword;
	m_tWeaponParam[Wpn_LSword].iWeaponName_InShop = WpnAll_LSword_Tsubai;
	m_tWeaponParam[Wpn_LSword].iWeaponType = WEAPON_LSword;
	m_tWeaponParam[Wpn_LSword].iPrice = 100;
	m_tWeaponParam[Wpn_LSword].iReinforce = 0;
	m_tWeaponParam[Wpn_LSword].fDamage = 155.f;
	m_tWeaponParam[Wpn_LSword].fPlusDamage = 75.f;
	m_tWeaponParam[Wpn_LSword].fRadius = 0.9f;
	m_tWeaponParam[Wpn_LSword].fTrail_Min = 0.8f;
	m_tWeaponParam[Wpn_LSword].fTrail_Max = 2.1f;
	m_tWeaponParam[Wpn_LSword].fCol_Height = 1.4f;

	m_tWeaponParam[Wpn_LSword_Black].iWeaponName = Wpn_LSword_Black;
	m_tWeaponParam[Wpn_LSword_Black].iWeaponName_InShop = WpnAll_LSword_Black;
	m_tWeaponParam[Wpn_LSword_Black].iWeaponType = WEAPON_LSword;
	m_tWeaponParam[Wpn_LSword_Black].iPrice = 100;
	m_tWeaponParam[Wpn_LSword_Black].iReinforce = 0;
	m_tWeaponParam[Wpn_LSword_Black].fDamage = 155.f;
	m_tWeaponParam[Wpn_LSword_Black].fPlusDamage = 75.f;
	m_tWeaponParam[Wpn_LSword_Black].fRadius = 0.8f;
	m_tWeaponParam[Wpn_LSword_Black].fTrail_Min = 0.8f;
	m_tWeaponParam[Wpn_LSword_Black].fTrail_Max = 2.1f;
	m_tWeaponParam[Wpn_LSword_Black].fCol_Height = 1.3f;

	m_tWeaponParam[Wpn_LSword_Military].iWeaponName = Wpn_LSword_Military;
	m_tWeaponParam[Wpn_LSword_Military].iWeaponName_InShop = WpnAll_LSword_Nakil;
	m_tWeaponParam[Wpn_LSword_Military].iWeaponType = WEAPON_LSword;
	m_tWeaponParam[Wpn_LSword_Military].iPrice = 100;
	m_tWeaponParam[Wpn_LSword_Military].iReinforce = 0;
	m_tWeaponParam[Wpn_LSword_Military].fDamage = 155.f;
	m_tWeaponParam[Wpn_LSword_Military].fPlusDamage = 75.f;
	m_tWeaponParam[Wpn_LSword_Military].fRadius = 0.8f;
	m_tWeaponParam[Wpn_LSword_Military].fTrail_Min = 0.8f;
	m_tWeaponParam[Wpn_LSword_Military].fTrail_Max = 2.1f;
	m_tWeaponParam[Wpn_LSword_Military].fCol_Height = 1.5f;

	m_tWeaponParam[Wpn_LSword_Slave].iWeaponName = Wpn_LSword_Slave;
	m_tWeaponParam[Wpn_LSword_Slave].iWeaponName_InShop = WpnAll_LSword_DarkTsubai;
	m_tWeaponParam[Wpn_LSword_Slave].iWeaponType = WEAPON_LSword;
	m_tWeaponParam[Wpn_LSword_Slave].iPrice = 100;
	m_tWeaponParam[Wpn_LSword_Slave].iReinforce = 0;
	m_tWeaponParam[Wpn_LSword_Slave].fDamage = 155.f;
	m_tWeaponParam[Wpn_LSword_Slave].fPlusDamage = 75.f;
	m_tWeaponParam[Wpn_LSword_Slave].fRadius = 0.8f;
	m_tWeaponParam[Wpn_LSword_Slave].fTrail_Min = 0.8f;
	m_tWeaponParam[Wpn_LSword_Slave].fTrail_Max = 2.1f;
	m_tWeaponParam[Wpn_LSword_Slave].fCol_Height = 1.5f;

	//===========================================================================================
	// 도끼창
	//===========================================================================================

	m_tWeaponParam[Wpn_Halberd].iWeaponName = Wpn_Halberd;
	m_tWeaponParam[Wpn_Halberd].iWeaponName_InShop = WpnAll_Halberd_SolidPiercer;
	m_tWeaponParam[Wpn_Halberd].iWeaponType = WEAPON_Halberd;
	m_tWeaponParam[Wpn_Halberd].iPrice = 100;
	m_tWeaponParam[Wpn_Halberd].iReinforce = 0;
	m_tWeaponParam[Wpn_Halberd].fDamage = 160.f;
	m_tWeaponParam[Wpn_Halberd].fPlusDamage = 80.f;
	m_tWeaponParam[Wpn_Halberd].fRadius = 0.7f;
	m_tWeaponParam[Wpn_Halberd].fTrail_Min = 1.2f;
	m_tWeaponParam[Wpn_Halberd].fTrail_Max = 2.3f;
	m_tWeaponParam[Wpn_Halberd].fCol_Height = 1.6f;

	m_tWeaponParam[Wpn_Halberd_Black].iWeaponName = Wpn_Halberd_Black;
	m_tWeaponParam[Wpn_Halberd_Black].iWeaponName_InShop = WpnAll_Halberd_Black;
	m_tWeaponParam[Wpn_Halberd_Black].iWeaponType = WEAPON_Halberd;
	m_tWeaponParam[Wpn_Halberd_Black].iPrice = 100;
	m_tWeaponParam[Wpn_Halberd_Black].iReinforce = 0;
	m_tWeaponParam[Wpn_Halberd_Black].fDamage = 160.f;
	m_tWeaponParam[Wpn_Halberd_Black].fPlusDamage = 80.f;
	m_tWeaponParam[Wpn_Halberd_Black].fRadius = 0.7f;
	m_tWeaponParam[Wpn_Halberd_Black].fTrail_Min = 1.2f;
	m_tWeaponParam[Wpn_Halberd_Black].fTrail_Max = 2.3f;
	m_tWeaponParam[Wpn_Halberd_Black].fCol_Height = 1.4f;

	m_tWeaponParam[Wpn_Halberd_Military].iWeaponName = Wpn_Halberd_Military;
	m_tWeaponParam[Wpn_Halberd_Military].iWeaponName_InShop = WpnAll_Halberd_Nakil;
	m_tWeaponParam[Wpn_Halberd_Military].iWeaponType = WEAPON_Halberd;
	m_tWeaponParam[Wpn_Halberd_Military].iPrice = 100;
	m_tWeaponParam[Wpn_Halberd_Military].iReinforce = 0;
	m_tWeaponParam[Wpn_Halberd_Military].fDamage = 160.f;
	m_tWeaponParam[Wpn_Halberd_Military].fPlusDamage = 80.f;
	m_tWeaponParam[Wpn_Halberd_Military].fRadius = 0.7f;
	m_tWeaponParam[Wpn_Halberd_Military].fTrail_Min = 1.2f;
	m_tWeaponParam[Wpn_Halberd_Military].fTrail_Max = 2.3f;
	m_tWeaponParam[Wpn_Halberd_Military].fCol_Height = 1.4f;

	m_tWeaponParam[Wpn_Halberd_Slave].iWeaponName = Wpn_Halberd_Slave;
	m_tWeaponParam[Wpn_Halberd_Slave].iWeaponName_InShop = WpnAll_Halberd_DarkBodish;
	m_tWeaponParam[Wpn_Halberd_Slave].iWeaponType = WEAPON_Halberd;
	m_tWeaponParam[Wpn_Halberd_Slave].iPrice = 100;
	m_tWeaponParam[Wpn_Halberd_Slave].iReinforce = 0;
	m_tWeaponParam[Wpn_Halberd_Slave].fDamage = 160.f;
	m_tWeaponParam[Wpn_Halberd_Slave].fPlusDamage = 80.f;
	m_tWeaponParam[Wpn_Halberd_Slave].fRadius = 0.8f;
	m_tWeaponParam[Wpn_Halberd_Slave].fTrail_Min = 1.2f;
	m_tWeaponParam[Wpn_Halberd_Slave].fTrail_Max = 2.3f;
	m_tWeaponParam[Wpn_Halberd_Slave].fCol_Height = 1.3f;

	//===========================================================================================
	// 해머
	//===========================================================================================

	m_tWeaponParam[Wpn_Hammer].iWeaponName = Wpn_Hammer;
	m_tWeaponParam[Wpn_Hammer].iWeaponName_InShop = WpnAll_Hammer_ImperseAnker;
	m_tWeaponParam[Wpn_Hammer].iWeaponType = WEAPON_Hammer;
	m_tWeaponParam[Wpn_Hammer].iPrice = 100;
	m_tWeaponParam[Wpn_Hammer].iReinforce = 0;
	m_tWeaponParam[Wpn_Hammer].fDamage = 155.f;
	m_tWeaponParam[Wpn_Hammer].fPlusDamage = 75.f;
	m_tWeaponParam[Wpn_Hammer].fRadius = 0.7f;
	m_tWeaponParam[Wpn_Hammer].fTrail_Min = 0.75f;
	m_tWeaponParam[Wpn_Hammer].fTrail_Max = 1.5f;
	m_tWeaponParam[Wpn_Hammer].fCol_Height = 1.1f;

	m_tWeaponParam[Wpn_Hammer_Black].iWeaponName = Wpn_Hammer_Black;
	m_tWeaponParam[Wpn_Hammer_Black].iWeaponName_InShop = WpnAll_Hammer_Black;
	m_tWeaponParam[Wpn_Hammer_Black].iWeaponType = WEAPON_Hammer;
	m_tWeaponParam[Wpn_Hammer_Black].iPrice = 100;
	m_tWeaponParam[Wpn_Hammer_Black].iReinforce = 0;
	m_tWeaponParam[Wpn_Hammer_Black].fDamage = 155.f;
	m_tWeaponParam[Wpn_Hammer_Black].fPlusDamage = 75.f;
	m_tWeaponParam[Wpn_Hammer_Black].fRadius = 0.75f;
	m_tWeaponParam[Wpn_Hammer_Black].fTrail_Min = 0.75f;
	m_tWeaponParam[Wpn_Hammer_Black].fTrail_Max = 1.5f;
	m_tWeaponParam[Wpn_Hammer_Black].fCol_Height = 1.0f;

	m_tWeaponParam[Wpn_Hammer_Military].iWeaponName = Wpn_Hammer_Military;
	m_tWeaponParam[Wpn_Hammer_Military].iWeaponName_InShop = WpnAll_Hammer_Nakil;
	m_tWeaponParam[Wpn_Hammer_Military].iWeaponType = WEAPON_Hammer;
	m_tWeaponParam[Wpn_Hammer_Military].iPrice = 100;
	m_tWeaponParam[Wpn_Hammer_Military].iReinforce = 0;
	m_tWeaponParam[Wpn_Hammer_Military].fDamage = 155.f;
	m_tWeaponParam[Wpn_Hammer_Military].fPlusDamage = 75.f;
	m_tWeaponParam[Wpn_Hammer_Military].fRadius = 1.1f;
	m_tWeaponParam[Wpn_Hammer_Military].fTrail_Min = 0.75f;
	m_tWeaponParam[Wpn_Hammer_Military].fTrail_Max = 1.5f;
	m_tWeaponParam[Wpn_Hammer_Military].fCol_Height = 1.9f;

	m_tWeaponParam[Wpn_Hammer_Slave].iWeaponName = Wpn_Hammer_Slave;
	m_tWeaponParam[Wpn_Hammer_Slave].iWeaponName_InShop = WpnAll_Hammer_DarkHeavyAxe;
	m_tWeaponParam[Wpn_Hammer_Slave].iWeaponType = WEAPON_Hammer;
	m_tWeaponParam[Wpn_Hammer_Slave].iPrice = 100;
	m_tWeaponParam[Wpn_Hammer_Slave].iReinforce = 0;
	m_tWeaponParam[Wpn_Hammer_Slave].fDamage = 155.f;
	m_tWeaponParam[Wpn_Hammer_Slave].fPlusDamage = 75.f;
	m_tWeaponParam[Wpn_Hammer_Slave].fRadius = 0.7f;
	m_tWeaponParam[Wpn_Hammer_Slave].fTrail_Min = 0.75f;
	m_tWeaponParam[Wpn_Hammer_Slave].fTrail_Max = 1.5f;
	m_tWeaponParam[Wpn_Hammer_Slave].fCol_Height = 0.9f;

	//===========================================================================================
	// 총검
	//===========================================================================================

	m_tWeaponParam[Wpn_Gun].iWeaponName = Wpn_Gun;
	m_tWeaponParam[Wpn_Gun].iWeaponName_InShop = WpnAll_Gun_Bayonet;
	m_tWeaponParam[Wpn_Gun].iWeaponType = WEAPON_Gun;
	m_tWeaponParam[Wpn_Gun].iPrice = 100;
	m_tWeaponParam[Wpn_Gun].iReinforce = 0;
	m_tWeaponParam[Wpn_Gun].fDamage = 70.f;
	m_tWeaponParam[Wpn_Gun].fPlusDamage = 30.f;
	m_tWeaponParam[Wpn_Gun].fRadius = 0.5f;
	m_tWeaponParam[Wpn_Gun].fTrail_Min = 0.75f;
	m_tWeaponParam[Wpn_Gun].fTrail_Max = 1.5f;
	m_tWeaponParam[Wpn_Gun].fCol_Height = 1.0f;

	m_tWeaponParam[Wpn_Gun_Black].iWeaponName = Wpn_Gun_Black;
	m_tWeaponParam[Wpn_Gun_Black].iWeaponName_InShop = WpnAll_Gun_ReverTardor;
	m_tWeaponParam[Wpn_Gun_Black].iWeaponType = WEAPON_Gun;
	m_tWeaponParam[Wpn_Gun_Black].iPrice = 100;
	m_tWeaponParam[Wpn_Gun_Black].iReinforce = 0;
	m_tWeaponParam[Wpn_Gun_Black].fDamage = 70.f;
	m_tWeaponParam[Wpn_Gun_Black].fPlusDamage = 30.f;
	m_tWeaponParam[Wpn_Gun_Black].fRadius = 0.5f;
	m_tWeaponParam[Wpn_Gun_Black].fTrail_Min = 0.75f;
	m_tWeaponParam[Wpn_Gun_Black].fTrail_Max = 1.5f;
	m_tWeaponParam[Wpn_Gun_Black].fCol_Height = 1.1f;

	m_tWeaponParam[Wpn_Gun_Military].iWeaponName = Wpn_Gun_Military;
	m_tWeaponParam[Wpn_Gun_Military].iWeaponName_InShop = WpnAll_Gun_Nakil;
	m_tWeaponParam[Wpn_Gun_Military].iWeaponType = WEAPON_Gun;
	m_tWeaponParam[Wpn_Gun_Military].iPrice = 100;
	m_tWeaponParam[Wpn_Gun_Military].iReinforce = 0;
	m_tWeaponParam[Wpn_Gun_Military].fDamage = 70.f;
	m_tWeaponParam[Wpn_Gun_Military].fPlusDamage = 30.f;
	m_tWeaponParam[Wpn_Gun_Military].fRadius = 0.5f;
	m_tWeaponParam[Wpn_Gun_Military].fTrail_Min = 0.75f;
	m_tWeaponParam[Wpn_Gun_Military].fTrail_Max = 1.5f;
	m_tWeaponParam[Wpn_Gun_Military].fCol_Height = 1.1f;

	m_tWeaponParam[Wpn_Gun_Slave].iWeaponName = Wpn_Gun_Slave;
	m_tWeaponParam[Wpn_Gun_Slave].iWeaponName_InShop = WpnAll_Gun_DarkBayonet;
	m_tWeaponParam[Wpn_Gun_Slave].iWeaponType = WEAPON_Gun;
	m_tWeaponParam[Wpn_Gun_Slave].iPrice = 100;
	m_tWeaponParam[Wpn_Gun_Slave].iReinforce = 0;
	m_tWeaponParam[Wpn_Gun_Slave].fDamage = 70.f;
	m_tWeaponParam[Wpn_Gun_Slave].fPlusDamage = 30.f;
	m_tWeaponParam[Wpn_Gun_Slave].fRadius = 0.5f;
	m_tWeaponParam[Wpn_Gun_Slave].fTrail_Min = 0.75f;
	m_tWeaponParam[Wpn_Gun_Slave].fTrail_Max = 1.5f;
	m_tWeaponParam[Wpn_Gun_Slave].fCol_Height = 1.1f;

	//===========================================================================================
	// 방패
	//===========================================================================================

	m_tWeaponParam[WPN_Shield_Normal].iWeaponName = WPN_Shield_Normal;
	m_tWeaponParam[WPN_Shield_Normal].iWeaponName_InShop = WpnAll_END;
	m_tWeaponParam[WPN_Shield_Normal].iWeaponType = WEAPON_Shield;
	m_tWeaponParam[WPN_Shield_Normal].iPrice = 100;
	m_tWeaponParam[WPN_Shield_Normal].iReinforce = 0;
	m_tWeaponParam[WPN_Shield_Normal].fDamage = 25.f;
	m_tWeaponParam[WPN_Shield_Normal].fPlusDamage = 10.f;
	m_tWeaponParam[WPN_Shield_Normal].fRadius = 0.7f;
	m_tWeaponParam[WPN_Shield_Normal].fTrail_Min = 0.f;
	m_tWeaponParam[WPN_Shield_Normal].fTrail_Max = 1.f;
	m_tWeaponParam[WPN_Shield_Normal].fCol_Height = 0.f;

	//===========================================================================================

	m_tWeaponParam[WPN_QueenLance].iWeaponName = WPN_QueenLance;
	m_tWeaponParam[WPN_QueenLance].iWeaponName_InShop = WpnAll_END;
	m_tWeaponParam[WPN_QueenLance].iWeaponType = WEAPON_SSword;
	m_tWeaponParam[WPN_QueenLance].iPrice = 100;
	m_tWeaponParam[WPN_QueenLance].iReinforce = 0;
	m_tWeaponParam[WPN_QueenLance].fDamage = 25.f;
	m_tWeaponParam[WPN_QueenLance].fPlusDamage = 10.f;
	m_tWeaponParam[WPN_QueenLance].fRadius = 1.4f;
	m_tWeaponParam[WPN_QueenLance].fTrail_Min = 0.3f;
	m_tWeaponParam[WPN_QueenLance].fTrail_Max = 3.4f;
	m_tWeaponParam[WPN_QueenLance].fCol_Height = 1.7f;

	m_tWeaponParam[WPN_QueenShield].iWeaponName = WPN_QueenShield;
	m_tWeaponParam[WPN_QueenShield].iWeaponName_InShop = WpnAll_END;
	m_tWeaponParam[WPN_QueenShield].iWeaponType = WEAPON_Shield;
	m_tWeaponParam[WPN_QueenShield].iPrice = 100;
	m_tWeaponParam[WPN_QueenShield].iReinforce = 0;
	m_tWeaponParam[WPN_QueenShield].fDamage = 25.f;
	m_tWeaponParam[WPN_QueenShield].fPlusDamage = 10.f;
	m_tWeaponParam[WPN_QueenShield].fRadius = 0.8f;
	m_tWeaponParam[WPN_QueenShield].fTrail_Min = 0.f;
	m_tWeaponParam[WPN_QueenShield].fTrail_Max = 1.f;
	m_tWeaponParam[WPN_QueenShield].fCol_Height = 0.f;

	//===========================================================================================

	m_tWeaponParam[WPN_FrostBlood_IceGirl].iWeaponName = WPN_FrostBlood_IceGirl;
	m_tWeaponParam[WPN_FrostBlood_IceGirl].iWeaponName_InShop = WpnAll_END;
	m_tWeaponParam[WPN_FrostBlood_IceGirl].iWeaponType = WEAPON_SSword;
	m_tWeaponParam[WPN_FrostBlood_IceGirl].iPrice = 100;
	m_tWeaponParam[WPN_FrostBlood_IceGirl].iReinforce = 0;
	m_tWeaponParam[WPN_FrostBlood_IceGirl].fDamage = 25.f;
	m_tWeaponParam[WPN_FrostBlood_IceGirl].fPlusDamage = 10.f;
	m_tWeaponParam[WPN_FrostBlood_IceGirl].fRadius = 0.7f;
	m_tWeaponParam[WPN_FrostBlood_IceGirl].fTrail_Min = 0.3f;
	m_tWeaponParam[WPN_FrostBlood_IceGirl].fTrail_Max = 1.6f;
	m_tWeaponParam[WPN_FrostBlood_IceGirl].fCol_Height = 0.9f;

	//===========================================================================================

	m_tWeaponParam[WPN_DeerKingShield].iWeaponName = WPN_DeerKingShield;
	m_tWeaponParam[WPN_DeerKingShield].iWeaponName_InShop = WpnAll_END;
	m_tWeaponParam[WPN_DeerKingShield].iWeaponType = WEAPON_Shield;
	m_tWeaponParam[WPN_DeerKingShield].iPrice = 100;
	m_tWeaponParam[WPN_DeerKingShield].iReinforce = 0;
	m_tWeaponParam[WPN_DeerKingShield].fDamage = 25.f;
	m_tWeaponParam[WPN_DeerKingShield].fPlusDamage = 10.f;
	m_tWeaponParam[WPN_DeerKingShield].fRadius = 1.4f;
	m_tWeaponParam[WPN_DeerKingShield].fTrail_Min = 0.f;
	m_tWeaponParam[WPN_DeerKingShield].fTrail_Max = 1.f;
	m_tWeaponParam[WPN_DeerKingShield].fCol_Height = 0.f;

	return S_OK;
}

void CWeapon_Inven::SetUp_Default()
{
	m_pExplainUI = static_cast<CExplainWeaponUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_ExplainWeaponUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pExplainUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pExitIcon = static_cast<CInventory_Icon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_InvenIcon", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pExitIcon, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	m_pExitIcon->Set_UI_Pos(m_fPosX + 120.f, m_fPosY - 203.f);
	m_pExitIcon->Set_UI_Size(40.f, 40.f);
	m_pExitIcon->Set_Type(CInventory_Icon::ICON_EXIT);

	
}

void CWeapon_Inven::SetUp_SlotPos()
{
	_uint iIdx = 0;
	for (auto& pWeaponSlot : m_vecWeaponSlot)
	{		
		pWeaponSlot->Set_UI_Pos(m_fPosX - 100.f + 52.f * (iIdx % 5), m_fPosY - 150.f + 52.f * (iIdx / 5));
		pWeaponSlot->Set_ViewZ(m_fViewZ - 0.1f);
		iIdx++;
	}
}

void CWeapon_Inven::SetUp_SubUI_Active(_bool bIsActive)
{
	_uint iIdx = 0;
	for (auto& pWeaponSlot : m_vecWeaponSlot)
	{
		pWeaponSlot->Set_Active(bIsActive);
		iIdx++;
	}

	m_pExplainUI->Set_Active(bIsActive);
	m_pExitIcon->Set_Active(bIsActive);
}

void CWeapon_Inven::Late_Init()
{
	if (m_bLateInit)
		return;

	m_bLateInit = true;

	m_pPlayer = static_cast<CPlayer*>(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));

	m_vecWeaponSlot.reserve(2);

	WPN_PARAM tWeaponParam = {};

	tWeaponParam.iWeaponName = Wpn_SSword;
	tWeaponParam.iWeaponName_InShop = WpnAll_SSword_Red;
	tWeaponParam.iWeaponType = WEAPON_SSword;
	tWeaponParam.iPrice = 100;
	tWeaponParam.iReinforce = 0;
	tWeaponParam.fDamage = 1000.f;
	tWeaponParam.fPlusDamage = 100;
	tWeaponParam.fRadius = 0.6f;
	tWeaponParam.fTrail_Min = 0.6f;
	tWeaponParam.fTrail_Max = 1.8f;
	tWeaponParam.fCol_Height = 1.1f;


	Add_Weapon(tWeaponParam);
	Regist_Weapon(m_vecWeaponSlot[0]);
	
	tWeaponParam.iWeaponName = Wpn_LSword;
	tWeaponParam.iWeaponName_InShop = WpnAll_LSword_Tsubai;
	tWeaponParam.iWeaponType = WEAPON_LSword;
	tWeaponParam.iPrice = 100;
	tWeaponParam.iReinforce = 0;
	tWeaponParam.fDamage = 155.f;
	tWeaponParam.fPlusDamage = 75.f;
	tWeaponParam.fRadius = 0.9f;
	tWeaponParam.fTrail_Min = 0.8f;
	tWeaponParam.fTrail_Max = 2.1f;
	tWeaponParam.fCol_Height = 1.4f;


	Add_Weapon(tWeaponParam);
	
}

void CWeapon_Inven::Reset_SlotSelect()
{
	for (auto& iter : m_vecWeaponSlot)
	{
		iter->Set_Select(false);
	}
	
	LOOP(2)
	{
		if(nullptr != m_pRegistSlot[i])
			m_pRegistSlot[i]->Set_Select(true);
	}
}

void CWeapon_Inven::Add_Weapon(WPN_PARAM tAddWpnParam)
{
	CWeapon_Slot* pSlot = static_cast<CWeapon_Slot*>(g_pManagement->Clone_GameObject_Return(L"GameObject_WeaponSlot", nullptr));
	pSlot->Set_UI_Size(50.f, 50.f);
	pSlot->Set_WeaponParam(tAddWpnParam);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pSlot, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);	
	m_vecWeaponSlot.push_back(pSlot);

	SetUp_SlotPos();
}

void CWeapon_Inven::Sell_Weapon()
{
	_ulong idx = 0;
	for (auto& pSlot : m_vecWeaponSlot)
	{
		if (pSlot->Get_SelectShop())
		{
			pSlot->Set_Dead();
			m_vecWeaponSlot.erase(m_vecWeaponSlot.begin() + idx);
			m_vecWeaponSlot.shrink_to_fit();
			break;
		}
		++idx;
	}

	SetUp_SlotPos();
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
	if(m_pTempWeapon)
		Safe_Release(m_pTempWeapon);

	CUI::Free();
}
