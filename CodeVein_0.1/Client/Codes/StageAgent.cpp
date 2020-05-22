#include "stdafx.h"
#include "StageAgent.h"

#include "Scene_Stage_Base.h"
#include "Scene_Stage_Training.h"
#include "Scene_Stage_01.h"
#include "Scene_Stage_02.h"
#include "Scene_Stage_03.h"
#include "Scene_Stage_04.h"

#include "ScriptManager.h"

IMPLEMENT_SINGLETON(CStageAgent)

CStageAgent::CStageAgent()
{
}

void CStageAgent::Change_Stage(_Device _pGraphicDev)
{
	// 스테이지 조명 삭제
	g_pManagement->Clear_LightList();
	
	// 씬 클리어
	g_pManagement->Clear_Instance(SCENE_STAGE);
	

	CScene* pScene = 
		(g_eSceneID_Cur == SCENE_STAGE_BASE ? (CScene*)CScene_Stage_Base::Create(_pGraphicDev , true) :
			g_eSceneID_Cur == SCENE_STAGE_01 ? (CScene*)CScene_Stage_01::Create(_pGraphicDev, true) :
			g_eSceneID_Cur == SCENE_STAGE_02 ? (CScene*)CScene_Stage_02::Create(_pGraphicDev, true) :
			g_eSceneID_Cur == SCENE_STAGE_03 ? (CScene*)CScene_Stage_03::Create(_pGraphicDev, true) :
			g_eSceneID_Cur == SCENE_STAGE_04 ? (CScene*)CScene_Stage_04::Create(_pGraphicDev, true) : (CScene*)CScene_Stage_Training::Create(_pGraphicDev, true));
	
	//if (m_sOldStageIdx != g_eSceneID_Cur)
	//{
		m_sOldStageIdx = g_eSceneID_Cur;

		_short eStageIdx = 
			g_eSceneID_Cur == SCENE_STAGE_01 ? 1 :
			g_eSceneID_Cur == SCENE_STAGE_02 ? 2 :
			g_eSceneID_Cur == SCENE_STAGE_03 ? 3 :
			g_eSceneID_Cur == SCENE_STAGE_04 ? 4 : 999;

			CScriptManager::Get_Instance()->Set_StageIdx(eStageIdx);
			CScriptManager::Get_Instance()->Reset_ScriptEvent(eStageIdx, true);
			CScriptManager::Get_Instance()->Reset_Script_DynmicObject();
			CScriptManager::Get_Instance()->Ready_Script_DynamicObject(eStageIdx);

	// 씬 세팅
	g_pManagement->SetUp_CurrentScene(pScene);
}

void CStageAgent::Update_StageAgent(_Device _pGraphicDev)
{
	// 바꾸는게 예약되있으면
	if (m_bReserveChangeStage)
	{
		m_bReserveChangeStage = false;
		Change_Stage(_pGraphicDev);
	}
}

void CStageAgent::Reserve_ChangeStage()
{
	m_bReserveChangeStage = true;
}

void CStageAgent::Free()
{
}
