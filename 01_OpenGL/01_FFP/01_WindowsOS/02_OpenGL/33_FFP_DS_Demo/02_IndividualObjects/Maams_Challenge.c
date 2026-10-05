/**************************************************
 * S.G.M.P
 * @file: Maams_Challenge.c
 * @brief: Implementation of Maams Challenge Video
 * @author: Mayur S. Tadekar(mrtadekar@gmail.com)
 * @Copyright ©2020-2023 MarshallingVoid
 * @date: 14 January 2025 (Tuesday) 07:20:40 PM
 **************************************************/

// Header Files
#include <windows.h>
#include <Mmsystem.h>
#include <stdio.h>
#include <GL/gl.h>

#include "OGL.h"
#include "palva_palvi.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#define LOG_ENTRY(file, str) (fprintf(file, "LOG >>> %s\n", str))

// Global Variable Declarations
BOOL bFullscreen = FALSE;

int scene = 1;
int close_eye = 1;

//	Global Variable Definitions
HDC gmst_hdc = NULL;
HGLRC hmst_ghrc = NULL;
BOOL gbmst_Fullscreen = FALSE;
BOOL gbmst_ActiveWindow = TRUE;
BOOL gbmst_Done = FALSE;
BOOL gb_EscapedPressed = FALSE;

DWORD dwmst_Style;
HWND gmst_hwnd = NULL;
FILE *gpmst_File = NULL;
DWORD gmst_DesktopWidth = 0;
DWORD gmst_DesktopHeight = 0;
WINDOWPLACEMENT wpmst_Prev;

//	Global Function Declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//	Entry Point Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// Function Signature
	int Initialize(void);
	void Uninitialize(void);
	void display(void);
	void update(void);

	// Variable Declaration
	WNDCLASSEX mst_wndcls;
	HWND mst_hwnd;
	MSG mst_msg;
	TCHAR mst_szAppName[] = TEXT("CWindow");

	// Code
	if (NULL != (gpmst_File = fopen("LogFile.txt", "w")))
	{
		fprintf(gpmst_File, "Window Started Successfully\n");
	}
	else
	{
		fprintf(gpmst_File, "Logfile cannot be created\n");
		exit(0);
	}

	mst_wndcls.cbSize = sizeof(WNDCLASSEX);
	mst_wndcls.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
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

	gmst_DesktopWidth = GetSystemMetrics(SM_CXSCREEN);
	gmst_DesktopHeight = GetSystemMetrics(SM_CYSCREEN);

	//	Create Window
	mst_hwnd = CreateWindow(mst_szAppName,
							TEXT("Mayur Tadekar - FFP - Windows OS - 01 Bluescreen"),
							WS_OVERLAPPEDWINDOW,
							(gmst_DesktopWidth / 2 - WIN_WIDTH / 2),
							(gmst_DesktopHeight / 2 - WIN_HEIGHT / 2),
							WIN_WIDTH,
							WIN_HEIGHT,
							NULL,
							NULL,
							hInstance,
							NULL);

	gmst_hwnd = mst_hwnd;

	//	Show Window
	ShowWindow(mst_hwnd, iCmdShow);

	//	Paint background of the window
	UpdateWindow(mst_hwnd);

	int mst_result = Initialize();

	if (mst_result != 0)
	{
		fprintf(gpmst_File, "Initialize() Failed\n");
		DestroyWindow(mst_hwnd);
		gmst_hwnd = NULL;
	}
	else
	{
		fprintf(gpmst_File, "Initialize() Completed Successfully\n");
	}

	SetForegroundWindow(gmst_hwnd);
	SetFocus(gmst_hwnd);

	//	Game Loop
	while (FALSE == gbmst_Done)
	{
		if (PeekMessage(&mst_msg, NULL, 0, 0, PM_REMOVE))
		{
			if (WM_QUIT == mst_msg.message)
			{
				gbmst_Done = TRUE;
			}
			else
			{
				TranslateMessage(&mst_msg);
				DispatchMessage(&mst_msg);
			}
		}
		else
		{
			if (TRUE == gbmst_ActiveWindow)
			{

				if (TRUE == gb_EscapedPressed)
				{
					gbmst_Done = TRUE;
				}

				display();
				update();
			}
		}
	}

	Uninitialize();

	return ((int)mst_msg.wParam);
}

// Callback Function Definition
LRESULT CALLBACK WndProc(HWND mst_hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// Function Signature
	void toggleFullscreen(void);
	void resize(int, int);
	void TimerFunc();

	// Code
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rc;
	TCHAR str[] = TEXT("Hello World!!!");

	switch (iMsg)
	{

	case WM_CREATE:
		ZeroMemory((void *)&wpmst_Prev, sizeof(WINDOWPLACEMENT));
		wpmst_Prev.length = sizeof(WINDOWPLACEMENT);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			gb_EscapedPressed = TRUE;
			break;
		}
		break;

	case WM_CHAR:
		switch (wParam)
		{
		case 'f':
		case 'F':
			if (FALSE == gbmst_Fullscreen)
			{
				toggleFullscreen();
				gbmst_Fullscreen = TRUE;
			}
			else
			{
				toggleFullscreen();
				gbmst_Fullscreen = FALSE;
			}
			break;
		}
		break;

	case WM_TIMER:
		KillTimer(gmst_hwnd, 1);

		TimerFunc();

		SetTimer(gmst_hwnd, 1, 100, NULL);
		break;

	case WM_ERASEBKGND:
		return (0);

	case WM_SETFOCUS:
		gbmst_ActiveWindow = TRUE;
		break;

	case WM_KILLFOCUS:
		gbmst_ActiveWindow = FALSE;
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_CLOSE:
		Uninitialize();
		break;

	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;
	}

	return (DefWindowProc(mst_hwnd, iMsg, wParam, lParam));
}

