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
#include "YachaMan.h"
#include "Hunter.h"
#include "Monkey.h"
// =================================
#include "QueensKnight.h"
#include "PoisonButterfly.h"
#include "IceGirl.h"
#include "FireBoy.h"
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

	case Script_Stage04:
	{
		Stage_04_Scripts();
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

void CScriptManager::Reset_ScriptEvent(_ushort _sScriptStageIdx, _bool _bResetAll)
{
	if (_bResetAll)
	{
		for (_uint i = 0; i < Script_Stage_End; ++i)
		{
			for (_uint j = 0; j < MaxTriggerCnt; ++j)
			{
				m_bEventTrigger[i][j] = false;
			}
		}
	}
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

void CScriptManager::Reset_ObjectPoolVector()
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

	case Script_Stage04:
	{
		Ready_Stage04_DynamicObject();
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
		if (m_bEventTrigger[Script_Stage01][0] == false)
		{
			m_bEventTrigger[Script_Stage01][0] = true;

			m_vecObjectPool[0]->Set_Enable(true);
			m_vecObjectPool[1]->Set_Enable(true);
			m_vecObjectPool[2]->Set_Enable(true);
		}
	}

	case 36:
	{
		if (m_bEventTrigger[Script_Stage01][1] == false)
		{
			m_bEventTrigger[Script_Stage01][1] = true;

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
		if (m_bEventTrigger[Script_Stage01][2] == false)
		{
			m_bEventTrigger[Script_Stage01][2] = true;

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
		if (m_bEventTrigger[Script_Stage01][3] == false)
		{
			m_bEventTrigger[Script_Stage01][3] = true;

			m_vecObjectPool[10]->Set_Enable(true);
		}
	}

	case 0:
	{
		if (m_bEventTrigger[Script_Stage01][4] == false)
		{
			m_bEventTrigger[Script_Stage01][4] = true;

			m_vecObjectPool[11]->Set_Enable(true);
			m_vecObjectPool[12]->Set_Enable(true);
			m_vecObjectPool[13]->Set_Enable(true);
			m_vecObjectPool[14]->Set_Enable(true);
			m_vecObjectPool[15]->Set_Enable(true);
		}
	}

	case 93:
	{
		if (m_bEventTrigger[Script_Stage01][5] == false)
		{
			m_bEventTrigger[Script_Stage01][5] = true;

			m_vecObjectPool[16]->Set_Enable(true);
			m_vecObjectPool[17]->Set_Enable(true);
			m_vecObjectPool[18]->Set_Enable(true);
			m_vecObjectPool[19]->Set_Enable(true);
		}
	}


	case 104:
	{
		if (m_bEventTrigger[Script_Stage01][6] == false)
		{
			m_bEventTrigger[Script_Stage01][6] = true;

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
		if (m_bEventTrigger[Script_Stage01][7] == false)
		{
			m_bEventTrigger[Script_Stage01][7] = true;

			m_vecObjectPool[22]->Set_Enable(true);
			m_vecObjectPool[23]->Set_Enable(true);
			m_vecObjectPool[24]->Set_Enable(true);
			m_vecObjectPool[25]->Set_Enable(true);
			m_vecObjectPool[26]->Set_Enable(true);
		}
	}

	case 66:
	{
		if (m_bEventTrigger[Script_Stage01][8] == false)
		{
			m_bEventTrigger[Script_Stage01][8] = true;

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
		if (m_bEventTrigger[Script_Stage01][9] == false)
		{
			m_bEventTrigger[Script_Stage01][9] = true;

			m_vecObjectPool[28]->Set_Enable(true);
			//m_vecObjectPool[29]->Set_Enable(true);

			// 보스 HP바 생성
			g_pManagement->Add_GameObject_ToLayer(L"GameObject_BossHP", SCENE_STAGE, L"Layer_BossHP");
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
		if (m_bEventTrigger[Script_Stage01][11] == false)
		{
			m_bEventTrigger[Script_Stage01][11] = true;
			m_bTimer = true;

			CMassageUI* pMassageUI = static_cast<CMassageUI*>(g_pManagement->Get_GameObjectBack(L"Layer_BossMassageUI", SCENE_STAGE));
			pMassageUI->Set_Check_Play_BossnameUI(true);
		}

		break;
	}
	}
}

void CScriptManager::St03_Sub0()
{
	switch (m_dwCellIdx)
	{
	case 7:
	{
		if (m_bEventTrigger[Script_Stage03][0] == false)
		{
			m_bEventTrigger[Script_Stage03][0] = true;

			m_vecObjectPool[0]->Set_Enable(true);
			m_vecObjectPool[1]->Set_Enable(true);
			m_vecObjectPool[2]->Set_Enable(true);
		}

		break;
	}

	case 27:
	{
		if (m_bEventTrigger[Script_Stage03][1] == false)
		{
			m_bEventTrigger[Script_Stage03][1] = true;

			m_vecObjectPool[3]->Set_Enable(true);
			m_vecObjectPool[4]->Set_Enable(true);
			m_vecObjectPool[5]->Set_Enable(true);
		}

		break;
	}
	}
}

void CScriptManager::St03_Sub1()
{
	switch (m_dwCellIdx)
	{
	case 8:
	{
		if (m_bEventTrigger[Script_Stage03][2] == false)
		{
			m_bEventTrigger[Script_Stage03][2] = true;

			m_vecObjectPool[6]->Set_Enable(true);
			m_vecObjectPool[7]->Set_Enable(true);
			m_vecObjectPool[8]->Set_Enable(true);
			m_vecObjectPool[9]->Set_Enable(true);
		}

		break;
	}
	}
}

void CScriptManager::St03_Sub2()
{
	switch (m_dwCellIdx)
	{
	case 29:
	{
		if (m_bEventTrigger[Script_Stage03][3] == false)
		{
			m_bEventTrigger[Script_Stage03][3] = true;

			m_vecObjectPool[10]->Set_Enable(true);
			m_vecObjectPool[11]->Set_Enable(true);
			m_vecObjectPool[12]->Set_Enable(true);
			m_vecObjectPool[13]->Set_Enable(true);
			m_vecObjectPool[14]->Set_Enable(true);
		}

		break;
	}
	}
}

void CScriptManager::St03_Sub3()
{
}

void CScriptManager::St03_Sub4()
{
}

void CScriptManager::St04_Sub0()
{
	switch (m_dwCellIdx)
	{
	case 0:
	{
		if (m_bEventTrigger[Script_Stage04][0] == false)
		{
			m_bEventTrigger[Script_Stage04][0] = true;
			m_bTimer = true;

			// 중앙 잡몹 4
			m_vecObjectPool[0]->Set_Enable(true);
			m_vecObjectPool[1]->Set_Enable(true);
			m_vecObjectPool[2]->Set_Enable(true);
			m_vecObjectPool[3]->Set_Enable(true);

			// 언덕 잡몹 1
			m_vecObjectPool[4]->Set_Enable(true);
		}

		break;
	}
	}
}

void CScriptManager::St04_Sub1()
{
	switch (m_dwCellIdx)
	{
	case 106:
	{
		if (m_bEventTrigger[Script_Stage04][1] == false)
		{
			m_bEventTrigger[Script_Stage04][1] = true;

			// 여왕의 기사
			m_vecObjectPool[5]->Set_Enable(true);
		}

		break;
	}
	}
}

void CScriptManager::St04_Sub2()
{
	switch (m_dwCellIdx)
	{
	case 106:
	{
		// 보스 등장 UI
		if (m_bEventTrigger[Script_Stage04][2] == false)
		{
			m_bEventTrigger[Script_Stage04][2] = true;
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
	switch (m_dwSubsetIdx)
	{
	case 0:
	{
		St03_Sub0();
		break;
	}

	case 1:
	{
		St03_Sub1();
		break;
	}

	case 2:
	{
		St03_Sub2();
		break;
	}

	case 3:
	{
		St03_Sub3();
		break;
	}

	case 4:
	{
		St03_Sub4();
		break;
	}
	}
}

void CScriptManager::Stage_04_Scripts()
{
	switch (m_dwSubsetIdx)
	{
	case 0:
	{
		St04_Sub0();
		break;
	}

	case 1:
	{
		St04_Sub1();
		break;
	}

	case 2:
	{
		St04_Sub2();
		break;
	}
	}
}

void CScriptManager::Ready_Base_DynamicObject()
{
}

void CScriptManager::Ready_Stage01_DynamicObject()
{
	m_vecObjectPool.reserve(40);

	_v3 vPos[30] =
	{
		// 초반 3명
		_v3(144.551f, -18.08f, 79.895f),
		_v3(145.498f, -18.08f, 84.775f),
		_v3(150.690f, -18.08f, 94.981f),
		// =========================================

		// 창, 검, 창, 총, 총
		_v3(135.253f, -17.667f, 110.645f),
		_v3(132.46f, -17.7f, 118.243f),
		_v3(106.398f, -17.150f, 114.131f),
		_v3(104.769f, -17.150f, 121.635f),
		_v3(105.458f, -17.150f, 108.336f),
		// =========================================

		// 이전 2명
		_v3(95.754f, -17.15f, 106.058f),
		_v3(87.195f, -17.15f, 105.301f),
		// =========================================

		// 총, 총
		_v3(72.357f, -17.15f, 141.364f),
		// =========================================

		// 이전 5마리
		_v3(61.826f, -17.15f, 115.219f),
		_v3(65.298f, -17.15f, 125.649f),
		_v3(62.689f, -17.15f, 127.093f),
		_v3(71.845f, -17.15f, 132.392f),
		_v3(102.201f, -17.15f, 141.943f),
		// =========================================

		// 댕댕이 4마리
		_v3(64.588f, -0.1f, 151.872f),
		_v3(43.606f, -0.1f, 151.288f),
		_v3(41.517f, -0.1f, 147.294f),
		_v3(54.421f, -0.1f, 143.731f),
		// =========================================

		// 이전 겐지 둘
		_v3(85.174f, 0.1f, 154.160f),
		_v3(87.157f, 0.1f, 167.728f),
		// =========================================

		//  검, 검, 창, 검, 야
		_v3(9.853f, 0.1f, 122.874f),
		_v3(22.171f, 0.1f, 127.027f),
		_v3(6.095f, 0.1f, 109.448f),
		_v3(15.527f, 0.1f, 102.195f),
		_v3(-4.185f, 0.1f, 60.353f),
		// =========================================

		// 검 겐지
		_v3(43.606f, 0.1f, 151.288f),
		// =========================================

		// 보스
		_v3(-0.955f, 0.8f, -5.525f)
		// =========================================
	};

	CGameObject* pInstance = nullptr;

	//=======================================================================
	// 4명

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
	m_vecObjectPool.reserve(20);

	_v3 vPos[15] =
	{
		// 서브셋 0 - 3마리 ( 기사 1 , 원숭 2 )
		_v3(47.569f, -5.47f, -36.860f),
		_v3(35.370f, -3.55f, -55.322f),
		_v3(31.870f, -3.55f, -53.653f),
		// =========================================
		// 서브셋 0- 3마리 ( 기사 3 )
		_v3(30.912f, -3.55f, -91.540f),
		_v3(36.853f, -3.55f, -86.966f),
		_v3(25.788f, -3.55f, -90.002f),
		// =========================================
		// 서브셋 1 - 4마리 ( 원숭 4 )
		_v3(35.58f, 0.15f, -129.26f),
		_v3(17.215f, 0.15f, -127.65f),
		_v3(-9.383f, 0.15f, -127.54f),
		_v3(0.656f, 0.15f, -124.26f),
		// =========================================
		// 서브셋 2 - 3마리 ( 야차 1 , 겐지 2 )
		_v3(-3.340f, 0.15f, -83.591f),
		_v3(-12.567f, 0.15f, -84.645f),
		_v3(6.185f, 0.15f, -84.751f),
		// =========================================
		// 서브셋 4 - 2마리 ( 얼음보스 1 , 불보스 1 )
		_v3(-9.115f, 0.15f, -6.472f),
		_v3(6.557f, 0.15f, -4.748f)
	};

	CGameObject* pInstance = nullptr;

	//=======================================================================
	// 서브셋 0 - 3마리 ( 기사 1 , 원숭 2 )

	{
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Hunter", nullptr);
		TARGET_TO_NAV(pInstance)->Set_SubsetIndex(0);
		TARGET_TO_NAV(pInstance)->Set_Index(39);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);

		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Hunter", nullptr);
		TARGET_TO_NAV(pInstance)->Set_SubsetIndex(0);
		TARGET_TO_NAV(pInstance)->Set_Index(71);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);

		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Hunter", nullptr);
		TARGET_TO_NAV(pInstance)->Set_SubsetIndex(0);
		TARGET_TO_NAV(pInstance)->Set_Index(60);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
	}

	//=======================================================================
	// 서브셋 0- 3마리 ( 기사 3 )

	{
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Hunter", nullptr);
		TARGET_TO_NAV(pInstance)->Set_SubsetIndex(1);
		TARGET_TO_NAV(pInstance)->Set_Index(17);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);

		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Hunter", nullptr);
		TARGET_TO_NAV(pInstance)->Set_SubsetIndex(1);
		TARGET_TO_NAV(pInstance)->Set_Index(18);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);

		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Hunter", nullptr);
		TARGET_TO_NAV(pInstance)->Set_SubsetIndex(1);
		TARGET_TO_NAV(pInstance)->Set_Index(14);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
	}

	//=======================================================================
	// 서브셋 1 - 4마리 ( 원숭 4 )

	{
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Hunter", nullptr);
		TARGET_TO_NAV(pInstance)->Set_SubsetIndex(2);
		TARGET_TO_NAV(pInstance)->Set_Index(9);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);

		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Hunter", nullptr);
		TARGET_TO_NAV(pInstance)->Set_SubsetIndex(2);
		TARGET_TO_NAV(pInstance)->Set_Index(27);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);

		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Hunter", nullptr);
		TARGET_TO_NAV(pInstance)->Set_SubsetIndex(2);
		TARGET_TO_NAV(pInstance)->Set_Index(38);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);

		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Hunter", nullptr);
		TARGET_TO_NAV(pInstance)->Set_SubsetIndex(2);
		TARGET_TO_NAV(pInstance)->Set_Index(44);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
	}

	//=======================================================================
	// 서브셋 2 - 3마리 ( 야차 1 , 겐지 2 )

	{
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_YachaMan", nullptr);
		TARGET_TO_NAV(pInstance)->Set_SubsetIndex(3);
		TARGET_TO_NAV(pInstance)->Set_Index(34);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);

		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Hunter", nullptr);
		TARGET_TO_NAV(pInstance)->Set_SubsetIndex(3);
		TARGET_TO_NAV(pInstance)->Set_Index(46);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);

		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Hunter", nullptr);
		TARGET_TO_NAV(pInstance)->Set_SubsetIndex(3);
		TARGET_TO_NAV(pInstance)->Set_Index(44);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
	}

	//=======================================================================
	// 서브셋 4 - 2마리 ( 얼음보스 1 , 불보스 1 )

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_IceGirl", &CIceGirl::INFO(10.f, 5.f, 2.f));
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(4);
	TARGET_TO_NAV(pInstance)->Set_Index(22);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_FireBoy", &CIceGirl::INFO(10.f, 5.f, 2.f));
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(4);
	TARGET_TO_NAV(pInstance)->Set_Index(25);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);

	_ulong dwIndex = 0;

	for (auto& iter : m_vecObjectPool)
	{
		iter->Set_Enable(false);
		TARGET_TO_TRANS(iter)->Set_Pos(vPos[dwIndex]);
		TARGET_TO_NAV(iter)->Ready_NaviMesh(m_pGraphicDev, L"Navmesh_Stage_03.dat");

		g_pManagement->Add_GameOject_ToLayer_NoClone(iter, SCENE_STAGE, L"Layer_Monster", nullptr);

		++dwIndex;
	}
}

