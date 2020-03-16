// Collider_Page.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "Collider_Page.h"
#include "afxdialogex.h"


// CCollider_Page 대화 상자입니다.

IMPLEMENT_DYNAMIC(CCollider_Page, CPropertyPage)

CCollider_Page::CCollider_Page()
	: CPropertyPage(IDD_COLLIDER_PAGE)
{

}

CCollider_Page::~CCollider_Page()
{
}

void CCollider_Page::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCollider_Page, CPropertyPage)
END_MESSAGE_MAP()


// CCollider_Page 메시지 처리기입니다.


BOOL CCollider_Page::OnInitDialog()
{
	CPropertyPage::OnInitDialog();



	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
