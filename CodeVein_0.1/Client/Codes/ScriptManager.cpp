#include "stdafx.h"
#include "ScriptManager.h"
#include "Management.h"

// =================================
// ∏ÛΩ∫≈Õ
// =================================
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
	switch (m_dwCellIdx)
	{
	case 0:
	{
		if (m_bEventTrigger[Script_Stage01][10] == false)
		{
			m_bEventTrigger[Script_Stage01][10] = true;
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
			m_vecObjectPool[10]->Set_Enable(true);
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

			m_vecObjectPool[11]->Set_Enable(true);
			m_vecObjectPool[12]->Set_Enable(true);
			m_vecObjectPool[13]->Set_Enable(true);
			m_vecObjectPool[14]->Set_Enable(true);
			m_vecObjectPool[15]->Set_Enable(true);
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

			// ¡ﬂæ” ¿‚∏˜ 4
			m_vecObjectPool[0]->Set_Enable(true);
			m_vecObjectPool[1]->Set_Enable(true);
			m_vecObjectPool[2]->Set_Enable(true);
			m_vecObjectPool[3]->Set_Enable(true);

			// æ¥ˆ ¿‚∏˜ 1
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

			// ø©ø’¿« ±‚ªÁ
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
		// ∫∏Ω∫ µÓ¿Â UI
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
	_int		iNavIdx = 0;

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

	iNavIdx = TARGET_TO_NAV(pInstance)->Get_CellIndex();

	// √¢∞’
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordShieldGenji",
		&CSwordShieldGenji::INFO(CSwordShieldGenji::White, CSwordShieldGenji::NF_Ani::LookAround3, 50.f, 7.f, 2.f,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	iNavIdx = TARGET_TO_NAV(pInstance)->Get_CellIndex();
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

	iNavIdx = TARGET_TO_NAV(pInstance)->Get_CellIndex();

	// √—∞’
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji",
		&CGunGenji::INFO(CGunGenji::Jungle, CGunGenji::NF_Ani::Talk, 10.f, 10.f, 2.f,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	iNavIdx = TARGET_TO_NAV(pInstance)->Get_CellIndex();

	// ∞À∞’
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji",
		&CSwordGenji::INFO(CSwordGenji::Normal, CSwordGenji::NF_Ani::Sit1, 10.f, 10.f, 2.f,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	iNavIdx = TARGET_TO_NAV(pInstance)->Get_CellIndex();
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

	iNavIdx = TARGET_TO_NAV(pInstance)->Get_CellIndex();

	// √—∞’
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji",
		&CGunGenji::INFO(CGunGenji::Normal, CGunGenji::NF_Ani::Talk, 10.f, 10.f, 2.f,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	iNavIdx = TARGET_TO_NAV(pInstance)->Get_CellIndex();

	// ∞À∞’
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji",
		&CSwordGenji::INFO(CSwordGenji::White, CSwordGenji::NF_Ani::Sit1, 10.f, 10.f, 2.f,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	iNavIdx = TARGET_TO_NAV(pInstance)->Get_CellIndex();

	// ∞À∞’
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji",
		&CSwordGenji::INFO(CSwordGenji::Jungle, CSwordGenji::NF_Ani::Sit1, 10.f, 10.f, 2.f,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	iNavIdx = TARGET_TO_NAV(pInstance)->Get_CellIndex();

	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordShieldGenji",
		&CSwordShieldGenji::INFO(CSwordShieldGenji::White, CSwordShieldGenji::NF_Ani::LookAround3, 50.f, 7.f, 2.f,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	iNavIdx = TARGET_TO_NAV(pInstance)->Get_CellIndex();

	// ∞À∞’
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji",
		&CSwordGenji::INFO(CSwordGenji::Normal, CSwordGenji::NF_Ani::Sit1, 10.f, 10.f, 2.f,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	iNavIdx = TARGET_TO_NAV(pInstance)->Get_CellIndex();
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

	iNavIdx = TARGET_TO_NAV(pInstance)->Get_CellIndex();

	// √—∞’
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji",
		&CGunGenji::INFO(CGunGenji::Normal, CGunGenji::NF_Ani::Talk, 10.f, 10.f, 2.f,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	iNavIdx = TARGET_TO_NAV(pInstance)->Get_CellIndex();

	// ¥¡¥Î
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Wolf",
		&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	iNavIdx = TARGET_TO_NAV(pInstance)->Get_CellIndex();

	// ¥¡¥Î
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Wolf",
		&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	iNavIdx = TARGET_TO_NAV(pInstance)->Get_CellIndex();

	// ¥¡¥Î
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Wolf",
		&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	iNavIdx = TARGET_TO_NAV(pInstance)->Get_CellIndex();


	// ¥¡¥Î
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Wolf",
		&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	iNavIdx = TARGET_TO_NAV(pInstance)->Get_CellIndex();

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

	iNavIdx = TARGET_TO_NAV(pInstance)->Get_CellIndex();

	// ¥¡¥Î
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Wolf",
		&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	iNavIdx = TARGET_TO_NAV(pInstance)->Get_CellIndex();

	// √—∞’
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji",
		&CGunGenji::INFO(CGunGenji::Normal, CGunGenji::NF_Ani::Talk, 10.f, 10.f, 2.f,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	iNavIdx = TARGET_TO_NAV(pInstance)->Get_CellIndex();

	// √—∞’
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji",
		&CGunGenji::INFO(CGunGenji::Normal, CGunGenji::NF_Ani::Talk, 10.f, 10.f, 2.f,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	iNavIdx = TARGET_TO_NAV(pInstance)->Get_CellIndex();

	//æﬂ¬˜
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_YachaMan",
		&CMonster::MONSTER_STATUS(CMonster::COLOR_NONE, WEAPON_None,
			true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(vPos[iIndex].w), 0.f), 1));
	pInstance->Set_Enable(false);
	m_vecObjectPool.push_back(pInstance);
	Safe_AddRef(pInstance);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	++iIndex;

	iNavIdx = TARGET_TO_NAV(pInstance)->Get_CellIndex();
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
	// ================================================================================================================
}

void CScriptManager::Ready_Stage02_DynamicObject()
{
}

void CScriptManager::Ready_Stage03_DynamicObject()
{
	m_vecObjectPool.reserve(20);

	_v3 vPos[20] =
	{
		//Subset0 TriggerIdx6 ø¯º˛¿Ã, ≈‰≈€, «Â≈Õ(√¢), ∞’¡ˆ(∞ÀπÊ), ∞’¡ˆ(√—)
		_v3(52.63f, -7.45f, -109.401f),
		_v3(50.24f, -7.45f, -22.575f),
		_v3(51.739f, -7.45f, -45.18f),
		_v3(43.761f, -7.45f, -45.78f),
		_v3(28.25f, -3.55f, -63.323f),

		//Subset0 TriggerIdx51 æﬂ¬˜∏«, «Â≈Õ(√—), «Â≈Õ(√—)
		_v3(34.51f, -3.55f, -89.696f),
		_v3(30.574f, -3.55f, -98.505f),
		_v3(43.386f, -3.55f, -96.611f),

		//Subset1 TriggerIdx35 ≈‰≈€, ≈‰≈€, ø¯º˛¿Ã, «Â≈Õ(∏¡ƒ°), «Â≈Õ(¥Î∞À)
		_v3(34.397f, 0.15f, -131.02f),
		_v3(37.962f, 0.15f, -129.12f),
		_v3(35.826f, 0.15f, -131.74f),
		_v3(-7.468f, 0.15f, -130.03f),
		_v3(-4.332f, 0.15f, -132.74f),

		//Subset2 TriggerIdx24 ø¯º˛¿Ã, ø¯º˛¿Ã, æﬂ¬˜∏«, «Â≈Õ(∞À)
		_v3(-10.123f, 0.15f, -99.011f),
		_v3(5.851f, 0.15f, -98.279f),
		_v3(-4.572f, 0.15f, -87.141f),
		_v3(-8.776f, 0.15f, -79.109f),
		//ø©±‚±Ó¡ˆ 17∏∂∏Æ¿« ∏ÛΩ∫≈Õ

		// =========================================
		// º≠∫Íº¬ 4 - 2∏∂∏Æ ( æÛ¿Ω∫∏Ω∫ 1 , ∫“∫∏Ω∫ 1 )
		_v3(-9.115f, 0.15f, -6.472f),
		_v3(6.557f, 0.15f, -4.748f)

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

	//=======================================================================
	//Subset0 TriggerIdx6 ø¯º˛¿Ã, ≈‰≈€, «Â≈Õ(√¢), ∞’¡ˆ(∞ÀπÊ), ∞’¡ˆ(√—)
	{
		//ø¯º˛¿Ã
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Monkey", 
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
			true, _v3(vPos[iIndex].x,vPos[iIndex].y,vPos[iIndex].z), _v3(0.f, 0.f/*D3DXToRadian(vPos[iIndex].w)*/,0.f), 3));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;

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

		// ∞ÀπÊ∞’
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji",
			&CSwordGenji::INFO(CSwordGenji::Jungle, CSwordGenji::NF_Ani::Sit1, 10.f, 10.f, 2.f,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), V3_NULL, 3));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;

		// √—∞’
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji",
			&CGunGenji::INFO(CGunGenji::Normal, CGunGenji::NF_Ani::Talk, 10.f, 10.f, 2.f,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), V3_NULL, 3));
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;
	}

	//=======================================================================
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

	//=======================================================================
	//Subset1 TriggerIdx35 ≈‰≈€, ≈‰≈€, ø¯º˛¿Ã, «Â≈Õ(∏¡ƒ°), «Â≈Õ(¥Î∞À)
	{
		// ≈‰≈€
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Cocoon",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), V3_NULL, 3));
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;

		// ≈‰≈€
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Cocoon",
			&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
				true, _v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), V3_NULL, 3));
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

	//=======================================================================
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

	// º≠∫Íº¬ 4 - 2∏∂∏Æ ( æÛ¿Ω∫∏Ω∫ 1 , ∫“∫∏Ω∫ 1 )
	//=======================================================================
	{
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_IceGirl", &CIceGirl::INFO(70.f, 20.f, 2.f, true,
			_v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(180.f), 0.f), 3));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		++iIndex;

		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_FireBoy", &CIceGirl::INFO(70.f, 20.f, 2.f, true,
			_v3(vPos[iIndex].x, vPos[iIndex].y, vPos[iIndex].z), _v3(0.f, D3DXToRadian(180.f), 0.f), 3));
		pInstance->Set_Enable(false);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
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
		// ∞°øÓµ• 4∏∂∏Æ , æ¥ˆ 1∏∂∏Æ
		_v3(8.834f, -3.9f, 73.108f),
		_v3(9.218f, -3.9f, 82.427f),
		_v3(-11.670f, -3.9f, 85.848f),
		_v3(-11.915f, -3.9f, 68.994f),
		_v3(-17.021f, 2.1f, 133.317f),
		// =========================================
		// ∫∏Ω∫ 1∏∂∏Æ
		_v3(-2.71f, -0.1f, -26.031f)
	};

	CGameObject* pInstance = nullptr;

	//=======================================================================
	// 4 ∏∂∏Æ + æ¥ˆ 1∏∂∏Æ

	{
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Hunter", &CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::BLACK, WEAPON_STATE::WEAPON_Halberd));
		TARGET_TO_NAV(pInstance)->Set_SubsetIndex(1);
		TARGET_TO_NAV(pInstance)->Set_Index(46);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);

		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Hunter", &CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::BLACK, WEAPON_STATE::WEAPON_Halberd));
		TARGET_TO_NAV(pInstance)->Set_SubsetIndex(1);
		TARGET_TO_NAV(pInstance)->Set_Index(33);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);

		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Hunter", &CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::BLACK, WEAPON_STATE::WEAPON_Halberd));
		TARGET_TO_NAV(pInstance)->Set_SubsetIndex(1);
		TARGET_TO_NAV(pInstance)->Set_Index(76);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);

		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Hunter", &CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::BLACK, WEAPON_STATE::WEAPON_Halberd));
		TARGET_TO_NAV(pInstance)->Set_SubsetIndex(1);
		TARGET_TO_NAV(pInstance)->Set_Index(66);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);

		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Hunter", &CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::BLACK, WEAPON_STATE::WEAPON_Halberd));
		TARGET_TO_NAV(pInstance)->Set_SubsetIndex(1);
		TARGET_TO_NAV(pInstance)->Set_Index(88);
		m_vecObjectPool.push_back(pInstance);
		Safe_AddRef(pInstance);
	}

	//=======================================================================
	// ∫∏Ω∫ 1∏∂∏Æ
	{
		pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_QueensKnight", &CQueensKnight::INFO(45.f, 20.f, 2.f));
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

void CScriptManager::Check_YetiTrap()
{
	for (auto& vector_iter : m_vecTrapPool)
	{
		if(false == vector_iter->Get_Enable())
			continue;

		if(false == vector_iter->Get_SummonYeti())
			continue;

		m_vecObjectPool[vector_iter->Get_YetiIndex()]->Set_Enable(true);
	}

	return;
}
