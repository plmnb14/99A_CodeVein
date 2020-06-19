#include "stdafx.h"
#include "..\Headers\Armor_Inven.h"
#include "UI_Manager.h"
#include "Player.h"

CArmor_Inven::CArmor_Inven(_Device pDevice)
	: CUI(pDevice)
{
}

CArmor_Inven::CArmor_Inven(const CArmor_Inven & rhs)
	: CUI(rhs)
{
}

ARMOR_PARAM CArmor_Inven::Get_UseArmorParam()
{
	return m_tRegistParam;
}

HRESULT CArmor_Inven::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CArmor_Inven::Ready_GameObject(void * pArg)
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

	SetUp_Default();

	return NOERROR;
}

_int CArmor_Inven::Update_GameObject(_double TimeDelta)
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

		CUI_Manager::Get_Instance()->Get_MouseUI()->Set_Active(false);
	}

	if (!m_bIsActive)
		return NO_EVENT;
	else
		CUI_Manager::Get_Instance()->Get_MouseUI()->Set_Active(true);
	
	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.0f);

	Click_Inven();
		
	return NO_EVENT;
}

_int CArmor_Inven::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CArmor_Inven::Render_GameObject()
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

HRESULT CArmor_Inven::Add_Component()
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

HRESULT CArmor_Inven::SetUp_ConstantTable()
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

void CArmor_Inven::SetUp_Default()
{
	m_pExplainUI = static_cast<CExplainArmorUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_ExplainArmorUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pExplainUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pExitIcon = static_cast<CInventory_Icon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_InvenIcon", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pExitIcon, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	m_pExitIcon->Set_UI_Pos(m_fPosX + 120.f, m_fPosY - 203.f);
	m_pExitIcon->Set_UI_Size(40.f, 40.f);
	m_pExitIcon->Set_Type(CInventory_Icon::ICON_EXIT);
}

void CArmor_Inven::Click_Inven()
{
	if (!m_bIsActive)
		return;

	for (auto& pSlot : m_vecArmorSlot)
	{
		if (pSlot->Pt_InRect())
		{
			if (m_pSoundSlot != pSlot)
				CUI_Manager::Get_Instance()->Stop_Play_UISound(L"UI_CommonHover.wav", CSoundManager::CHANNELID::UI_Click, CSoundManager::Effect_Sound);
			m_pSoundSlot = pSlot;

			ARMOR_PARAM tParam = pSlot->Get_ArmorParam();
			m_pExplainUI->Set_ArmorParam(tParam);

			if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
			{
				CTotal_Inven* pTotal_Inven = CUI_Manager::Get_Instance()->Get_Total_Inven();

				Reset_SelectSlot();
				pSlot->Set_Select(true);
				pTotal_Inven->Set_ArmorParam(tParam);

				_float fMaxHP = tParam.fHP + tParam.fPlusHP;
				m_pPlayer->Set_ArmorSlot((ARMOR_All_DATA)tParam.iArmorName, &tParam);

				CUI_Manager::Get_Instance()->Stop_Play_UISound(L"UI_CommonClick.wav", CSoundManager::CHANNELID::UI_Click, CSoundManager::Effect_Sound);
			}
		}
	}

	if (m_pExitIcon->Pt_InRect() && g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
	{
		m_bIsActive = false;
		CUI_Manager::Get_Instance()->Get_Total_Inven()->Set_Active(true);
	}
}

void CArmor_Inven::Regist_Armor(CArmor_Slot * pArmorSlot)
{
	/*if (pArmorSlot->Get_ArmorParam().iArmorType == ARMOR_End)
		return;*/
	if (m_tRegistParam.iArmorType == ARMOR_End)
	{
		m_tRegistParam = pArmorSlot->Get_ArmorParam();
		pArmorSlot->Set_Select(true);
	}
	else
		return;
}

void CArmor_Inven::UnRegist_Armor(CArmor_Slot * pArmorSlot)
{
	// 교체만 가능. 해제 불가
	return;

	if (pArmorSlot->Get_ArmorParam().iArmorType == ARMOR_End)
		return;
	if (pArmorSlot->Get_ArmorParam().iArmorType == m_tRegistParam.iArmorType)
	{
		pArmorSlot->Init_Param();
		pArmorSlot->Set_Select(false);
	}
}

void CArmor_Inven::Reset_SelectSlot()
{
	for (auto& iter : m_vecArmorSlot)
		iter->Set_Select(false);
}

void CArmor_Inven::SetUp_SlotPos()
{
	_uint iIdx = 0;
	for (auto& vector_iter : m_vecArmorSlot)
	{
		vector_iter->Set_UI_Pos(m_fPosX - 100.f + 52.f * (iIdx % 5), m_fPosY - 150.f + 52.f * (iIdx / 5));
		vector_iter->Set_ViewZ(m_fViewZ - 0.1f);
		iIdx++;
	}

}

void CArmor_Inven::SetUp_SubUI_Active(_bool bIsActive)
{
	for (auto& pSlot : m_vecArmorSlot)
	{
		pSlot->Set_Active(bIsActive);
	}
	m_pExplainUI->Set_Active(bIsActive);
	m_pExitIcon->Set_Active(bIsActive);
}

void CArmor_Inven::Late_Init()
{
	if (m_bLateInit)
		return;

	m_bLateInit = true;

	m_pPlayer = static_cast<CPlayer*>(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));

	m_vecArmorSlot.reserve(1);

	m_tRegistParam.iArmorType = ARMOR_Drape;
	m_tRegistParam.iArmorName = ArmorAll_Drape_DarkNightHook;
	m_tRegistParam.fHP = 1000.f;
	m_tRegistParam.fDef = 10.f;
	m_tRegistParam.fPlusDef = 10.f;
	m_tRegistParam.fPlusHP = 50.f;
	m_tRegistParam.iPrice = 100;
	m_tRegistParam.iReinforce = 0;

	Add_Armor(m_tRegistParam);
	m_vecArmorSlot[0]->Set_Select(true);
	_float fMaxHP = m_tRegistParam.fHP + m_tRegistParam.fPlusHP;
	m_pPlayer->Set_ArmorSlot((ARMOR_All_DATA)m_tRegistParam.iArmorName, &m_tRegistParam);

	CTotal_Inven* pTotal_Inven = CUI_Manager::Get_Instance()->Get_Total_Inven();
	pTotal_Inven->Set_ArmorParam(m_tRegistParam);

	/*m_tRegistParam.iArmorType = ARMOR_Drape;
	m_tRegistParam.iArmorName = ArmorAll_Drape_DarkNightHook;
	m_tRegistParam.fHP = 1000.f;
	m_tRegistParam.fDef = 50.f;
	m_tRegistParam.fPlusDef = 10.f;
	m_tRegistParam.fPlusHP = 50.f;
	m_tRegistParam.iPrice = 100;
	m_tRegistParam.iReinforce = 0;

	Add_Armor(m_tRegistParam);*/
}

