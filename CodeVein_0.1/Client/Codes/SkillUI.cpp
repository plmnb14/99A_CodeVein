#include "stdafx.h"
#include "..\Headers\SkillUI.h"
#include "Active_Icon.h"
#include "PlayerFontUI.h"
#include "UI_Manager.h"

CSkillUI::CSkillUI(_Device pDevice)
	: CUI(pDevice)
{
}

CSkillUI::CSkillUI(const CSkillUI & rhs)
	: CUI(rhs)
{
}

Skill_ID CSkillUI::Get_Skill_ID(_uint iIndex)
{
	if (iIndex >= 8)
		return SkillID_End;

	return m_pActive[iIndex]->Get_Skill_ID();
}

void CSkillUI::Set_Active_State(CActive_Icon::ACTIVE_STATE eState, _uint iIndex)
{
	if (iIndex < 0 || iIndex >= 8)
		return;

	m_pActive[iIndex]->Set_Active_State(eState);
}

HRESULT CSkillUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CSkillUI::Ready_GameObject(void * pArg)
{
	CUI::Ready_GameObject(pArg);

	SetUp_Default();

	return NOERROR;
}

_int CSkillUI::Update_GameObject(_double TimeDelta)
{
	if (g_eSceneID_Cur == SCENE_STAGE_BASE)
	{
		Set_Active(false);
		for (_int i = 0; i < 8; i++)
			m_pActive[i]->Set_Active(false);
		//m_pSkillGaugeCur->Set_Active(false);
		//m_pSkillGaugeMax->Set_Active(false);
		return S_OK;
	}
	else
	{
		Set_Active(true);
		for (_int i = 0; i < 8; i++)
			m_pActive[i]->Set_Active(true);
		//m_pSkillGaugeCur->Set_Active(true);
		//m_pSkillGaugeMax->Set_Active(true);
	}

	CUI::Update_GameObject(TimeDelta);

	
	for (_uint i = 0; i < 8; ++i)
	{
		Skill_ID eSkill_ID = CUI_Manager::Get_Instance()->Get_Total_Inven()->Get_Registration_Skill(i);
		m_pActive[i]->Set_Skill_ID(eSkill_ID);
	}
	return NO_EVENT;
}

void CSkillUI::SetUp_Default()
{
	//1.45078125‬f
	_float fPosX1 = 1179.36456f, fPosY1 = 552.8056f;
	_float fPosX2 = 1121.4647f, fPosY2 = 631.38395f;

	for (_uint i = 0; i < 8; ++i)
	{
		m_pActive[i] = static_cast<CActive_Icon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_ActiveIcon", nullptr));
		g_pManagement->Add_GameOject_ToLayer_NoClone(m_pActive[i], SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
		
		if (i < 4)
			m_pActive[i]->Set_UI_Size(64.f, 64.f);
		else
			m_pActive[i]->Set_UI_Size(54.f, 54.f);
	}

	// 가운데 위
	m_pActive[0]->Set_UI_Pos(fPosX1, fPosY1 - 35.f);
	// 왼쪽 중간
	m_pActive[1]->Set_UI_Pos(fPosX1 - 35.f, fPosY1);
	// 오른쪽 중간
	m_pActive[2]->Set_UI_Pos(fPosX1 + 35.f, fPosY1);
	// 가운데 아래
	m_pActive[3]->Set_UI_Pos(fPosX1, fPosY1 + 35.f);


	// 가운데 위
	m_pActive[4]->Set_UI_Pos(fPosX2, fPosY2 - 30.f);
	// 왼쪽 중간
	m_pActive[5]->Set_UI_Pos(fPosX2 - 30.f, fPosY2);
	// 오른쪽 중간
	m_pActive[6]->Set_UI_Pos(fPosX2 + 30.f, fPosY2);
	// 가운데 아래
	m_pActive[7]->Set_UI_Pos(fPosX2, fPosY2 + 30.f);

	
}


CSkillUI * CSkillUI::Create(_Device pGraphic_Device)
{
	CSkillUI* pInstance = new CSkillUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CSkillUI::Clone_GameObject(void * pArg)
{
	CSkillUI* pInstance = new CSkillUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CSkillUI::Free()
{
	CUI::Free();
}
