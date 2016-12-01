// Game2016O.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Game2016O.h"
#include "HSM\StateMachineManager.h"
#include "SMain.h"
#include "SGame.h"
#include "SIntroduction.h"
#include "HSM\EventWin32.h"
#include <io.h>
#include <fcntl.h>
#include "SMainMenu.h"
#include "SGameOver.h"
#include "SCredits.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
CStateMachineManager g_Game;					// The Game
// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
#ifdef _DEBUG
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	HWND g_hWnd = GetConsoleWindow();
	FILE* pNewStream = NULL;
	freopen_s(&pNewStream, "CON", "w", stdout);
	char *pBuffer = (char*)malloc(32);
	setvbuf(stdout, pBuffer, _IOFBF, 32);
	SetConsoleTitle(TEXT("Debugueando"));
#endif

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GAME2016O, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
	// En initInstance se crea la ventana por eso vamos a inicializar los estados
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GAME2016O));

    MSG msg;

    // Main message loop:
	// Get Message bloquea la aplicacion ¿,, cuando hay mensajes en la cola .
	// duerme y despierta le hilo principal
	// PeekMessage no es bloqueante no pone el hilo a dormir. Obtiene el mensaje si existiera alguno.
	// Si quiero verificar si hay algun mensaje puedo o no remover el mensaje para eso es el ultimo parameto
	bool bExit = false;
	CEventBase AppLoop;
	AppLoop.m_ulEventType = APP_LOOP;
	while (!bExit)
	{
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (WM_QUIT == msg.message)
			{
				bExit = true;
			}
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		// Hay que verificar que el ultio mensaje no haya sido psotquitmessage
		// Este es mi idle (Antes no tenia este espacio de trabajo)
		// Application time

		g_Game.Dispatch(&AppLoop); 
		g_Game.ProcessEvents(); // Se procesa todo lo que s ehaya producido
	}
    

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GAME2016O));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = 0; // (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_GAME2016O);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }
   // Si hay hWnd puedo continuar con mi aplicacion osea hacer la maquina
   // pSMain
   CSMain* pSMain = new CSMain();
   CSGame* pSGame = new CSGame();
   CSGameOver* pSGameOver = new CSGameOver();
   CSCredits* pSCredits = new CSCredits();
   CSIntroduction* pSIntroduction = new CSIntroduction();
   CSMainMenu* pSMainMenu = new CSMainMenu();


   g_Game.RegisterState(pSCredits, CLSID_CSCredits, 0);
   g_Game.RegisterState(pSGameOver, CLSID_CSGameOver, 0);
   g_Game.RegisterState(pSIntroduction, CLSID_CSIntroduction, 0);
   g_Game.RegisterState(pSGame, CLSID_CSGame, 0);
   g_Game.RegisterState(pSMainMenu, CLSID_CSMainMenu, 0);


   g_Game.RegisterState(pSMain, CLSID_CSMain, CLSID_CSMainMenu); //CLSID_CSIntroduction   CLSID_CSMainMenu
   
   g_Game.LinkToSuperState(CLSID_CSCredits, CLSID_CSMain);
   g_Game.LinkToSuperState(CLSID_CSGameOver, CLSID_CSMain);
   g_Game.LinkToSuperState(CLSID_CSGame, CLSID_CSMain);
   g_Game.LinkToSuperState(CLSID_CSIntroduction, CLSID_CSMain);
   g_Game.LinkToSuperState(CLSID_CSMainMenu, CLSID_CSMain);
   g_Game.SetInitialState(CLSID_CSMain);
   pSMain->m_hWnd = hWnd;
   pSMain->m_hInstance = hInstance;
   g_Game.Start();

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
#include "HSM\EventWin32.h"
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	auto Event = new CEventWin32(hWnd, message, wParam, lParam);
	g_Game.Dispatch(Event);
	SAFE_DELETE(Event);
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
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
            //PAINTSTRUCT ps;
            //HDC hdc = BeginPaint(hWnd, &ps);
            //// TODO: Add any drawing code that uses hdc here...
            //EndPaint(hWnd, &ps);
			ValidateRect(hWnd, NULL);
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

// Message handler for about box.
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
