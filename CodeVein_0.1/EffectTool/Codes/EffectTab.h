#pragma once
#include "afxwin.h"
#include "resource.h"       // �� ��ȣ�Դϴ�.

// CEffectTab ��ȭ �����Դϴ�.

class CEffectTab : public CDialog
{
	DECLARE_DYNAMIC(CEffectTab)

public:
	CEffectTab(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CEffectTab();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EFFECTTAB };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};
