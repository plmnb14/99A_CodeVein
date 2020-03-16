#pragma once



// CResource_Form 폼 뷰입니다.

class CResource_Form : public CFormView
{
	DECLARE_DYNCREATE(CResource_Form)

protected:
	CResource_Form();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CResource_Form();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RESOURCE_FORM };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
};


