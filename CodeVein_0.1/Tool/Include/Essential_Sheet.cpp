// Essential_Sheet.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Tool.h"
#include "Essential_Sheet.h"


// CEssential_Sheet

IMPLEMENT_DYNAMIC(CEssential_Sheet, CPropertySheet)

CEssential_Sheet::CEssential_Sheet(CWnd * pParentWnd)
{
	CPropertySheet::AddPage(&m_Page_Inspector);
	//CPropertySheet::AddPage(&m_Page_Pivot);
}

CEssential_Sheet::~CEssential_Sheet()
{

}

void CEssential_Sheet::Free()
{
	//m_Page_Inspector.CloseWindow();
	//CPropertySheet::RemovePage(&m_Page_Inspector);
}


BEGIN_MESSAGE_MAP(CEssential_Sheet, CPropertySheet)
END_MESSAGE_MAP()


// CEssential_Sheet �޽��� ó�����Դϴ�.
