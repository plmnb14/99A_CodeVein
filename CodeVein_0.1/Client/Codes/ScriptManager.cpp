#include "stdafx.h"
#include "ScriptManager.h"
#include "Management.h"

// =================================
// ����
// =================================
#include "GunGenji.h"
#include "SwordGenji.h"
#include "SwordShieldGenji.h"
#include "YachaMan.h"
#include "BlackWolf.h"
#include "PoisonButterfly.h"
#include "BlackWolf.h"
#include "YachaMan.h"
// =================================
#include "UI_Manager.h"
#include "MassageUI.h"

IMPLEMENT_SINGLETON(CScriptManager)


CScriptManager::CScriptManager()
{
}

void CScriptManager::Update_ScriptMgr(_double _dDeltaTime, _ulong _dwSubsetIdx, _ulong _dwCellIdx)
{
	m_fDeltaTime = (_float)_dDeltaTime;
	m_dwSubsetIdx = _dwSubsetIdx;
	m_dwCellIdx = _dwCellIdx;

	Update_ScriptEvent();

	if (m_bTimer)
	{
		if (m_fTimer < 2.5f)
		{
			m_fTimer += DELTA_60;

			if (m_fTimer >= 2.5f)
			{
				m_fTimer = 0.f;
				m_fTimer = false;

				CUI_Manager::Get_Instance()->Set_UI_Active(L"Layer_BossHP", true);
			}
		}
	}
}

void CScriptManager::Update_ScriptEvent()
{
	switch (m_eStageIdx)
	{
	case Script_Base:
	{
		Stage_Base_Scripts();
		break;
	}

	case Script_Stage01:
	{
		Stage_01_Scripts();
		break;
	}

	case Script_Stage02:
	{
		Stage_02_Scripts();
		break;
	}

	case Script_Stage03:
	{
		Stage_03_Scripts();
		break;
	}

	case Script_Stage_Training:
	{
		break;

	}

	case Script_Stage_End:
	{
		break;

	}

	}
}

void CScriptManager::Reset_ScriptEvent()
{
}

void CScriptManager::Reset_Script_DynmicObject()
{
	for (auto& iter : m_vecObjectPool)
	{
		Safe_Release(iter);
	}

	m_vecObjectPool.shrink_to_fit();
	m_vecObjectPool.clear();
}

void CScriptManager::Ready_ScriptManager(_Device _pGraphicDev)
{
	m_pGraphicDev = _pGraphicDev;
	Safe_AddRef(m_pGraphicDev);
}

void CScriptManager::Ready_Script_DynamicObject(_ulong _dwStageIdx)
{
	switch (m_eStageIdx)
	{
	case Script_Base:
	{
		Ready_Base_DynamicObject();
		break;
	}
	case Script_Stage01:
	{
		Ready_Stage01_DynamicObject();
		break;
	}

	case Script_Stage02:
	{
		Ready_Stage02_DynamicObject();
		break;
	}

	case Script_Stage03:
	{
		Ready_Stage03_DynamicObject();
		break;
	}

	case Script_Stage_Training:
	{
		break;

	}

	case Script_Stage_End:
	{
		break;

	}

	}
}

void CScriptManager::St01_Sub0()
{
	switch (m_dwCellIdx)
	{
	case 5 :
	{
		if (m_bEventTrigger_0[0] == false)
		{
			m_bEventTrigger_0[0] = true;

			m_vecObjectPool[0]->Set_Enable(true);
			m_vecObjectPool[1]->Set_Enable(true);
			m_vecObjectPool[2]->Set_Enable(true);
		}
	}

	case 36:
	{
		if (m_bEventTrigger_0[1] == false)
		{
			m_bEventTrigger_0[1] = true;

			m_vecObjectPool[3]->Set_Enable(true);
			m_vecObjectPool[4]->Set_Enable(true);
			m_vecObjectPool[5]->Set_Enable(true);
			m_vecObjectPool[6]->Set_Enable(true);
			m_vecObjectPool[7]->Set_Enable(true);
		}
	}
	}
}

void CScriptManager::St01_Sub1()
{
	switch (m_dwCellIdx)
	{
	case 41:
	{
		if (m_bEventTrigger_0[2] == false)
		{
			m_bEventTrigger_0[2] = true;

			m_vecObjectPool[8]->Set_Enable(true);
			m_vecObjectPool[9]->Set_Enable(true);
		}
	}
	}
}

