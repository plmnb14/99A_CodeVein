// Inspector_Page.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "Inspector_Page.h"
#include "afxdialogex.h"


// CInspector_Page 대화 상자입니다.

IMPLEMENT_DYNAMIC(CInspector_Page, CPropertyPage)

CInspector_Page::CInspector_Page()
	: CPropertyPage(IDD_INSPECTOR_PAGE)
	, m_strWorkPos_Y(_T("0.000"))
	, m_strWork_Y_Rate(_T("0.000"))
	, m_fWork_Y(0)
	, m_fWork_Rate(0)
	, m_strMouse_X(_T("0.000"))
	, m_strMouse_Y(_T("0.000"))
	, m_strMouse_Z(_T("0.000"))
{
	m_vInspectorPos = V3_NULL;
	m_vInspectorRotate = V3_NULL;
	m_vInspectorScale = {0.5,0.5,0.5};
}

CInspector_Page::~CInspector_Page()
{
}

void CInspector_Page::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, WORK_Y, m_strWorkPos_Y);
	DDX_Text(pDX, WORK_Y_RATE, m_strWork_Y_Rate);
	DDX_Text(pDX, IDC_MOUSE_X, m_strMouse_X);
	DDX_Text(pDX, IDC_MOUSE_Y, m_strMouse_Y);
	DDX_Text(pDX, IDC_MOUSE_Z, m_strMouse_Z);
}


BEGIN_MESSAGE_MAP(CInspector_Page, CPropertyPage)
	ON_EN_CHANGE(WORK_Y, &CInspector_Page::OnEnChangeY)
	ON_EN_CHANGE(WORK_Y_RATE, &CInspector_Page::OnEnChangeYRate)
	ON_EN_CHANGE(IDC_MOUSE_X, &CInspector_Page::OnEnChangeMouseX)
	ON_EN_CHANGE(IDC_MOUSE_Y, &CInspector_Page::OnEnChangeMouseY)
	ON_EN_CHANGE(IDC_MOUSE_Z, &CInspector_Page::OnEnChangeMouseZ)
END_MESSAGE_MAP()


void CInspector_Page::free()
{
}

void CInspector_Page::Update_MousePos()
{
	m_strMouse_X.Format(_T("%.3f"), g_MousePos.x);
	m_strMouse_Y.Format(_T("%.3f"), g_MousePos.y);
	m_strMouse_Z.Format(_T("%.3f"), g_MousePos.z);

	SetDlgItemText(IDC_MOUSE_X, m_strMouse_X);
	SetDlgItemText(IDC_MOUSE_Y, m_strMouse_Y);
	SetDlgItemText(IDC_MOUSE_Z, m_strMouse_Z);
}

void CInspector_Page::Add_WorkY(_bool _bPlus)
{
	m_fWork_Y += (_bPlus ? m_fWork_Rate : -m_fWork_Rate);
	g_fWorld_Y += (_bPlus ? m_fWork_Rate : -m_fWork_Rate);

	m_strWorkPos_Y.Format(_T("%.3f"), m_fWork_Y);

	SetDlgItemText(WORK_Y, m_strWorkPos_Y);
}

void CInspector_Page::OnEnChangeY()
{
	GetDlgItemText(WORK_Y, m_strWorkPos_Y);
	m_fWork_Y = (_float)_tstof(m_strWorkPos_Y);
}


void CInspector_Page::OnEnChangeYRate()
{
	GetDlgItemText(WORK_Y_RATE, m_strWork_Y_Rate);
	m_fWork_Rate = (_float)_tstof(m_strWork_Y_Rate);
}


void CInspector_Page::OnEnChangeMouseX()
{
}


void CInspector_Page::OnEnChangeMouseY()
{
}


void CInspector_Page::OnEnChangeMouseZ()
{
}
