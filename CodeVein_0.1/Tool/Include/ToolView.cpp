
// ToolView.cpp : CToolView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "Tool.h"
#endif

#include "ToolDoc.h"
#include "ToolView.h"
#include "MainFrm.h"

#include "Inspector_Form.h"
#include "Inspector_Sheet.h"
#include "Essential_Sheet.h"
#include "Inspector_Page.h"
#include "Pivot_Page.h"
#include "Management.h"

#include "Terrain_Guide.h"
#include "CollisionMgr.h"
#include "Timer_Manager.h"
#include "FrameMgr.h"
#include "CameraMgr.h"

// 전역변수
HWND g_hWnd;

// CToolView

IMPLEMENT_DYNCREATE(CToolView, CView)

BEGIN_MESSAGE_MAP(CToolView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CToolView 생성/소멸

CToolView::CToolView()
	:
	m_fCrossLength(5.f), m_fConvertLength(0.f),
	m_vSelectMove(),
	m_bObjMouse(false), m_bSelectObj(false), m_bSelect(false),
	m_bActive_Pos_x(true), m_bActive_Pos_y(true), m_bActive_Pos_z(true),
	m_bActive_NevMesh(false), 
	m_bNavMesh_Mode(false),
	m_bAutoPick(false),
	m_bLateInit(false)
{
}

CToolView::~CToolView()
{
	//m_pPage_Inspecter->free();
	//m_pPage_MapTool->Free();
	//m_pPage_Navmesh->Free();

	CMainFrame* pMainFrm = static_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	CInspector_Form* pInspector = static_cast<CInspector_Form*>(pMainFrm->m_MainSplitter.GetPane(0, 1));
	//CEssential_Sheet* pEsnSheet = static_cast<CEssential_Sheet*>(pInspector->m_pEssential_Sheet);

	//pEsnSheet->Free();
	pInspector->Free();

	Free();
}

BOOL CToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CToolView 그리기

void CToolView::OnDraw(CDC* /*pDC*/)
{
	CToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CToolView 인쇄

BOOL CToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CToolView 진단

#ifdef _DEBUG
void CToolView::AssertValid() const
{
	CView::AssertValid();
}

void CToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CToolDoc* CToolView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CToolDoc)));
	return (CToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CToolView 메시지 처리기


void CToolView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	g_hWnd = m_hWnd;

	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());

	// GetWindowRect: 윈도우의 모든 테두리를 포함한 사각형 크기를 얻어온다.
	// 이 때 좌표는 스크린 좌표 기준이다.
	RECT rcMain = {};
	pMainFrm->GetWindowRect(&rcMain); // MainFrame의 크기를 얻어옴.

									  // 이렇게 얻어온 Main의 left, top을 0, 0기준으로 셋팅.
	::SetRect(&rcMain, 0, 0, rcMain.right - rcMain.left, rcMain.bottom - rcMain.top);

	// GetClientRect: 윈도우의 테두리를 제외한 순수 클라이언트 영역의 사각형 크기를 얻어온다.
	// 이 때 좌표는 클라이언트 좌표 기준이다. 이 때 left, top은 무조건 0, 0부터 시작.
	RECT rcView = {};
	GetClientRect(&rcView);

	// MainFrm과 View의 간격을 구한다.
	int iGapX = rcMain.right - rcView.right;
	int iGapY = rcMain.bottom - rcView.bottom;

	// MainFrame의 크기 조정.
	pMainFrm->SetWindowPos(
		nullptr, 0, 0, 1610, 720, SWP_NOZORDER);

	Setup_Default();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}

