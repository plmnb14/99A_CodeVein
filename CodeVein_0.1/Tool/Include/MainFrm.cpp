
// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "Tool.h"

#include "MainFrm.h"
#include "ToolView.h"

#include "MainForm.h"

#include "Inspector_Form.h"
#include "Resource_Form.h"
#include "ObjectList_Form.h"
#include "Scene_ObjList_Form.h"

#include "FileInfo.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
}

CMainFrame::~CMainFrame()
{
	//delete m_MainSplitter;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_DISABLED | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("도구 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	SetMenu(NULL);

	::DestroyMenu(GetMenu()->GetSafeHmenu());
	m_wndToolBar.EnableWindow(FALSE);

	this->SetWindowText(L"DD");

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기



BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// 메쉬 경로 추출
	CFileInfo::Create_Mesh_PathInfo();
	// 텍스쳐 경로 추출
	CFileInfo::Create_Texture_PathInfo();

	m_MainSplitter.CreateStatic(this, 1, 2);
	m_MainSplitter.SetColumnInfo(0, 1280, 0);
	m_MainSplitter.SetColumnInfo(1, 300, 0);

	m_MainSplitter.CreateView(0, 0, RUNTIME_CLASS(CToolView), CSize(1280, 720), pContext);
	m_MainSplitter.CreateView(0, 1, RUNTIME_CLASS(CInspector_Form), CSize(300, 720), pContext);

	return TRUE;
}
