#include "stdafx.h"
#include "..\Headers\MainApp.h"
#include "Scene_Logo.h"
#include "CameraMgr.h"
#include "UI_Manager.h"
#include "Item_Manager.h"
#include "ParticleMgr.h"
#include "ScriptManager.h"
#include "ObjectPool_Manager.h"
#include "SoundManager.h"


CMainApp::CMainApp()
{
}

HRESULT CMainApp::Ready_MainApp()
{
	if (FAILED(Ready_Default_Setting(CGraphic_Device::WINMODE_WIN, g_nWinCX, g_nWinCY)))
		return E_FAIL;

	if (FAILED(Ready_Component_Prototype()))
		return E_FAIL;

	if (FAILED(Ready_Start_Scene(SCENE_LOGO)))
		return E_FAIL;

	// 볼륨조절
	g_pSoundManager->Set_Volume(CSoundManager::Master_Volume, 0.5f);

	return S_OK;
}

_int CMainApp::Update_MainApp(_double TimeDelta)
{
	if (nullptr == g_pManagement)
		return -1;
	
	m_fFrameDeltaTimer = (_float)TimeDelta;

	CCameraMgr::Get_Instance()->Update();
	CParticleMgr::Get_Instance()->Update_ParticleManager(TimeDelta);

	CObjectPool_Manager::Get_Instance()->Update_ObjectPool(TimeDelta);
	CObjectPool_Manager::Get_Instance()->LateUpdate_ObjectPool(TimeDelta);

	g_pSoundManager->Update_SoundManager();

	return g_pManagement->Update_Management(TimeDelta);
}	

void CMainApp::LateUpdate_MainApp(_double TimeDelta)
{
	m_pStageAgent->Update_StageAgent(m_pGraphic_Dev);

	Global_KeyInput();
}

HRESULT CMainApp::Render_MainApp()
{
	if (nullptr == m_pGraphic_Dev ||
		nullptr == g_pManagement ||
		nullptr == m_pRenderer)
		return E_FAIL;
	
	m_pGraphic_Dev->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DXCOLOR(0.f, 0.f, 1.f, 1.f), 1.f, 0);
	m_pGraphic_Dev->BeginScene();

	if (FAILED(m_pRenderer->Draw_RenderList()))
		return E_FAIL;

	// 씬의 렌더.(디버깅적요소, 더미)
	if (FAILED(g_pManagement->Render_Management()))
		return E_FAIL;

	if (m_bFrameRender)
		g_pManagement->Render_Sprite_Begin();

	if (m_bFrameRender)
	{
		g_pFrame_Manager->Render_Frame(m_fFrameDeltaTimer);
		g_pManagement->Render_Sprite_End();
	}

	m_pGraphic_Dev->EndScene();
	m_pGraphic_Dev->Present(nullptr, nullptr, 0, nullptr);
	
	return S_OK;
}

HRESULT CMainApp::Ready_Default_Setting(CGraphic_Device::WINMODE eMode, _ushort nWinCX, _ushort nWinCY)
{	
	if (nullptr == g_pManagement)
		return E_FAIL;

	// 엔진초기화.
	if (FAILED(g_pManagement->Ready_Engine(SCENE_END)))
		return E_FAIL;

	// 장치초기화.(For.Graphic_Device)
	if (FAILED(g_pManagement->Ready_GraphicDev(g_hWnd, eMode, nWinCX, nWinCY, &m_pGraphic_Dev)))
		return E_FAIL;
	// 장치초기화.(For.Input_Device)
	if (FAILED(g_pManagement->Ready_InputDev(g_hInst, g_hWnd)))
		return E_FAIL;

	if (FAILED(g_pManagement->Set_InputDev()))
		return E_FAIL;

	if (FAILED(g_pSoundManager->Ready_SoundManager()))
		return E_FAIL;

	CScriptManager::Get_Instance()->Ready_ScriptManager(m_pGraphic_Dev);

	m_pStageAgent = CStageAgent::Get_Instance();
	Safe_AddRef(m_pStageAgent);

	g_pSoundManager->Load_Directory_SouneFile_W(L"BGM");
	g_pSoundManager->Load_Directory_SouneFile_W(L"UI/UI_WeaponShop");

	g_pSoundManager->Stop_Sound(CSoundManager::Background_01);
	g_pSoundManager->Play_Sound(L"Title_Intro_02.wav", CSoundManager::Background_01, CSoundManager::BGM_Sound); 

	return S_OK;
}

