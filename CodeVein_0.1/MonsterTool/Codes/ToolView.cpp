#include "stdafx.h"
#ifndef SHARED_HANDLERS
#include "Tool.h"
#endif
#include "ToolDoc.h"
#include "ToolView.h"
#include "MainFrm.h"
#include "SetingView.h"
#include "MonsterTool.h"

#include "Management.h"
#include "Terrain_Guide.h"
#include "CollisionMgr.h"
#include "Timer_Manager.h"
#include "FrameMgr.h"
#include "CameraMgr.h"
#include "Terrain_Guide.h"

#include "TestMonster.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HWND					g_hWnd;
HINSTANCE				g_hInst;
Engine::RAY				g_Ray;
_mat					g_matWorld;
_v3						g_MousePos;
_float					g_fCross;
_float					g_fWorld_Y = 0.f;

IMPLEMENT_DYNCREATE(CToolView, CView)

CToolView::CToolView()
{
}

CToolView::~CToolView()
{
	for (auto& list_iter : m_listMeshPathInfo)
		Engine::Safe_Delete(list_iter);

	m_listMeshPathInfo.clear();
	
	for (auto& list_iter : m_listWeaponPathInfo)
		Engine::Safe_Delete(list_iter);
	
	m_listWeaponPathInfo.clear();

	Release();
}

void CToolView::OnDraw(CDC* /*pDC*/)
{
	return;
}

CToolDoc* CToolView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CToolDoc)));
	return (CToolDoc*)m_pDocument;
}

#ifdef _DEBUG
void CToolView::AssertValid() const
{
	CView::AssertValid();
}

void CToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

void CToolView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	g_hWnd = m_hWnd;
	g_hInst = AfxGetInstanceHandle();

	CMainFrame* pMainFrm = static_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());

	RECT rcMain = {};
	pMainFrm->GetWindowRect(&rcMain);
	::SetRect(&rcMain, 0, 0, rcMain.right - rcMain.left, rcMain.bottom - rcMain.top);

	RECT rcView = {};
	GetClientRect(&rcView);

	_int iGapX = rcMain.right - rcView.right; 
	_int iGapY = rcMain.bottom - rcView.bottom;

	pMainFrm->SetWindowPos(	nullptr, 0, 0, WINCX + iGapX, 900 + iGapY, SWP_NOZORDER);

	CManagement* pManagement = CManagement::Get_Instance();
	IF_NULL_RETURN(pManagement);

	CTimer_Manager::Get_Instance()->initialize_Timer(L"FrameTime");
	CTimer_Manager::Get_Instance()->initialize_Timer(L"Timer_Fps_60");
	pManagement->Ready_Engine(SCENE_END);
	pManagement->Ready_GraphicDev(g_hWnd, CGraphic_Device::WINMODE_WIN, WINCX, WINCY, &m_pDevice);
	pManagement->Ready_Component_Manager(m_pDevice);
	pManagement->Ready_Gizmo(m_pDevice);

	CInput_Device::Get_Instance()->Ready_Input_Dev(g_hInst, g_hWnd);
	CInput_Device::Get_Instance()->Set_InputDev();

	_tchar szDynamicExtractPath[STR_128] = L"";
	_tchar szDynamicSavePath[STR_128] = L"";
	_tchar szWeaponExtractPath[STR_128] = L"";
	_tchar szWeaponSavePath[STR_128] = L"";
	lstrcpy(szDynamicExtractPath, L"..\\..\\Client\\Resources\\Mesh\\DynamicMesh");
	lstrcpy(szDynamicSavePath, L"../../Data/Load_MeshData/Mesh_Dynamic_Path.dat");
	lstrcpy(szWeaponExtractPath, L"..\\..\\Client\\Resources\\Mesh\\Weapons");
	lstrcpy(szWeaponSavePath, L"../../Data/Load_MeshData/Mesh_Weapon_Path.dat");

	Create_Mesh_PathInfo(szDynamicExtractPath, szDynamicSavePath, szWeaponExtractPath, szWeaponSavePath);

	pManagement->LoadMesh_FromPath(m_pDevice, szDynamicSavePath);
	pManagement->LoadMesh_FromPath(m_pDevice, szWeaponSavePath);
	CCameraMgr::Get_Instance()->Reserve_ContainerSize(2);
	CCameraMgr::Get_Instance()->Ready_Camera(m_pDevice, DYNAMIC_CAM, L"Tool_FreeCam", TOOL_VIEW, DEFAULT_MODE);
	CCameraMgr::Get_Instance()->Set_MainCamera(DYNAMIC_CAM, L"Tool_FreeCam");
	CCameraMgr::Get_Instance()->Set_MainPos(_v3{ 0,3,-5 });

	m_pGreed = Engine::CTerrain_Guide::Create(m_pDevice);

	m_pMainfrm = static_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	m_pSetingView = static_cast<CSetingView*>(m_pMainfrm->m_MainSplitter.GetPane(0, 1));

	return;
}

