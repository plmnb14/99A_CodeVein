#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "Tool.h"
#include "MainFrm.h"
#include "ToolDoc.h"

#include "ToolView.h"
#include "MonsterTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CToolApp, CWinApp)
END_MESSAGE_MAP()

CToolApp::CToolApp()
{
	SetAppID(_T("Tool.AppID.NoVersion"));
}

CToolApp				theApp;

BOOL CToolApp::InitInstance()
{
	CWinApp::InitInstance();

	EnableTaskbarInteraction(FALSE);

	SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));
	LoadStdProfileSettings(4);
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CToolDoc),
		RUNTIME_CLASS(CMainFrame),
		RUNTIME_CLASS(CToolView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}

int CToolApp::ExitInstance()
{
	AfxOleTerm(FALSE);

	return CWinApp::ExitInstance();
}

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()

};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

void CToolApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// �̰��� �̿��ϸ� Ŭ���̾�Ʈó�� Update�� ������ ���� PeekMessage
BOOL CToolApp::OnIdle(LONG lCount)
{
	CMainFrame* pFrameWnd = static_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	CToolView* m_pToolView = static_cast<CToolView*>(pFrameWnd->m_MainSplitter.GetPane(0, 0));

	m_pToolView->Start();

	return true;
}
