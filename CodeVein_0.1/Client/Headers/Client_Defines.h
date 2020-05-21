#pragma once

//#define PlaySound g_pSoundManager;

static const unsigned short g_nWinCX = 1280;
static const unsigned short g_nWinCY = 720;

extern HINSTANCE	g_hInst;
extern HWND			g_hWnd;

///////////////////////////////////////////////////////////////////////////////

extern int				g_eSceneID_Cur;
extern int				g_eSTeleportID_Cur;

extern _v3				g_vWindDir;

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

extern bool			g_bActiveCam;
extern bool			g_bOnStage[8];
extern bool			g_bReleaseMode;
extern short		g_eStageIdx_Cur;
extern short		g_sPlayerLevel;

namespace Client
{
	class CSoundManager;
	class CClothManager;
}

extern Client::CSoundManager*	g_pSoundManager;
extern Client::CClothManager*	g_pClothManager;

using namespace Client;

//

class CMyStrCmp
{
public:
	CMyStrCmp() {}
	CMyStrCmp(const TCHAR* pString)
		:m_pString(pString)
	{

	}
	~CMyStrCmp() {}
public:
	template <typename T>
	bool operator()(T& rPair)
	{
		return !lstrcmp(rPair.first, m_pString);
	}
private:
	const TCHAR* m_pString;
};

