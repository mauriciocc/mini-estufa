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
	static UINT_PTR timer = NULL;
	static WORD TIMER_ID = 999;
	static HANDLE serialPort = NULL;

	PAINTSTRUCT ps;
	HDC hdc;
		
	BOOLEAN timerEvent = FALSE;
	switch(message) {
	
		case WM_INITDIALOG: {			
			SetDlgItemText(hDlg, F_COM, _T("COM1"));			
			break;
		}
		case WM_TIMER: {
			timerEvent = TRUE;
		}
		case WM_COMMAND: {						
			int event_type = HIWORD(wParam);
			if(event_type == BN_CLICKED) {
				
				int event_id = LOWORD(wParam);


				if(event_id == BTN_TOGGLE_CONNECT) {
					if(serialPort == NULL) {
						char* port = (char*) malloc(32);
						GetDlgItemTextA(hDlg, F_COM, port, 31);						
						serialPort = openPort(port);			
						free(port);
						if(serialPort == INVALID_HANDLE_VALUE){
							MessageBoxA(hDlg, 
								"Não foi possível estabelecer conxão com a porta informada, verifique se o dispositivo esta conectado ao PC e a porta esta configurada corretamente",
								"Atenção!",
								MB_ICONWARNING | MB_OK);
							return TRUE;
						}						
						SetDlgItemTextA(hDlg, BTN_TOGGLE_CONNECT, "Desconectar");
						EnableWindow(GetDlgItem(hDlg, F_COM), FALSE);
						event_id = BTN_START_SYNC;
					} else {
						CloseHandle(serialPort);
						serialPort = NULL;
						SetDlgItemTextA(hDlg, BTN_TOGGLE_CONNECT, "Conectar");
						EnableWindow(GetDlgItem(hDlg, F_COM), TRUE);	
						event_id = BTN_STOP_SYNC;
					}						
				}

				if(event_id == BTN_START_SYNC) {
					if(timer == NULL) {
						timer = SetTimer(hDlg, TIMER_ID, 1000, (TIMERPROC) NULL);
						EnableWindow(GetDlgItem(hDlg, BTN_START_SYNC), FALSE);						
						EnableWindow(GetDlgItem(hDlg, BTN_STOP_SYNC), TRUE);
						
						timerEvent = TRUE;
					}
				}

				if(event_id == BTN_STOP_SYNC) {
					if(timer != NULL) {
						KillTimer(hDlg, timer);
						timer = NULL;
						EnableWindow(GetDlgItem(hDlg, BTN_START_SYNC), TRUE);						
						EnableWindow(GetDlgItem(hDlg, BTN_STOP_SYNC), FALSE);
						return TRUE;
					}
				}

				

				if(serialPort == NULL) {
					MessageBoxA(hDlg, 
						"Conecte ao dispositivo antes de executar operações!",
						"Atenção!",
						MB_ICONWARNING | MB_OK
						);
					return TRUE;
				}

				int plant = processSelectEvent(event_id);
				BOOLEAN plantChanged = plant >= 0;
				if(plantChanged) {
					protocolWriteVar(serialPort, PROT_T_PLANT, plant);
				} 

				if(event_id == B_APPLY_TIME) { 
					char* result = retrieveDateTime(hDlg);													
					tm* timeStruct = parseDate(result);
					word timeVal = toWord(timeStruct->tm_hour, timeStruct->tm_min);
					protocolWriteVar(serialPort, PROT_T_TIME, timeVal);
					free(result);
					free(timeStruct);
				}

				if(event_id == B_APPLY_TIME || event_id == B_READ_SENSORS || plantChanged || timerEvent) {					
					readAndUpdateScreen(hDlg, serialPort);										
					updateLogs(hDlg, serialPort);
				}

				if(event_id == BTN_READ_LOGS) {
					updateLogs(hDlg, serialPort);
				}

				if(event_id == BTN_CLEAR_LOGS) {
					protocolWriteVar(serialPort, PROT_T_STATUS, LOG_ID);
					SetDlgItemTextA(hDlg, F_LOG_OUTPUT, "");
				}
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