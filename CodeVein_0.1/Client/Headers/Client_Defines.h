#pragma once

static const unsigned short g_nWinCX = 1280;
static const unsigned short g_nWinCY = 720;

extern HINSTANCE	g_hInst;
extern HWND			g_hWnd;

///////////////////////////////////////////////////////////////////////////////

//extern Engine::CManagement*			g_pManagement;
//extern Engine::CTimer_Manager*		g_pTimer_Manager;
//extern Engine::CFrameMgr*			g_pFrame_Manager;
//extern Engine::CGraphic_Device*		g_pGraphic_Device;

namespace Engine
{
	class CGraphic_Device;
	class CManagement;
	class CTimer_Manager;
	class CFrameMgr;
}

extern Engine::CManagement*			g_pManagement;
extern Engine::CTimer_Manager*		g_pTimer_Manager;
extern Engine::CFrameMgr*			g_pFrame_Manager;

///////////////////////////////////////////////////////////////////////////////

//enum SCENEID { SCENE_STATIC, SCENE_LOGO, SCENE_STAGE, SCENE_END };

namespace Client {}

using namespace Client;

