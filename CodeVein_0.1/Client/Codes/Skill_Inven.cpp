#include "stdafx.h"
#include "..\Headers\Skill_Inven.h"
#include "UI_Manager.h"

CSkill_Inven::CSkill_Inven(_Device pDevice)
	: CUI(pDevice)
{
}

CSkill_Inven::CSkill_Inven(const CSkill_Inven & rhs)
	: CUI(rhs)
{
}

void CSkill_Inven::Set_SkillUI_TotalInven(_uint iIndex)
{
	m_iRegistIdx = iIndex;
}

HRESULT CSkill_Inven::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CSkill_Inven::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	m_fPosX = 229.5f;
	m_fPosY = 325.5f;

	m_fSizeX = 280.f;
	m_fSizeY = 471.f;
	m_fViewZ = 4.f;

	SetUp_Default();

	
	/*LOOP(12)
	{
		Add_Skill_Data(Skill_ID(i));
	}*/
	Add_Skill_Data(Dragon_Lunge);

	SetUp_SlotPos();

	return NOERROR;
}

_int CSkill_Inven::Update_GameObject(_double TimeDelta)
{
	if (m_bIsActive && !m_bIsSubActive)
	{
		SetUp_SubUI_Active(true);
		Search_Regist_Skill();
		m_bIsSubActive = true;
	}
	else if (!m_bIsActive && m_bIsSubActive)
	{
		SetUp_SubUI_Active(false);

		for (auto& iter : m_vecSlot)
		{
			iter->Set_Regist(false);
			iter->Set_Select(false);
		}

		m_pExplainUI->Set_Type(SkillID_End);

		m_bIsSubActive = false;
	}

	if (!m_bIsActive)
		return NO_EVENT;
	CUI::Update_GameObject(TimeDelta);
	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.0f);

	Click_SubUI();
	//Search_Regist_Skill();

	

	/*if (!m_bIsActive)
	{
		for (auto& iter : m_vecSlot)
		{
			iter->Set_Regist(false);
			iter->Set_Select(false);
		}

		m_pExplainUI->Set_Type(SkillID_End);
	}*/

	return NO_EVENT;
}

_int CSkill_Inven::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CSkill_Inven::Render_GameObject()
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

HRESULT CSkill_Inven::Add_Component()
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

HRESULT CSkill_Inven::SetUp_ConstantTable()
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

void CSkill_Inven::SetUp_Default()
{
	m_pExitIcon = static_cast<CInventory_Icon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_InvenIcon", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pExitIcon, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	m_pExitIcon->Set_UI_Pos(m_fPosX + 120.f, m_fPosY - 203.f);
	m_pExitIcon->Set_UI_Size(40.f, 40.f);
	m_pExitIcon->Set_Type(CInventory_Icon::ICON_EXIT);

	m_pExplainUI = static_cast<CExplainSkillUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_ExplainSkillUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pExplainUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	m_pExplainUI->Set_UI_Pos(WINCX * 0.5f + 10.f, WINCY * 0.5f - 20.f);
	m_pExplainUI->Set_UI_Size(700.f, 700.f);
}

void CSkill_Inven::Click_SubUI()
{
	if (!m_bIsActive)
		return;
	// 나가기 버튼 클릭시
	if (m_pExitIcon->Pt_InRect() && g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
	{
		m_bIsActive = false;
		CUI_Manager::Get_Instance()->Get_Total_Inven()->Set_Active(true);
		CUI_Manager::Get_Instance()->Stop_Play_UISound(L"UI_Open.ogg", CSoundManager::CHANNELID::UI_Open_Close, CSoundManager::Effect_Sound);
	}

	// 스킬 슬롯 선택시
	for (_uint i = 0; i < m_vecSlot.size(); ++i)
	{
		if (m_vecSlot[i]->Pt_InRect() && /*g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB) && */(!m_vecSlot[i]->Get_Regist()))
		{
			if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
			{
				CUI_Manager::Get_Instance()->Stop_Play_UISound(L"Slot_Regist.ogg", CSoundManager::CHANNELID::UI_Click, CSoundManager::Effect_Sound);
				Reset_Select_Slot();
				m_vecSlot[i]->Set_Select(true);
				CUI_Manager::Get_Instance()->Get_Total_Inven()->Set_Skill_ID(m_iRegistIdx, m_vecSlot[i]->Get_SkillID());
			}
			else if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_RB))
			{
				CUI_Manager::Get_Instance()->Stop_Play_UISound(L"Slot_Regist.ogg", CSoundManager::CHANNELID::UI_Click, CSoundManager::Effect_Sound);
				CUI_Manager::Get_Instance()->Get_Total_Inven()->Set_Skill_ID(m_iRegistIdx, SkillID_End);
				m_vecSlot[i]->Set_Select(false);
			}
		}
	}

	for (auto& iter : m_vecSlot)
	{
		if (iter->Pt_InRect())
		{
			m_pExplainUI->Set_Type(iter->Get_SkillID());
		}
	}
}

void CSkill_Inven::Search_Regist_Skill()
{
	LOOP(8)
	{
		/*if (i == m_iRegistIdx)
			continue;*/

		Skill_ID eSkill_ID = CUI_Manager::Get_Instance()->Get_Total_Inven()->Get_Registration_Skill(i);

		for (auto& iter : m_vecSlot)
		{
			if (iter->Get_SkillID() == eSkill_ID)
			{					
				if (i == m_iRegistIdx)
					iter->Set_Select(true);
				else
					iter->Set_Regist(true);		
			}
		}
	}
	
}

void CSkill_Inven::Reset_Select_Slot()
{
	for (auto& iter : m_vecSlot)
		iter->Set_Select(false);
}


void CSkill_Inven::SetUp_SlotPos()
{
	_uint iIdx = 0;
	for (auto& iter : m_vecSlot)
	{
		iter->Set_UI_Pos(m_fPosX - 100.f + 50.f * (iIdx % 5), m_fPosY - 150.f + 50.f * (iIdx / 5));
		iIdx++;
	}
}

void CSkill_Inven::SetUp_SubUI_Active(_bool bIsActive)
{
	_uint iIdx = 0;
	for (auto& iter : m_vecSlot)
	{

		iter->Set_Active(bIsActive);
		iIdx++;
	}
	m_pExitIcon->Set_Active(bIsActive);
	m_pExplainUI->Set_Active(bIsActive);
}

void CSkill_Inven::Add_Skill_Data(Skill_ID eSkillID)
{
	CSkillSlot* pSlot = static_cast<CSkillSlot*>(g_pManagement->Clone_GameObject_Return(L"GameObject_SkillSlot", nullptr));
	pSlot->Set_SkillID(eSkillID);
	pSlot->Set_UI_Size(50.f, 50.f);
	pSlot->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pSlot, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	m_vecSlot.push_back(pSlot);
	m_vecData.push_back(eSkillID);

	SetUp_SlotPos();
}

CSkill_Inven * CSkill_Inven::Create(_Device pGraphic_Device)
{
	CSkill_Inven* pInstance = new CSkill_Inven(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CSkill_Inven::Clone_GameObject(void * pArg)
{
	CSkill_Inven* pInstance = new CSkill_Inven(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CSkill_Inven::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	CUI::Free();
}
