#pragma once

static const unsigned short g_nWinCX = 1280;
static const unsigned short g_nWinCY = 720;

extern HINSTANCE	g_hInst;
extern HWND			g_hWnd;

///////////////////////////////////////////////////////////////////////////////

namespace Engine
{
	class CGraphic_Device;
	class CManagement;
	class CTimer_Manager;
	class CInput_Device;
	class CFrameMgr;
	class CTexture;
	class CMyPhysx;
}

extern Engine::CInput_Device*		g_pInput_Device;
extern Engine::CManagement*			g_pManagement;
extern Engine::CTimer_Manager*		g_pTimer_Manager;
extern Engine::CFrameMgr*			g_pFrame_Manager;
extern Engine::CTexture*			g_pDissolveTexture;
extern Engine::CMyPhysx*			g_pPhysx;

///////////////////////////////////////////////////////////////////////////////

//enum SCENEID { SCENE_STATIC, SCENE_LOGO, SCENE_STAGE, SCENE_END };

extern bool		g_bActiveCam;
extern bool		g_bOnStage[8];
extern bool		g_bReleaseMode;
extern short	g_sStageIdx_Cur;
extern short	g_sPlayerLevel;

namespace Client {}

using namespace Client;

