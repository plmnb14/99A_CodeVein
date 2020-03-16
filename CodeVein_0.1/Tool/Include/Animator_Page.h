#pragma once


// CAnimator_Page 대화 상자입니다.

class CAnimator_Page : public CPropertyPage
{
	DECLARE_DYNAMIC(CAnimator_Page)

public:
	CAnimator_Page();
	virtual ~CAnimator_Page();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ANIMATOR_PAGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
