#pragma once
#include "afxcmn.h"
#include "RenderObject.h"
#include "afxwin.h"


// CScene_ObjList_Form 폼 뷰입니다.

class CScene_ObjList_Form : public CFormView
{
	DECLARE_DYNCREATE(CScene_ObjList_Form)

protected:
	CScene_ObjList_Form();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CScene_ObjList_Form();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SCENE_OBJLIST_FORM };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	_bool Add_Object_OnTree(Engine::CRenderObject* _pRendObj);

public:
	void Add_On_List(_tchar* _name);

public:
	virtual void Free();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	afx_msg void OnTvnSelchangedTree2(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void OnInitialUpdate();

	afx_msg void DataChange();

public:
	_tchar*							m_arrObject[STR_128];
	list<_tchar*>					m_listObject;
	list<_tchar*>					m_listNewObject;

public:
	HTREEITEM hObjectRoot;
	HTREEITEM hItemNew;

public:
	CTreeCtrl m_pTreeCtrl;
	CButton m_eButton;
	afx_msg void OnBnClickedButton2();
};


