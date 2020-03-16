#pragma once



// CMainForm 폼 뷰입니다.

class CMainForm : public CFormView
{
	DECLARE_DYNCREATE(CMainForm)

protected:
	CMainForm();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CMainForm();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAINFORM };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN64_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
};


