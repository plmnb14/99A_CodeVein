#pragma once



// CInspector_Sheet

#include "NavMesh_Page.h"
#include "MapTool_Page.h"

class CInspector_Sheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CInspector_Sheet)

public:
	explicit CInspector_Sheet(CWnd* pParentWnd = NULL);
	virtual ~CInspector_Sheet();

public:
	void Delete_Page();

protected:
	DECLARE_MESSAGE_MAP()

public:
	//CAnimator_Page		m_Page_Animator;
	//CCollider_Page		m_Page_Collider;
	CNavMesh_Page		m_page_Navmesh;
	CMapTool_Page		m_page_MapTool;
};


