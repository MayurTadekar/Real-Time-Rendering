/**************************************************
 * S.G.M.P
 * @file: OGL.c
 * @brief: Implementation of Bluescreen with Empty Shaders
 * @author: Mayur S. Tadekar(mrtadekar@gmail.com)
 * @Copyright ©2020-2025 MarshallingVoid
 * @date: 22 June 2025 (Sunday) 07:39:28 PM
 **************************************************/

//	Headers
#include <windows.h>
#include <stdio.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/vmath/vmath.h>

#include "OGL.h"

#define	WIN_WIDTH 	800
#define	WIN_HEIGHT 	600

// #define		LOG_ENTRY(file, str)		( fprintf(file, "LOG >>> %s\n",str ) )


#define LOG_ERROR(fileName, error)                \
	{                                             \
		FILE *gpLogFile = fopen(fileName, "a");   \
		fprintf(logFile, "ERROR: %s\n", error); \
		fclose(logFile);                          \
		logFile = NULL;                           \
		exit(1);                                  \
	}


#define LOG_ENTRY(fileName, error)              \
	{                                           \
		FILE *logFile = fopen(fileName, "a");   \
		fprintf(logFile, "LOG: %s\n", error); 	\
		fclose(logFile);                        \
		logFile = NULL;                         \
	}

#define LOG_ENTRY_WITH_PARAM(fileName, message, ...)            \
	{                                                           \
		FILE *logFile = fopen(fileName, "a");                 \
		fprintf(logFile, "INFO: " message "\n", __VA_ARGS__); \
		fclose(logFile);                                      \
		logFile = NULL;                                       \
	}

#define     FIND_DISTANCE(x1, y1, x2, y2)   {  (float) sqrt( ( (x2-x1) * (x2-x1) ) + ( (y2-y1) * (y2-y1) )  ) }

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

GLuint      fragmentShaderObject = 0;
GLuint      vertexShaderObject = 0;
GLuint      shaderProgramObject = 0;

// GLuint		vao = 0;
// GLuint		vboPosition = 0;
// GLuint		vboColor = 0;
GLuint		mvpMatrixUniform = 0;
vmath::mat4	perspectiveProjectionMatrix;

enum    {
    RED_HOR_LINE = 0,
    BLUE_HOR_LINE,
    GREEN_VERT_LINE,
    BLUE_VERT_LINE,
};

GLuint      vao[4];
GLuint      vbo_position[4];
GLuint      vbo_color[4];


enum {
    TRIANGLE = 0,
    SQUARE,
    CIRCLE,
    CIRCLE_2,
};

