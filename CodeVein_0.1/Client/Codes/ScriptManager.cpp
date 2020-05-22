#include "stdafx.h"
#include "ScriptManager.h"
#include "Management.h"

// =================================
// ∏ÛΩ∫≈Õ
#include "GunGenji.h"
#include "SwordGenji.h"
#include "SwordShieldGenji.h"
#include "YachaMan.h"
#include "Wolf.h"
#include "YachaMan.h"
#include "Hunter.h"
#include "Monkey.h"
#include "Cocoon.h"
#include "Yeti.h"
// =================================
#include "PoisonButterfly.h"
#include "DeerKing.h"
#include "IceGirl.h"
#include "FireBoy.h"
#include "QueensKnight.h"
// =================================
#include "UI_Manager.h"

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
				m_bTimer = false;

				CUI_Manager::Get_Instance()->Set_BossUI_Active(true);
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
		//Stage_Base_Scripts();
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

	else
	{
		for (_uint i = 0; i < MaxTriggerCnt; ++i)
		{
			m_bEventTrigger[_sScriptStageIdx][i] = false;
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
	case 0:
	{
		if (m_bEventTrigger[Script_Stage01][0] == false)
		{
			m_bEventTrigger[Script_Stage01][0] = true;

			m_vecObjectPool[0]->Set_Enable(true);
			m_vecObjectPool[1]->Set_Enable(true);
			m_vecObjectPool[2]->Set_Enable(true);
			m_vecObjectPool[3]->Set_Enable(true);
		}
	}

	case 36:
	{
		if (m_bEventTrigger[Script_Stage01][1] == false)
		{
			m_bEventTrigger[Script_Stage01][1] = true;

			m_vecObjectPool[4]->Set_Enable(true);
			m_vecObjectPool[5]->Set_Enable(true);
			m_vecObjectPool[6]->Set_Enable(true);
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

			m_vecObjectPool[7]->Set_Enable(true);
			m_vecObjectPool[8]->Set_Enable(true);
			m_vecObjectPool[9]->Set_Enable(true);
			m_vecObjectPool[10]->Set_Enable(true);
			m_vecObjectPool[11]->Set_Enable(true);
			m_vecObjectPool[12]->Set_Enable(true);
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

			m_vecObjectPool[13]->Set_Enable(true);
			m_vecObjectPool[14]->Set_Enable(true);
			m_vecObjectPool[15]->Set_Enable(true);
			m_vecObjectPool[16]->Set_Enable(true);
			m_vecObjectPool[17]->Set_Enable(true);
			m_vecObjectPool[18]->Set_Enable(true);
		}
	}

	case 0:
	{
		if (m_bEventTrigger[Script_Stage01][4] == false)
		{
			m_bEventTrigger[Script_Stage01][4] = true;
		}
	}

	case 93:
	{
		if (m_bEventTrigger[Script_Stage01][5] == false)
		{
			m_bEventTrigger[Script_Stage01][5] = true;
		}
	}


	case 104:
	{
		if (m_bEventTrigger[Script_Stage01][6] == false)
		{
			m_bEventTrigger[Script_Stage01][6] = true;

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

			m_vecObjectPool[19]->Set_Enable(true);
			m_vecObjectPool[20]->Set_Enable(true);
			m_vecObjectPool[21]->Set_Enable(true);
			m_vecObjectPool[22]->Set_Enable(true);
		}
	}

	case 66:
	{
		if (m_bEventTrigger[Script_Stage01][8] == false)
		{
			m_bEventTrigger[Script_Stage01][8] = true;

			m_vecObjectPool[23]->Set_Enable(true);
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

			m_vecObjectPool[24]->Set_Enable(true);

			// ∫∏Ω∫ HPπŸ ª˝º∫
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
}

void CScriptManager::St02_Sub0()
{
	switch (m_dwCellIdx)
	{
	case 15:
	case 18:
	case 19:
	{
		//0-15 øπ∆º ≈‰≈€ √—∞’ ¥¡¥Î º∫∞‘
		if (m_bEventTrigger[Script_Stage02][0] == false)
		{
			m_bEventTrigger[Script_Stage02][0] = true;

			m_vecTrapPool[0]->Set_Enable(true); //0π¯ øπ∆º
			m_vecObjectPool[1]->Set_Enable(true); //≈‰≈€
			m_vecObjectPool[2]->Set_Enable(true); //√—∞’
			m_vecObjectPool[3]->Set_Enable(true); //¥¡¥Î
			m_vecObjectPool[4]->Set_Enable(true); //º∫∞‘
		}

	}
		break;

	case 114:
	{
		//0-114 ≈‰≈€ ≈‰≈€ øπ∆º º∫∞‘
		if (m_bEventTrigger[Script_Stage02][1] == false)
		{
			m_bEventTrigger[Script_Stage02][1] = true;

			m_vecObjectPool[5]->Set_Enable(true); //≈‰≈€
			m_vecObjectPool[6]->Set_Enable(true); //≈‰≈€
			m_vecTrapPool[1]->Set_Enable(true); //7π¯ øπ∆º
			m_vecObjectPool[8]->Set_Enable(true); //º∫∞‘
		}

	}
		break;

	}
}

void CScriptManager::St02_Sub1()
{
	switch (m_dwCellIdx)
	{
	case 62:
	{
		//1-62 øπ∆º ≈‰≈€ √—∞’ √¢∞’ √—∞’ º∫∞‘ øπ∆º
		if (m_bEventTrigger[Script_Stage02][2] == false)
		{
			m_bEventTrigger[Script_Stage02][2] = true;

			m_vecTrapPool[2]->Set_Enable(true); //9π¯ øπ∆º
			m_vecObjectPool[10]->Set_Enable(true); //≈‰≈€

			m_vecObjectPool[11]->Set_Enable(true); //√—∞’
			m_vecObjectPool[12]->Set_Enable(true); //√¢∞’
			m_vecObjectPool[13]->Set_Enable(true);//√—∞’

			m_vecObjectPool[14]->Set_Enable(true);//º∫∞‘
			m_vecTrapPool[3]->Set_Enable(true); //15π¯ øπ∆º
		}

	}
	break;

	}

}

void CScriptManager::St02_Sub2()
{
	switch (m_dwCellIdx)
	{
	case 40:
	{
		//2-40 ¥¡¥Î ¥¡¥Î ¥¡¥Î ≈‰≈€ º∫∞‘
		if (m_bEventTrigger[Script_Stage02][3] == false)
		{
			m_bEventTrigger[Script_Stage02][3] = true;

			m_vecObjectPool[16]->Set_Enable(true); //¥¡¥Î
			m_vecObjectPool[17]->Set_Enable(true); //¥¡¥Î
			m_vecObjectPool[18]->Set_Enable(true); //¥¡¥Î
			m_vecObjectPool[19]->Set_Enable(true); //≈‰≈€
			m_vecObjectPool[20]->Set_Enable(true); //º∫∞‘
		}
	}
		break;
	}

}

void CScriptManager::St02_Sub3()
{
	switch (m_dwCellIdx)
	{
	case 66:
	{
		//3-66 øπ∆º øπ∆º øπ∆º
		if (m_bEventTrigger[Script_Stage03][4] == false)
		{
			m_bEventTrigger[Script_Stage03][4] = true;

			m_vecTrapPool[4]->Set_Enable(true); //21π¯ øπ∆º
			m_vecTrapPool[5]->Set_Enable(true); //22π¯ øπ∆º
			m_vecTrapPool[6]->Set_Enable(true); //23π¯ øπ∆º
		}

	}
		break;
	case 14:
	{
		//3-14 ªÁΩøø’
		if (m_bEventTrigger[Script_Stage03][5] == false)
		{
			m_bEventTrigger[Script_Stage03][5] = true;

			m_vecObjectPool[24]->Set_Enable(true); // 24π¯

			m_bFogOff = true;
		}

	}
	break;
	}
}

void CScriptManager::St02_Sub4()
{
}

void CScriptManager::St03_Sub0()
{
	switch (m_dwCellIdx)
	{
	case 6:
	{
		if (m_bEventTrigger[Script_Stage03][0] == false)
		{
			m_bEventTrigger[Script_Stage03][0] = true;

			m_vecObjectPool[0]->Set_Enable(true);
			m_vecObjectPool[1]->Set_Enable(true);
			m_vecObjectPool[2]->Set_Enable(true);
			m_vecObjectPool[3]->Set_Enable(true);
			m_vecObjectPool[4]->Set_Enable(true);
		}

		break;
	}

	case 51:
	{
		if (m_bEventTrigger[Script_Stage03][1] == false)
		{
			m_bEventTrigger[Script_Stage03][1] = true;

			m_vecObjectPool[5]->Set_Enable(true);
			m_vecObjectPool[6]->Set_Enable(true);
			m_vecObjectPool[7]->Set_Enable(true);
		}

		break;
	}
	}
}

void CScriptManager::St03_Sub1()
{
	switch (m_dwCellIdx)
	{
	case 35:
	{
		if (m_bEventTrigger[Script_Stage03][2] == false)
		{
			m_bEventTrigger[Script_Stage03][2] = true;

			m_vecObjectPool[5]->Set_Enable(true);
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
	case 24:
	{
		if (m_bEventTrigger[Script_Stage03][3] == false)
		{
			m_bEventTrigger[Script_Stage03][3] = true;

			m_vecObjectPool[10]->Set_Enable(true);
			m_vecObjectPool[11]->Set_Enable(true);
			m_vecObjectPool[12]->Set_Enable(true);
			m_vecObjectPool[13]->Set_Enable(true);
		}

		break;
	}
	}
}

void CScriptManager::St03_Sub3()
{
	switch (m_dwCellIdx)
	{
	case 5:
	{
		if (m_bEventTrigger[Script_Stage03][4] == false)
		{
			m_bEventTrigger[Script_Stage03][4] = true;

			m_vecObjectPool[14]->Set_Enable(true);
			m_vecObjectPool[15]->Set_Enable(true);
		}

		break;
	}
	}
}

void CScriptManager::St03_Sub4()
{
}

void CScriptManager::St04_Sub0()
{
	switch (m_dwCellIdx)
	{
	case 46:
	{
		if (m_bEventTrigger[Script_Stage04][0] == false)
		{
			m_bEventTrigger[Script_Stage04][0] = true;

			m_vecObjectPool[0]->Set_Enable(true);
			m_vecObjectPool[1]->Set_Enable(true);
			m_vecObjectPool[2]->Set_Enable(true);
			m_vecObjectPool[3]->Set_Enable(true);
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
	case 106: //114π¯ π‚¿∏∏È º“»Ø¿∏∑Œ ª˝∞¢«ÿµ“
	{
		if (m_bEventTrigger[Script_Stage04][1] == false)
		{
			m_bEventTrigger[Script_Stage04][1] = true;

			// ø©ø’¿« ±‚ªÁ
			m_vecObjectPool[5]->Set_Enable(true);
		}

		break;
	}
	}
}

void CScriptManager::St04_Sub2()
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
	Check_YetiTrap();

	switch (m_dwSubsetIdx)
	{
	case 0:
	{
		St02_Sub0();
		break;
	}

	case 1:
	{
		St02_Sub1();
		break;
	}

	case 2:
	{
		St02_Sub2();
		break;
	}

	case 3:
	{
		St02_Sub3();
		break;
	}

	case 4:
	{
		St02_Sub4();
		break;
	}

	}
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
	m_vecObjectPool.reserve(25);

	_v4 vPos[25] =
	{
		// ================================================================================================================
		// 25∏∂∏Æ
		// ================================================================================================================
		// ∞À∞’, ∞À∞’, ∞À∞’, √¢∞’
		// ================================================================================================================
		_v4(149.756f, -18.08f, 89.3907f, 176.f),
		_v4(144.419f, -18.08f, 78.1315f, 77.f),
		_v4(141.593f, -17.7f, 102.344f, -281.f),
		_v4(138.36f, -17.7f, 110.634f, 131.f),
		// ================================================================================================================
		// æﬂ¬˜ , √—∞’, ∞À∞’
		// ================================================================================================================
		_v4(104.911f, -17.15f, 114.246f, 79.f),
		_v4(103.837f, -17.15f, 106.89f, 23.f),
		_v4(74.5243f, -17.15f, 108.566f, 186.f),
		// ================================================================================================================
		// æﬂ¬˜, √—∞’, ∞À∞’, ∞À∞’ ,√¢∞’, ∞À∞’
		// ================================================================================================================
		_v4(63.1959f, -17.150f, 117.537f, 5.f),
		_v4(67.4774f, -17.150f, 144.162f, 186.f),
		_v4(72.5364f, -17.150f, 137.352f, 252.f),
		_v4(88.6899f, -17.150f, 139.469f, 7.f),
		_v4(91.0369f, -17.150f, 142.409f, 221.f),
		_v4(93.2527f, -4.2f, 134.889f, 54.f),
		// ================================================================================================================
		// ∞À∞’, √—∞’, ¥Û, ¥Û, ¥Û, ¥Û
		// ================================================================================================================
		_v4(84.356f, 0.1f, 158.816f, 166.f),
		_v4(88.37f, 0.1f, 166.539f, 183.f),
		_v4(41.4651f, 0.1f, 153.171f, 96.f),
		_v4(37.0877f, 0.1f, 143.726f, 54.f),
		_v4(33.1281f, 0.1f, 147.35f, 83.f),
		_v4(29.5839f, 0.1f, 153.327f, 102.f),
		// ================================================================================================================
		// √¢∞’, ¥Û , √—∞’, √—∞’, æﬂ¬˜
		// ================================================================================================================
		_v4(8.286f, 0.1f, 107.834f, 27.f),
		_v4(3.648f, 0.1f, 117.118f, 396.f),
		_v4(15.2567f, 0.1f, 101.224f, -44.9926f),
		_v4(20.45f, 0.1f, 116.49f, 278.682f),
		_v4(-5.112f, 0.1f, 54.158f, 11.f),
		// ================================================================================================================
		// µ∂≥™∫Ò
		// ================================================================================================================
		_v4(-2.81f, 0.8f, -2.54f, 356.f)
		// ================================================================================================================


	};

	CGameObject* pInstance = nullptr;
	_uint		iIndex = 0;
	//_uint		iTestNaviIdx = 0;

	// ================================================================================================================
	// 25∏∂∏Æ
	// ================================================================================================================
	// ∞À∞’, ∞À∞’, ∞À∞’, √¢∞’
	// ================================================================================================================
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji",
		&CSwordGenji::INFO(CSwordGenji::Normal, CSwordGenji::NF_Ani::Talk, 10.f, 10.f, 2.f,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	//iTestNaviIdx = TARGET_TO_NAV(pInstance)->Get_CellIndex();

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji",
		&CSwordGenji::INFO(CSwordGenji::White, CSwordGenji::NF_Ani::Sit1, 10.f, 5.f, 2.f,
			true, _v3(vPos[0].x, vPos[0].y, vPos[0].z), _v3(0.f, vPos[0].w, 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji",
		&CSwordGenji::INFO(CSwordGenji::Jungle, CSwordGenji::NF_Ani::LookAround1, 10.f, 5.f, 2.f,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	// √¢∞’
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordShieldGenji",
		&CSwordShieldGenji::INFO(CSwordShieldGenji::White, CSwordShieldGenji::NF_Ani::LookAround3, 50.f, 7.f, 2.f,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	// ================================================================================================================
	// æﬂ¬˜ , √—∞’, ∞À∞’
	// ================================================================================================================
	// æﬂ¬˜
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_YachaMan",
		&CMonster::MONSTER_STATUS(CMonster::COLOR_NONE, WEAPON_None,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	// √—∞’
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji",
		&CGunGenji::INFO(CGunGenji::Jungle, CGunGenji::NF_Ani::Talk, 10.f, 10.f, 2.f,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	// ∞À∞’
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji",
		&CSwordGenji::INFO(CSwordGenji::Normal, CSwordGenji::NF_Ani::Sit1, 10.f, 10.f, 2.f,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	// ================================================================================================================
	// æﬂ¬˜, √—∞’, ∞À∞’, ∞À∞’ ,√¢∞’, ∞À∞’
	// ================================================================================================================
	//æﬂ¬˜
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_YachaMan",
		&CMonster::MONSTER_STATUS(CMonster::COLOR_NONE, WEAPON_None,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	// √—∞’
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji",
		&CGunGenji::INFO(CGunGenji::Normal, CGunGenji::NF_Ani::Talk, 10.f, 10.f, 2.f,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	// ∞À∞’
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji",
		&CSwordGenji::INFO(CSwordGenji::White, CSwordGenji::NF_Ani::Sit1, 10.f, 10.f, 2.f,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	// ∞À∞’
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji",
		&CSwordGenji::INFO(CSwordGenji::Jungle, CSwordGenji::NF_Ani::Sit1, 10.f, 10.f, 2.f,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	// √¢∞’
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordShieldGenji",
		&CSwordShieldGenji::INFO(CSwordShieldGenji::White, CSwordShieldGenji::NF_Ani::LookAround3, 50.f, 7.f, 2.f,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	// ∞À∞’
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji",
		&CSwordGenji::INFO(CSwordGenji::Normal, CSwordGenji::NF_Ani::Sit1, 10.f, 10.f, 2.f,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	// ================================================================================================================
	// ∞À∞’, √—∞’, ¥Û, ¥Û, ¥Û, ¥Û
	// ================================================================================================================
	// ∞À∞’
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji",
		&CSwordGenji::INFO(CSwordGenji::Jungle, CSwordGenji::NF_Ani::Sit1, 10.f, 10.f, 2.f,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	// √—∞’
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji",
		&CGunGenji::INFO(CGunGenji::Normal, CGunGenji::NF_Ani::Talk, 10.f, 10.f, 2.f,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	// ¥¡¥Î
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Wolf",
		&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	// ¥¡¥Î
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Wolf",
		&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	// ¥¡¥Î
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Wolf",
		&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	// ¥¡¥Î
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Wolf",
		&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	// ================================================================================================================
	// √¢∞’, ¥Û , √—∞’, √—∞’, æﬂ¬˜
	// ================================================================================================================
	// √¢∞’
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordShieldGenji",
		&CSwordShieldGenji::INFO(CSwordShieldGenji::White, CSwordShieldGenji::NF_Ani::LookAround3, 50.f, 7.f, 2.f,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	// ¥¡¥Î
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Wolf",
		&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	// √—∞’
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji",
		&CGunGenji::INFO(CGunGenji::Normal, CGunGenji::NF_Ani::Talk, 10.f, 10.f, 2.f,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	// √—∞’
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji",
		&CGunGenji::INFO(CGunGenji::Normal, CGunGenji::NF_Ani::Talk, 10.f, 10.f, 2.f,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	//æﬂ¬˜
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_YachaMan",
		&CMonster::MONSTER_STATUS(CMonster::COLOR_NONE, WEAPON_None,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	// ================================================================================================================
	// µ∂≥™∫Ò
	// ================================================================================================================
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_PoisonButterfly",
		&CPoisonButterfly::INFO(80.f, 18.f, 1.f,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Boss", nullptr);

}

void CScriptManager::Ready_Stage02_DynamicObject()
{
	m_vecObjectPool.reserve(26);
	m_vecTrapPool.reserve(8);

	_v4 vPos[25] =
	{ 
		//0-15,18,19
		_v4(40.185f, 3.2f, -208.907f, -76.f), //øπ∆º
		_v4(56.934f, 3.067f, -204.f, -64.f), //≈‰≈€
		_v4(53.503f, 3.133f, -201.412f, -120.f), //√—∞’
		_v4(56.934f, 3.067f, -204.f, -85.f), //¥¡¥Î
		_v4(57.878f, 3.017f, -216.88f, 0.f), //º∫∞‘

		//0-114
		_v4(107.573f, 3.717f, -216.271f, -64.f), //≈‰≈€
		_v4(108.344f, 3.817f, -210.993f, 0.f), //≈‰≈€
		_v4(121.388f, 3.85f, -200.46f, -45.f), //øπ∆º
		_v4(118.852f, 3.917f, -195.32f, 0.f), //º∫∞‘

		//1-60
		_v4(143.764f, 4.f, -172.628f, -12.f), //øπ∆º
		_v4(127.102f, 4.f, -165.291f, -64.f), //≈‰≈€
		_v4(141.012f, 4.f, -167.41f, -87.f), //√—∞’
		_v4(145.014f, 4.f, -143.589f, -55.f), //√¢∞’
		_v4(138.418f, 4.f, -139.081f, -60.f), //√—∞’
		_v4(121.109f, 4.f, -147.791f, 0.f), //º∫∞‘
		_v4(125.129f, 4.f, -133.48f, 60.f), //øπ∆º

		//2-40
		_v4(90.622f, 3.883f, -114.32f, -55.f), //¥¡¥Î
		_v4(95.239f, 3.967f, -112.003f, -125.f), //¥¡¥Î
		_v4(98.839f, 3.983f, -109.451f, -25.f), //¥¡¥Î
		_v4(81.055f, 3.95f, -107.338f, -45.f), //≈‰≈€
		_v4(79.2f, 3.933f, -103.601f, 0.f), //º∫∞‘

		//3-66
		_v4(52.076f, 0.f, -49.954f, -87.f), //øπ∆º
		_v4(54.972f, 0.f, -38.486f, -63.f), //øπ∆º
		_v4(38.042f, 0.f, -38.411f, -45.f), //øπ∆º

		_v4(-6.272f, -0.667f, 0.243f, -10.3593f),

	};

	CGameObject* pInstance = nullptr;
	_uint		iIndex = 0;
	_uint		iTestNaviIdx = 0;

	// ================================================================================================================
	//0-15 øπ∆º ≈‰≈€ √—∞’ ¥¡¥Î º∫∞‘
	{
		//øπ∆º ∆Æ∑¶
		pInstance = g_pManagement->Clone_GameObject_Return(L"GameObject_YetiTrap", &CYetiTrap::YETITRAPINFO(iIndex, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), V3_NULL));
		pInstance->Set_Enable(true);
		m_vecTrapPool.push_back(static_cast<CYetiTrap*>(pInstance));
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Trap", nullptr);
		//øπ∆º
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Yeti",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w),0.f), 2));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;

		

		//≈‰≈€
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Cocoon",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::WHITE, WEAPON_STATE::WEAPON_None,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 2));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;

		

		// √—∞’
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji",
			&CGunGenji::INFO(CGunGenji::Normal, CGunGenji::NF_Ani::CheckGun, 10.f, 10.f, 2.f,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 2));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;

		

		//¥¡¥Î
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Wolf",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::WHITE, WEAPON_STATE::WEAPON_None,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 2));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;

		

		//º∫∞‘
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Urchin",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::WHITE, WEAPON_STATE::WEAPON_None,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 2));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;

		

	}

	// ================================================================================================================
	//0-114 ≈‰≈€ ≈‰≈€ øπ∆º º∫∞‘
	{
		//≈‰≈€
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Cocoon",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::WHITE, WEAPON_STATE::WEAPON_None,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 2));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;

		

		//≈‰≈€
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Cocoon",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::WHITE, WEAPON_STATE::WEAPON_None,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 2));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;

		

		//øπ∆º ∆Æ∑¶
		pInstance = g_pManagement->Clone_GameObject_Return(L"GameObject_YetiTrap", &CYetiTrap::YETITRAPINFO(iIndex, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), V3_NULL));
		pInstance->Set_Enable(true);
		m_vecTrapPool.push_back(static_cast<CYetiTrap*>(pInstance));
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Trap", nullptr);
		//øπ∆º
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Yeti",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), V3_NULL, 2));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;

		

		//º∫∞‘
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Urchin",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::WHITE, WEAPON_STATE::WEAPON_None,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 2));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;

		
	}

	// ================================================================================================================
	//1-62 øπ∆º ≈‰≈€ √—∞’ √¢∞’ √—∞’ º∫∞‘ øπ∆º
	{	
		//øπ∆º ∆Æ∑¶
		pInstance = g_pManagement->Clone_GameObject_Return(L"GameObject_YetiTrap", &CYetiTrap::YETITRAPINFO(iIndex, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), V3_NULL));
		pInstance->Set_Enable(true);
		m_vecTrapPool.push_back(static_cast<CYetiTrap*>(pInstance));
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Trap", nullptr);
		//øπ∆º
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Yeti",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 2));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;

		

		//≈‰≈€
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Cocoon",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::WHITE, WEAPON_STATE::WEAPON_None,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 2));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;

		

		// √—∞’
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji",
			&CGunGenji::INFO(CGunGenji::Normal, CGunGenji::NF_Ani::Talk, 10.f, 10.f, 2.f,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 2));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;

		

		// √¢∞’
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordShieldGenji",
			&CSwordShieldGenji::INFO(CSwordShieldGenji::Normal, CSwordShieldGenji::NF_Ani::Talk, 10.f, 10.f, 2.f,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 2));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;

		

		// √—∞’
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji",
			&CGunGenji::INFO(CGunGenji::Normal, CGunGenji::NF_Ani::Sit2, 10.f, 10.f, 2.f,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 2));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;

		

		//º∫∞‘
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Urchin",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::WHITE, WEAPON_STATE::WEAPON_None,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 2));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;

		

		//øπ∆º ∆Æ∑¶
		pInstance = g_pManagement->Clone_GameObject_Return(L"GameObject_YetiTrap", &CYetiTrap::YETITRAPINFO(iIndex, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), V3_NULL));
		pInstance->Set_Enable(true);
		m_vecTrapPool.push_back(static_cast<CYetiTrap*>(pInstance));
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Trap", nullptr);
		//øπ∆º
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Yeti",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), V3_NULL, 2));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;

		
	}

	// ================================================================================================================
	//2-40 ¥¡¥Î ¥¡¥Î ¥¡¥Î ≈‰≈€ º∫∞‘
	{
		//¥¡¥Î
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Wolf",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::WHITE, WEAPON_STATE::WEAPON_None,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 2));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;

		

		//¥¡¥Î
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Wolf",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::WHITE, WEAPON_STATE::WEAPON_None,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 2));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;

		

		//¥¡¥Î
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Wolf",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::WHITE, WEAPON_STATE::WEAPON_None,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 2));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;

		

		//≈‰≈€
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Cocoon",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::WHITE, WEAPON_STATE::WEAPON_None,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 2));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;

		

		//º∫∞‘
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Urchin",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::WHITE, WEAPON_STATE::WEAPON_None,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 2));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;

		
	}

	// ================================================================================================================
	//3-66 øπ∆º øπ∆º øπ∆º
	{
		//øπ∆º ∆Æ∑¶
		pInstance = g_pManagement->Clone_GameObject_Return(L"GameObject_YetiTrap", &CYetiTrap::YETITRAPINFO(iIndex, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), V3_NULL));
		pInstance->Set_Enable(true);
		m_vecTrapPool.push_back(static_cast<CYetiTrap*>(pInstance));
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Trap", nullptr);
		//øπ∆º
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Yeti",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 2));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;

		

		//øπ∆º ∆Æ∑¶
		pInstance = g_pManagement->Clone_GameObject_Return(L"GameObject_YetiTrap", &CYetiTrap::YETITRAPINFO(iIndex, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), V3_NULL));
		pInstance->Set_Enable(true);
		m_vecTrapPool.push_back(static_cast<CYetiTrap*>(pInstance));
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Trap", nullptr);
		//øπ∆º
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Yeti",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 2));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;

		

		//øπ∆º ∆Æ∑¶
		pInstance = g_pManagement->Clone_GameObject_Return(L"GameObject_YetiTrap", &CYetiTrap::YETITRAPINFO(iIndex, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), V3_NULL));
		pInstance->Set_Enable(true);
		m_vecTrapPool.push_back(static_cast<CYetiTrap*>(pInstance));
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Trap", nullptr);
		//øπ∆º
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Yeti",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 2));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;

		
	}

	// ================================================================================================================
	//3-14 ªÁΩøø’
	{
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_DeerKing", &CDeerKing::INFO(80.f, 18.f, 1.f,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 2));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Boss", nullptr);

		
	}
}

void CScriptManager::Ready_Stage03_DynamicObject()
{
	m_vecObjectPool.reserve(17);

	_v3 vPos[17] =
	{
		//Subset0 TriggerIdx6 ≈‰≈€, «Â≈Õ(√¢)
		_v3(50.24f, -7.45f, -22.575f), //0π¯
		_v3(51.739f, -7.45f, -45.18f),

		//Subset0 TriggerIdx51 æﬂ¬˜∏«, «Â≈Õ(√—), «Â≈Õ(√—)
		_v3(34.51f, -3.55f, -89.696f),
		_v3(30.574f, -3.55f, -98.505f),
		_v3(43.386f, -3.55f, -96.611f),

		//Subset1 TriggerIdx35 ≈‰≈€, ≈‰≈€, ø¯º˛¿Ã, «Â≈Õ(∏¡ƒ°), «Â≈Õ(¥Î∞À)
		_v3(34.397f, 0.15f, -131.02f), //5π¯
		_v3(37.962f, 0.15f, -129.12f),
		_v3(35.826f, 0.15f, -131.74f),
		_v3(-7.468f, 0.15f, -130.03f),
		_v3(-4.332f, 0.15f, -132.74f),

		//Subset2 TriggerIdx24 ø¯º˛¿Ã, ø¯º˛¿Ã, æﬂ¬˜∏«, «Â≈Õ(∞À)
		_v3(-10.123f, 0.15f, -99.011f), //10π¯
		_v3(5.851f, 0.15f, -98.279f),
		_v3(-4.572f, 0.15f, -87.141f),
		_v3(-8.776f, 0.15f, -79.109f),

		// º≠∫Íº¬ 4 - 2∏∂∏Æ ( æÛ¿Ω∫∏Ω∫ 1 , ∫“∫∏Ω∫ 1 )
		_v3(-9.115f, 0.15f, -6.472f), //
		_v3(6.557f, 0.15f, -4.748f) //15π¯

		//¡∂∏Ì
		//_v3(45.686f, -3.55f, -84.834f),
		//_v3(26.489f, -3.55f, -84.834f),
		//_v3(-5.948f, 0.15f, -115.63f),
		//_v3(-0.396f, 0.15f, -112.06f),
		//_v3(-13.96f, 0.15f, -90.2f),
		//_v3(4.9f, 0.15f, -92.64f),
		//_v3(-0.138f, 0.15f, -64.397f),
		//_v3(-6.153f, 0.15f, -64.405f),
		//_v3(4.163f, 0.15f, -17.589f),
		//_v3(-9.395f, 0.15f, -1.262f),

	};

	CGameObject* pInstance = nullptr;
	_uint		iIndex = 0;
	_uint		iTestNaviIdx = 0;

	// ================================================================================================================
	//Subset0 TriggerIdx6 ≈‰≈€, «Â≈Õ(√¢)
	{
		//≈‰≈€
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Cocoon",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), V3_NULL, 3));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;
		//«Â≈Õ(√¢)
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Hunter",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_Halberd,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), V3_NULL, 3));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;

		

	}

	// ================================================================================================================
	//Subset0 TriggerIdx51 æﬂ¬˜∏«, «Â≈Õ(√—), «Â≈Õ(√—)
	{
		//æﬂ¬˜∏«
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_YachaMan",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, 0.f, 0.f), 3));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;
		//«Â≈Õ(√—)
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Hunter",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_Gun,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), V3_NULL, 3));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;
		//«Â≈Õ(√—)
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Hunter",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_Gun,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), V3_NULL, 3));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;
	}

	 // ================================================================================================================
	//Subset1 TriggerIdx35 ≈‰≈€, ≈‰≈€, ø¯º˛¿Ã, «Â≈Õ(∏¡ƒ°), «Â≈Õ(¥Î∞À)
	{
		// ≈‰≈€
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Cocoon",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), V3_NULL, 3));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;
		// ≈‰≈€
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Cocoon",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), V3_NULL, 3));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;
		//ø¯º˛¿Ã
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Monkey",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), V3_NULL, 3));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;
		//«Â≈Õ(∏¡ƒ°)
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Hunter",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_Hammer,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), V3_NULL, 3));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;
		//«Â≈Õ(¥Î∞À)
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Hunter",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_LSword,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), V3_NULL, 3));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;
	}

	 // ================================================================================================================
	//Subset2 TriggerIdx24 ø¯º˛¿Ã, ø¯º˛¿Ã, æﬂ¬˜∏«, «Â≈Õ(∞À)
	{
		//ø¯º˛¿Ã
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Monkey",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), V3_NULL, 3));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;
		//ø¯º˛¿Ã
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Monkey",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), V3_NULL, 3));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;
		//æﬂ¬˜∏«
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_YachaMan",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, 0.f, 0.f), 3));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;

		//«Â≈Õ(∞À)
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Hunter",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_SSword,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), V3_NULL, 3));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;
	}

	// ================================================================================================================
	// º≠∫Íº¬ 4 - 2∏∂∏Æ ( æÛ¿Ω∫∏Ω∫ 1 , ∫“∫∏Ω∫ 1 )
	{
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_IceGirl", &CIceGirl::INFO(70.f, 20.f, 2.f, true,
			_v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(180.f), 0.f), 3));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Boss", nullptr);
		++iIndex;

		

		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_FireBoy", &CFireBoy::INFO(70.f, 20.f, 2.f, true,
			_v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(180.f), 0.f), 3));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Boss", nullptr);
		++iIndex;
	}

	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_IceGirl", &CIceGirl::INFO(70.f, 20.f, 2.f));
	//TARGET_TO_NAV(pInstance)->Set_SubsetIndex(4);
	//TARGET_TO_NAV(pInstance)->Set_Index(22);
	//TARGET_TO_TRANS(pInstance)->Set_Angle(AXIS_Y, D3DXToRadian(180.f));
	//m_vecObjectPool.push_back(pInstance);
	//Safe_AddRef(pInstance);

	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_FireBoy", &CIceGirl::INFO(70.f, 20.f, 2.f));
	//TARGET_TO_NAV(pInstance)->Set_SubsetIndex(4);
	//TARGET_TO_NAV(pInstance)->Set_Index(25);
	//TARGET_TO_TRANS(pInstance)->Set_Angle(AXIS_Y, D3DXToRadian(180.f));
	//m_vecObjectPool.push_back(pInstance);
	//Safe_AddRef(pInstance);

}

void CScriptManager::Ready_Stage04_DynamicObject()
{
	m_vecObjectPool.reserve(8);

	_v3 vPos[6] =
	{
		//Subset0 TriggerIdx46 µ˚¿Ã∏«, ≈‰≈€x4
		_v3(-1.625f, -3.883f, 77.897f),
		_v3(-8.819f, -3.883f, 70.1f),
		_v3(-9.208f, -3.883f, 85.404f),
		_v3(6.937f, -3.883f, 69.14f),
		_v3(7.177f, -3.883f, 85.947f),

		//Subset1 TriggerIdx114 ø©ø’±‚ªÁ
		_v3(-1.469f, 0.1f, -3.486f),

		//// ∞°øÓµ• 4∏∂∏Æ , æ¥ˆ 1∏∂∏Æ
		//_v3(8.834f, -3.9f, 73.108f),
		//_v3(9.218f, -3.9f, 82.427f),
		//_v3(-11.670f, -3.9f, 85.848f),
		//_v3(-11.915f, -3.9f, 68.994f),
		//_v3(-17.021f, 2.1f, 133.317f),
		//// =========================================
		//// ∫∏Ω∫ 1∏∂∏Æ
		//_v3(-2.71f, -0.1f, -26.031f)
	};

	CGameObject* pInstance = nullptr;
	_uint		iIndex = 0;
	_uint		iTestNaviIdx = 0;

	// // ================================================================================================================
	//Subset0 TriggerIdx46 µ˚¿Ã∏«, ≈‰≈€x4
	{
		//µ˚¿Ã∏«
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_ThaiMan",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, 0.f/*D3DXToRadian(vPos[iIndex].w)*/, 0.f), 4));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;
		//≈‰≈€
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Cocoon",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), V3_NULL, 4));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;
		//≈‰≈€
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Cocoon",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), V3_NULL, 4));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;
		//≈‰≈€
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Cocoon",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), V3_NULL, 4));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;
		//≈‰≈€
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Cocoon",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), V3_NULL, 4));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;
	}

	// // ================================================================================================================
	//Subset1 TriggerIdx114 ø©ø’±‚ªÁ
	{
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_QueensKnight", &CIceGirl::INFO(45.f, 20.f, 2.f, true,
			_v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, 0.f/*D3DXToRadian(180.f)*/, 0.f), 4));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Boss", nullptr);
		++iIndex;
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

void CScriptManager::Check_YetiTrap()
{
	for (auto& vector_iter : m_vecTrapPool)
	{
		if(false == vector_iter->Get_Enable())
			continue;

		if(false == vector_iter->Get_SummonYeti())
			continue;

		m_vecObjectPool[vector_iter->Get_YetiIndex()]->Set_Enable(true);
		vector_iter->Set_Enable(false);
	}

	return;
}
 