BEGIN_MESSAGE_MAP(CToolView, CView)
END_MESSAGE_MAP()

CString CToolView::Convert_RelativePath(const _tchar * pFullPath)
{
	_tchar szCurrentDir[STR_128] = L"";
	::GetCurrentDirectory(STR_128, szCurrentDir);

	_tchar szRelativePath[STR_128] = L"";
	::PathRelativePathTo(szRelativePath, szCurrentDir, FILE_ATTRIBUTE_DIRECTORY, pFullPath, FILE_ATTRIBUTE_DIRECTORY);

	return CString(szRelativePath);
}

void CToolView::Create_Mesh_PathInfo(const _tchar* szDynamicExtractPath, const _tchar* szDynamicSavePath,
									const _tchar* szWeaponExtractPath, const _tchar* szWeaponSavePath)
{
	cout << "DynamicMesh 추출 . . ." << endl;
	cout << endl;
	Extract_Mesh_PathInfo(szDynamicExtractPath, m_listMeshPathInfo, true);

	cout << endl;
	cout << "DynamicMesh 추출 완료 . . ." << endl;
	Save_Mesh_PathInfo(m_listMeshPathInfo, szDynamicSavePath);

	cout << "WeaponMesh 추출 . . ." << endl;
	cout << endl;
	Extract_Mesh_PathInfo(szWeaponExtractPath, m_listWeaponPathInfo, false);

	cout << endl;
	cout << "WeaponMesh 추출 완료 . . ." << endl;
	Save_Mesh_PathInfo(m_listWeaponPathInfo, szWeaponSavePath);

	return;
}

void CToolView::Extract_Mesh_PathInfo(const _tchar * pPath, list<MESH_INFO*>& rPathInfoLst, _bool _bIsDynamic)
{
	wstring wstrFindPath = wstring(pPath) + L"\\*";

	CFileFind find;

	BOOL bIsFind = find.FindFile(wstrFindPath.c_str());

	while (bIsFind)
	{
		bIsFind = find.FindNextFile();

		if (find.IsDots())
			continue;

		else if (find.IsDirectory())
		{
			Extract_Mesh_PathInfo(find.GetFilePath(), rPathInfoLst, _bIsDynamic);
		}
		else
		{
			if (find.IsSystem())
				continue;

			CString strFullPath = find.GetFilePath();

			if (strFullPath.Find(L".X") == -1)
				continue;

			CString strRelative = Convert_RelativePath(strFullPath);

			Engine::MESH_INFO* pPathInfo = new Engine::MESH_INFO;

			_tchar szBuf[STR_128] = L"";
			lstrcpy(szBuf, strRelative);

			// 폴더까지의 경로
			::PathRemoveFileSpec(szBuf);
			wstring tmpPathString = szBuf;
			pPathInfo->wstrImgPath = tmpPathString + L"\\";

			// 파일 이름
			lstrcpy(szBuf, strRelative);
			::PathRemoveBackslash(szBuf);
			wstring tmpString = ::PathFindFileName(szBuf);

			// 파일명
			pPathInfo->wstrFileName = tmpString;

			PathRemoveExtension(szBuf);
			tmpString = PathFindFileName(szBuf);

			// 키 값
			pPathInfo->wstrStateKey = L"Mesh_" + tmpString;

			pPathInfo->bIsDynamic = _bIsDynamic;

			rPathInfoLst.push_back(pPathInfo);

			wcout << pPathInfo->wstrStateKey<<" 를 추출했습니다" << endl;
		}
	}

	return;
}