GLuint      vao_shape[4];
GLuint      vbo_shape_position[4];
GLuint      vbo_shape_color[4];

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
	if( NULL != (gpmst_File = fopen("log.txt", "w")))
    {   
    	LOG_ENTRY("log.txt", "Window Started Successfully\n");
    	fclose(gpmst_File);
    }
    else
    {
    	MessageBox(NULL, "Log file cannot be opened", "Message", MB_OK);
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
						TEXT("Mayur Tadekar - PP - Windows OS - 04_Perspective - 01_BlackAndWhite - 02_Colored - 01_Triangle"),
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

	int mst_result = initialize();

	if(mst_result != 0)
	{
		LOG_ENTRY("log.txt", "Initialize() Failed\n");
		DestroyWindow(mst_hwnd);
		gmst_hwnd = NULL;
	}
	else
	{
		LOG_ENTRY("log.txt", "Initialize() Completed Successfully\n");
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
	void uninitialize(void);

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

	LOG_ENTRY("log.txt", " Entered Initialize().");

	// Function Signature
	void resize(int, int);
	void printGLInfo();

	// Variable Declaration
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;
	GLenum glewResult;
	int extensionCounts = 0;

	// Code
	gmst_hdc = GetDC(gmst_hwnd);
	if( NULL == gmst_hdc )
	{
		LOG_ENTRY("log.txt", "GetDC() failed.");
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
		LOG_ENTRY("log.txt", "ChoosePixelFormat() failed.");
		// DestroyWindow(gmst_hwnd);
		return(-2);
	}

	if( FALSE == SetPixelFormat(gmst_hdc, iPixelFormatIndex, &pfd) )
	{
		LOG_ENTRY("log.txt", "SetPixelFormat() Failed.");
		// DestroyWindow(gmst_hwnd);
		return(-3);
	}

	hmst_ghrc = wglCreateContext(gmst_hdc);
	if( NULL == hmst_ghrc)
	{
		LOG_ENTRY("log.txt", " wglCreateContext() Failed.");
		// DestroyWindow(gmst_hwnd);
		return(-4);
	}

	if( FALSE == wglMakeCurrent(gmst_hdc, hmst_ghrc) )
	{
		LOG_ENTRY("log.txt", "wglMakeCurrent() Failed.");
		// DestroyWindow(gmst_hwnd);
		return(-5);
	}

    glewResult = glewInit();
    if (GLEW_OK != glewResult)
    {
        LOG_ENTRY("log.txt", "wglMakeCurrent() Failed.");
		// DestroyWindow(gmst_hwnd);
		return(-6);
    }

    printGLInfo();

    glGetIntegerv(GL_NUM_EXTENSIONS, &extensionCounts);
    LOG_ENTRY_WITH_PARAM("log.txt", "Extension Counts: %d", extensionCounts);
    for (int le = 0; le < extensionCounts; le++)
    {
        LOG_ENTRY_WITH_PARAM("log.txt", "%s\n", glGetStringi(GL_EXTENSIONS, le));
    }

    // Vertex Shader
    const GLchar* vertexShaderSourceCode = 
    "#version 460 core\n"			\
    "\n"							\
    "in vec4 vPosition;\n"			\
    "in vec4 vColor;\n"				\
    "out vec4 out_vColor;\n"		\
    "uniform mat4 uMVPMatrix;\n"	\
    "void main(void)\n"				\
    "{\n"							\
    "	gl_Position = uMVPMatrix * vPosition;\n"	\
    "	out_vColor = vColor;\n"		\
    "}\n"							\
    "\n";
    
    vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShaderObject, 1, (GLchar**)&vertexShaderSourceCode, NULL);

    glCompileShader(vertexShaderObject);

    GLint status = 0;
    GLint infoLogLength = 0;
    GLchar* szInfoLog = NULL;

    glGetShaderiv(  vertexShaderObject, 
                    GL_COMPILE_STATUS, 
                    &status);

    if( GL_FALSE == status) {
        glGetShaderiv(  vertexShaderObject, 
                        GL_INFO_LOG_LENGTH, 
                        &infoLogLength);
        
        if( 0 < infoLogLength) {
            szInfoLog = (char*)malloc(sizeof(char) * infoLogLength) ;
            
            if( NULL != szInfoLog ) {
                GLsizei written;
                glGetShaderInfoLog( vertexShaderObject, 
                                    infoLogLength, 
                                    &written, 
                                    szInfoLog);

                LOG_ENTRY_WITH_PARAM("log.txt", "Vertex Shader Compilation Log: %s\n", szInfoLog);
                free(szInfoLog);
                return(-7);
            }
        }
    }

    // Fragment Shader
    const GLchar* fragmentShaderSourceCode = 
    "#version 460 core\n"			\
    "\n"							\
    "in vec4 out_vColor;\n"			\
    "out vec4 Fragcolor;\n"			\
    "void main(void)\n"				\
    "{\n"							\
    "	Fragcolor = vec4(out_vColor);\n"\
    "}\n"							\
    "\n";
    
    fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragmentShaderObject, 1, (GLchar**)&fragmentShaderSourceCode, NULL);

    glCompileShader(fragmentShaderObject);

    status = 0;
    infoLogLength = 0;
    szInfoLog = NULL;

    glGetShaderiv(  fragmentShaderObject, 
                    GL_COMPILE_STATUS, 
                    &status);

    if( GL_FALSE == status) {
        glGetShaderiv(  fragmentShaderObject, 
                        GL_INFO_LOG_LENGTH, 
                        &infoLogLength);
        
        if( 0 < infoLogLength) {
            szInfoLog = (char*)malloc(sizeof(char) * infoLogLength) ;
            
            if( NULL != szInfoLog ) {
                GLsizei written;
                glGetShaderInfoLog( fragmentShaderObject, 
                                    infoLogLength, 
                                    &written, 
                                    szInfoLog);

                LOG_ENTRY_WITH_PARAM("log.txt", "Fragment Shader Compilation Log: %s\n", szInfoLog);
                free(szInfoLog);
                return(-8);
            }
        }
    }

    infoLogLength = 0;
    szInfoLog = NULL;

    shaderProgramObject = glCreateProgram();

    glAttachShader(shaderProgramObject, vertexShaderObject);
    glAttachShader(shaderProgramObject, fragmentShaderObject);

    glBindAttribLocation(   shaderProgramObject, 
                            MST_ATTRIBUTES_POSITION, 
                            "vPosition");

    glBindAttribLocation(   shaderProgramObject, 
                            MST_ATTRIBUTES_COLOR, 
                            "vColor");

    glLinkProgram(shaderProgramObject);

    GLint   shaderProgramLinkStatus = 0;

    glGetProgramiv( shaderProgramObject, 
                    GL_LINK_STATUS, 
                    &shaderProgramLinkStatus);

    if( GL_FALSE == shaderProgramLinkStatus) {
        glGetShaderiv(  shaderProgramObject, 
                        GL_INFO_LOG_LENGTH, 
                        &infoLogLength);

        if( 0 < infoLogLength) {
            szInfoLog = (char *)malloc(sizeof(char) * infoLogLength) ;
            if( NULL != szInfoLog) {
                GLint written;
                glGetProgramInfoLog(    shaderProgramObject, 
                                        infoLogLength,
                                        &written, 
                                        szInfoLog);
                LOG_ENTRY_WITH_PARAM("log.txt", "Shader Program Link Log: %s\n", szInfoLog);
                free(szInfoLog);
                return(-9);
            }
        }
    }

    mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "uMVPMatrix");

     //  GRAPH
    {
        //  RED_LINE
        {
            const GLfloat red_line[] = {
                5.0f, 0.0f, 0.0f,
                -5.0f, 0.0f, 0.0f,
            };
            
            const GLfloat red_line_color[] = {
                1.0f, 0.0f, 0.0f,
                1.0f, 0.0f, 0.0f,
            };

            glGenVertexArrays(1, &vao[RED_HOR_LINE]);

            glBindVertexArray(vao[RED_HOR_LINE]);     //   RECORDING CASET
            {
                // VERTEX
                glGenBuffers(1, &vbo_position[RED_HOR_LINE]);
                glBindBuffer(GL_ARRAY_BUFFER, vbo_position[RED_HOR_LINE]);
                glBufferData(GL_ARRAY_BUFFER, sizeof(red_line), red_line, GL_STATIC_DRAW);
                glVertexAttribPointer(MST_ATTRIBUTES_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
                glEnableVertexAttribArray(MST_ATTRIBUTES_POSITION);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                // COLOR
                glGenBuffers(1, &vbo_color[RED_HOR_LINE]);
                glBindBuffer(GL_ARRAY_BUFFER, vbo_color[RED_HOR_LINE]);
                glBufferData(GL_ARRAY_BUFFER, sizeof(red_line_color), red_line_color, GL_STATIC_DRAW);
                glVertexAttribPointer(MST_ATTRIBUTES_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
                glEnableVertexAttribArray(MST_ATTRIBUTES_COLOR);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            
            }
            glBindVertexArray(0);       // FINISH RECORDING CASET

        }

        //  BLUE_HOR_LINE
        {

            GLfloat *blue_hor_line = (GLfloat*) calloc( 40 * 3, sizeof(GLfloat) );
            GLfloat *blue_hor_line_color = (GLfloat*) calloc( 40 * 3, sizeof(GLfloat) );

            for(int i = 0 ; i < 20; i++) {
                //  Vertex
                *(blue_hor_line + (i*2 + 0) * 3 + 0) = -5.0f;
                *(blue_hor_line + (i*2 + 0) * 3 + 1) = 0.2f * ( i < 10 ? i+1 : ( -1.0f * (i-9) ) );
                *(blue_hor_line + (i*2 + 0) * 3 + 2) = 0.0f;
                //  Color
                *(blue_hor_line_color + (i*2 + 0) * 3 + 0) = 0.0f;
                *(blue_hor_line_color + (i*2 + 0) * 3 + 1) = 0.0f;
                *(blue_hor_line_color + (i*2 + 0) * 3 + 2) = 1.0f;
                
                //  Vertex
                *(blue_hor_line + (i*2 + 1) * 3 + 0) = 5.0f;
                *(blue_hor_line + (i*2 + 1) * 3 + 1) = 0.2f * ( i < 10 ? i+1 : ( -1.0f * (i-9) ) );
                *(blue_hor_line + (i*2 + 1) * 3 + 2) = 0.0f;
                //  Color
                *(blue_hor_line_color + (i*2 + 1) * 3 + 0) = 0.0f;
                *(blue_hor_line_color + (i*2 + 1) * 3 + 1) = 0.0f;
                *(blue_hor_line_color + (i*2 + 1) * 3 + 2) = 1.0f;
            } 

            glGenVertexArrays(1, &vao[BLUE_HOR_LINE]);
            
            glBindVertexArray(vao[BLUE_HOR_LINE]);
            {
                // Vertices
                glGenBuffers(1, &vbo_position[BLUE_HOR_LINE]);
                glBindBuffer(GL_ARRAY_BUFFER, vbo_position[BLUE_HOR_LINE]);
                glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * (40 * 3), blue_hor_line, GL_STATIC_DRAW);
                glVertexAttribPointer(MST_ATTRIBUTES_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
                glEnableVertexAttribArray(MST_ATTRIBUTES_POSITION);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                // Color
                glGenBuffers(1, &vbo_color[BLUE_HOR_LINE]);
                glBindBuffer(GL_ARRAY_BUFFER, vbo_color[BLUE_HOR_LINE]);
                glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 40 * 3, blue_hor_line_color, GL_STATIC_DRAW);
                glVertexAttribPointer(MST_ATTRIBUTES_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
                glEnableVertexAttribArray(MST_ATTRIBUTES_COLOR);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

            }
            glBindVertexArray(0);

            free(blue_hor_line_color);
            free(blue_hor_line);
        }
            
        //  GREEN_LINE
        {

            const GLfloat green_line[] = {
                0.0f, 3.0f, 0.0f,
                0.0f,-3.0f, 0.0f,
            };
            
            const GLfloat green_line_color[] = {
                0.0f, 1.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
            };

            glGenVertexArrays(1, &vao[GREEN_VERT_LINE]);

            glBindVertexArray(vao[GREEN_VERT_LINE]);     //   RECORDING CASET
            {
                // VERTEX
                glGenBuffers(1, &vbo_position[GREEN_VERT_LINE]);
                glBindBuffer(GL_ARRAY_BUFFER, vbo_position[GREEN_VERT_LINE]);
                glBufferData(GL_ARRAY_BUFFER, sizeof(green_line), green_line, GL_STATIC_DRAW);
                glVertexAttribPointer(MST_ATTRIBUTES_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
                glEnableVertexAttribArray(MST_ATTRIBUTES_POSITION);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                // COLOR
                glGenBuffers(1, &vbo_color[GREEN_VERT_LINE]);
                glBindBuffer(GL_ARRAY_BUFFER, vbo_color[GREEN_VERT_LINE]);
                glBufferData(GL_ARRAY_BUFFER, sizeof(green_line_color), green_line_color, GL_STATIC_DRAW);
                glVertexAttribPointer(MST_ATTRIBUTES_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
                glEnableVertexAttribArray(MST_ATTRIBUTES_COLOR);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            
            }
            glBindVertexArray(0);       // FINISH RECORDING CASET

        }

        //  BLUE_VERT_LINE
        {

            GLfloat *blue_vert_line = (GLfloat*) calloc( 60 * 3, sizeof(GLfloat) );
            GLfloat *blue_vert_line_color = (GLfloat*) calloc( 60 * 3, sizeof(GLfloat) );

            for(int i = 0 ; i < 30; i++) {
                //  Vertex
                *(blue_vert_line + (i*2 + 0) * 3 + 1) = 3.0f;
                *(blue_vert_line + (i*2 + 0) * 3 + 0) = 0.25f * ( i < 15 ? i+1 : ( -1.0f * (i-14) ) );
                *(blue_vert_line + (i*2 + 0) * 3 + 2) = 0.0f;
                //  Color
                *(blue_vert_line_color + (i*2 + 0) * 3 + 0) = 0.0f;
                *(blue_vert_line_color + (i*2 + 0) * 3 + 1) = 0.0f;
                *(blue_vert_line_color + (i*2 + 0) * 3 + 2) = 1.0f;
                
                //  Vertex
                *(blue_vert_line + (i*2 + 1) * 3 + 1) =-3.0f;
                *(blue_vert_line + (i*2 + 1) * 3 + 0) = 0.25f * ( i < 15 ? i+1 : ( -1.0f * (i-14) ) );
                *(blue_vert_line + (i*2 + 1) * 3 + 2) = 0.0f;
                //  Color
                *(blue_vert_line_color + (i*2 + 1) * 3 + 0) = 0.0f;
                *(blue_vert_line_color + (i*2 + 1) * 3 + 1) = 0.0f;
                *(blue_vert_line_color + (i*2 + 1) * 3 + 2) = 1.0f;
            } 

            glGenVertexArrays(1, &vao[BLUE_VERT_LINE]);
            
            glBindVertexArray(vao[BLUE_VERT_LINE]);
            {
                // Vertices
                glGenBuffers(1, &vbo_position[BLUE_VERT_LINE]);
                glBindBuffer(GL_ARRAY_BUFFER, vbo_position[BLUE_VERT_LINE]);
                glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * (60 * 3), blue_vert_line, GL_STATIC_DRAW);
                glVertexAttribPointer(MST_ATTRIBUTES_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
                glEnableVertexAttribArray(MST_ATTRIBUTES_POSITION);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                // Color
                glGenBuffers(1, &vbo_color[BLUE_VERT_LINE]);
                glBindBuffer(GL_ARRAY_BUFFER, vbo_color[BLUE_VERT_LINE]);
                glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 60 * 3, blue_vert_line_color, GL_STATIC_DRAW);
                glVertexAttribPointer(MST_ATTRIBUTES_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
                glEnableVertexAttribArray(MST_ATTRIBUTES_COLOR);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

            }
            glBindVertexArray(0);

            free(blue_vert_line_color);
            free(blue_vert_line);
        }

    }

    //  TRIANGLE   
    const GLfloat triangle[] = {
        0.0f, 0.25f * 4, 0.0f,
        -0.25f * 4, -0.25f * 4, 0.0f,
        0.25f * 4, -0.25f * 4, 0.0f,
    };
    
    const GLfloat triangle_color[] = {
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
    };

    glGenVertexArrays(1, &vao_shape[TRIANGLE]);

    glBindVertexArray(vao_shape[TRIANGLE]);     //   RECORDING CASET
    {
        // VERTEX
        glGenBuffers(1, &vbo_shape_position[TRIANGLE]);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_shape_position[TRIANGLE]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
        glVertexAttribPointer(MST_ATTRIBUTES_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(MST_ATTRIBUTES_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // COLOR
        glGenBuffers(1, &vbo_shape_color[TRIANGLE]);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_shape_color[TRIANGLE]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_color), triangle_color, GL_STATIC_DRAW);
        glVertexAttribPointer(MST_ATTRIBUTES_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(MST_ATTRIBUTES_COLOR);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    }
    glBindVertexArray(0);       // FINISH RECORDING CASET

    //  SQUARE   
    {
        const GLfloat square[] = {
            -0.25f * 4,  0.25f * 4, 0.0f,
            -0.25f * 4, -0.25f * 4, 0.0f,
             0.25f * 4, -0.25f * 4, 0.0f,
             0.25f * 4,  0.25f * 4, 0.0f
        };
        
        const GLfloat square_color[] = {
            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f
        };

        glGenVertexArrays(1, &vao_shape[SQUARE]);

        glBindVertexArray(vao_shape[SQUARE]);     //   RECORDING CASET
        {
            // VERTEX
            glGenBuffers(1, &vbo_shape_position[SQUARE]);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_shape_position[SQUARE]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_STATIC_DRAW);
            glVertexAttribPointer(MST_ATTRIBUTES_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(MST_ATTRIBUTES_POSITION);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // COLOR
            glGenBuffers(1, &vbo_shape_color[SQUARE]);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_shape_color[SQUARE]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(square_color), square_color, GL_STATIC_DRAW);
            glVertexAttribPointer(MST_ATTRIBUTES_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(MST_ATTRIBUTES_COLOR);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        }
        glBindVertexArray(0);       // FINISH RECORDING CASET

    }

    //  CIRCLE   
    {
        GLfloat circle[33 * 3];        
        GLfloat circle_color[33 * 3];

        GLfloat     a = FIND_DISTANCE( triangle[3], triangle[4], triangle[6], triangle[7] );
        GLfloat     b = FIND_DISTANCE( triangle[6], triangle[7], triangle[0], triangle[1] );
        GLfloat     c = FIND_DISTANCE( triangle[0], triangle[1], triangle[3], triangle[4] );

        GLfloat center_X = ( (a * triangle[0]) + (b * triangle[3]) + ( c * triangle[6]) ) / ( a + b + c);
        GLfloat center_Y = ( (a * triangle[1]) + (b * triangle[4]) + ( c * triangle[7]) ) / ( a + b + c);

        for( int i = 0 ; i <= 32; i++) {

            float angle = 2.0f * M_PI * i / 32;

            circle[ i*3 + 0 ] = (center_Y + triangle[1]) * sinf(angle);
            circle[ i*3 + 1 ] = ( center_Y + triangle[1]) * cosf(angle) + center_Y;
            circle[ i*3 + 2 ] = 0.0f;

            circle_color[ i*3 + 0 ] = 1.0f;
            circle_color[ i*3 + 1 ] = 1.0f;
            circle_color[ i*3 + 2 ] = 0.0f;
        }

        glGenVertexArrays(1, &vao_shape[CIRCLE]);

        glBindVertexArray(vao_shape[CIRCLE]);     //   RECORDING CASET
        {
            // VERTEX
            glGenBuffers(1, &vbo_shape_position[CIRCLE]);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_shape_position[CIRCLE]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(circle), circle, GL_STATIC_DRAW);
            glVertexAttribPointer(MST_ATTRIBUTES_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(MST_ATTRIBUTES_POSITION);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // COLOR
            glGenBuffers(1, &vbo_shape_color[CIRCLE]);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_shape_color[CIRCLE]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(circle_color), circle_color, GL_STATIC_DRAW);
            glVertexAttribPointer(MST_ATTRIBUTES_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(MST_ATTRIBUTES_COLOR);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        }
        glBindVertexArray(0);       // FINISH RECORDING CASET

    }

    //  CIRCLE_2   
    {
        GLfloat circle[33 * 3];        
        GLfloat circle_color[33 * 3];

        
        for( int i = 0 ; i <= 32; i++) {

            float angle = 2.0f * M_PI * i / 32;

            circle[ i*3 + 0 ] = 1.414f * sinf(angle);
            circle[ i*3 + 1 ] = 1.414f * cosf(angle);
            circle[ i*3 + 2 ] = 0.0f;

            circle_color[ i*3 + 0 ] = 1.0f;
            circle_color[ i*3 + 1 ] = 1.0f;
            circle_color[ i*3 + 2 ] = 0.0f;
        }

        glGenVertexArrays(1, &vao_shape[CIRCLE_2]);

        glBindVertexArray(vao_shape[CIRCLE_2]);     //   RECORDING CASET
        {
            // VERTEX
            glGenBuffers(1, &vbo_shape_position[CIRCLE_2]);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_shape_position[CIRCLE_2]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(circle), circle, GL_STATIC_DRAW);
            glVertexAttribPointer(MST_ATTRIBUTES_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(MST_ATTRIBUTES_POSITION);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // COLOR
            glGenBuffers(1, &vbo_shape_color[CIRCLE_2]);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_shape_color[CIRCLE_2]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(circle_color), circle_color, GL_STATIC_DRAW);
            glVertexAttribPointer(MST_ATTRIBUTES_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(MST_ATTRIBUTES_COLOR);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        }
        glBindVertexArray(0);       // FINISH RECORDING CASET

    }

	// Depth Related Functions
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//Set Color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glShadeModel(GL_SMOOTH);

	perspectiveProjectionMatrix = vmath::mat4::identity();

	// Warmup call to Resize
	resize(WIN_WIDTH, WIN_HEIGHT);

	LOG_ENTRY("log.txt", " Exit Initialize().");
	return(0);
}

void resize(int width, int height)
{

	LOG_ENTRY("log.txt", " Entered Resize().");

	// Code
	if(height <= 0)
		height = 1;

	glViewport(0, 0, (GLsizei) width, (GLsizei)height);

    perspectiveProjectionMatrix = vmath::perspective(45.0f, (float)width/(float)height, 1.0f, 100.0f);
}

void display(void)
{
	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgramObject);

    vmath::mat4 modelViewMatrix = vmath::mat4::identity();

    modelViewMatrix *= vmath::translate(0.0f, 0.0f, -5.0f);

    vmath::mat4 modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);   
    
    //  GRAPH
    {

        glBindVertexArray(vao[RED_HOR_LINE]);
        glDrawArrays(GL_LINES, 0, 2);
        glBindVertexArray(0);
        
        glBindVertexArray(vao[BLUE_HOR_LINE]);
        glDrawArrays(GL_LINES, 0, 40);
        glBindVertexArray(0);
        
        glBindVertexArray(vao[GREEN_VERT_LINE]);
        glDrawArrays(GL_LINES, 0, 2);
        glBindVertexArray(0);
        
        glBindVertexArray(vao[BLUE_VERT_LINE]);
        glDrawArrays(GL_LINES, 0, 60);
        glBindVertexArray(0);

    }

    //  SHAPE   
    {
        glBindVertexArray(vao_shape[TRIANGLE]);
        glDrawArrays(GL_LINE_LOOP, 0, 3);
        glBindVertexArray(0);
        
        glBindVertexArray(vao_shape[SQUARE]);
        glDrawArrays(GL_LINE_LOOP, 0, 4);
        glBindVertexArray(0);
        
        glBindVertexArray(vao_shape[CIRCLE]);
        glDrawArrays(GL_LINE_LOOP, 0, 33);
        glBindVertexArray(0);
        
        glBindVertexArray(vao_shape[CIRCLE_2]);
        glDrawArrays(GL_LINE_LOOP, 0, 33);
        glBindVertexArray(0);
    }
    glUseProgram(0);

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
		LOG_ENTRY("log.txt", "Program Terminated Successfully...\n");
		fclose(gpmst_File);
		gpmst_File = NULL;
	}

}


void printGLInfo()
{
	// Code
    LOG_ENTRY_WITH_PARAM("log.txt", "OpenGL Renderer: %s", glGetString(GL_RENDERER));

    LOG_ENTRY_WITH_PARAM("log.txt", "OpenGL Version: %s", glGetString(GL_VERSION));

    LOG_ENTRY_WITH_PARAM("log.txt", "GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    LOG_ENTRY_WITH_PARAM("log.txt", "GLEW Version: %s", glewGetString(GLEW_VERSION));
}
