// Inspector_Form.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "Inspector_Form.h"
#include "Inspector_Sheet.h"
#include "Essential_Sheet.h"


// CInspector_Form

IMPLEMENT_DYNCREATE(CInspector_Form, CFormView)

CInspector_Form::CInspector_Form()
	: CFormView(IDD_INSPECTOR_FORM),
	m_pEssential_Sheet(nullptr)
{

}

CInspector_Form::~CInspector_Form()
{
	
}

void CInspector_Form::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

void CInspector_Form::Free()
{
	Engine::Safe_Delete(m_Inspector_Sheet);
	Engine::Safe_Delete(m_pEssential_Sheet);
}

BEGIN_MESSAGE_MAP(CInspector_Form, CFormView)
	ON_STN_CLICKED(IDC_PLACEHOLDER, &CInspector_Form::OnStnClickedPlaceholder)
	ON_STN_CLICKED(IDC_PLACEHOLDER2, &CInspector_Form::OnStnClickedPlaceholder2)
END_MESSAGE_MAP()


// CInspector_Form 진단입니다.

#ifdef _DEBUG
void CInspector_Form::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CInspector_Form::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CInspector_Form 메시지 처리기입니다.


void CInspector_Form::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	GetParentFrame()->RecalcLayout();

	//ResizeParentToFit();

	m_pPlaceHolder = GetDlgItem(IDC_PLACEHOLDER);

	m_pEssential_Sheet = new CEssential_Sheet(m_pPlaceHolder);

	if (!m_pEssential_Sheet->Create(m_pPlaceHolder, WS_CHILD | WS_VISIBLE, 0))
	{

		delete m_Inspector_Sheet;

		m_Inspector_Sheet = NULL;

		return;
	}

	CRect rcSheet;

	rcSheet.SetRect(0,0, 300,330);

	m_pTabCtrl = m_pEssential_Sheet->GetTabControl();

	m_pTabCtrl->MoveWindow(0, 0, rcSheet.Width(), rcSheet.Height());

	m_pEssential_Sheet->SetWindowPos(NULL, 12, 0, rcSheet.Width(), rcSheet.Height(), SWP_NOZORDER | SWP_NOACTIVATE);

	m_pEssential_Sheet->ModifyStyleEx(0, WS_EX_CONTROLPARENT);

	m_pEssential_Sheet->ModifyStyle(0, WS_TABSTOP);
	m_pEssential_Sheet->SetActivePage(0);



	m_pPlaceHolder = GetDlgItem(IDC_PLACEHOLDER2);

	m_Inspector_Sheet = new CInspector_Sheet(m_pPlaceHolder);

	if (!m_Inspector_Sheet->Create(m_pPlaceHolder, WS_CHILD | WS_VISIBLE, 0))
	{

		delete m_Inspector_Sheet;

		m_Inspector_Sheet = NULL;

		return;
	}

	rcSheet.SetRect(0, 0, 300, 900);
	//m_Inspector_Sheet->MoveWindow(rcSheet);
	//m_pPlaceHolder->GetWindowRect(&rcSheet);

	//ScreenToClient(&rcSheet);

	m_pTabCtrl = m_Inspector_Sheet->GetTabControl();

	m_pTabCtrl->MoveWindow(0, 0, rcSheet.Width(), rcSheet.Height());

	m_Inspector_Sheet->SetWindowPos(NULL, 12, 0, rcSheet.Width(), rcSheet.Height(), SWP_NOZORDER | SWP_NOACTIVATE);

	m_Inspector_Sheet->ModifyStyleEx(0, WS_EX_CONTROLPARENT);

	m_Inspector_Sheet->ModifyStyle(0, WS_TABSTOP);

	m_Inspector_Sheet->SetActivePage(0);

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}


void CInspector_Form::OnStnClickedPlaceholder()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CInspector_Form::OnStnClickedPlaceholder2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