void CArmor_Inven::Add_Armor(ARMOR_PARAM tArmorParam)
{
	CUI::UI_DESC* pDesc = nullptr;
	CArmor_Slot* pSlot = nullptr;


	pDesc = new CUI::UI_DESC;

	pDesc->fSizeX = 50.f;
	pDesc->fSizeY = 50.f;
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_ArmorSlot", SCENE_MORTAL, L"Layer_PlayerUI", pDesc);
	pSlot = static_cast<CArmor_Slot*>(g_pManagement->Get_GameObjectBack(L"Layer_PlayerUI", SCENE_MORTAL));
	pSlot->Set_ArmorParam(tArmorParam);
	m_vecArmorSlot.push_back(pSlot);

	// 슬롯 생성시 위치 조정
	for (_uint i = 0; i < m_vecArmorSlot.size(); ++i)
	{
		m_vecArmorSlot[i]->Set_Active(m_bIsActive);
		m_vecArmorSlot[i]->Set_ViewZ(m_fViewZ - 0.1f);
		m_vecArmorSlot[i]->Set_UI_Pos(m_fPosX - 103.f + 52.f * (i % 5), m_fPosY - 140.f + 52.f * (i / 5));
	}

	SetUp_SlotPos();
}

void CArmor_Inven::Sell_Armor()
{
	_ulong idx = 0;
	for (auto& pSlot : m_vecArmorSlot)
	{
		if (pSlot->Get_SelectShop())
		{
			pSlot->Set_Dead();
			m_vecArmorSlot.erase(m_vecArmorSlot.begin() + idx);
			m_vecArmorSlot.shrink_to_fit();
			break;
		}
		++idx;
	}

	SetUp_SlotPos();
}

CArmor_Inven * CArmor_Inven::Create(_Device pGraphic_Device)
{
	CArmor_Inven* pInstance = new CArmor_Inven(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CArmor_Inven::Clone_GameObject(void * pArg)
{
	CArmor_Inven* pInstance = new CArmor_Inven(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CArmor_Inven::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	CUI::Free();
}
