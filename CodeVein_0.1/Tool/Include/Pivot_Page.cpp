// Pivot_Page.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Tool.h"
#include "Pivot_Page.h"
#include "afxdialogex.h"


// CPivot_Page ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CPivot_Page, CPropertyPage)

CPivot_Page::CPivot_Page()
	: CPropertyPage(IDD_PIVOT_PAGE)
{

}

CPivot_Page::~CPivot_Page()
{
}

void CPivot_Page::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPivot_Page, CPropertyPage)
	ON_BN_CLICKED(IDC_RADIO1, &CPivot_Page::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CPivot_Page::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &CPivot_Page::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO11, &CPivot_Page::OnBnClickedRadio11)
	ON_BN_CLICKED(IDC_RADIO13, &CPivot_Page::OnBnClickedRadio13)
	ON_BN_CLICKED(IDC_RADIO4, &CPivot_Page::OnBnClickedRadio4)
	ON_BN_CLICKED(IDC_RADIO14, &CPivot_Page::OnBnClickedRadio14)
	ON_BN_CLICKED(IDC_RADIO15, &CPivot_Page::OnBnClickedRadio15)
	ON_BN_CLICKED(IDC_RADIO5, &CPivot_Page::OnBnClickedRadio5)
END_MESSAGE_MAP()


// CPivot_Page �޽��� ó�����Դϴ�.


void CPivot_Page::OnBnClickedRadio1()
{
	//m_eRenderPivot_X = ENGINE::X_LEFT;
}


void CPivot_Page::OnBnClickedRadio2()
{
	//m_eRenderPivot_X = ENGINE::X_CENTER;
}


void CPivot_Page::OnBnClickedRadio3()
{
	//m_eRenderPivot_X = ENGINE::X_RIGHT;
}


void CPivot_Page::OnBnClickedRadio11()
{
	//m_eRenderPivot_Y = ENGINE::Y_UPPER;
}


void CPivot_Page::OnBnClickedRadio13()
{
	//m_eRenderPivot_Y = ENGINE::Y_CENTER;
}


void CPivot_Page::OnBnClickedRadio4()
{
	//m_eRenderPivot_Y = ENGINE::Y_UNDER;
}


void CPivot_Page::OnBnClickedRadio14()
{
	//m_eRenderPivot_Z = ENGINE::Z_FRONT;
}


void CPivot_Page::OnBnClickedRadio15()
{
	//m_eRenderPivot_Z = ENGINE::Z_CENTER;
}


void CPivot_Page::OnBnClickedRadio5()
{
	//m_eRenderPivot_Z = ENGINE::Z_BACK;
}
