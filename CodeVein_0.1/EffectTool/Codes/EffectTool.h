
// EffectTool.h : EffectTool ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.


// CEffectToolApp:
// �� Ŭ������ ������ ���ؼ��� EffectTool.cpp�� �����Ͻʽÿ�.
//

class CEffectToolApp : public CWinAppEx
{
public:
	CEffectToolApp();


// �������Դϴ�.
public:
	virtual BOOL OnIdle(LONG lCount);
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// �����Դϴ�.
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CEffectToolApp theApp;