void CScriptManager::St01_Sub2()
{
	switch (m_dwCellIdx)
	{
	case 18:
	{
		if (m_bEventTrigger_0[3] == false)
		{
			m_bEventTrigger_0[3] = true;

			m_vecObjectPool[10]->Set_Enable(true);
		}
	}

	case 0:
	{
		if (m_bEventTrigger_0[4] == false)
		{
			m_bEventTrigger_0[4] = true;

			m_vecObjectPool[11]->Set_Enable(true);
			m_vecObjectPool[12]->Set_Enable(true);
			m_vecObjectPool[13]->Set_Enable(true);
			m_vecObjectPool[14]->Set_Enable(true);
			m_vecObjectPool[15]->Set_Enable(true);
		}
	}

	case 93:
	{
		if (m_bEventTrigger_0[5] == false)
		{
			m_bEventTrigger_0[5] = true;

			m_vecObjectPool[16]->Set_Enable(true);
			m_vecObjectPool[17]->Set_Enable(true);
			m_vecObjectPool[18]->Set_Enable(true);
			m_vecObjectPool[19]->Set_Enable(true);
		}
	}


	case 104:
	{
		if (m_bEventTrigger_0[6] == false)
		{
			m_bEventTrigger_0[6] = true;

			m_vecObjectPool[20]->Set_Enable(true);
			m_vecObjectPool[21]->Set_Enable(true);
		}
	}
	}
}

void CScriptManager::St01_Sub3()
{
	switch (m_dwCellIdx)
	{
	case 126:
	{
		if (m_bEventTrigger_0[7] == false)
		{
			m_bEventTrigger_0[7] = true;

			m_vecObjectPool[22]->Set_Enable(true);
			m_vecObjectPool[23]->Set_Enable(true);
			m_vecObjectPool[24]->Set_Enable(true);
			m_vecObjectPool[25]->Set_Enable(true);
			m_vecObjectPool[26]->Set_Enable(true);
		}
	}

	case 66:
	{
		if (m_bEventTrigger_0[8] == false)
		{
			m_bEventTrigger_0[8] = true;

			m_vecObjectPool[27]->Set_Enable(true);
		}
	}
	}
}

void CScriptManager::St01_Sub4()
{
	switch (m_dwCellIdx)
	{
	case 52:
	{
		if (m_bEventTrigger_0[9] == false)
		{
			m_bEventTrigger_0[9] = true;

			m_vecObjectPool[28]->Set_Enable(true);
			//m_vecObjectPool[29]->Set_Enable(true);

			// ���� HP�� ����
			//g_pManagement->Add_GameObject_ToLayer(L"GameObject_BossHP", SCENE_STAGE, L"Layer_BossHP");
		}
	}
	}
}

void CScriptManager::St01_Sub5()
{
}

void CScriptManager::St01_Sub6()
{
	switch (m_dwCellIdx)
	{
	case 0:
	{
		if (m_bEventTrigger_0[11] == false)
		{
			m_bEventTrigger_0[11] = true;
			m_bTimer = true;

			CMassageUI* pMassageUI = static_cast<CMassageUI*>(g_pManagement->Get_GameObjectBack(L"Layer_BossMassageUI", SCENE_STAGE));
			pMassageUI->Set_Check_Play_BossnameUI(true);
		}

		break;
	}
	}
}

void CScriptManager::Stage_Base_Scripts()
{
}

void CScriptManager::Stage_01_Scripts()
{
	switch (m_dwSubsetIdx)
	{
	case 0:
	{
		St01_Sub0();
		break;
	}

	case 1:
	{
		St01_Sub1();
		break;
	}

	case 2:
	{
		St01_Sub2();
		break;
	}

	case 3:
	{
		St01_Sub3();
		break;
	}

	case 4:
	{
		St01_Sub4();
		break;
	}

	case 5:
	{
		St01_Sub5();
		break;
	}

	case 6:
	{
		St01_Sub6();
		break;
	}

	}
}

void CScriptManager::Stage_02_Scripts()
{
}

void CScriptManager::Stage_03_Scripts()
{
}

void CScriptManager::Ready_Base_DynamicObject()
{
}

