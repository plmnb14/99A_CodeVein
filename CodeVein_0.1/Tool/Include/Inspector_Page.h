#pragma once
#include "afxwin.h"


// CInspector_Page 대화 상자입니다.

class CInspector_Page : public CPropertyPage
{
	DECLARE_DYNAMIC(CInspector_Page)

public:
	CInspector_Page();
	virtual ~CInspector_Page();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INSPECTOR_PAGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual void free();

public:
	_v3 Get_InspectorPos() { return m_vInspectorPos;}
	_v3 Get_InspectorRotate() { return m_vInspectorRotate; }
	_v3 Get_InspectorSacle() { return m_vInspectorScale; };

	_float Get_WorkY() { return m_fWork_Y; };
	_float Get_WorkRate() { return m_fWork_Rate; };

	void Update_MousePos();

public:
	void Add_WorkY(_bool _bPlus = TRUE);

public:
	afx_msg void OnEnChangeY();
	afx_msg void OnEnChangeYRate();

public:
	_v3 m_vInspectorPos;
	_v3 m_vInspectorRotate;
	_v3 m_vInspectorScale;

	CString m_strWorkPos_Y;
	CString m_strWork_Y_Rate;
	CString m_strMouse_X;
	CString m_strMouse_Y;
	CString m_strMouse_Z;

	_float	m_fWork_Y;
	_float	m_fWork_Rate;
	afx_msg void OnEnChangeMouseX();
	afx_msg void OnEnChangeMouseY();
	afx_msg void OnEnChangeMouseZ();
};
