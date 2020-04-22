#include "stdafx.h"
#include "..\Headers\StageSelectUI.h"
#include "Player.h"
#include "StageUI.h"

CStageSelectUI::CStageSelectUI(_Device pDevice)
	: CUI(pDevice)
{
}

CStageSelectUI::CStageSelectUI(const CStageSelectUI & rhs)
	: CUI(rhs)
{
}

HRESULT CStageSelectUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CStageSelectUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	LOOP(5)
	{
		g_pManagement->Add_GameObject_ToLayer(L"GameObject_StageUI", SCENE_STAGE, L"Layer_StageUI");
		CStageUI* pStageUI = static_cast<CStageUI*>(g_pManagement->Get_GameObjectBack(L"Layer_StageUI", SCENE_STAGE));
		pStageUI->Set_Teleport_Menu(i);
		m_vecStageUI.push_back(pStageUI);
	}

	m_bIsActive = false;

	/*m_vecStageUI[0]->Set_Teleport_Menu(CStageUI::Teleport_Home_1);
	m_vecStageUI[1]->Set_Teleport_Menu(CStageUI::Teleport_St01_1);
	m_vecStageUI[2]->Set_Teleport_Menu(CStageUI::Teleport_St02_1);
	m_vecStageUI[3]->Set_Teleport_Menu(CStageUI::Teleport_St03_1);
	m_vecStageUI[4]->Set_Teleport_Menu(CStageUI::Teleport_St04_1);*/
	
	return NOERROR;
}

_int CStageSelectUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	if (m_bIsDead)
		return DEAD_OBJ;

	m_pTarget = static_cast<CPlayer*>(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));
	if (nullptr == m_pTarget)
		return NO_EVENT;

	LOOP(5)
		m_vecStageUI[i]->Set_Active(m_bIsActive);

	
	m_pTransformCom->Set_Angle(TARGET_TO_TRANS(m_pTarget)->Get_Angle());
	_v3 vLookZ = TARGET_TO_TRANS(m_pTarget)->Get_Axis(AXIS_Z);
	_v3 vLookX = TARGET_TO_TRANS(m_pTarget)->Get_Axis(AXIS_X);
	m_pTransformCom->Set_Pos(TARGET_TO_TRANS(m_pTarget)->Get_Pos() + (*V3_NORMAL_SELF(&vLookZ) + _v3(0.f, 2.f, 0.f) - *V3_NORMAL_SELF(&vLookX) * _float(m_iSelectIndex) * 1.8f));
	

	for (_uint i = 0; i < m_vecStageUI.size(); ++i)
	{
		TARGET_TO_TRANS(m_vecStageUI[i])->Set_Pos(m_pTransformCom->Get_Pos() + *V3_NORMAL_SELF(&vLookX) * 1.8f * _float(i));
	}
	
	
	/*if (g_pInput_Device->Key_Up(DIK_LEFT) && m_bIsActive)
	{
		if (0 != m_iSelectIndex)
		{
			m_iSelectIndex--;

			LOOP(_int(m_vecStageUI.size()))
				m_vecStageUI[i]->Set_Alpha(0.1f);
		}
	}
	else if (g_pInput_Device->Key_Up(DIK_RIGHT) && m_bIsActive)
	{
		if (CStageUI::Teleport_End - 1 != m_iSelectIndex)
		{
			m_iSelectIndex++;
			LOOP(_int(m_vecStageUI.size()))
				m_vecStageUI[i]->Set_Alpha(0.1f);
		}
			
	}*/

	for (_uint i = 0; i < m_vecStageUI.size(); ++i)
	{
		(i == m_iSelectIndex) ? m_vecStageUI[i]->Set_Select(true) : m_vecStageUI[i]->Set_Select(false);
	}
	for (_uint i = 0; i < m_vecStageUI.size(); ++i)
	{
		(i == m_iSelectIndex) ? 
			TARGET_TO_TRANS(m_vecStageUI[i])->Set_Scale(_v3(2.1567f, 1.3f, 0.f)) : TARGET_TO_TRANS(m_vecStageUI[i])->Set_Scale(_v3(1.72536f, 1.04f, 0.f));
	}
			
	//if (g_pInput_Device->Key_Up(DIK_P))
	//{
	//	m_bIsActive = !m_bIsActive;
	//	
	//	/*LOOP(5)
	//		m_vecStageUI[i]->Set_Active(!m_vecStageUI[i]->Get_Active());*/
	//}
	if (g_pInput_Device->Key_Up(DIK_J))
	{
		if (m_vecStageUI[m_iSelectIndex]->Get_MaxSubStage() > m_vecStageUI[m_iSelectIndex]->Get_SubStage())
			m_vecStageUI[m_iSelectIndex]->Set_SubStage(m_vecStageUI[m_iSelectIndex]->Get_SubStage() + 1);
		
	}
		
	if (g_pInput_Device->Key_Up(DIK_K))
	{
		if (m_vecStageUI[m_iSelectIndex]->Get_SubStage() > 0)
			m_vecStageUI[m_iSelectIndex]->Set_SubStage(m_vecStageUI[m_iSelectIndex]->Get_SubStage() - 1);
	}
		
	if (g_pInput_Device->Key_Up(DIK_RETURN))
		cout << Teleport_Stage() << endl;
	return NO_EVENT;
}

