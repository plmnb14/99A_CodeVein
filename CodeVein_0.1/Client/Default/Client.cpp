// Client.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "Client.h"
#include "MainApp.h"

#include "Management.h"
#include "SoundManager.h"
#include "ClothManager.h"

#define MAX_LOADSTRING 100

// ���� ����:
HINSTANCE g_hInst;                                // ���� �ν��Ͻ��Դϴ�.
WCHAR szTitle[MAX_LOADSTRING];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
WCHAR szWindowClass[MAX_LOADSTRING];            // �⺻ â Ŭ���� �̸��Դϴ�.
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
// ���������� �ѹ��̶� ��ȴ��� Ȯ���ϴ� �������� (MeshLoad�� ����)
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

// [0] - ���� �ε� Ÿ��Ʋ
// [1] - Ʈ���̴�
// [2] - ���̽� Stage_00
// [3] - �������� 1 Stage_01
// [4] - �������� 2 Stage_02
// [5] - �������� 3 Stage_03
// [6] - �������� 4 Stage_04
// [7] - ��� ���ҽ� ����
// ================================================
_v3 g_vWindDir = { 0.f, 0.f, 0.f };
// ================================================



// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
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

    // TODO: ���⿡ �ڵ带 �Է��մϴ�.
	CMainApp*		pMainApp = nullptr;

    // ���� ���ڿ��� �ʱ�ȭ�մϴ�.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
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
//  �Լ�: MyRegisterClass()
//
//  ����: â Ŭ������ ����մϴ�.
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
//   �Լ�: InitInstance(HINSTANCE, int)
//
//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   ����:
//
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	g_hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

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
//  �Լ�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ����:  �� â�� �޽����� ó���մϴ�.
//
//  WM_COMMAND  - ���� ���α׷� �޴��� ó���մϴ�.
//  WM_PAINT    - �� â�� �׸��ϴ�.
//  WM_DESTROY  - ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // �޴� ������ ���� �м��մϴ�.
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
            // TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�.
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

// ���� ��ȭ ������ �޽��� ó�����Դϴ�.
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
