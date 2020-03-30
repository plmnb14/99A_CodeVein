#pragma once
//////////////���� ��� ������ ������ ���� Ŭ������
class CFixedSplitterWnd :public CSplitterWnd
{
public:
	CFixedSplitterWnd();
	~CFixedSplitterWnd();

protected:
	afx_msg LRESULT OnNcHitTest(CPoint point);
	DECLARE_MESSAGE_MAP();
};
//////////////���� ��� ������ ������ ���� Ŭ������

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


