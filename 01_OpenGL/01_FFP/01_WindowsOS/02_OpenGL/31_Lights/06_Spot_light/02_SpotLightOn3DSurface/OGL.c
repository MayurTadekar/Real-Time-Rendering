/**************************************************
 * S.G.M.P
 * @file: OGL.c
 * @brief: Implementation of  in Window OpenGL code
 * @author: Mayur S. Tadekar(mrtadekar@gmail.com)
 * @Copyright ©2020-2025 MarshallingVoid
 * @date: 23 February 2025 (Sunday) 09:35:15 PM
 **************************************************/

//	Headers
#include <windows.h>
#include <stdio.h>
#include "OGL.h"

#include <GL/gl.h>
#include <GL/glu.h>

#define	WIN_WIDTH 	800
#define	WIN_HEIGHT 	600

#define MAX_X	50
#define	DELTA_X (1.0f / 10.0f)

#define MAX_Z	50
#define	DELTA_Z (1.0f / 10.0f)

#define MAX_STRIPS	50 * 10

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

BOOL gbLight = FALSE;
BOOL gbRotate = FALSE;

GLUquadric *mst_Quadric = NULL;

GLfloat lightAmbient[] = { 0.1f, 0.1f, 0.1f, 1.0f };	
GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };	
GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPosition[] = { 0.0f, 20.0f, 0.0f, 1.0f };
GLfloat lightDirection[] = { 0.0f, -1.0f, 0.0f};


GLfloat materialAmbient[] = {0.0215, 0.1745, 0.0215, 1.0f,
							 0.135, 0.2225, 0.1575, 1.0f,
							 0.05375, 0.05, 0.06625, 1.0f,
							 0.25, 0.20725, 0.20725, 1.0f,
							 0.1745, 0.01175, 0.01175, 1.0f,
							 0.1, 0.18725, 0.1745, 1.0f,
							 0.329412, 0.223529, 0.027451, 1.0f,
							 0.2125, 0.1275, 0.054, 1.0f,
							 0.25, 0.25, 0.25, 1.0f,
							 0.19125, 0.0735, 0.0225, 1.0f,
							 0.24725, 0.1995, 0.0745, 1.0f,
							 0.19225, 0.19225, 0.19225, 1.0f,
							 0.0, 0.0, 0.0, 1.0f,
							 0.0, 0.1, 0.06, 1.0f,
							 0.0, 0.0, 0.0, 1.0f,
							 0.0, 0.0, 0.0, 1.0f,
							 0.0, 0.0, 0.0, 1.0f,
							 0.0, 0.0, 0.0, 1.0f,
							 0.02, 0.02, 0.02, 1.0f,
							 0.0, 0.05, 0.05, 1.0f,
							 0.0, 0.05, 0.0, 1.0f,
							 0.05, 0.0, 0.0, 1.0f,
							 0.05, 0.05, 0.05, 1.0f,
							 0.05, 0.05, 0.0, 1.0f};

GLfloat materialDiffuse[] = {0.07568, 0.61424, 0.07568, 1.0f,
							 0.54, 0.89, 0.63, 1.0f,
							 0.18275, 0.17, 0.22525, 1.0f,
							 1.0, 0.829, 0.829, 1.0f,
							 0.61424, 0.04136, 0.04136, 1.0f,
							 0.396, 0.74151, 0.69102, 1.0f,
							 0.780392, 0.568627, 0.113725, 1.0f,
							 0.714, 0.4284, 0.18144, 1.0f,
							 0.4, 0.4, 0.4, 1.0f,
							 0.7038, 0.27048, 0.0828, 1.0f,
							 0.75164, 0.60648, 0.22648, 1.0f,
							 0.50754, 0.50754, 0.50754, 1.0f,
							 0.01, 0.01, 0.01, 1.0f,
							 0.0, 0.50980392, 0.50980392, 1.0f,
							 0.1, 0.35, 0.1, 1.0f,
							 0.5, 0.0, 0.0, 1.0f,
							 0.55, 0.55, 0.55, 1.0f,
							 0.5, 0.5, 0.0, 1.0f,
							 0.01, 0.01, 0.01, 1.0f,
							 0.4, 0.5, 0.5, 1.0f,
							 0.4, 0.5, 0.4, 1.0f,
							 0.5, 0.4, 0.4, 1.0f,
							 0.5, 0.5, 0.5, 1.0f,
							 0.5, 0.5, 0.4, 1.0f};

