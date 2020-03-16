#pragma once


// CCollider_Page 대화 상자입니다.

class CCollider_Page : public CPropertyPage
{
	DECLARE_DYNAMIC(CCollider_Page)

public:
	CCollider_Page();
	virtual ~CCollider_Page();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_COLLIDER_PAGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
