// TrailTab.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "TrailTab.h"
#include "afxdialogex.h"


// CTrailTab 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTrailTab, CDialog)

CTrailTab::CTrailTab(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_TRAILTAB, pParent)
{

}

CTrailTab::~CTrailTab()
{
}

void CTrailTab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTrailTab, CDialog)
END_MESSAGE_MAP()


// CTrailTab 메시지 처리기입니다.