GLfloat materialSpecular[] = {0.633, 0.727811, 0.633, 1.0f,
							  0.316228, 0.316228, 0.316228, 1.0f,
							  0.332741, 0.328634, 0.346435, 1.0f,
							  0.296648, 0.296648, 0.296648, 1.0f,
							  0.727811, 0.626959, 0.626959, 1.0f,
							  0.297254, 0.30829, 0.306678, 1.0f,
							  0.992157, 0.941176, 0.807843, 1.0f,
							  0.393548, 0.271906, 0.166721, 1.0f,
							  0.774597, 0.774597, 0.774597, 1.0f,
							  0.256777, 0.137622, 0.086014, 1.0f,
							  0.628281, 0.555802, 0.366065, 1.0f,
							  0.508273, 0.508273, 0.508273, 1.0f,
							  0.50, 0.50, 0.50, 1.0f,
							  0.50196078, 0.50196078, 0.50196078, 1.0f,
							  0.45, 0.55, 0.45, 1.0f,
							  0.7, 0.6, 0.6, 1.0f,
							  0.70, 0.70, 0.70, 1.0f,
							  0.60, 0.60, 0.50, 1.0f,
							  0.4, 0.4, 0.4, 1.0f,
							  0.04, 0.7, 0.7, 1.0f,
							  0.04, 0.7, 0.04, 1.0f,
							  0.7, 0.04, 0.04, 1.0f,
							  0.7, 0.7, 0.7, 1.0f,
							  0.7, 0.7, 0.04, 1.0f};

GLfloat materialShininess[] = {0.6 * 128,
							   0.1 * 128,
							   0.3 * 128,
							   0.088 * 128,
							   0.6 * 128,
							   0.1 * 128,
							   0.21794872 * 128,
							   0.2 * 128,
							   0.6 * 128,
							   0.1 * 128,
							   0.4 * 128,
							   0.4 * 128,
							   0.25 * 128,
							   0.25 * 128,
							   0.25 * 128,
							   0.25 * 128,
							   0.25 * 128,
							   0.25 * 128,
							   0.078125 * 128,
							   0.078125 * 128,
							   0.078125 * 128,
							   0.078125 * 128,
							   0.078125 * 128,
							   0.078125 * 128};


GLfloat angleCutOff = 25.0f;
GLfloat angleExponent = 20.0f;

GLfloat floorColor[MAX_STRIPS][3];

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
						TEXT("Mayur Tadekar - FFP - Windows OS - 31_Lights - 06_Spot_light"),
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
		{
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

			case 'L':
			case 'l':
				if(gbLight)
				{
					gbLight = FALSE;
					glDisable(GL_LIGHTING);
				} 
				else
				{
					gbLight = TRUE;
					glEnable(GL_LIGHTING);
				}
				break;

			case 'W':
			case 'w':
				lightPosition[2] = lightPosition[2] - 0.2f;
				break;

			case 'S':
			case 's':
					lightPosition[2] = lightPosition[2] + 0.2f;
				break;

			case 'A':
			case 'a':
					lightPosition[0] = lightPosition[0] - 0.2f;
				break;

			case 'D':
			case 'd':
					lightPosition[0] = lightPosition[0] + 0.2f;
				break;

			case 'R':
			case 'r':
				lightPosition[0] = 0.0f;
				lightPosition[1] = 20.0f;
				lightPosition[2] = 0.0f;
				lightPosition[3] = 1.0f;
				break;

			case '1':
				angleCutOff -= 0.5f;
				break;

			case '2':
				angleCutOff += 0.5f;
				break;

			case '3':
				angleExponent -= 0.5f;
				break;

			case '4':
				angleExponent += 0.5f;
				break;
			}

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

	case WM_TIMER:
		KillTimer(gmst_hwnd, 1234);

		if(gbRotate == TRUE)
			update();

		SetTimer(gmst_hwnd, 1234, 10, NULL);
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
	pfd.cDepthBits = 32;

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

	// Depth Related Functions
	glShadeModel(GL_SMOOTH);

	glClearDepth(1.0f);

	glEnable(GL_DEPTH_TEST);

	glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// Light Related Functions
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightDirection);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, angleCutOff);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, angleExponent);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0f);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0f);

	glEnable(GL_LIGHT0);

	//Set Color
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	mst_Quadric = gluNewQuadric();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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
	// Function Declaration
	void DrawFloor(void);

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt( 	20.0f, 60.0f, 80.0f,
				20.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f);

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, angleCutOff);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, angleExponent);

	int index = 0;
	for (int ole = 0; ole < 6; ++ole)
	{
		for (int ile = 0; ile < 4; ++ile)
		{

			glMaterialfv(GL_FRONT, GL_AMBIENT, &materialAmbient[index * 4]);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, &materialDiffuse[index * 4]);
			glMaterialfv(GL_FRONT, GL_SPECULAR, &materialSpecular[index * 4]);
			glMaterialf(GL_FRONT, GL_SHININESS, materialShininess[index]);
			index++;

			glPushMatrix();
			{
				glTranslatef(ole * 12.0f, 0.0f, ile * 12.0f);
				glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

				/* SPHERE */
				gluSphere(mst_Quadric, 5.0f, 100, 100);
			}
			glPopMatrix();
		}
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


