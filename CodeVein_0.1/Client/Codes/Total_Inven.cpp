#include "stdafx.h"
#include "..\Headers\Total_Inven.h"
#include "UI_Manager.h"
#include "Player.h"

CTotal_Inven::CTotal_Inven(_Device pDevice)
	: CUI(pDevice)
{
}

CTotal_Inven::CTotal_Inven(const CTotal_Inven & rhs)
	: CUI(rhs)
{
}

Skill_ID CTotal_Inven::Get_Registration_Skill(_uint iNum)
{
	if (iNum < 0 || iNum > m_vecSkillIcon.size() - 1)
		return SkillID_End;

	return m_vecSkillIcon[iNum]->Get_SkillID();
}

WEAPON_DATA CTotal_Inven::Get_Registration_Weapon(_uint iNum)
{
	return (WEAPON_DATA)m_pWeapon_Slot[iNum]->Get_WeaponParam().iWeaponType;
}

void CTotal_Inven::Set_Skill_ID(_uint iNum, Skill_ID eSkillID)
{
	if (iNum < 0 || iNum > m_vecSkillIcon.size() - 1)
		return;

	m_vecSkillIcon[iNum]->Set_SkillID(eSkillID);
}

void CTotal_Inven::Set_WeaponParam(_uint iIndex, WPN_PARAM tWpnParam)
{
	m_pWeapon_Slot[iIndex]->Set_WeaponParam(tWpnParam);
}

void CTotal_Inven::Set_ArmorParam(ARMOR_PARAM tArmorParam)
{
	m_pArmor_Slot->Set_ArmorParam(tArmorParam);
}

HRESULT CTotal_Inven::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CTotal_Inven::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	m_fPosX = WINCX * 0.5f;
	m_fPosY = WINCY * 0.5f;
	m_fSizeX = WINCX;
	m_fSizeY = WINCY;
	m_fViewZ = 5.f;

	m_bIsActive = false;

	SetUp_Default();

	return NOERROR;
}

_int CTotal_Inven::Update_GameObject(_double TimeDelta)
{
	if (m_bIsActive && !m_bIsSubActive)
	{
		CUI_Manager::Get_Instance()->Get_Instance()->Get_Inventory()->Set_Active(false);
		CUI_Manager::Get_Instance()->Get_Instance()->Get_Inventory()->Set_Detail(false);
		CUI_Manager::Get_Instance()->Get_StatusUI()->Set_Active(true);

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

	Late_Init();

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.0f);

	


	Click_Icon();
	
	// 퀵슬롯 정보
	//CQuickSlot* pQuickSlot = CUI_Manager::Get_Instance()->Get_QuickSlot();

	//// 활성화 상태라면
	//if (m_bIsActive)
	//{
	//	CUI_Manager::Get_Instance()->Get_Instance()->Get_Inventory()->Set_Active(false);
	//	CUI_Manager::Get_Instance()->Get_Instance()->Get_Inventory()->Set_Detail(false);		
	//	// 스테이터스 창 활성화
	//	CUI_Manager::Get_Instance()->Get_StatusUI()->Set_Active(true);
	//}

	
	
	//// 무기
	//LOOP(2)
	//{
	//	CWeapon_Inven* pWeaponInven = CUI_Manager::Get_Instance()->Get_Weapon_Inven();
	//	m_pWeapon_Slot[i]->Set_WeaponParam(pWeaponInven->Get_UseWeaponParam(i));
	//}
	
	// 아장
	//CArmor_Inven* pArmorInven = CUI_Manager::Get_Instance()->Get_Armor_Inven();
	//m_pArmor_Slot->Set_ArmorParam(pArmorInven->Get_UseArmorParam());
	
	return NO_EVENT;
}

_int CTotal_Inven::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CTotal_Inven::Render_GameObject()
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

HRESULT CTotal_Inven::Add_Component()
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

HRESULT CTotal_Inven::SetUp_ConstantTable()
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