void CScriptManager::Ready_Stage04_DynamicObject()
{
	m_vecObjectPool.reserve(6);

	_v3 vPos[6] =
	{
		// 가운데 4마리 , 언덕 1마리
		_v3(8.834f, -3.9f, 73.108f),
		_v3(9.218f, -3.9f, 82.427f),
		_v3(-11.670f, -3.9f, 85.848f),
		_v3(-11.915f, -3.9f, 68.994f),
		_v3(-17.021f, 2.1f, 133.317f),
		// =========================================
		// 보스 1마리
		_v3(-2.71f, -0.1f, -26.031f)
	};

	CGameObject* pInstance = nullptr;

	//=======================================================================
	// 4 마리 + 언덕 1마리

	{
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Hunter", nullptr);
		TARGET_TO_NAV(pInstance)->Set_SubsetIndex(1);
		TARGET_TO_NAV(pInstance)->Set_Index(46);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);

		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Hunter", nullptr);
		TARGET_TO_NAV(pInstance)->Set_SubsetIndex(1);
		TARGET_TO_NAV(pInstance)->Set_Index(33);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);

		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Hunter", nullptr);
		TARGET_TO_NAV(pInstance)->Set_SubsetIndex(1);
		TARGET_TO_NAV(pInstance)->Set_Index(76);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);

		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Hunter", nullptr);
		TARGET_TO_NAV(pInstance)->Set_SubsetIndex(1);
		TARGET_TO_NAV(pInstance)->Set_Index(66);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);

		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Hunter", nullptr);
		TARGET_TO_NAV(pInstance)->Set_SubsetIndex(1);
		TARGET_TO_NAV(pInstance)->Set_Index(88);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
	}

	//=======================================================================
	// 보스 1마리
	{
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_QueensKnight", &CQueensKnight::INFO(10.f, 5.f, 2.f));
		TARGET_TO_NAV(pInstance)->Set_SubsetIndex(2);
		TARGET_TO_NAV(pInstance)->Set_Index(10);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
	}

	_ulong dwIndex = 0;

	for (auto& iter : m_vecObjectPool)
	{
		iter->Set_Enable(false);
		TARGET_TO_TRANS(iter)->Set_Pos(vPos[dwIndex]);
		TARGET_TO_NAV(iter)->Ready_NaviMesh(m_pGraphicDev, L"Navmesh_Stage_04.dat");

		g_pManagement->Add_GameOject_ToLayer_NoClone(iter, SCENE_STAGE, L"Layer_Monster", nullptr);

		++dwIndex;
	}
}

void CScriptManager::Free()
{
	for (auto& iter : m_vecObjectPool)
	{
		Safe_Release(iter);
	}

	Safe_Release(m_pGraphicDev);
}
