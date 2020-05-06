#include "stdafx.h"
#include "..\Headers\SkillUI.h"
#include "Active_Icon.h"
#include "PlayerFontUI.h"

CSkillUI::CSkillUI(_Device pDevice)
	: CUI(pDevice)
{
}

CSkillUI::CSkillUI(const CSkillUI & rhs)
	: CUI(rhs)
{
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
	CUI::Update_GameObject(TimeDelta);

	return NO_EVENT;
}

void CSkillUI::SetUp_Default()
{
	_float fPosX1 = WINCX * 0.9f, fPosY1 = WINCY * 0.75f;
	_float fPosX2 = WINCX * 0.83f, fPosY2 = WINCY * 0.89f;

	for (_uint i = 0; i < 8; ++i)
	{
		m_pActive[i] = static_cast<CActive_Icon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_ActiveIcon", nullptr));
		g_pManagement->Add_GameOject_ToLayer_NoClone(m_pActive[i], SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
		
		if (i < 4)
			m_pActive[i]->Set_UI_Size(64.f, 64.f);
		else
			m_pActive[i]->Set_UI_Size(54.f, 54.f);
	}

	m_pActive[0]->Set_UI_Pos(fPosX1, fPosY1 - 35.f);
	m_pActive[1]->Set_UI_Pos(fPosX1 - 35.f, fPosY1);
	m_pActive[2]->Set_UI_Pos(fPosX1 + 35.f, fPosY1);
	m_pActive[3]->Set_UI_Pos(fPosX1, fPosY1 + 35.f);
	m_pActive[4]->Set_UI_Pos(fPosX2, fPosY2 - 30.f);
	m_pActive[5]->Set_UI_Pos(fPosX2 - 30.f, fPosY2);
	m_pActive[6]->Set_UI_Pos(fPosX2 + 30.f, fPosY2);
	m_pActive[7]->Set_UI_Pos(fPosX2, fPosY2 + 30.f);

	/*m_pActive[0]->Set_Skill_Index(Skill_OneHand_Active_01);
	m_pActive[0]->Set_Active_State(CActive_Icon::ACTIVE_BUFF);*/
}

void CSkillUI::Set_SkillIcon(_uint iIndex, Skill_Index eSkill_Index)
{
	if (iIndex < 0 || iIndex >= 8)
		return;

	m_pActive[iIndex]->Set_Skill_Index(eSkill_Index);
}
//
//void CSkillUI::Set_SkillGaugeCur(_ulong iNum)
//{
//	m_pSkillGaugeCur->Set_Number(iNum);
//}
//
//void CSkillUI::Set_SkillGaugeMax(_ulong iNum)
//{
//	m_pSkillGaugeMax->Set_Number(iNum);
//}
//
//_ulong CSkillUI::Get_SkillGaugeCur()
//{
//	return m_pSkillGaugeCur->Get_Number();
//}
//
//_ulong CSkillUI::Get_SkillGaugeMax()
//{
//	return m_pSkillGaugeMax->Get_Number();
//}

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
