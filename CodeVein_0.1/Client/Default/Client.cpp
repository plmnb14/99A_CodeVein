// Client.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Client.h"
#include "MainApp.h"

#include "Management.h"
#include "SoundManager.h"
#include "ClothManager.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE g_hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
HWND g_hWnd;

CManagement*				g_pManagement;
CTimer_Manager*				g_pTimer_Manager;
CFrameMgr*					g_pFrame_Manager;
CInput_Device*				g_pInput_Device;
CTexture*					g_pDissolveTexture;
CMyPhysx*					g_pPhysx;
Client::CSoundManager*		g_pSoundManager = nullptr;
Client::CClothManager*		g_pClothManager = nullptr;

// ================================================
// 스테이지를 한번이라도 들렸는지 확인하는 전역변수 (MeshLoad를 위해)
// ================================================
bool				g_bActiveCam	= false;
bool				g_bOnStage[8]	= {};
bool				g_bReleaseMode	= false;
short				g_sStageIdx_Cur = 0;
short				g_sPlayerLevel = 1;
short				g_sQuadTreeCX = 0;
short				g_sQuadTreeCY = 0;
int					g_eSceneID_Cur = SCENE_END;
int					g_eSTeleportID_Cur = TeleportID_End;

// [0] - 최초 로딩 타이틀
// [1] - 트레이닝
// [2] - 베이스 Stage_00
// [3] - 스테이지 1 Stage_01
// [4] - 스테이지 2 Stage_02
// [5] - 스테이지 3 Stage_03
// [6] - 스테이지 4 Stage_04
// [7] - 모든 리소스 레디
// ================================================
_v3 g_vWindDir = { 0.f, 0.f, 0.f };
// ================================================



// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{

#ifdef _DEBUG
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // _DEBUG

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.
	CMainApp*		pMainApp = nullptr;

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 응용 프로그램 초기화를 수행합니다.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

	g_pManagement = CManagement::Get_Instance();
	Safe_AddRef(g_pManagement);

	g_pSoundManager = CSoundManager::Get_Instance();

	g_pTimer_Manager = CTimer_Manager::Get_Instance();

	g_pFrame_Manager = CFrameMgr::Get_Instance();
	g_pInput_Device = CInput_Device::Get_Instance();
	g_pPhysx = CMyPhysx::Get_Instance();
	g_pClothManager = CClothManager::Get_Instance();

	g_pPhysx->Ready_MyPhysx();

	pMainApp = CMainApp::Create();

	g_pTimer_Manager->initialize_Timer(L"FrameTime");
	g_pTimer_Manager->initialize_Timer(L"Timer_Fps_60");
	g_pTimer_Manager->initialize_Timer(L"Timer_Fps_60_2");

	if (nullptr == pMainApp)
		return FALSE;

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENT));

    MSG msg;
   
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{ 
			if (WM_QUIT == msg.message)
				break;

			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		else
		{
			if (nullptr == pMainApp)
				break;

			g_pTimer_Manager->Set_DeltaTime(L"FrameTime");

			if (g_pFrame_Manager->Lock_Frame(60.f, g_pTimer_Manager->Get_DeltaTime(L"FrameTime")))
			{
				g_pTimer_Manager->Set_DeltaTime(L"Timer_Fps_60");
				g_pTimer_Manager->Set_DeltaTime(L"Timer_Fps_60_2");

				_float fFrame = g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60");

				g_pPhysx->Update_MyPhysx(1.f / 60.f);

				if (pMainApp->Update_MainApp(fFrame) < 0)
					break;

				if (FAILED(pMainApp->Render_MainApp()))
					break;

				pMainApp->LateUpdate_MainApp(fFrame);
			}
		}
	}
	
	g_pClothManager->Destroy_Instance();
	g_pSoundManager->Destroy_Instance();
	g_pTimer_Manager->Destroy_Instance();
	g_pFrame_Manager->Destroy_Instance();
	
	Safe_Release(pMainApp);
	
    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CLIENT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	g_hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   RECT		rcWindow = { 0, 0, g_nWinCX, g_nWinCY };

   AdjustWindowRect(&rcWindow, WS_OVERLAPPEDWINDOW, TRUE);

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   g_hWnd = hWnd;

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다.
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
