#pragma once
//////////////분할 경계 프레임 고정을 위한 클래스임
class CFixedSplitterWnd :public CSplitterWnd
{
public:
	CFixedSplitterWnd();
	~CFixedSplitterWnd();

protected:
	afx_msg LRESULT OnNcHitTest(CPoint point);
	DECLARE_MESSAGE_MAP();
};
//////////////분할 경계 프레임 고정을 위한 클래스임

class CMainFrame : public CFrameWnd
{
	
protected:
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual ~CMainFrame();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()

public:
	CFixedSplitterWnd	m_MainSplitter;
};