void CTotal_Inven::SetUp_Default()
{
	CUI::UI_DESC* pDesc = nullptr;
	
	CInventory_Icon* pInstance = nullptr;
	LOOP(3)
	{
		pInstance = static_cast<CInventory_Icon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_InvenIcon",nullptr));
		pInstance->Set_UI_Pos(235.f + 50.f * i, 130.f);
		pInstance->Set_UI_Size(40.f, 40.f);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
		
		m_vecIcon.push_back(pInstance);
	}
	m_vecIcon[0]->Set_Type(CInventory_Icon::ICON_ALL);
	m_vecIcon[1]->Set_Type(CInventory_Icon::ICON_PET);
	m_vecIcon[2]->Set_Type(CInventory_Icon::ICON_EXIT);

	LOOP(3)
	{
		pDesc = new CUI::UI_DESC;
		pDesc->fPosX = 239.f + 57.f * i;
		pDesc->fPosY = 200.f;
		pDesc->fSizeX = 55.f;
		pDesc->fSizeY = 55.f;

		if (2 > i)
		{
			g_pManagement->Add_GameObject_ToLayer(L"GameObject_WeaponSlot", SCENE_MORTAL, L"Layer_PlayerUI", pDesc);
			m_pWeapon_Slot[i] = static_cast<CWeapon_Slot*>(g_pManagement->Get_GameObjectBack(L"Layer_PlayerUI", SCENE_MORTAL));
			WPN_PARAM tWpnParam = {};
			tWpnParam.iWeaponName = WPN_DATA_End;
			tWpnParam.iWeaponType = WEAPON_End;
			m_pWeapon_Slot[i]->Set_WeaponParam(tWpnParam);
		}
		else if (2 == i)
		{
			g_pManagement->Add_GameObject_ToLayer(L"GameObject_ArmorSlot", SCENE_MORTAL, L"Layer_PlayerUI", pDesc);
			m_pArmor_Slot = static_cast<CArmor_Slot*>(g_pManagement->Get_GameObjectBack(L"Layer_PlayerUI", SCENE_MORTAL));
			
			m_pArmor_Slot->Init_Param();
		}
	}
	
	pDesc = new CUI::UI_DESC;
	pDesc->fPosX = 375.f;
	pDesc->fPosY = 322.f;
	pDesc->fSizeX = 90.f;
	pDesc->fSizeY = 67.f;
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_BloodCodeIcon", SCENE_MORTAL, L"Layer_PlayerUI", pDesc);
	m_pBloodCode = static_cast<CBloodCode_Icon*>(g_pManagement->Get_GameObjectBack(L"Layer_PlayerUI", SCENE_MORTAL));

	LOOP(8)
	{
		pDesc = new CUI::UI_DESC;
		pDesc->fPosX = 215.f + 40.f * i;
		pDesc->fPosY = 488.f;
		pDesc->fSizeX = 35.f;
		pDesc->fSizeY = 35.f;
		pDesc->iIndex = 0;
		g_pManagement->Add_GameObject_ToLayer(L"GameObject_InfoSlot", SCENE_MORTAL, L"Layer_PlayerUI", pDesc);
		m_pQuickSlotInfo[i] = static_cast<CInfo_Slot*>(g_pManagement->Get_GameObjectBack(L"Layer_PlayerUI", SCENE_MORTAL));
	}
	
	LOOP(8)
	{
		CSkillSlot* pSkillSlot = static_cast<CSkillSlot*>(g_pManagement->Clone_GameObject_Return(L"GameObject_SkillSlot", nullptr));
		pSkillSlot->Set_UI_Size(50.f, 50.f);
		pSkillSlot->Set_ViewZ(m_fViewZ - 0.1f);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pSkillSlot, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
		m_vecSkillIcon.push_back(pSkillSlot);
	}

	m_vecSkillIcon[0]->Set_UI_Pos(536.f, 230.f);
	m_vecSkillIcon[1]->Set_UI_Pos(486.f, 260.f);
	m_vecSkillIcon[2]->Set_UI_Pos(586.f, 260.f);
	m_vecSkillIcon[3]->Set_UI_Pos(536.f, 290.f);

	m_vecSkillIcon[4]->Set_UI_Pos(536.f, 350.f);
	m_vecSkillIcon[5]->Set_UI_Pos(486.f, 380.f);
	m_vecSkillIcon[6]->Set_UI_Pos(586.f, 380.f);
	m_vecSkillIcon[7]->Set_UI_Pos(536.f, 410.f);

	m_pNoticeUI = static_cast<CNoticeUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_NoticeUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pNoticeUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	m_pNoticeUI->Set_UI_Pos(248.f, 81.f);
	m_pNoticeUI->Set_UI_Size(186.664f, 40.5f); // 280 : 60 -> 4.6666 : 1
	m_pNoticeUI->Set_ViewZ(m_fViewZ - 0.1f);
}

