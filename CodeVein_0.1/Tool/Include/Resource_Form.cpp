// Resource_Form.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "Resource_Form.h"


// CResource_Form

IMPLEMENT_DYNCREATE(CResource_Form, CFormView)

CResource_Form::CResource_Form()
	: CFormView(IDD_RESOURCE_FORM)
{

}

CResource_Form::~CResource_Form()
{
}

void CResource_Form::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CResource_Form, CFormView)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CResource_Form::OnLvnItemchangedList1)
END_MESSAGE_MAP()


// CResource_Form 진단입니다.

#ifdef _DEBUG
void CResource_Form::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CResource_Form::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CResource_Form 메시지 처리기입니다.


void CResource_Form::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}