void CToolView::Setup_Default()
{
	CTimer_Manager::Get_Instance()->initialize_Timer(L"FrameTime");
	CTimer_Manager::Get_Instance()->initialize_Timer(L"Timer_Fps_60");
	CManagement::Get_Instance()->Ready_Engine(SCENE_END);
	CManagement::Get_Instance()->Ready_GraphicDev(g_hWnd, CGraphic_Device::WINMODE_WIN, WINCX, WINCY, &g_pGraphicDev);
	CManagement::Get_Instance()->Ready_Component_Manager(g_pGraphicDev);

	CInput_Device::Get_Instance()->Ready_Input_Dev(AfxGetInstanceHandle(), g_hWnd);
	CInput_Device::Get_Instance()->Set_InputDev();

	CManagement::Get_Instance()->LoadMesh_FromPath(g_pGraphicDev, L"../../Data/Mesh_Path.dat");

	SetUp_Cam();
	SetUp_Layer();

	g_pGraphicDev->GetTransform(D3DTS_WORLD, &g_matWorld);
}

void CToolView::SetUp_Layer()
{
	m_pGreedTerrain = Engine::CTerrain_Guide::Create(g_pGraphicDev);
	Engine::CCollider* tmpCol = TARGET_TO_COL(m_pGreedTerrain);

	tmpCol->Set_Radius(_v3{ 1000.f , 1.f , 1000.f });
	tmpCol->Set_CenterPos(TARGET_TO_TRANS(m_pGreedTerrain)->Get_Pos() - _v3{ 0, TARGET_TO_COL(m_pGreedTerrain)->Get_Radius().y,0 });
	tmpCol->SetUp_Box();
	tmpCol->Set_Type(COL_AABB);
}

void CToolView::SetUp_Cam()
{
	CCameraMgr::Get_Instance()->Reserve_ContainerSize(2);
	CCameraMgr::Get_Instance()->Ready_Camera(g_pGraphicDev, DYNAMIC_CAM, L"Tool_FreeCam", TOOL_VIEW, DEFAULT_MODE);
	CCameraMgr::Get_Instance()->Set_MainCamera(DYNAMIC_CAM, L"Tool_FreeCam");
	CCameraMgr::Get_Instance()->Set_MainPos(_v3{ 0,3,-5 });
}

void CToolView::Function()
{
	CTimer_Manager::Get_Instance()->Set_DeltaTime(L"FrameTime");
	
	if (CFrameMgr::Get_Instance()->Lock_Frame(60.f, CTimer_Manager::Get_Instance()->Get_DeltaTime(L"FrameTime")))
	{
		CTimer_Manager::Get_Instance()->Set_DeltaTime(L"Timer_Fps_60");
	
		_float fFrame = DELTA_60;
	
		Global_KeyState();
	
		Update();
		Render();
	}
}


void CToolView::Update()
{
	g_Ray = Engine::CInput_Device::Get_Instance()->Get_Ray(RAY_TO_WORLD);

	LateInit();
	Check_ActivePage();

	m_pPage_Inspecter->Update_MousePos();
	CCameraMgr::Get_Instance()->Update();
	CInput_Device::Get_Instance()->Set_InputDev();

	if(m_bActive_Object)
		m_pPage_MapTool->Update();

	Selected_Object();
	Selected_Transform();

	Change_Data();
}

void CToolView::LateInit()
{
	if (!m_bLateInit)
	{
		Get_Pages();

		m_bLateInit = true;
	}
}

void CToolView::Render()
{
	g_pGraphicDev->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f), 1.f, 0);
	g_pGraphicDev->BeginScene();

	m_pGreedTerrain->Render_GameObject();

	m_pPage_Navmesh->Render();
	m_pPage_MapTool->Render();

	g_pGraphicDev->EndScene();
	g_pGraphicDev->Present(nullptr, nullptr, 0, nullptr);
}

