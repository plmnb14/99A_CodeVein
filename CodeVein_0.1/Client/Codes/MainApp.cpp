#include "stdafx.h"
#include "..\Headers\MainApp.h"
#include "Scene_Logo.h"
#include "CameraMgr.h"
#include "UI_Manager.h"
#include "Item_Manager.h"

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

	return S_OK;
}

_int CMainApp::Update_MainApp(_double TimeDelta)
{
	if (nullptr == g_pManagement)
		return -1;
	
	CCameraMgr::Get_Instance()->Update();
	
	return g_pManagement->Update_Management(TimeDelta);
}	

void CMainApp::LateUpdate_MainApp(_double TimeDelta)
{
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


	return S_OK;
}

HRESULT CMainApp::Ready_Component_Prototype()
{
	if (nullptr == g_pManagement)
		return E_FAIL;

	g_pManagement->Ready_Component_Manager(m_pGraphic_Dev);
	g_pManagement->LoadTex_FromPath(m_pGraphic_Dev, L"../../Data/Tex_Path.dat");
	g_pManagement->LoadMesh_FromPath(m_pGraphic_Dev, L"../../Data/Mesh_Path.dat");
	g_pManagement->Ready_Gizmo(m_pGraphic_Dev);

	m_pRenderer = static_cast<CRenderer*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Renderer"));
	//Safe_AddRef(m_pRenderer);
	
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
		CCameraMgr::Get_Instance()->Set_CamView(TOOL_VIEW);
	}

	// 3인칭 카메라
	if (g_pInput_Device->Key_Down(DIK_NUMPAD1))
	{
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
	Safe_Release(m_pGraphic_Dev);

	Safe_Release(m_pRenderer);
	
	CCameraMgr::Get_Instance()->Destroy_Instance();
	CUI_Manager::Get_Instance()->Destroy_Instance();
	CItem_Manager::Get_Instance()->Destroy_Instance();
	
	Safe_Release(g_pManagement);
	
	CManagement::Release_Engine();
}



