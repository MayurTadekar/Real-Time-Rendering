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
#include "Sphere.h"

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

GLuint		vao = 0;
GLuint		vboPosition = 0;
GLuint		vboNormal = 0;
GLuint		vboElements = 0;

GLuint		modelMatrixUniform = 0;
GLuint		viewMatrixUniform = 0;
GLuint		projectionMatrixUniform = 0;
vmath::mat4	perspectiveProjectionMatrix;

float angle = 0.0f;

float   sphere_vertices[1146];
float   sphere_normals[1146];
float   sphere_textures[764];
unsigned short  sphere_elements[2280];

GLuint  gNumVertices;
GLuint  gNumElements;

GLuint uKeyPressed = 0;

GLuint uLightDiffuse = 0;
GLuint uLightSpecular = 0;
GLuint uLightAmbient = 0;

GLuint uMaterialDiffuse = 0;
GLuint uMaterialAmbient = 0;
GLuint uMaterialSpecular = 0;
GLuint uMaterialShininess = 0;

GLfloat lightAmbient[] = { 0.1f, 0.1f, 0.1f };
GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f };
GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f };

GLfloat lightPosition[] = { 2.0f, 2.0f, 2.0f, 1.0f };

GLfloat materialAmbient[] = { 0.1f, 0.1f, 0.1f };
GLfloat materialDiffuse[] = { 1.0f, 1.0f, 1.0f };
GLfloat materialSpecular[] = { 1.0f, 1.0f, 1.0f };
GLfloat materialShininess = 20.0f;

GLuint uPosition = 0;