_int CStageSelectUI::Late_Update_GameObject(_double TimeDelta)
{
	return _int();
}

HRESULT CStageSelectUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	return NOERROR;
}

_uint CStageSelectUI::Select_Stage()
{
	if (!m_bIsActive)
		return _uint(CStageUI::Teleport_End);

	m_bIsActive = false;
	return _uint(m_vecStageUI[m_iSelectIndex]->Get_Teleport_Menu());
}

void CStageSelectUI::Move_Right()
{
	if (!m_bIsActive)
		return;
	//if (g_pInput_Device->Key_Up(DIK_RIGHT) && m_bIsActive)
	{
		if (CStageUI::Teleport_End - 1 != m_iSelectIndex)
		{
			m_iSelectIndex++;
			LOOP(_int(m_vecStageUI.size()))
				m_vecStageUI[i]->Set_Alpha(0.1f);
		}

	}
}

void CStageSelectUI::Move_Left()
{
	if (!m_bIsActive)
		return;
	//if (g_pInput_Device->Key_Up(DIK_LEFT))
	{
		if (0 != m_iSelectIndex)
		{
			m_iSelectIndex--;

			LOOP(_int(m_vecStageUI.size()))
				m_vecStageUI[i]->Set_Alpha(0.1f);
		}
	}
}

_uint CStageSelectUI::Teleport_Stage()
{
	_uint iTeleportMenu = m_vecStageUI[m_iSelectIndex]->Get_Teleport_Menu();
	_uint iSubStage = m_vecStageUI[m_iSelectIndex]->Get_SubStage();
	_uint iStageNumber = 0;
	switch (iTeleportMenu)
	{
	case CStageUI::Teleport_Home:
	{
		if (0 == iSubStage)
			iStageNumber = TeleportID_Home_1;
		else if(1 == iSubStage)
			iStageNumber = TeleportID_Home_2;
	}
		break;
	case CStageUI::Teleport_St01:
	{
		if(0 == iSubStage)
			iStageNumber = TeleportID_St01_1;
		else if(1 == iSubStage)
			iStageNumber = TeleportID_St01_2;
		else if(2 == iSubStage)
			iStageNumber = TeleportID_St01_3;
	}
		break;
	case CStageUI::Teleport_St02:
	{
		if(0 == iSubStage)
			iStageNumber = TeleportID_St02_1;
		else if(1 == iSubStage)
			iStageNumber = TeleportID_St02_2;
		else if(2 == iSubStage)
			iStageNumber = TeleportID_St02_3;
	}
		break;
	case CStageUI::Teleport_St03:
	{
		if(0 == iSubStage)
			iStageNumber = TeleportID_St03_1;
		else if(1 == iSubStage)
			iStageNumber = TeleportID_St03_2;
	}
		break;
	case CStageUI::Teleport_St04:
	{
		if(0 == iSubStage)
			iStageNumber = TeleportID_St04_1;
		else if(1 == iSubStage)
			iStageNumber = TeleportID_St04_2;
	}
		break;
	}
	return _uint(iStageNumber);
}

CStageSelectUI * CStageSelectUI::Create(_Device pGraphic_Device)
{
	CStageSelectUI* pInstance = new CStageSelectUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CStageSelectUI::Clone_GameObject(void * pArg)
{
	CStageSelectUI* pInstance = new CStageSelectUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CStageSelectUI::Free()
{
	Safe_Release(m_pTransformCom);

	CUI::Free();
}
