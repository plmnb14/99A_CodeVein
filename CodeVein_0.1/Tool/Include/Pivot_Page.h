#pragma once


// CPivot_Page 대화 상자입니다.

class CPivot_Page : public CPropertyPage
{
	DECLARE_DYNAMIC(CPivot_Page)

public:
	CPivot_Page();
	virtual ~CPivot_Page();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PIVOT_PAGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio11();
	afx_msg void OnBnClickedRadio13();
	afx_msg void OnBnClickedRadio4();
	afx_msg void OnBnClickedRadio14();
	afx_msg void OnBnClickedRadio15();
	afx_msg void OnBnClickedRadio5();

public:
	//ENGINE::PIVOT_X Get_RenderPivotX() { return m_eRenderPivot_X; }
	//ENGINE::PIVOT_Y Get_RenderPivotY() { return m_eRenderPivot_Y; }
	//ENGINE::PIVOT_Z Get_RenderPivotZ() { return m_eRenderPivot_Z; }

public:
	//ENGINE::PIVOT_X m_eRenderPivot_X = ENGINE::X_CENTER;
	//ENGINE::PIVOT_Y m_eRenderPivot_Y = ENGINE::Y_CENTER;
	//ENGINE::PIVOT_Z m_eRenderPivot_Z = ENGINE::Z_CENTER;
};