void CToolView::Save_Mesh_PathInfo(list<MESH_INFO*>& rPathInfoLst, const _tchar* szPath)
{
	HANDLE hFile = ::CreateFile(szPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

	DWORD dwByte = 0;

	for (auto& pPathInfo : rPathInfoLst)
	{
		if (INVALID_HANDLE_VALUE == hFile)
			::MSG_BOX("Save Failed. [INVALID_HANDLE_VALUE]");

		const _tchar* szKey = pPathInfo->wstrStateKey.c_str();
		const _tchar* szFile = pPathInfo->wstrFileName.c_str();
		const _tchar* szPath = pPathInfo->wstrImgPath.c_str();
		const _bool bDynamic = pPathInfo->bIsDynamic;

		::WriteFile(hFile, szKey, sizeof(_tchar) * STR_128, &dwByte, nullptr);
		::WriteFile(hFile, szFile, sizeof(_tchar) * STR_128, &dwByte, nullptr);
		::WriteFile(hFile, szPath, sizeof(_tchar) * STR_128, &dwByte, nullptr);
		::WriteFile(hFile, &bDynamic, sizeof(_bool), &dwByte, nullptr);

		wcout << pPathInfo->wstrStateKey<<"를 저장했습니다" << endl;
		if (rPathInfoLst.size() == 1)
			break;
	}

	CloseHandle(hFile);

	cout << "저장 완료 . . . !" << endl;
	cout << endl;

	return;
}

void CToolView::Start()
{
	CTimer_Manager::Get_Instance()->Set_DeltaTime(L"FrameTime");

	if (CFrameMgr::Get_Instance()->Lock_Frame(60.f, CTimer_Manager::Get_Instance()->Get_DeltaTime(L"FrameTime")))
	{
		CTimer_Manager::Get_Instance()->Set_DeltaTime(L"Timer_Fps_60");

		Update(DELTA_60);
		LateUpdate(DELTA_60);
		Render();
	}
}

void CToolView::Update(const _float & fTimeDelta)
{
	CCameraMgr::Get_Instance()->Update();
	CInput_Device::Get_Instance()->Set_InputDev();

	KeyDown();
	KeyUp();

	m_fTime += fTimeDelta;
	m_dwRenderCnt++;

	m_pSetingView->m_pMonsterTool->Update(fTimeDelta);

	return;
}

void CToolView::LateUpdate(const _float & fTimeDelta)
{
	if (m_fTime >= 1.f)
	{
		wsprintf(m_szFPS, L"심선생 몬스터교실 FPS: %d", m_dwRenderCnt);
		m_pMainfrm->SetWindowTextW(m_szFPS);
		m_fTime = 0.f;
		m_dwRenderCnt = 0;
	}

	m_pSetingView->m_pMonsterTool->LateUpdate(fTimeDelta);

	return;
}

void CToolView::Render()
{
	m_pDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f), 1.f, 0);
	m_pDevice->BeginScene();

	m_pGreed->Render_GameObject();
	m_pSetingView->m_pMonsterTool->Render();
	
	m_pDevice->EndScene();
	m_pDevice->Present(nullptr, nullptr, 0, nullptr);

	return;
}

void CToolView::KeyUp()
{
	if (Engine::CInput_Device::Get_Instance()->Key_Up(DIK_SPACE))
	{
		Engine::CCameraMgr::Get_Instance()->Set_MouseCtrl(false);
	}
}

void CToolView::KeyDown()
{

	if (Engine::CInput_Device::Get_Instance()->Key_Down(DIK_DELETE))
	{
	}

	if (Engine::CInput_Device::Get_Instance()->Key_Down(DIK_NUMPAD6))
	{
		// 기즈모 On / Off
		CManagement::Get_Instance()->Gizmo_Toggle();
	}

	if (Engine::CInput_Device::Get_Instance()->Key_Down(DIK_NUMPAD7))
	{
		// 기즈모 On / Off
		CManagement::Get_Instance()->Gizmo_CellEnable();
	}

	if (Engine::CInput_Device::Get_Instance()->Key_Down(DIK_NUMPAD8))
	{
		// 기즈모 On / Off
		CManagement::Get_Instance()->Gizmo_ColliderEnable();
	}

	if (Engine::CInput_Device::Get_Instance()->Get_DIMouseState(Engine::CInput_Device::DIM_LB))
	{
		if (Engine::CInput_Device::Get_Instance()->Get_MousePos().x <= 0 || Engine::CInput_Device::Get_Instance()->Get_MousePos().x >= WINCX)
			return;

		if (Engine::CInput_Device::Get_Instance()->Get_MousePos().y <= 0 || Engine::CInput_Device::Get_Instance()->Get_MousePos().y >= WINCY)
			return;

		_v3 vMouse = g_Ray.vRayPos + g_Ray.vRayDir * g_fCross;

	}

	if (Engine::CInput_Device::Get_Instance()->Key_Down(DIK_SPACE))
	{
		Engine::CCameraMgr::Get_Instance()->Set_MouseCtrl(true);
		CInput_Device::Get_Instance()->Calc_MouseLockPos();
	}
}

void CToolView::Release()
{
	m_pSetingView->m_pMonsterTool->Release();

	Engine::CCameraMgr::Get_Instance()->Destroy_Instance();
	Engine::CFrameMgr::Get_Instance()->Destroy_Instance();
	Engine::CTimer_Manager::Get_Instance()->Destroy_Instance();

	Safe_Release(m_pDevice);
	Safe_Release(m_pGreed);

	Engine::CManagement::Release_Engine();
}
