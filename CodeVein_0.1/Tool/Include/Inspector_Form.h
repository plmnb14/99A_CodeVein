#pragma once



// CInspector_Form �� ���Դϴ�.
#include "Inspector_Sheet.h"
#include "Essential_Sheet.h"

class CInspector_Form : public CFormView
{
	DECLARE_DYNCREATE(CInspector_Form)

protected:
	CInspector_Form();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CInspector_Form();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INSPECTOR_FORM };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN64_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

public:
	void Free();

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();

public:
	CInspector_Sheet* m_Inspector_Sheet;
	CEssential_Sheet* m_pEssential_Sheet;

public:
	CWnd*			  m_pPlaceHolder;
	CTabCtrl*		  m_pTabCtrl;

	afx_msg void OnStnClickedPlaceholder();
	afx_msg void OnStnClickedPlaceholder2();
};


