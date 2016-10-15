// exemplo01.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ui_mini_estufa.h"
#include "Protocol.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HWND janela_baixo;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	PrincipalProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_EXEMPLO01, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_EXEMPLO01));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
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
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EXEMPLO01));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_EXEMPLO01);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
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
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   
   return TRUE;
}


INT_PTR CALLBACK PrincipalProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hdc;
		
	switch(message) {
	
		case WM_INITDIALOG: {
			//CheckDlgButton(hDlg, RB_ASPLENIO, 1);
			SetDlgItemText(hDlg, F_COM, _T("COM1"));
			break;
		}
		case WM_COMMAND: {						
			int event_type = HIWORD(wParam);
			if(event_type == BN_CLICKED) {
				char* port = (char*) malloc(32);
				GetDlgItemTextA(hDlg, F_COM, port, 31);

				int event_id = LOWORD(wParam);
				int plant = processSelectEvent(event_id);
				BOOLEAN plantChanged = plant >= 0;
				if(plantChanged) {
					protocolWriteVar(port, PROT_T_PLANT, plant);
				} 
				if(event_id == B_READ_SENSORS || plantChanged) {
					
					SetDlgItemInt(hDlg, F_TEMP, protocolReadVar(port, PROT_T_TEMP), FALSE);
					SetDlgItemInt(hDlg, F_LUX, protocolReadVar(port, PROT_T_LUX), FALSE);
					word currentPlant = protocolReadVar(port, PROT_T_PLANT);
					SetDlgItemTextA(hDlg, F_DISPLAY_PLANT, plantName(currentPlant));					
					syncPlant(currentPlant, hDlg);
					word time = protocolReadVar(port, PROT_T_TIME);
					char buf[64];
					sprintf(buf, "%.2d:%.2d", HIBYTE(time), LOBYTE(time));
					SetDlgItemTextA(hDlg, F_ARD_TIME, buf);
					
				} else if(event_id == B_APPLY_TIME) { 
					char* result = retrieveDateTime(hDlg);													
					tm* timeStruct = parseDate(result);

					free(result);
					free(timeStruct);
				}

				free(port);
			}		

			break;
		}
		case WM_CLOSE:
			EndDialog(hDlg, WM_CLOSE);
			DestroyWindow(janela_baixo);			
			break;		
		default:
			return FALSE;

	}
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
		case WM_CREATE:
			janela_baixo = hWnd;
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, PrincipalProc);			
			break;		
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		int retorno;
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:			
			retorno = MessageBox(hWnd, _T("Você deseja mesmo sair?"), _T("Atenção"), MB_OKCANCEL | MB_ICONWARNING);		
			if(retorno == 1) {
				DestroyWindow(hWnd);
			}			
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...

		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY: {
		PostQuitMessage(0);
		break;
	}
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