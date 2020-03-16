#pragma once

#include "Inspector_Page.h"

// CEssential_Sheet

class CEssential_Sheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CEssential_Sheet)

public:
	explicit CEssential_Sheet(CWnd* pParentWnd = NULL);
	virtual ~CEssential_Sheet();

public:
	void Free();

protected:
	DECLARE_MESSAGE_MAP()

	CInspector_Page		m_Page_Inspector;

};