void toggleFullscreen(void)
{
	// Variable Declarations
	MONITORINFO mi = {sizeof(MONITORINFO)};

	// Code
	if (FALSE == gbmst_Fullscreen)
	{
		dwmst_Style = GetWindowLong(gmst_hwnd, GWL_STYLE);
		if (WS_OVERLAPPEDWINDOW & dwmst_Style)
		{
			if (GetWindowPlacement(gmst_hwnd, &wpmst_Prev) && GetMonitorInfo(MonitorFromWindow(gmst_hwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(gmst_hwnd, GWL_STYLE, (dwmst_Style & ~WS_OVERLAPPEDWINDOW));
				SetWindowPos(gmst_hwnd,
							 HWND_TOP,
							 mi.rcMonitor.left,
							 mi.rcMonitor.top,
							 (mi.rcMonitor.right - mi.rcMonitor.left),
							 (mi.rcMonitor.bottom - mi.rcMonitor.top),
							 SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
		gbmst_Fullscreen = TRUE;
	}
	else
	{
		SetWindowLong(gmst_hwnd, GWL_STYLE, (dwmst_Style | WS_OVERLAPPEDWINDOW));
		SetWindowPlacement(gmst_hwnd, &wpmst_Prev);
		SetWindowPos(gmst_hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
		gbmst_Fullscreen = FALSE;
	}
}

int Initialize(void)
{

	fprintf(gpmst_File, " Entered Initialize().");

	// Function Signature
	void resize(int, int);

	// Variable Declaration
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;

	// Code
	gmst_hdc = GetDC(gmst_hwnd);
	if (NULL == gmst_hdc)
	{
		LOG_ENTRY(gpmst_File, "GetDC() failed.");
		// DestroyWindow(gmst_hwnd);
		return (-1);
	}

	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;

	iPixelFormatIndex = ChoosePixelFormat(gmst_hdc, &pfd);

	if (0 == iPixelFormatIndex)
	{
		LOG_ENTRY(gpmst_File, "ChoosePixelFormat() failed.");
		// DestroyWindow(gmst_hwnd);
		return (-2);
	}

	if (FALSE == SetPixelFormat(gmst_hdc, iPixelFormatIndex, &pfd))
	{
		LOG_ENTRY(gpmst_File, "SetPixelFormat() Failed.");
		// DestroyWindow(gmst_hwnd);
		return (-3);
	}

	hmst_ghrc = wglCreateContext(gmst_hdc);
	if (NULL == hmst_ghrc)
	{
		LOG_ENTRY(gpmst_File, " wglCreateContext() Failed.");
		// DestroyWindow(gmst_hwnd);
		return (-4);
	}

	if (FALSE == wglMakeCurrent(gmst_hdc, hmst_ghrc))
	{
		LOG_ENTRY(gpmst_File, "wglMakeCurrent() Failed.");
		// DestroyWindow(gmst_hwnd);
		return (-5);
	}

	// Set Color
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Warmup call to Resize
	resize(WIN_WIDTH, WIN_HEIGHT);

	fprintf(gpmst_File, " Exit Initialize().");
	return (0);
}

void resize(int width, int height)
{

	LOG_ENTRY(gpmst_File, " Entered Resize().");

	// Code
	if (height <= 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	LOG_ENTRY(gpmst_File, " Exit Resize().");
}

void display(void)
{
	// Code
	glClear(GL_COLOR_BUFFER_BIT);

	if (scene)
	{
		Scene_01(0.0f, 0.5f, 50.0f, close_eye % 10 == 0);
	}
	else
	{
		Scene_02(0.0f, 0.505f, 50.0f, close_eye % 10 == 0);
	}

	SwapBuffers(gmst_hdc);
}

void update(void)
{
	// Code
}

void Uninitialize(void)
{
	// Code
	void toggleFullscreen(void);

	if (TRUE == gbmst_Fullscreen)
	{
		toggleFullscreen();
		gbmst_Fullscreen = FALSE;
	}

	if (hmst_ghrc == wglGetCurrentContext())
	{
		wglMakeCurrent(NULL, NULL);
	}

	if (hmst_ghrc)
	{
		wglDeleteContext(hmst_ghrc);
		hmst_ghrc = NULL;
	}

	if (gmst_hdc)
	{
		ReleaseDC(gmst_hwnd, gmst_hdc);
		gmst_hdc = NULL;
	}

	if (gmst_hwnd)
	{
		DestroyWindow(gmst_hwnd);
		gmst_hwnd = NULL;
	}

	if (gpmst_File)
	{
		fprintf(gpmst_File, "Program Terminated Successfully...\n");
		fclose(gpmst_File);
		gpmst_File = NULL;
	}
}

void TimerFunc()
{
	//	Code
	scene = !scene;

	close_eye++;

	if (close_eye > 10)
		close_eye = 0;
}
