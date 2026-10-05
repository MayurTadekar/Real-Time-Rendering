/**************************************************
 * S.G.M.P
 * @file: OGL.c
 * @brief: Implementation of  in Window OpenGL code
 * @author: Mayur S. Tadekar(mrtadekar@gmail.com)
 * @Copyright ©2020-2025 MarshallingVoid
 * @date: 27 March 2025 (Thursday) 12:30:21 PM
 **************************************************/

//	Headers
#include <windows.h>
#include <stdio.h>
#include "OGL.h"

#include <GL/gl.h>
#include <GL/glu.h>

#define	WIN_WIDTH 	800
#define	WIN_HEIGHT 	600

#define		LOG_ENTRY(file, str)		( fprintf(file, "LOG >>> %s\n",str ) )

//	Global Function Declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//	Global Variable Definitions
HDC 	gmst_hdc = NULL;
HGLRC 	hmst_ghrc = NULL;
BOOL 	gbmst_Fullscreen = FALSE;
BOOL 	gbmst_ActiveWindow = TRUE;
BOOL	gbmst_Done = FALSE;
BOOL	gb_EscapedPressed = FALSE;


DWORD	dwmst_Style;
HWND	gmst_hwnd = NULL;
FILE 	*gpmst_File = NULL;
DWORD 	gmst_DesktopWidth = 0;
DWORD 	gmst_DesktopHeight = 0;
WINDOWPLACEMENT 	wpmst_Prev;

//	Entry Point Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// Function Signature
	int initialize(void);
	void uninitialize(void);
	void display(void);
	void update(void);

	// Variable Declaration
	WNDCLASSEX	mst_wndcls;
	HWND	mst_hwnd;
	MSG		mst_msg;
	TCHAR	mst_szAppName[] = TEXT("CWindow");

	// Code
	if( NULL != (gpmst_File = fopen("LogFile.txt", "w")))
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
	mst_wndcls.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	mst_wndcls.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	mst_wndcls.hCursor = LoadCursor(NULL, IDC_ARROW);
	mst_wndcls.lpszClassName = mst_szAppName;
	mst_wndcls.lpszMenuName = NULL;

	//	Register Window Class
	RegisterClassEx(&mst_wndcls);

    gmst_DesktopWidth = GetSystemMetrics(SM_CXSCREEN);
    gmst_DesktopHeight = GetSystemMetrics(SM_CYSCREEN);

	//	Create Window
	mst_hwnd = CreateWindow(mst_szAppName,
						TEXT("Mayur Tadekar - FFP - Windows OS - 12_Pattern"),
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

	initialize();

	//	Show Window
	ShowWindow(mst_hwnd, iCmdShow);

	//	Paint background of the window
	UpdateWindow(mst_hwnd);

	int mst_result = initialize();

	if(mst_result != 0)
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
	while (FALSE == gbmst_Done )
	{
		if(PeekMessage(&mst_msg, NULL, 0, 0, PM_REMOVE))
		{
			if(WM_QUIT == mst_msg.message)
			{
				gbmst_Done = TRUE;
			}
			else
			{
				gbmst_Done = FALSE;
				TranslateMessage(&mst_msg);
				DispatchMessage(&mst_msg);
			}
		}
		else
		{
			if(TRUE == gbmst_ActiveWindow)
			{

				if(TRUE == gb_EscapedPressed)
				{
					gbmst_Done = TRUE;
				}

				display();
				update();
			}
		}
	}

	uninitialize();

	return((int)mst_msg.wParam);
} 

