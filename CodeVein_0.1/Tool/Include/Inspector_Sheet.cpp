// Inspector_Sheet.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Tool.h"
#include "Inspector_Sheet.h"


// CInspector_Sheet

IMPLEMENT_DYNAMIC(CInspector_Sheet, CPropertySheet)



CInspector_Sheet::CInspector_Sheet(CWnd * pParentWnd)
{
	CPropertySheet::AddPage(&m_page_Navmesh);
	CPropertySheet::AddPage(&m_page_MapTool);
}

CInspector_Sheet::~CInspector_Sheet()
{

}

void CInspector_Sheet::Delete_Page()
{
}


BEGIN_MESSAGE_MAP(CInspector_Sheet, CPropertySheet)
END_MESSAGE_MAP()


// CInspector_Sheet �޽��� ó�����Դϴ�.