void CTotal_Inven::Click_Icon()
{
	if (!m_bIsActive)
		return;

	if (m_vecIcon[0]->Pt_InRect() &&
		g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
	{
		g_pSoundManager->Play_Sound(L"UI_CommonHover.wav", CSoundManager::Total_Inven_All, CSoundManager::Effect_Sound);
		m_pInventory = CUI_Manager::Get_Instance()->Get_Inventory();
		m_pInventory->Set_Active(true);
		CUI_Manager::Get_Instance()->Get_Expendables_Inven()->Set_Active(true);
		m_bIsActive = false;
	}
	else if (m_vecIcon[1]->Pt_InRect() &&
		g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
	{	
		g_pSoundManager->Play_Sound(L"UI_CommonHover.wav", CSoundManager::Total_Inven_Pet, CSoundManager::Effect_Sound);
		m_bIsActive = false;
		// 스테이터스 창 비활성화
		CUI_Manager::Get_Instance()->Get_StatusUI()->Set_Active(false);
		// 펫 인벤토리 활성화
		CUI_Manager::Get_Instance()->Get_Pet_Inven()->Set_Active(true);
	}
	else if (m_vecIcon[2]->Pt_InRect() &&
		g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
	{
		g_pSoundManager->Play_Sound(L"UI_CommonHover.wav", CSoundManager::Total_Inven_Exit, CSoundManager::Effect_Sound);
		m_bIsActive = false;
		// 스테이터스 창 비활성화
		CUI_Manager::Get_Instance()->Get_StatusUI()->Set_Active(false);
	}
	
	vector<CExpendables_Slot*> vecQuickSlot = * CUI_Manager::Get_Instance()->Get_Expendables_Inven()->Get_QuickSlot();
	
	for (_uint i = 0; i < 8; ++i)
	{
		if (i < vecQuickSlot.size())
		{
			m_pQuickSlotInfo[i]->Set_SlotInfo(CExpendables::EXPEND_TYPE(vecQuickSlot[i]->Get_Type()));
			m_pQuickSlotInfo[i]->Set_Number(vecQuickSlot[i]->Get_Size());	
		}			
		else if (i < 8)
		{
			m_pQuickSlotInfo[i]->Set_SlotInfo(CExpendables::EXPEND_END);
			m_pQuickSlotInfo[i]->Set_Number(0);
		}
			
	}

	// 스킬 설정
	for(_uint i = 0; i < m_vecSkillIcon.size(); ++i)
	{
		if (m_vecSkillIcon[i]->Pt_InRect())
		{
			m_pNoticeUI->Set_UI_Index(i + 1);
			

			if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
			{
				SkillSlot_Touch_Sound(i);
				m_bIsActive = false;

				CUI_Manager::Get_Instance()->Get_Skill_Inven()->Set_SkillUI_TotalInven(i);
				CUI_Manager::Get_Instance()->Get_Skill_Inven()->Set_Active(true);
			}

		}
	}

	LOOP(2)
	{
		if (m_pWeapon_Slot[i]->Pt_InRect() && g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
		{
			CWeapon_Inven* pWeapon_Inven = CUI_Manager::Get_Instance()->Get_Weapon_Inven();
			pWeapon_Inven->Set_RegistIndex(i);
			m_bIsActive = false;
			pWeapon_Inven->Set_Active(true);
		}
	}

	if (m_pArmor_Slot->Pt_InRect() && g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
	{
		CArmor_Inven* pArmor_Inven = CUI_Manager::Get_Instance()->Get_Armor_Inven();
		pArmor_Inven->Set_Active(true);
		m_bIsActive = false;
	}
}

void CTotal_Inven::SkillSlot_Touch_Sound(_uint i)
{
	if (i > 7)
		return;

	_uint iChannel = CSoundManager::CHANNELID::Total_Skill_Slot01 + i;

	g_pSoundManager->Play_Sound(L"UI_CommonHover.wav", CSoundManager::CHANNELID(iChannel), CSoundManager::Effect_Sound);
}

void CTotal_Inven::SetUp_SubUI_Active(_bool bIsActive)
{
	// 퀵슬롯 아이콘 활성화
	LOOP(8)
	{
		m_pQuickSlotInfo[i]->Set_Active(bIsActive);
	}

	// 스킬 아이콘 활성화
	for (auto& iter : m_vecSkillIcon)
	{
		iter->Set_Active(bIsActive);
	}

	m_pNoticeUI->Set_Active(bIsActive);
	// 블러드코드 아이콘
	m_pBloodCode->Set_Active(bIsActive);
	m_pBloodCode->Set_Type(m_ePlayerBloodCode);

	// 무기
	LOOP(2)
	{
		m_pWeapon_Slot[i]->Set_Active(bIsActive);
	}

	m_pArmor_Slot->Set_Active(bIsActive);

	// 인벤 아이콘 활성화
	for (auto& iter : m_vecIcon)
		iter->Set_Active(bIsActive);
}

void CTotal_Inven::Late_Init()
{
	if (m_bIsInit)
		return;

	m_bIsInit = true;

	m_pPlayer = static_cast<CPlayer*>(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));
}

CTotal_Inven * CTotal_Inven::Create(_Device pGraphic_Device)
{
	CTotal_Inven* pInstance = new CTotal_Inven(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CTotal_Inven::Clone_GameObject(void * pArg)
{
	CTotal_Inven* pInstance = new CTotal_Inven(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CTotal_Inven::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	CUI::Free();
}