// Callback Function Definition
LRESULT CALLBACK WndProc(HWND mst_hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// Function Signature
	void toggleFullscreen(void);
	void resize(int, int);

	// Code
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rc;
	TCHAR	str[] = TEXT("Hello World!!!");

	switch(iMsg)
	{

	case WM_CREATE:
		ZeroMemory((void*)&wpmst_Prev, sizeof(WINDOWPLACEMENT));
		wpmst_Prev.length = sizeof(WINDOWPLACEMENT);
		break;

	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:
			gb_EscapedPressed = TRUE;
			break;
		}
		break;

	case WM_CHAR:
		switch(wParam)
		{
		case 'f':
		case 'F':
			if(FALSE == gbmst_Fullscreen)
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

	case WM_ERASEBKGND:
		return(0);

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
		uninitialize();
		break;

	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;
	}

	return(DefWindowProc(mst_hwnd, iMsg, wParam, lParam));
}

void toggleFullscreen(void)
{
	// Variable Declarations
	MONITORINFO mi = { sizeof(MONITORINFO) };

	// Code
	if( FALSE == gbmst_Fullscreen )
	{
		dwmst_Style = GetWindowLong(gmst_hwnd, GWL_STYLE);
		if( WS_OVERLAPPEDWINDOW & dwmst_Style )
		{
			if( GetWindowPlacement(gmst_hwnd, &wpmst_Prev) && GetMonitorInfo( MonitorFromWindow(gmst_hwnd, MONITORINFOF_PRIMARY), &mi ) )
			{
				SetWindowLong( gmst_hwnd, GWL_STYLE, (dwmst_Style & ~WS_OVERLAPPEDWINDOW));
				SetWindowPos( gmst_hwnd, 
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


int initialize(void)
{

	fprintf( gpmst_File, " Entered Initialize().");

	// Function Signature
	void resize(int, int);

	// Variable Declaration
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;

	// Code
	gmst_hdc = GetDC(gmst_hwnd);
	if( NULL == gmst_hdc )
	{
		LOG_ENTRY(gpmst_File, "GetDC() failed.");
		// DestroyWindow(gmst_hwnd);
		return(-1);
	}

	ZeroMemory( &pfd, sizeof(PIXELFORMATDESCRIPTOR) );
	pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR );
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;

	iPixelFormatIndex = ChoosePixelFormat(gmst_hdc, &pfd);

	if( 0 == iPixelFormatIndex )
	{
		LOG_ENTRY(gpmst_File, "ChoosePixelFormat() failed.");
		// DestroyWindow(gmst_hwnd);
		return(-2);
	}

	if( FALSE == SetPixelFormat(gmst_hdc, iPixelFormatIndex, &pfd) )
	{
		LOG_ENTRY(gpmst_File, "SetPixelFormat() Failed.");
		// DestroyWindow(gmst_hwnd);
		return(-3);
		
	}

	hmst_ghrc = wglCreateContext(gmst_hdc);
	if( NULL == hmst_ghrc)
	{
		LOG_ENTRY(gpmst_File, " wglCreateContext() Failed.");
		// DestroyWindow(gmst_hwnd);
		return(-4);
	}

	if( FALSE == wglMakeCurrent(gmst_hdc, hmst_ghrc) )
	{
		LOG_ENTRY(gpmst_File, "wglMakeCurrent() Failed.");
		// DestroyWindow(gmst_hwnd);
		return(-5);
	}

	glEnable(GL_POINT_SIZE);
	glPointSize(4.0f);

	glEnable(GL_LINE_WIDTH);
	glLineWidth(4.0f);

	//Set Color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Warmup call to Resize
	resize(WIN_WIDTH, WIN_HEIGHT);

	fprintf( gpmst_File, " Exit Initialize().");
	return(0);
}

void resize(int width, int height)
{

	LOG_ENTRY( gpmst_File, " Entered Resize().");

	// Code
	if(height <= 0)
		height = 1;

	glViewport(0, 0, (GLsizei) width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(	45.0f,
					(GLfloat)width / (GLfloat)height,
					0.1f,
					100.0f);

	LOG_ENTRY( gpmst_File, " Exit Resize().");
}

void display(void)
{
	// Variable Definition
	GLfloat x_center = 0.3f * 3.0f / 2.0f;
	GLfloat y_center = 0.3f * 3.0f / 2.0f;

	// Code
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	//	रांगोळी चे टिपके 
	glLoadIdentity();
	glTranslatef(-2.5f, 1.0f, -6.0f);
	glBegin(GL_POINTS);
	{
		for(int ole = 0; ole < 4; ++ole)
		{
			for(int ile = 0; ile < 4; ++ile)
			{
				glVertex3f(	0.3f * ile - x_center, 
							0.3f * ole - y_center, 
							0.0f);
			}
		}
	}
	glEnd();

	//	Lines ची रांगोळी  
	glLoadIdentity();
	glTranslatef(0.0f, 1.0f, -6.0f);
	
	for(int ole = 0; ole < 3; ++ole)
	{
		glBegin(GL_LINE_STRIP);
		{		
			for(int ile = 0; ile < 4; ++ile)
			{
				glVertex3f(	0.3f * ile - x_center, 
							-0.3f * ole + y_center, 
							0.0f);

				if(ile != 3 && (ole+1) != 4)
					glVertex3f(	0.3f * ile - x_center, 
								-0.3f * (ole+1) + y_center, 
								0.0f);
			}
		}
		glEnd();

		glBegin(GL_LINE_STRIP);
		{		
			for(int ile = 0; ile < 4; ++ile)
			{
				glVertex3f(	0.3f * ile - x_center, 
							-0.3f * ole + y_center, 
							0.0f);
			}
		}
		glEnd();
	}

	//	Lines ने बनलेले box रांगोळी  
	glLoadIdentity();
	glTranslatef(2.5f, 1.0f, -6.0f);
	
	for(int ole = 0; ole < 4; ++ole)
	{
		glBegin(GL_LINE_STRIP);
		{		
			for(int ile = 0; ile < 4; ++ile)
			{
				glVertex3f(	0.3f * ile - x_center, 
							-0.3f * ole + y_center, 
							0.0f);
			}
		}
		glEnd();

		glBegin(GL_LINE_STRIP);
		{		
			for(int ile = 0; ile < 4; ++ile)
			{
				glVertex3f(	0.3f * ole - x_center, 
							-0.3f * ile + y_center, 
							0.0f);
			}
		}
		glEnd();
	}

	//	Lines + Box ची रांगोळी  
	glLoadIdentity();
	glTranslatef(-2.5f, -1.0f, -6.0f);
	
	for(int ole = 0; ole < 4; ++ole)
	{
		if(ole < 3)
		{
			glBegin(GL_LINE_STRIP);
			{		
				for(int ile = 0; ile < 4; ++ile)
				{
					glVertex3f(	0.3f * ile - x_center, 
								-0.3f * ole + y_center, 
								0.0f);

					glVertex3f(	0.3f * ile - x_center, 
								-0.3f * (ole+1) + y_center, 
								0.0f);
				}
			}
			glEnd();
		}

		glBegin(GL_LINE_STRIP);
		{		
			for(int ile = 0; ile < 4; ++ile)
			{
				glVertex3f(	0.3f * ile - x_center, 
							-0.3f * ole + y_center, 
							0.0f);
			}
		}
		glEnd();
	}


	//	Lines Connection ची रांगोळी 
	glLoadIdentity();
	glTranslatef(0.0f, -1.0f, -6.0f);
	glBegin(GL_LINES);
	{
		// Line 01
		glVertex3f(	0.0f - x_center,
					0.0f + y_center,
					0.0f);

		glVertex3f(	0.9f - x_center,
					0.0f + y_center,
					0.0f);

		// Line 02
		glVertex3f(	0.0f - x_center,
					0.0f + y_center,
					0.0f);

		glVertex3f(	0.9f - x_center,
					-0.3f + y_center,
					0.0f);

		// Line 03
		glVertex3f(	0.0f - x_center,
					0.0f + y_center,
					0.0f);

		glVertex3f(	0.9f - x_center,
					-0.6f + y_center,
					0.0f);

		// Line 04
		glVertex3f(	0.0f - x_center,
					0.0f + y_center,
					0.0f);

		glVertex3f(	0.9f - x_center,
					-0.9f + y_center,
					0.0f);

		// Line 05
		glVertex3f(	0.0f - x_center,
					0.0f + y_center,
					0.0f);

		glVertex3f(	0.6f - x_center,
					-0.9f + y_center,
					0.0f);

		// Line 06
		glVertex3f(	0.0f - x_center,
					0.0f + y_center,
					0.0f);

		glVertex3f(	0.3f - x_center,
					-0.9f + y_center,
					0.0f);

		// Line 07
		glVertex3f(	0.0f - x_center,
					0.0f + y_center,
					0.0f);

		glVertex3f(	0.0f - x_center,
					-0.9f + y_center,
					0.0f);

		// Line 08
		glVertex3f(	0.9f - x_center,
					0.0f + y_center,
					0.0f);

		glVertex3f(	0.9f - x_center,
					-0.9f + y_center,
					0.0f);

		// Line 08
		glVertex3f(	0.0f - x_center,
					-0.9f + y_center,
					0.0f);

		glVertex3f(	0.9f - x_center,
					-0.9f + y_center,
					0.0f);
	}
	glEnd();

	//	Lines ने बनलेले box रांगोळी and Color 
	glLoadIdentity();
	glTranslatef(2.5f, -1.0f, -6.0f);

	for(int ole = 0; ole < 4; ++ole)
	{
		glBegin(GL_TRIANGLE_STRIP);
		{		
			if(ole < 3)
			{
				for(int ile = 0; ile < 4; ++ile)
				{
					switch(ole)
					{
					case 2:
						glColor3f(1.0f, 0.0f, 0.0f);
						break;

					case 1:
						glColor3f(0.0f, 1.0f, 0.0f);
						break;

					case 0:
						glColor3f(0.0f, 0.0f, 1.0f);
						break;
					}

					glVertex3f(	-0.3f * ole + y_center, 
								0.3f * ile - x_center, 
								0.0f);

					glVertex3f(	-0.3f * (ole+1) + y_center, 
								0.3f * ile - x_center, 
								0.0f);
				}
			}
				
			glColor3f(1.0f, 1.0f, 1.0f);			
		}
		glEnd();
	}
	
	for(int ole = 0; ole < 4; ++ole)
	{
		glBegin(GL_LINE_STRIP);
		{		
			for(int ile = 0; ile < 4; ++ile)
			{
				glVertex3f(	0.3f * ile - x_center, 
							-0.3f * ole + y_center, 
							0.0f);
			}
		}
		glEnd();

		glBegin(GL_LINE_STRIP);
		{		
			for(int ile = 0; ile < 4; ++ile)
			{
				glVertex3f(	0.3f * ole - x_center, 
							-0.3f * ile + y_center, 
							0.0f);
			}
		}
		glEnd();
	}

	SwapBuffers(gmst_hdc);
}


void update(void)
{
	// Code
}

void uninitialize(void)
{
	// Code
	void toggleFullscreen(void);

	if(TRUE == gbmst_Fullscreen)
	{
		toggleFullscreen();
		gbmst_Fullscreen = FALSE;
	}

	if( hmst_ghrc == wglGetCurrentContext())
	{
		wglMakeCurrent(NULL, NULL);
	}

	if ( hmst_ghrc )
	{
		wglDeleteContext(hmst_ghrc);
		hmst_ghrc = NULL;
	}

	if( gmst_hdc )
	{
		ReleaseDC(gmst_hwnd, gmst_hdc);
		gmst_hdc = NULL;
	}

	if( gmst_hwnd )
	{
		DestroyWindow(gmst_hwnd);
		gmst_hwnd = NULL;
	}

	if(gpmst_File)
	{
		fprintf(gpmst_File, "Program Terminated Successfully...\n");
		fclose(gpmst_File);
		gpmst_File = NULL;
	}

}


