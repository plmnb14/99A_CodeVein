#include "stdafx.h"
#include "ScriptManager.h"
#include "Management.h"

// =================================
// 몬스터
// =================================
#include "GunGenji.h"
#include "SwordGenji.h"
#include "SwordShieldGenji.h"
#include "YachaMan.h"
#include "BlackWolf.h"
#include "PoisonButterfly.h"
// =================================

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
		cout << "아" << endl;
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
			m_vecObjectPool[3]->Set_Enable(true);
			m_vecObjectPool[4]->Set_Enable(true);

		}
	}
	}
}

void CScriptManager::St01_Sub1()
{
}

void CScriptManager::St01_Sub2()
{
}

void CScriptManager::St01_Sub3()
{
}

void CScriptManager::St01_Sub4()
{
}

void CScriptManager::St01_Sub5()
{
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
		break;
	}

	case 2:
	{
		break;
	}

	case 3:
	{
		break;
	}

	case 4:
	{
		break;
	}

	case 5:
	{
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

	_v3 vPos[15] = 
	{
		_v3(144.551f, -18.08f, 79.895f),
		_v3(145.498f, -18.08f, 84.775f),
		_v3(150.690f, -18.08f, 94.981f),
		_v3(117.045f, -18.08f, 111.482f), 
		_v3(95.754f, -17.15f, 106.058f),
		_v3(87.195f, -17.15f, 105.301f),					
		_v3(61.826f, -17.15f, 115.219f),
		_v3(65.298f, -17.15f, 125.649f),
		_v3(62.689f, -17.15f, 127.093f),
		_v3(71.845f, -17.15f, 132.392f),
		_v3(102.201f, -17.15f, 141.943f),
		_v3(85.174f, 0.1f, 154.160f),
		_v3(87.157f, 0.1f, 167.728f), 
		_v3(43.606f, 0.1f, 151.288f),
		_v3(-0.955f, 0.8f, -5.525f)
	};

	CGameObject* pInstance = nullptr;

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::White ,CSwordGenji::NF_Ani::Talk, 10.f, 5.f, 2.f));
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji", &CGunGenji::INFO(CGunGenji::Jungle, CGunGenji::NF_Ani::Sit1, 10.f, 5.f, 2.f));
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::Normal, CSwordGenji::NF_Ani::Talk, 10.f, 5.f, 2.f));
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::Normal, CSwordGenji::NF_Ani::Talk, 10.f, 5.f, 2.f));
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::White, CSwordGenji::NF_Ani::Talk, 10.f, 5.f, 2.f));
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::Jungle, CSwordGenji::NF_Ani::Talk, 10.f, 5.f, 2.f));
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::White, CSwordGenji::NF_Ani::Talk, 10.f, 5.f, 2.f));
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::Jungle, CSwordGenji::NF_Ani::Talk, 10.f, 5.f, 2.f));
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji", &CGunGenji::INFO(CGunGenji::Jungle, CGunGenji::NF_Ani::Sit1, 10.f, 5.f, 2.f));
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji", &CGunGenji::INFO(CGunGenji::Jungle, CGunGenji::NF_Ani::Sit1, 10.f, 5.f, 2.f));
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::Normal, CSwordGenji::Talk, 10.f, 5.f, 2.f));
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::White, CSwordGenji::Talk, 10.f, 5.f, 2.f));
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::Jungle, CSwordGenji::Talk, 10.f, 5.f, 2.f));
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::White, CSwordGenji::Talk, 10.f, 5.f, 2.f));
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_PoisonButterfly", &CPoisonButterfly::INFO(10.f, 5.f, 2.f));
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);

	_ulong dwIndex = 0;

	for (auto& iter : m_vecObjectPool)
	{
		iter->Set_Enable(false);
		TARGET_TO_TRANS(iter)->Set_Pos(vPos[dwIndex]);
		TARGET_TO_NAV(iter)->Ready_NaviMesh(m_pGraphicDev, L"Navmesh_Stage_01.dat");
		TARGET_TO_NAV(iter)->Check_OnNavMesh(&vPos[dwIndex]);

		g_pManagement->Add_GameOject_ToLayer_NoClone(iter, SCENE_STAGE, L"Layer_Monster", nullptr);

		++dwIndex;
	}

	//

	/*
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::White));
	TARGET_TO_TRANS(pInstance)->Set_Pos(vPos[0]);	// 위치
	TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphicDev, L"Navmesh_Stage_01.dat");
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(0);
	TARGET_TO_NAV(pInstance)->Set_Index(32);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji", &CGunGenji::INFO(CGunGenji::Jungle));
	TARGET_TO_TRANS(pInstance)->Set_Pos(vPos[1]);	// 위치
	TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphicDev, L"Navmesh_Stage_01.dat");
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(0);
	TARGET_TO_NAV(pInstance)->Set_Index(39);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::Normal));
	TARGET_TO_TRANS(pInstance)->Set_Pos(vPos[2]);	// 위치
	TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphicDev, L"Navmesh_Stage_01.dat");
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(0);
	TARGET_TO_NAV(pInstance)->Set_Index(52);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::Normal));
	TARGET_TO_TRANS(pInstance)->Set_Pos(vPos[3]);	// 위치
	TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphicDev, L"Navmesh_Stage_01.dat");
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(0);
	TARGET_TO_NAV(pInstance)->Set_Index(64);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::White));
	TARGET_TO_TRANS(pInstance)->Set_Pos(vPos[4]);	// 위치
	TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphicDev, L"Navmesh_Stage_01.dat");
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(1);
	TARGET_TO_NAV(pInstance)->Set_Index(55);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::Jungle));
	TARGET_TO_TRANS(pInstance)->Set_Pos(vPos[5]);	// 위치
	TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphicDev, L"Navmesh_Stage_01.dat");
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(1);
	TARGET_TO_NAV(pInstance)->Set_Index(63);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::White));
	TARGET_TO_TRANS(pInstance)->Set_Pos(vPos[6]);	// 위치
	TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphicDev, L"Navmesh_Stage_01.dat");
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(2);
	TARGET_TO_NAV(pInstance)->Set_Index(28);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::Jungle));
	TARGET_TO_TRANS(pInstance)->Set_Pos(vPos[7]);	// 위치
	TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphicDev, L"Navmesh_Stage_01.dat");
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(2);
	TARGET_TO_NAV(pInstance)->Set_Index(34);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji", &CGunGenji::INFO(CGunGenji::White));
	TARGET_TO_TRANS(pInstance)->Set_Pos(vPos[8]);	// 위치
	TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphicDev, L"Navmesh_Stage_01.dat");
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(2);
	TARGET_TO_NAV(pInstance)->Set_Index(138);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji", &CGunGenji::INFO(CGunGenji::White));
	TARGET_TO_TRANS(pInstance)->Set_Pos(vPos[9]);	// 위치
	TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphicDev, L"Navmesh_Stage_01.dat");
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(2);
	TARGET_TO_NAV(pInstance)->Set_Index(45);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::Normal));
	TARGET_TO_TRANS(pInstance)->Set_Pos(vPos[10]);	// 위치
	TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphicDev, L"Navmesh_Stage_01.dat");
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(2);
	TARGET_TO_NAV(pInstance)->Set_Index(72);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);


	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::White));
	TARGET_TO_TRANS(pInstance)->Set_Pos(vPos[11]);	// 위치
	TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphicDev, L"Navmesh_Stage_01.dat");
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(3);
	TARGET_TO_NAV(pInstance)->Set_Index(25);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::Jungle));
	TARGET_TO_TRANS(pInstance)->Set_Pos(vPos[12]);	// 위치
	TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphicDev, L"Navmesh_Stage_01.dat");
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(3);
	TARGET_TO_NAV(pInstance)->Set_Index(36);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::White));
	TARGET_TO_TRANS(pInstance)->Set_Pos(vPos[13]);	// 위치
	TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphicDev, L"Navmesh_Stage_01.dat");
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(3);
	TARGET_TO_NAV(pInstance)->Set_Index(126);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_PoisonButterfly", nullptr);
	TARGET_TO_TRANS(pInstance)->Set_Pos(vPos[14]);	// 위치
	TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphicDev, L"Navmesh_Stage_01.dat");
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(6);
	TARGET_TO_NAV(pInstance)->Set_Index(2);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	*/
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