void CScriptManager::Ready_Stage01_DynamicObject()
{
	m_vecObjectPool.reserve(15);

	_v3 vPos[30] =
	{
		// �ʹ� 3��
		_v3(144.551f, -18.08f, 79.895f),
		_v3(145.498f, -18.08f, 84.775f),
		_v3(150.690f, -18.08f, 94.981f),
		// =========================================

		// â, ��, â, ��, ��
		_v3(135.253f, -17.667f, 110.645f),
		_v3(132.46f, -17.7f, 118.243f),
		_v3(106.398f, -17.150f, 114.131f),
		_v3(104.769f, -17.150f, 121.635f),
		_v3(105.458f, -17.150f, 108.336f),
		// =========================================

		// ���� 2��
		_v3(95.754f, -17.15f, 106.058f),
		_v3(87.195f, -17.15f, 105.301f),
		// =========================================

		// ��, ��
		_v3(72.357f, -17.15f, 141.364f),
		// =========================================

		// ���� 5����
		_v3(61.826f, -17.15f, 115.219f),
		_v3(65.298f, -17.15f, 125.649f),
		_v3(62.689f, -17.15f, 127.093f),
		_v3(71.845f, -17.15f, 132.392f),
		_v3(102.201f, -17.15f, 141.943f),
		// =========================================

		// ����� 4����
		_v3(64.588f, -0.1f, 151.872f),
		_v3(43.606f, -0.1f, 151.288f),
		_v3(41.517f, -0.1f, 147.294f),
		_v3(54.421f, -0.1f, 143.731f),
		// =========================================

		// ���� ���� ��
		_v3(85.174f, 0.1f, 154.160f),
		_v3(87.157f, 0.1f, 167.728f),
		// =========================================

		//  ��, ��, â, ��, ��
		_v3(9.853f, 0.1f, 122.874f),
		_v3(22.171f, 0.1f, 127.027f),
		_v3(6.095f, 0.1f, 109.448f),
		_v3(15.527f, 0.1f, 102.195f),
		_v3(-4.185f, 0.1f, 60.353f),
		// =========================================

		// �� ����
		_v3(43.606f, 0.1f, 151.288f),
		// =========================================

		// ����
		_v3(-0.955f, 0.8f, -5.525f)
		// =========================================
	};

	CGameObject* pInstance = nullptr;

	//=======================================================================
	// 4��

	{
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::White, CSwordGenji::NF_Ani::Talk, 10.f, 5.f, 2.f));
		TARGET_TO_NAV(pInstance)->Set_SubsetIndex(0);
		TARGET_TO_NAV(pInstance)->Set_Index(32);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);

		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji", &CGunGenji::INFO(CGunGenji::Jungle, CGunGenji::NF_Ani::Sit1, 10.f, 5.f, 2.f));
		TARGET_TO_NAV(pInstance)->Set_SubsetIndex(0);
		TARGET_TO_NAV(pInstance)->Set_Index(39);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);

		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::Normal, CSwordGenji::NF_Ani::Talk, 10.f, 5.f, 2.f));
		TARGET_TO_NAV(pInstance)->Set_SubsetIndex(0);
		TARGET_TO_NAV(pInstance)->Set_Index(52);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
	}

	//=======================================================================

	{
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordShieldGenji", &CSwordGenji::INFO(CSwordGenji::Normal, CSwordGenji::NF_Ani::LookAround1, 10.f, 5.f, 2.f));
		TARGET_TO_NAV(pInstance)->Set_SubsetIndex(0);
		TARGET_TO_NAV(pInstance)->Set_Index(81);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);

		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji", &CSwordGenji::INFO(CSwordGenji::White, CSwordGenji::NF_Ani::Talk, 10.f, 5.f, 2.f));
		TARGET_TO_NAV(pInstance)->Set_SubsetIndex(0);
		TARGET_TO_NAV(pInstance)->Set_Index(74);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);

		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordShieldGenji", &CGunGenji::INFO(CGunGenji::Jungle, CGunGenji::NF_Ani::Sit1, 10.f, 5.f, 2.f));
		TARGET_TO_NAV(pInstance)->Set_SubsetIndex(1);
		TARGET_TO_NAV(pInstance)->Set_Index(29);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);

		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji", &CSwordGenji::INFO(CSwordGenji::Normal, CSwordGenji::NF_Ani::LookAround2, 10.f, 5.f, 2.f));
		TARGET_TO_NAV(pInstance)->Set_SubsetIndex(1);
		TARGET_TO_NAV(pInstance)->Set_Index(39);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);

		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji", &CSwordGenji::INFO(CSwordGenji::Normal, CSwordGenji::NF_Ani::Talk, 10.f, 5.f, 2.f));
		TARGET_TO_NAV(pInstance)->Set_SubsetIndex(1);
		TARGET_TO_NAV(pInstance)->Set_Index(43);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
	}

	//=======================================================================

	{
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::White, CSwordGenji::NF_Ani::Talk, 10.f, 5.f, 2.f));
		TARGET_TO_NAV(pInstance)->Set_SubsetIndex(1);
		TARGET_TO_NAV(pInstance)->Set_Index(55);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);

		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::Jungle, CSwordGenji::NF_Ani::Sit1, 10.f, 5.f, 2.f));
		TARGET_TO_NAV(pInstance)->Set_SubsetIndex(1);
		TARGET_TO_NAV(pInstance)->Set_Index(63);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
	}

	//=======================================================================
	{
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji", &CSwordGenji::INFO(CSwordGenji::White, CSwordGenji::NF_Ani::Talk, 10.f, 5.f, 2.f));
		TARGET_TO_NAV(pInstance)->Set_SubsetIndex(2);
		TARGET_TO_NAV(pInstance)->Set_Index(42);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
	}

	//=======================================================================

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::White, CSwordGenji::NF_Ani::Talk, 10.f, 5.f, 2.f));
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(2);
	TARGET_TO_NAV(pInstance)->Set_Index(28);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::Jungle, CSwordGenji::NF_Ani::Talk, 10.f, 5.f, 2.f));
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(2);
	TARGET_TO_NAV(pInstance)->Set_Index(34);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji", &CGunGenji::INFO(CGunGenji::Jungle, CGunGenji::NF_Ani::Sit1, 10.f, 5.f, 2.f));
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(2);
	TARGET_TO_NAV(pInstance)->Set_Index(138);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji", &CGunGenji::INFO(CGunGenji::Jungle, CGunGenji::NF_Ani::LookAround3, 10.f, 5.f, 2.f));
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(2);
	TARGET_TO_NAV(pInstance)->Set_Index(45);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::Normal, CSwordGenji::Talk, 10.f, 5.f, 2.f));
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(2);
	TARGET_TO_NAV(pInstance)->Set_Index(72);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);

	//=======================================================================

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_BlackWolf", NULL);
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(3);
	TARGET_TO_NAV(pInstance)->Set_Index(84);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_BlackWolf", NULL);
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(3);
	TARGET_TO_NAV(pInstance)->Set_Index(126);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_BlackWolf", NULL);
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(3);
	TARGET_TO_NAV(pInstance)->Set_Index(129);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_BlackWolf", NULL);
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(3);
	TARGET_TO_NAV(pInstance)->Set_Index(120);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);

	//=======================================================================

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::White, CSwordGenji::Talk, 10.f, 5.f, 2.f));
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(3);
	TARGET_TO_NAV(pInstance)->Set_Index(25);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::Jungle, CSwordGenji::Talk, 10.f, 5.f, 2.f));
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(3);
	TARGET_TO_NAV(pInstance)->Set_Index(36);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);

	//=======================================================================

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::Normal, CSwordGenji::Sit1, 10.f, 5.f, 2.f));
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(4);
	TARGET_TO_NAV(pInstance)->Set_Index(23);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::Jungle, CSwordGenji::Sit2, 10.f, 5.f, 2.f));
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(4);
	TARGET_TO_NAV(pInstance)->Set_Index(19);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::White, CSwordGenji::Sit1, 10.f, 5.f, 2.f));
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(4);
	TARGET_TO_NAV(pInstance)->Set_Index(41);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::Jungle, CSwordGenji::Sit2, 10.f, 5.f, 2.f));
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(4);
	TARGET_TO_NAV(pInstance)->Set_Index(48);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_YachaMan", NULL);
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(5);
	TARGET_TO_NAV(pInstance)->Set_Index(31);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);

	//=======================================================================

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::White, CSwordGenji::Talk, 10.f, 5.f, 2.f));
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(3);
	TARGET_TO_NAV(pInstance)->Set_Index(126);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);

	//=======================================================================

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_PoisonButterfly", &CPoisonButterfly::INFO(45.f, 5.f, 2.f));
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(6);
	TARGET_TO_NAV(pInstance)->Set_Index(2);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);

	//=======================================================================

	_ulong dwIndex = 0;

	for (auto& iter : m_vecObjectPool)
	{
		iter->Set_Enable(false);
		TARGET_TO_TRANS(iter)->Set_Pos(vPos[dwIndex]);
		TARGET_TO_NAV(iter)->Ready_NaviMesh(m_pGraphicDev, L"Navmesh_Stage_01.dat");
		//TARGET_TO_NAV(iter)->Check_OnNavMesh(&vPos[dwIndex]);

		g_pManagement->Add_GameOject_ToLayer_NoClone(iter, SCENE_STAGE, L"Layer_Monster", nullptr);

		++dwIndex;
	}

}

void CScriptManager::Ready_Stage02_DynamicObject()
{
}

void CScriptManager::Ready_Stage03_DynamicObject()
{
}

void CScriptManager::Free()
{
	for (auto& iter : m_vecObjectPool)
	{
		Safe_Release(iter);
	}

	Safe_Release(m_pGraphicDev);
}
