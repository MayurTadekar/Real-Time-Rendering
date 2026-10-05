/**************************************************
 * S.G.M.P
 * @file: window.c
 * @brief:
 * @author: Mayur S. Tadekar(mrtadekar@gmail.com)
 * @Copyright ©2020-2023 MarshallingVoid
 * @date: 15 December 2024 (Sunday) 10:23:52 PM
 **************************************************/

//	Headers
#include <windows.h>

//	Global Function Declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


//	Entry Point Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// Variable Declaration
	WNDCLASSEX	mst_wndcls;
	HWND	mst_hwnd;
	MSG		mst_msg;
	TCHAR	mst_szAppName[] = TEXT("CWindow");

	// Code
	mst_wndcls.cbSize = sizeof(WNDCLASSEX);
	mst_wndcls.style = CS_HREDRAW | CS_VREDRAW;
	mst_wndcls.cbClsExtra = 0;
	mst_wndcls.cbWndExtra = 0;
	mst_wndcls.lpfnWndProc = WndProc;
	mst_wndcls.hInstance = hInstance;
	mst_wndcls.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	mst_wndcls.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	mst_wndcls.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	mst_wndcls.hCursor = LoadCursor(NULL, IDC_ARROW);
	mst_wndcls.lpszClassName = mst_szAppName;
	mst_wndcls.lpszMenuName = NULL;

	//	Register Window Class
	RegisterClassEx(&mst_wndcls);

	//	Create Window
	mst_hwnd = CreateWindow(mst_szAppName,
						TEXT("Mayur Tadekar - FFP - Windows OS - 02 MessageBox"),
						WS_OVERLAPPEDWINDOW,
						CW_USEDEFAULT,
						CW_USEDEFAULT,
						CW_USEDEFAULT,
						CW_USEDEFAULT,
						NULL,
						NULL,
						hInstance,
						NULL);

	//	Show Window
	ShowWindow(mst_hwnd, iCmdShow);

	//	Paint background of the window
	UpdateWindow(mst_hwnd);

	//	Message Loop
	while(GetMessage(&mst_msg, NULL, 0, 0))
	{
		TranslateMessage(&mst_msg);
		DispatchMessage(&mst_msg);
	}

	return((int)mst_msg.wParam);
} 

// Callback Function Definition
LRESULT CALLBACK WndProc(HWND mst_hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// Variable Declaration
	int num1 = 10;
	int num2 = 20;

	// Code
	switch(iMsg)
	{

	case WM_CHAR:
		switch(wParam)
		{
		case 'F':
			MessageBox(mst_hwnd, TEXT("F Key is pressed"), TEXT("WM_KEYDOWN"), MB_OK);
			break;

		case 'f':
			MessageBox(mst_hwnd, TEXT("f Key is pressed"), TEXT("WM_KEYDOWN"), MB_OK);
			break;

		case 'r':
		case 'R':
			int result = num1 + num2;
			TCHAR str_result[255];
			wsprintf(str_result, TEXT("Sum of %d & %d is %d."), num1, num2, result);
			MessageBox(mst_hwnd, str_result, TEXT("Addition Operation"), MB_OK);
			break;

		default:
			break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return(DefWindowProc(mst_hwnd, iMsg, wParam, lParam));
}