void CToolView::Selected_Object()
{
	if (m_bActive_NevMesh)
	{
		Engine::CCollisionMgr::Collision_Ray(m_pGreedTerrain, g_Ray, &g_fCross);

		_v3 tmpV3 = g_Ray.vRayPos + g_Ray.vRayDir * g_fCross;
		g_MousePos = tmpV3;

		m_pPage_MapTool->Set_MousePos(tmpV3);
	}

	else if (!m_pPage_MapTool->CheckObject())
	{
		Engine::CCollisionMgr::Collision_Ray(m_pGreedTerrain, g_Ray, &g_fCross);

		_v3 tmpV3 = g_Ray.vRayPos + g_Ray.vRayDir * g_fCross;
		g_MousePos = tmpV3;

		m_pPage_MapTool->Set_MousePos(tmpV3);
	}

	else
	{
		Engine::CCollisionMgr::Collision_Ray(m_pGreedTerrain, g_Ray, &g_fCross);

		_v3 tmpV3 = g_Ray.vRayPos + g_Ray.vRayDir * g_fCross;
		g_MousePos = tmpV3;

		m_pPage_MapTool->Set_MousePos(tmpV3);
	}
}

void CToolView::Selected_Transform()
{
	//if (m_pSelected == nullptr)
	//	return;

	(m_bActive_Pos_x && m_bActive_Pos_y && m_bActive_Pos_z ? 
		Trans_Pivot() : 
		Trans_Axis());
}

void CToolView::Trans_Pivot()
{
	_v3 vMouse = g_Ray.vRayPos + g_Ray.vRayDir * g_fCross;

	//TARGET_TO_TRANS(m_pSelected)->Set_Pos(vMouse);
}

void CToolView::Trans_Axis()
{
	//_v3	   vPos, vMouse, vResult;
	//_float fLength;
	//
	//vPos	= TARGET_TO_TRANS(m_pSelected)->Get_Pos();
	//fLength = D3DXVec3Length(&(m_tRay.vRayPos - vPos));
	//vMouse	= m_tRay.vRayPos + m_tRay.vRayDir * fLength;
	//
	//(m_bActive_Pos_x ? vResult.x = vMouse.x : vResult.x = vPos.x);
	//(m_bActive_Pos_y ? vResult.y = vMouse.y : vResult.y = vPos.y);
	//(m_bActive_Pos_z ? vResult.z = vMouse.z : vResult.z = vPos.z);
	//
	//TARGET_TO_TRANS(m_pSelected)->Set_Pos(vResult);
}

void CToolView::Check_ActivePage()
{
	CMainFrame* pMainFrm = static_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	CInspector_Form* pInspector = static_cast<CInspector_Form*>(pMainFrm->m_MainSplitter.GetPane(0, 1));
	CInspector_Sheet* pInsSheet = static_cast<CInspector_Sheet*>(pInspector->m_Inspector_Sheet);

	if (pInsSheet->GetActivePage() == m_pPage_Navmesh)
	{
		if (m_bActive_Object)
		{
			m_pPage_MapTool->Delete_MouseObj();
			m_bActive_Object = false;
		}
		m_bActive_NevMesh = true;
	}

	else if (pInsSheet->GetActivePage() == m_pPage_MapTool)
	{
		m_bActive_Object = true;
		m_bActive_NevMesh = false;
	}

	////cout << m_bActive_NevMesh << endl;
}

void CToolView::Get_Pages()
{
	CMainFrame* pMainFrm = static_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	CInspector_Form* pInspector = static_cast<CInspector_Form*>(pMainFrm->m_MainSplitter.GetPane(0, 1));
	CInspector_Sheet* pInsSheet = static_cast<CInspector_Sheet*>(pInspector->m_Inspector_Sheet);
	CEssential_Sheet* pEsnSheet = static_cast<CEssential_Sheet*>(pInspector->m_pEssential_Sheet);

	m_pPage_Inspecter = static_cast<CInspector_Page*>(pEsnSheet->GetPage(0));
	//m_pPage_Pivot = static_cast<CPivot_Page*>(pEsnSheet->GetPage(1));

	m_pPage_Navmesh = static_cast<CNavMesh_Page*>(pInsSheet->GetPage(0));
	m_pPage_MapTool = static_cast<CMapTool_Page*>(pInsSheet->GetPage(1));
}

