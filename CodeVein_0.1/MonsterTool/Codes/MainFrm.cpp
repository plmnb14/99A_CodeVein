#include "stdafx.h"
#include "Tool.h"

#include "MainFrm.h"
#include "ToolView.h"
#include "SetingView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CFixedSplitterWnd, CSplitterWnd)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
END_MESSAGE_MAP()

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE | WS_SYSMENU;
	cs.style &= ~FWS_ADDTOTITLE , ~WS_THICKFRAME,  ~WS_MAXIMIZEBOX; //창 크기 조절 기능, 창 최대화버튼, 타이틀 비활성화
	SetTitle(L"심선생 몬스터교실");

	cs.hMenu = NULL, cs.cx = 1920, cs.cy = 1080;

	return TRUE;
}

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	m_MainSplitter.CreateStatic(this, 1, 2);
	m_MainSplitter.CreateView(0, 0, RUNTIME_CLASS(CToolView), CSize(WINCX, 1080), pContext);
	m_MainSplitter.CreateView(0, 1, RUNTIME_CLASS(CSetingView), CSize(500, 1080), pContext); //640

	return TRUE;
}

CFixedSplitterWnd::CFixedSplitterWnd()
{
}

CFixedSplitterWnd::~CFixedSplitterWnd()
{
}

LRESULT CFixedSplitterWnd::OnNcHitTest(CPoint point)
{
	return HTNOWHERE;
}
