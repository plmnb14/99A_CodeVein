// MyFormView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "resource.h"
#include "MyFormView.h"

#include "MainFrm.h"
#include "EffectToolView.h"

// CMyFormView

IMPLEMENT_DYNCREATE(CMyFormView, CFormView)

CMyFormView::CMyFormView()
	: CFormView(IDD_MYFORMVIEW)
{

}

CMyFormView::~CMyFormView()
{
}

void CMyFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_TabCtrl);
}

BEGIN_MESSAGE_MAP(CMyFormView, CFormView)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CMyFormView::OnTcnSelchangeTab)
END_MESSAGE_MAP()


// CMyFormView 진단입니다.

#ifdef _DEBUG
void CMyFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CMyFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMyFormView 메시지 처리기입니다.


void CMyFormView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();


	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CString strTab_1 = _T("Particle");
	CString strTab_2 = _T("Effect");
	CString strTab_3 = _T("Trail");

	m_TabCtrl.InsertItem(1, strTab_1);
	m_TabCtrl.InsertItem(2, strTab_2);
	m_TabCtrl.InsertItem(3, strTab_3);

	CRect rc;
	m_TabCtrl.GetClientRect(&rc);

	m_ParticleTab.Create(IDD_PARTICLETAB, &m_TabCtrl);
	m_ParticleTab.SetWindowPos(NULL, 5, 25, rc.Width() - 10, rc.Height() - 28, SWP_SHOWWINDOW | SWP_NOZORDER);
	m_pWndShow = &m_ParticleTab;
	
	m_EffectTab.Create(IDD_EFFECTTAB, &m_TabCtrl);
	m_EffectTab.SetWindowPos(NULL, 5, 25, rc.Width() - 10, rc.Height() - 28, SWP_NOZORDER);
	
	m_TrailTab.Create(IDD_TRAILTAB, &m_TabCtrl);
	m_TrailTab.SetWindowPos(NULL, 5, 25, rc.Width() - 10, rc.Height() - 28, SWP_NOZORDER);
}


void CMyFormView::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	CEffectToolView* pView = dynamic_cast<CEffectToolView*>(pMainFrm->m_MainSplitter.GetPane(0, 1));

	if (m_pWndShow != nullptr)
	{
		m_pWndShow->ShowWindow(SW_HIDE);
		m_pWndShow = nullptr;
	}

	int iIdx = m_TabCtrl.GetCurSel();

	switch (iIdx)
	{
	case 0:
		m_ParticleTab.ShowWindow(SW_SHOW);
		m_pWndShow = &m_ParticleTab;
		//pView->m_eScene = CToolView::TOOL_TERRAINEDIT;
		break;
	case 1:
		m_EffectTab.ShowWindow(SW_SHOW);
		m_pWndShow = &m_EffectTab;
		//pView->m_eScene = CToolView::TOOL_MAPOBJ;
		break;
	case 2:
		m_TrailTab.ShowWindow(SW_SHOW);
		m_pWndShow = &m_TrailTab;
		//pView->m_eScene = CToolView::TOOL_MESH;
		break;
	}

}
