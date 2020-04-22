#include "stdafx.h"
#include "Tool.h"
#include "MainFrm.h"
#include "ToolView.h"
#include "SetingView.h"

#include "MonsterTool.h"

IMPLEMENT_DYNCREATE(CSetingView, CFormView)

CSetingView::CSetingView()
	: CFormView(IDD_SETINGVIEW), m_pMonsterTool(nullptr)
{
}

CSetingView::~CSetingView()
{
	Safe_Delete(m_pMonsterTool);
}

void CSetingView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_ToolTAB, m_pToolMenu);

	return;
}

BEGIN_MESSAGE_MAP(CSetingView, CFormView)
	ON_NOTIFY(TCN_SELCHANGE, IDC_ToolTAB, &CSetingView::OnTcnSelchangeToolTab)
END_MESSAGE_MAP()

#ifdef _DEBUG
void CSetingView::AssertValid() const
{
	CFormView::AssertValid();
}
#ifndef _WIN32_WCE
void CSetingView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif

void CSetingView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	m_pToolMenu.DeleteAllItems();
	m_pToolMenu.InsertItem(0, L"몬스터 툴");

	CRect rc;
	m_pToolMenu.GetWindowRect(rc);

	m_pMonsterTool = new CMonsterTool;
	m_pMonsterTool->Create(IDD_MONSTERTOOL, &m_pToolMenu);
	m_pMonsterTool->MoveWindow(0, 25, rc.Width(), rc.Height());
	m_pMonsterTool->ShowWindow(SW_SHOW);

	return;
}

void CSetingView::OnTcnSelchangeToolTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	CMainFrame* pFrameWnd = static_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	CToolView* pToolView = static_cast<CToolView*>(pFrameWnd->m_MainSplitter.GetPane(0, 1));

	_int select = m_pToolMenu.GetCurSel();

	switch (select)
	{
	case 0: //맵툴
		m_pMonsterTool->UpdateData(TRUE);
		m_pMonsterTool->ShowWindow(SW_SHOW);
		m_pMonsterTool->UpdateData(TRUE);
		break;

	}

	*pResult = 0;

	UpdateData(FALSE);

	return;
}
