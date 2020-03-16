// Animator_Page.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "Animator_Page.h"
#include "afxdialogex.h"


// CAnimator_Page 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAnimator_Page, CPropertyPage)

CAnimator_Page::CAnimator_Page()
	: CPropertyPage(IDD_ANIMATOR_PAGE)
{

}

CAnimator_Page::~CAnimator_Page()
{
}

void CAnimator_Page::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAnimator_Page, CPropertyPage)
END_MESSAGE_MAP()


// CAnimator_Page 메시지 처리기입니다.
