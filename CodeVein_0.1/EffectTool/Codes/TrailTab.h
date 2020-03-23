#pragma once
#include "afxwin.h"
#include "resource.h"       // 주 기호입니다.

// CTrailTab 대화 상자입니다.

class CTrailTab : public CDialog
{
	DECLARE_DYNAMIC(CTrailTab)

public:
	CTrailTab(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTrailTab();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TRAILTAB };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
