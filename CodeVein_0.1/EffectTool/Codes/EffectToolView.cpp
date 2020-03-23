
// EffectToolView.cpp : CEffectToolView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "EffectTool.h"
#endif

#include "EffectToolDoc.h"
#include "EffectToolView.h"
#include "MainFrm.h"
#include "MyFormView.h"

#include "Management.h"
#include "Terrain_Guide.h"
#include "CollisionMgr.h"
#include "Timer_Manager.h"
#include "FrameMgr.h"
#include "CameraMgr.h"
#include "Terrain_Guide.h"

#include "TexEffect.h"
#include "MeshEffect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ��������
HINSTANCE g_hInst;
HWND g_hWnd;

// CEffectToolView

IMPLEMENT_DYNCREATE(CEffectToolView, CView)

BEGIN_MESSAGE_MAP(CEffectToolView, CView)
	// ǥ�� �μ� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CEffectToolView ����/�Ҹ�

CEffectToolView::CEffectToolView()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CEffectToolView::~CEffectToolView()
{
	Release();
}

BOOL CEffectToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CView::PreCreateWindow(cs);
}

// CEffectToolView �׸���

void CEffectToolView::OnDraw(CDC* /*pDC*/)
{
	CEffectToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
}


// CEffectToolView �μ�

BOOL CEffectToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �⺻���� �غ�
	return DoPreparePrinting(pInfo);
}

void CEffectToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ��ϱ� ���� �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
}

void CEffectToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ� �� ���� �۾��� �߰��մϴ�.
}


// CEffectToolView ����

#ifdef _DEBUG
void CEffectToolView::AssertValid() const
{
	CView::AssertValid();
}

void CEffectToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

void CEffectToolView::Update()
{
	CTimer_Manager::Get_Instance()->Set_DeltaTime(L"FrameTime");

	if (CFrameMgr::Get_Instance()->Lock_Frame(60.f, CTimer_Manager::Get_Instance()->Get_DeltaTime(L"FrameTime")))
	{
		CTimer_Manager::Get_Instance()->Set_DeltaTime(L"Timer_Fps_60");

		_float fFrame = DELTA_60;

		CCameraMgr::Get_Instance()->Update();
		CInput_Device::Get_Instance()->Set_InputDev();
		CManagement::Get_Instance()->Update_Management(fFrame);

		m_pParticleTab->Update(fFrame);

		Render();
	}
}

void CEffectToolView::Render()
{
	m_pGraphicDev->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f), 1.f, 0);
	m_pGraphicDev->BeginScene();

	if (0 > m_pRenderer->Draw_RenderList())
		return;

	//// ���� ����.(����������, ����)
	//if (0 > CManagement::Get_Instance()->Render_Management())
	//	return;

	m_pTerrainGuide->Render_GameObject();
	m_pParticleTab->Render();

	m_pGraphicDev->EndScene();
	m_pGraphicDev->Present(nullptr, nullptr, 0, nullptr);
}

void CEffectToolView::Ready_Default()
{
	CTimer_Manager::Get_Instance()->initialize_Timer(L"FrameTime");
	CTimer_Manager::Get_Instance()->initialize_Timer(L"Timer_Fps_60");
	CManagement::Get_Instance()->Ready_Engine(SCENE_END);
	CManagement::Get_Instance()->Ready_GraphicDev(g_hWnd, CGraphic_Device::WINMODE_WIN, WINCX, WINCY, &m_pGraphicDev);
	CManagement::Get_Instance()->Ready_Component_Manager(m_pGraphicDev);
	CManagement::Get_Instance()->Ready_Gizmo(m_pGraphicDev);

	CInput_Device::Get_Instance()->Ready_Input_Dev(g_hInst, g_hWnd);
	CInput_Device::Get_Instance()->Set_InputDev();

	////m_pGraphicDev->GetTransform(D3DTS_WORLD, &g_matWorld);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pRenderer = static_cast<CRenderer*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, L"Renderer"));
}

void CEffectToolView::Ready_Camera()
{
	CCameraMgr::Get_Instance()->Reserve_ContainerSize(2);
	CCameraMgr::Get_Instance()->Ready_Camera(m_pGraphicDev, DYNAMIC_CAM, L"Tool_FreeCam", TOOL_VIEW, DEFAULT_MODE);
	CCameraMgr::Get_Instance()->Set_MainCamera(DYNAMIC_CAM, L"Tool_FreeCam");
	CCameraMgr::Get_Instance()->Set_MainPos(_v3{ 0,3,-5 });
}

void CEffectToolView::Ready_Layer()
{
	m_pTerrainGuide = Engine::CTerrain_Guide::Create(m_pGraphicDev);
	TARGET_TO_TRANS(m_pTerrainGuide)->Set_Pos(_v3{ 0.f ,0.f, 0.f });
}

void CEffectToolView::Release()
{
	Engine::CCameraMgr::Get_Instance()->Destroy_Instance();

	Engine::CFrameMgr::Get_Instance()->Destroy_Instance();
	Engine::CTimer_Manager::Get_Instance()->Destroy_Instance();

	Safe_Release(m_pGraphicDev);
	Safe_Release(m_pTerrainGuide);
	Safe_Release(m_pRenderer);

	Engine::CManagement::Release_Engine();
}

CEffectToolDoc* CEffectToolView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEffectToolDoc)));
	return (CEffectToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CEffectToolView �޽��� ó����


void CEffectToolView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());

	RECT rcMain = {};
	pMainFrm->GetWindowRect(&rcMain); 
	::SetRect(&rcMain, 0, 0, rcMain.right - rcMain.left, rcMain.bottom - rcMain.top);

	RECT rcView = {};
	GetClientRect(&rcView);

	// MainFrm�� View�� ������ ���Ѵ�.
	int iGapX = rcMain.right - rcView.right;
	int iGapY = rcMain.bottom - rcView.bottom;

	// MainFrame�� ũ�� ����.
	pMainFrm->SetWindowPos(
		nullptr, 0, 0, WINCX + iGapX, WINCY + iGapY, SWP_NOZORDER);

	g_hWnd = m_hWnd;
	g_hInst = AfxGetInstanceHandle();

	///////////////////////////////////////////////////////////////////////////////////////////
	// Init Device
	HRESULT hr = 0;
	Ready_Default();

	Ready_Camera();

	/////////////////////////////////////////////////////////////////////////////////////////////
	//// Ready Resource, Obj
	Ready_Resource();
	Ready_Layer();

	///////////////////////////////////////////////////////////////////////////////////////////
	// Init memVar
	m_pMainFrm = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	m_pParticleTab = &(dynamic_cast<CMyFormView*>(m_pMainFrm->m_MainSplitter.GetPane(0, 0))->m_ParticleTab);
	m_pParticleTab->Set_GraphicDev(m_pGraphicDev);
	
	m_pParticleTab->LateInit();
}

void CEffectToolView::Ready_Resource()
{

	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return;

	Safe_AddRef(pManagement);

	pManagement->LoadTex_FromPath(m_pGraphicDev, L"../../Data/Tex_Path.dat");
	pManagement->LoadMesh_FromPath(m_pGraphicDev, L"../../Data/Mesh_Path.dat");

	if (0 > (pManagement->Add_Prototype(L"GameObject_Effect", CTexEffect::Create(m_pGraphicDev))))
		return;
	if (0 > (pManagement->Add_Prototype(L"GameObject_MeshEffect", CMeshEffect::Create(m_pGraphicDev))))
		return;

	Safe_Release(pManagement);

	return;
}