BOOL bAnimation = 0;
BOOL bLight = 0;

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
						TEXT("Mayur Tadekar - PP - Windows OS - 04_Perspective - 01_BlackAndWhite - 01_SingleShape - 01_Triangle"),
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

				if(TRUE == bAnimation)
				{
					update();
				}
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

		case 'a':
		case 'A':
			bAnimation = (bAnimation == TRUE ? FALSE : TRUE);
			break;

		case 'l':
		case 'L':
			bLight = (bLight == TRUE ? FALSE : TRUE);
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

    // printGLInfo();

    // glGetIntegerv(GL_NUM_EXTENSIONS, &extensionCounts);
    // LOG_ENTRY_WITH_PARAM("log.txt", "Extension Counts: %d", extensionCounts);
    // for (int le = 0; le < extensionCounts; le++)
    // {
    //     LOG_ENTRY_WITH_PARAM("log.txt", "%s\n", glGetStringi(GL_EXTENSIONS, le));
    // }

    // Vertex Shader
    const GLchar* vertexShaderSourceCode = 
    "#version 450 core"                                                                                                 \
    "\n"                                                                                                                \
    "in vec4 vPosition;"                                                                                                \
    "in vec3 vNormal;"                                                                                                  \
    "uniform mat4 u_modelMatrix;"                                                                                       \
    "uniform mat4 u_viewMatrix;"                                                                                        \
    "uniform mat4 u_projectionMatrix;"                                                                                  \
    "uniform int  u_lKeyPressed;"                                                                                       \
    "uniform vec4  u_LightPosition;"                                                                                    \
    "out vec3 phong_ads_light;"                                                                                         \
    "out vec3 transformed_normal;"                                                                                      \
    "out vec3 light_direction;"                                                                                         \
    "out vec3 view_vector;"                                                                                             \
    "void main(void)"                                                                                                   \
    "{"                                                                                                                 \
    "  if( u_lKeyPressed == 1 ) "                                                                                       \
    "  {   "                                                                                                            \
    "   vec4 eye_coord =  u_viewMatrix * u_modelMatrix * vPosition;   "                                                 \
    "   transformed_normal = mat3(u_viewMatrix * u_modelMatrix) * vNormal;"                                             \
    "   light_direction = vec3(u_LightPosition - eye_coord);"                                                           \
    "   view_vector = -eye_coord.xyz;"                                                                                  \
    "   }"                                                                                                              \
    "   gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * vPosition;"                                   \
    "}";
    
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
    "#version 450 core"                             \
    "\n"                                            \
    "out vec4 Fragcolor;"                           \
    "in vec3 transformed_normal;"                                                                                               \
    "in vec3 light_direction;"                                                                                                  \
    "in vec3 view_vector;"                                                                                                      \
    "vec3 phong_ads_light;"                                                                                                     \
    "uniform vec3  u_la;"                                                                                                       \
    "uniform vec3  u_ld;"                                                                                                       \
    "uniform vec3  u_ls;"                                                                                                       \
    "uniform vec3  u_ka;"                                                                                                       \
    "uniform vec3  u_kd;"                                                                                                       \
    "uniform vec3  u_ks;"                                                                                                       \
    "uniform float  u_kshiny;"                                                                                                  \
    "uniform int  u_lKeyPressed;"                                                                                               \
    "void main(void)"                                                                                                           \
    "{"                                                                                                                         \
    "  if( u_lKeyPressed == 1 ) "                                                                                               \
    "   {   "                                                                                                                   \
    "   vec3 normalized_transformed_normal = normalize(transformed_normal);"                                                    \
    "   vec3 normalized_light_direction = normalize(light_direction);"                                                          \
    "   vec3 normalized_view_vector = normalize(view_vector);"                                                                  \
    "   vec3 reflection_vector = reflect( -normalized_light_direction, normalized_transformed_normal);"                         \
    "   vec3 intensity_ambient = u_la * u_ka;"                                                                                  \
    "   vec3 intensity_diffuse = u_ld * u_kd * max( dot(normalized_light_direction, normalized_transformed_normal), 0.0f);"     \
    "   vec3 intensity_specular = u_ls * u_ks * pow( max( dot( reflection_vector, normalized_view_vector ), 0.0f), u_kshiny);"  \
    "   phong_ads_light = intensity_ambient + intensity_diffuse + intensity_specular;"                                  \
    "   } else {"                                                                                                       \
    "       phong_ads_light = vec3(1.0f, 1.0f, 1.0f);"                                                                  \
    "   }"                                                                                                              \
    "   Fragcolor = vec4(phong_ads_light, 1.0f);"   \
    "}";
    
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
                            MST_ATTRIBUTES_NORMAL,
                            "vNormal");

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

	modelMatrixUniform = glGetUniformLocation(  shaderProgramObject,  "u_modelMatrix");
	viewMatrixUniform = glGetUniformLocation(  shaderProgramObject,  "u_viewMatrix");
    projectionMatrixUniform = glGetUniformLocation( shaderProgramObject, "u_projectionMatrix");
    
    uKeyPressed = glGetUniformLocation(  shaderProgramObject, "u_lKeyPressed");

    uLightDiffuse = glGetUniformLocation(   shaderProgramObject, "u_ld");   
    uLightSpecular = glGetUniformLocation(   shaderProgramObject, "u_ls");   
    uLightAmbient = glGetUniformLocation(   shaderProgramObject, "u_la");   

    uMaterialDiffuse = glGetUniformLocation(   shaderProgramObject, "u_kd");                                         
    uMaterialSpecular = glGetUniformLocation(   shaderProgramObject, "u_ks");                                         
    uMaterialAmbient = glGetUniformLocation(   shaderProgramObject, "u_ka");                                         
    uMaterialShininess = glGetUniformLocation(   shaderProgramObject, "u_kshiny");                                         
                                                
    uPosition = glGetUniformLocation(   shaderProgramObject, "u_LightPosition"); 
    
    getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);

    gNumVertices = getNumberOfSphereVertices();
    gNumElements = getNumberOfSphereElements();

    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    {
    	glGenBuffers(1, &vboPosition);
	    {
		    glBindBuffer(GL_ARRAY_BUFFER, vboPosition);
		    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
		    glVertexAttribPointer(MST_ATTRIBUTES_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		    glEnableVertexAttribArray(MST_ATTRIBUTES_POSITION);
		}
	    glBindBuffer(GL_ARRAY_BUFFER, 0);

    	glGenBuffers(1, &vboNormal);
	    {
		    glBindBuffer(GL_ARRAY_BUFFER, vboNormal);
		    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
		    glVertexAttribPointer(MST_ATTRIBUTES_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		    glEnableVertexAttribArray(MST_ATTRIBUTES_NORMAL);
		}
	    glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glGenBuffers(1, &vboElements);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboElements);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(0);

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

    vmath::mat4 modelMatrix = vmath::mat4::identity();
    vmath::mat4 viewMatrix = vmath::mat4::identity();

    viewMatrix *= vmath::translate(0.0f, 0.0f, -2.0f);

    modelMatrix *= vmath::rotate(angle, 1.0f, 0.0f, 0.0f);
    modelMatrix *= vmath::rotate(angle, 0.0f, 1.0f, 0.0f);
    modelMatrix *= vmath::rotate(angle, 0.0f, 0.0f, 1.0f);

    if( TRUE == bLight ) {
        glUniform1i(uKeyPressed, 1);

        glUniform3fv(uLightAmbient, 1, lightAmbient);
        glUniform3fv(uLightDiffuse, 1, lightDiffuse);
        glUniform3fv(uLightSpecular, 1, lightSpecular);
        
        glUniform3fv(uMaterialAmbient, 1, materialAmbient);
        glUniform3fv(uMaterialDiffuse, 1, materialDiffuse);
        glUniform3fv(uMaterialSpecular, 1, materialSpecular);

        glUniform1f(uMaterialShininess, materialShininess);

        glUniform4fv(uPosition, 1, (const GLfloat*) lightPosition);
    } else {
        glUniform1i(uKeyPressed, 0);
    }

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

    glBindVertexArray(vao);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboElements);

    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

    glBindVertexArray(0);

    glUseProgram(0);

	SwapBuffers(gmst_hdc);
}


void update(void)
{
	// Code

	angle += 0.5f;

	if(angle >= 360.0f)
		angle -= 360.0f;
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
