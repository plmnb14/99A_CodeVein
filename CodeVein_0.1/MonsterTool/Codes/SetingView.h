#pragma once
#include "afxwin.h"
#include "afxcmn.h"

class CMonsterTool;

class CSetingView : public CFormView
{
	DECLARE_DYNCREATE(CSetingView)

protected:
	CSetingView();
	virtual ~CSetingView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SETINGVIEW };
#endif

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()

public:
	virtual void OnInitialUpdate();
	afx_msg void OnTcnSelchangeToolTab(NMHDR *pNMHDR, LRESULT *pResult);

public:
	CTabCtrl			m_pToolMenu;
	CMonsterTool*		m_pMonsterTool;

};


