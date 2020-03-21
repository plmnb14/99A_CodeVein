#pragma once
#include "afxcmn.h"

#include "ParticleTab.h"
#include "EffectTab.h"
#include "TrailTab.h"

// CMyFormView �� ���Դϴ�.

class CMyFormView : public CFormView
{
	DECLARE_DYNCREATE(CMyFormView)

protected:
	CMyFormView();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CMyFormView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MYFORMVIEW };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual void OnInitialUpdate();
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()

public:
	CTabCtrl m_TabCtrl;

	CParticleTab	m_ParticleTab;
	CEffectTab		m_EffectTab;
	CTrailTab		m_TrailTab;
	CWnd*			m_pWndShow;

};