HRESULT CMainApp::Ready_Component_Prototype()
{
	if (nullptr == g_pManagement)
		return E_FAIL;

	g_pManagement->Ready_Component_Manager(m_pGraphic_Dev);

	g_pManagement->Ready_Gizmo(m_pGraphic_Dev);

	m_pRenderer = static_cast<CRenderer*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Renderer"));

	g_pFrame_Manager->Ready_FrameShader();
	
	return S_OK;
}

HRESULT CMainApp::Ready_Start_Scene(SCENEID eScene)
{
	if (nullptr == g_pManagement)
		return E_FAIL;

	CScene*		pScene = nullptr;

	switch (eScene)
	{
	case SCENE_LOGO:
		pScene = CScene_Logo::Create(m_pGraphic_Dev);
		break;

	case SCENE_STAGE:
		/*pScene = CScene_Logo::Create(m_pGraphic_Device);*/
		break;
	}

	if (nullptr == pScene)
		return E_FAIL;

	if (FAILED(g_pManagement->SetUp_CurrentScene(pScene)))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Global_KeyInput()
{
	// 자유 시점 카메라
	if (g_pInput_Device->Key_Down(DIK_NUMPAD0))
	{
		if(g_bActiveCam)
			CCameraMgr::Get_Instance()->Set_CamView(TOOL_VIEW);
	}

	// 3인칭 카메라
	if (g_pInput_Device->Key_Down(DIK_NUMPAD1))
	{
		if(g_bActiveCam)
			CCameraMgr::Get_Instance()->Set_CamView(BACK_VIEW);
	}

	// All Gizmo's Toggle On / Off
	if (g_pInput_Device->Key_Down(DIK_NUMPAD6))
	{
		g_pManagement->Gizmo_Toggle();
	}

	// Cell Gizmo's Toggle On / Off
	if (g_pInput_Device->Key_Down(DIK_NUMPAD7))
	{
		g_pManagement->Gizmo_CellEnable();
	}

	// Collider Gizmo Toggle On / Off
	if (g_pInput_Device->Key_Down(DIK_NUMPAD8))
	{
		g_pManagement->Gizmo_ColliderEnable();
	}

	// Frame Toggle On / Off
	if (g_pInput_Device->Key_Down(DIK_SLASH))
	{
		m_bFrameRender = m_bFrameRender ? false : true;
	}
}

CMainApp * CMainApp::Create()
{
	CMainApp*	pInstance = new CMainApp;

	if (FAILED(pInstance->Ready_MainApp()))
	{
		MSG_BOX("Failed To Creating CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;	
}

void CMainApp::Free()
{
	CObjectPool_Manager::Get_Instance()->Destroy_Instance();

	Safe_Release(m_pStageAgent);

	Safe_Release(g_pDissolveTexture);

	Safe_Release(m_pGraphic_Dev);
	
	CScriptManager::Get_Instance()->Destroy_Instance();
	CParticleMgr::Get_Instance()->Destroy_Instance();
	//CObjectPool_Manager::Get_Instance()->Destroy_Instance();

	Safe_Release(m_pRenderer);
	
	CCameraMgr::Get_Instance()->Destroy_Instance();
	CUI_Manager::Get_Instance()->Destroy_Instance();
	CItem_Manager::Get_Instance()->Destroy_Instance();
	
	Safe_Release(g_pManagement);
	
	CManagement::Release_Engine();
}