void CToolView::Change_Data()
{
	_float fWorkY = m_pPage_Inspecter->Get_WorkY();

	TARGET_TO_TRANS(m_pGreedTerrain)->Set_Pos(_v3{0.f ,fWorkY, 0.f});
}

void CToolView::Set_RendPivot()
{
	//m_pPage_MapTool->Set_RendPivot_X(m_pPage_Pivot->Get_RenderPivotX());
	//m_pPage_MapTool->Set_RendPivot_Y(m_pPage_Pivot->Get_RenderPivotY());
	//m_pPage_MapTool->Set_RendPivot_Z(m_pPage_Pivot->Get_RenderPivotZ());
}

void CToolView::Global_KeyState()
{
	Global_KeyDown();
	Global_KeyUp();
}

void CToolView::Global_KeyUp()
{
	if (Engine::CInput_Device::Get_Instance()->Key_Up(DIK_SPACE))
	{
		Engine::CCameraMgr::Get_Instance()->Set_MouseCtrl(false);
	}
}

void CToolView::Global_KeyDown()
{

	if (Engine::CInput_Device::Get_Instance()->Key_Down(DIK_DELETE))
	{
		if(m_bActive_NevMesh)
			m_pPage_Navmesh->Delete_SelectNav();

		if (m_bActive_Object)
			m_pPage_MapTool->Delete_SelectObject();
	}

	// Util
//////////////////////////////////////////////////////////////////////////////////////

	if (Engine::CInput_Device::Get_Instance()->Key_Down(DIK_Q))
	{
		// 기즈모 On / Off
		CManagement::Get_Instance()->Gizmo_Enable();
	}

	if (Engine::CInput_Device::Get_Instance()->Key_Down(DIK_R))
	{
		m_pPage_Inspecter->Add_WorkY(TRUE);
	}

	if (Engine::CInput_Device::Get_Instance()->Key_Down(DIK_F))
	{
		m_pPage_Inspecter->Add_WorkY(FALSE);
	}

	//	Mouse
//////////////////////////////////////////////////////////////////////////////////////
	if (Engine::CInput_Device::Get_Instance()->Get_DIMouseState(Engine::CInput_Device::DIM_LB))
	{
		if (Engine::CInput_Device::Get_Instance()->Get_MousePos().x <= 0 || Engine::CInput_Device::Get_Instance()->Get_MousePos().x >= WINCX)
			return;

		if (Engine::CInput_Device::Get_Instance()->Get_MousePos().y <= 0 || Engine::CInput_Device::Get_Instance()->Get_MousePos().y >= WINCY)
			return;

		_v3 vMouse = g_Ray.vRayPos + g_Ray.vRayDir * g_fCross;

		if (m_bActive_NevMesh)
		{
			m_pPage_Navmesh->Create_Vertex(m_pGraphicDev , vMouse);
		}

		else if (m_bActive_Object)
		{
			switch (m_pPage_MapTool->Get_EditMode())
			{
			case CMapTool_Page::_SELECT :
			{
				m_pPage_MapTool->Check_SelectObj();
				break;
			}

			case CMapTool_Page::_CREATE:
			{
				m_pPage_MapTool->CreateObject();
				break;
			}
			}
		}
	}

	//	Specific
//////////////////////////////////////////////////////////////////////////////////////

	if (CInput_Device::Get_Instance()->Key_Down(DIK_SPACE))
	{
		Engine::CCameraMgr::Get_Instance()->Set_MouseCtrl(true);
		CInput_Device::Get_Instance()->Calc_MouseLockPos();
	}
}

void CToolView::Free()
{
	Engine::CCameraMgr::Get_Instance()->Destroy_Instance();

	Engine::CFrameMgr::Get_Instance()->Destroy_Instance();
	Engine::CTimer_Manager::Get_Instance()->Destroy_Instance();

	Engine::Safe_Release(g_pGraphicDev);

	Engine::Safe_Release(m_pGreedTerrain);

	Engine::CManagement::Release_Engine();
}
