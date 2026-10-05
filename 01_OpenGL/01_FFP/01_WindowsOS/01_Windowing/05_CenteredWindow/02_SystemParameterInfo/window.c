/**************************************************
 * S.G.M.P
 * @file: window.c
 * @brief: Implementation of Fullscreen for win32 code
 * @author: Mayur S. Tadekar(mrtadekar@gmail.com)
 * @Copyright ©2020-2023 MarshallingVoid
 * @date: 15 December 2024 (Sunday) 10:23:52 PM
 **************************************************/

//	Headers
#include <windows.h>

#include "window.h"

#define		WIN_WIDTH	800
#define		WIN_HEIGHT	600

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
	DWORD 	desk_width;
	DWORD 	desk_height;
	RECT 	deskRect;

	// Code
	mst_wndcls.cbSize = sizeof(WNDCLASSEX);
	mst_wndcls.style = CS_HREDRAW | CS_VREDRAW;
	mst_wndcls.cbClsExtra = 0;
	mst_wndcls.cbWndExtra = 0;
	mst_wndcls.lpfnWndProc = WndProc;
	mst_wndcls.hInstance = hInstance;
	mst_wndcls.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	mst_wndcls.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MY_LOGO));
	mst_wndcls.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MY_LOGO));
	mst_wndcls.hCursor = LoadCursor(NULL, IDC_ARROW);
	mst_wndcls.lpszClassName = mst_szAppName;
	mst_wndcls.lpszMenuName = NULL;

	//	Register Window Class
	RegisterClassEx(&mst_wndcls);

	SystemParametersInfo(SPI_GETWORKAREA, 0, &deskRect, 0);

	//	Create Window
	mst_hwnd = CreateWindow(mst_szAppName,
						TEXT("Mayur Tadekar - FFP - Windows OS - 05 Centered Window SPI"),
						WS_OVERLAPPEDWINDOW,
						((deskRect.right - deskRect.left)/2 - WIN_WIDTH/2),
						((deskRect.bottom - deskRect.top)/2 - WIN_HEIGHT/2),
						WIN_WIDTH,
						WIN_HEIGHT,
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
	// Code
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rc;
	TCHAR	str[] = TEXT("Hello World!!!");

	switch(iMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return(DefWindowProc(mst_hwnd, iMsg, wParam, lParam));
}

