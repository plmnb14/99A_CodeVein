#pragma once
#include "afxwin.h"
#include "resource.h"       // �� ��ȣ�Դϴ�.

// CTrailTab ��ȭ �����Դϴ�.

class CTrailTab : public CDialog
{
	DECLARE_DYNAMIC(CTrailTab)

public:
	CTrailTab(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CTrailTab();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TRAILTAB };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};
