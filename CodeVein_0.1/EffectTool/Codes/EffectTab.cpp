// EffectTab.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EffectTab.h"
#include "afxdialogex.h"


// CEffectTab 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEffectTab, CDialog)

CEffectTab::CEffectTab(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_EFFECTTAB, pParent)
{

}

CEffectTab::~CEffectTab()
{
}

void CEffectTab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEffectTab, CDialog)
END_MESSAGE_MAP()


// CEffectTab 메시지 처리기입니다.
