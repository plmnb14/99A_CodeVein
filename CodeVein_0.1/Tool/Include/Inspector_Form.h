#pragma once



// CInspector_Form 폼 뷰입니다.
#include "Inspector_Sheet.h"
#include "Essential_Sheet.h"

class CInspector_Form : public CFormView
{
	DECLARE_DYNCREATE(CInspector_Form)

protected:
	CInspector_Form();           // 동적 만들기에 사용되는 protected 생성자입니다.
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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


