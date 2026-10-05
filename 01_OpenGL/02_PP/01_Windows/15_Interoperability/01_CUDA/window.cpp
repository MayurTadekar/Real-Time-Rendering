

// HEADERS
#include    <Windows.h>
#include    <stdio.h>
#include    <GL/glew.h>
#include    <GL/GL.h>
#include    <GL/vmath/vmath.h>

#include	<CUDA/cuda_gl_interop.h>
#include	<CUDA/cuda_runtime.h>

#include	"sinewave.h"

using   namespace vmath;


// MACROS
#define     WIN_WIDTH       800
#define     WIN_HEIGHT      600

#define     ERROR_EXIT(file, error, hwnd)   {                                           \
                                                fprintf(file, "ERROR: %s\n", error);    \
                                                DestroyWindow(hwnd);                    \
                                            }

#define     LoadIdentity(matrix)            {                               \
                                                matrix = mat4::identity();  \
                                            }

// VARIABLES
HWND        ghwnd;
HDC         ghdc;
HGLRC       ghrc;
DWORD       dwStyle;
bool        bFullscreen = false;
bool        bActivate = true;
FILE        *gpFile = NULL;
WINDOWPLACEMENT     wpPrev;

GLuint      fragmentShaderObject = 0;
GLuint      vertexShaderObject = 0;
GLuint      shaderProgramObject = 0;

enum    {
    MST_ATTRIBUTES_POSITION = 0,
    MST_ATTRIBUTES_COLOR,
    MST_ATTRIBUTES_NORMAL,
    MST_ATTRIBUTES_TEXCOORD
};

GLuint      vaoCpu;
GLuint      vaoGpu;
GLuint      vbo_positionCpu;
GLuint      vbo_positionGpu;
GLuint      vbo_color;
GLuint      mvpMatrixUniform;
mat4        perspectiveProjectionMatrix;

unsigned int gMeshWidth = 128;
unsigned int gMeshHeight = 128;

long arraySize = gMeshHeight * gMeshWidth * 4;

float* 	gPosition;
float 	animationTime = 0.0f;

float 	colorArray[3] = { 1.0f, 0.5f, 0.0f};

bool 	isBkBlack = true;
bool 	bOnGpu = false;

cudaError_t cudaResult;
struct cudaGraphicsResource*	cudaGraphicsResource = NULL;

// FUNCTION SIGNATURE
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// FUNCTIONS
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow) {

    // FUNCTION SIGNATURE
    void Display(void);
    void Initialize(void);
    void Uninitialize(void);
    
    // VARIABLES
    HWND        hwnd;
    MSG         msg;
    TCHAR       szAppName[] = TEXT("CWindow");
    WNDCLASSEX  wndcls;
    RECT        rcDesktop;
    bool        bDone = false;

    // CODE
    if( 0 != fopen_s(&gpFile, "log.txt", "w")) {
        MessageBox(NULL, TEXT("Log File Cannot be opened"), TEXT("ERROR"), MB_ICONERROR);
        exit(1);
    }

    wndcls.cbSize = sizeof(WNDCLASSEX);
    wndcls.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wndcls.cbClsExtra = 0;
    wndcls.cbWndExtra = 0;
    wndcls.lpfnWndProc = WndProc;
    wndcls.lpszClassName = szAppName;
    wndcls.lpszMenuName = NULL;
    wndcls.hInstance = hInstance;
    wndcls.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wndcls.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
    wndcls.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
    wndcls.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClassEx(&wndcls);

    SystemParametersInfo(SPI_GETWORKAREA, 0, &rcDesktop, 0);

    hwnd = CreateWindowEx(  WS_EX_APPWINDOW,
                            szAppName,
                            TEXT("Window"),
                            WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                            rcDesktop.right/2 - WIN_WIDTH/2,
                            rcDesktop.bottom/2 - WIN_HEIGHT/2,
                            WIN_WIDTH,
                            WIN_HEIGHT,
                            NULL,
                            NULL,
                            hInstance,
                            NULL);
    
    ghwnd = hwnd;

    Initialize();

    ShowWindow(hwnd, iCmdShow);
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);

    while( !bDone ) 
    {
        if( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
        {
            if( WM_QUIT == msg.message) 
            {
                bDone = true;
            } else 
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        } else 
        {
            if( bActivate )
                Display(); 
        }
    }

    Uninitialize();
    return((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) 
{

    // FUNCTION SIGNATURE
    void Resize(int, int);
    void Uninitialize(void);
    void ToggleFullscreen(void);

    // CODE
    switch( iMsg ){
        case WM_DESTROY:
            Uninitialize();
            PostQuitMessage(0);
            break;

        case WM_SETFOCUS:
            bActivate = true;
            break;

        case WM_KILLFOCUS:   
            bActivate = false;
            break;

        case WM_SIZE:
            Resize(LOWORD(lParam), HIWORD(lParam));
            break;

        case WM_ERASEBKGND:
            return(0);

        case WM_KEYDOWN:    
            switch(LOWORD(wParam)) {
                case 0x1B:
                    DestroyWindow(hwnd);
                    break;
                
                case 0x46:
                case 0x66:
                    ToggleFullscreen();
                    break;
            }
            break;

        case WM_CHAR:
        	switch(wParam)
        	{
        		case 'i':
        		case 'I':
        			gMeshWidth *= 2;
        			gMeshHeight *= 2;
        			if( 2048 <= gMeshHeight )
        			{
        				gMeshHeight = 2048;
        				gMeshWidth = 2048;
        			}

        			if( false == bOnGpu )
        			{
        				arraySize = gMeshHeight * gMeshWidth * 4;
        				gPosition = (float*) realloc(gPosition, arraySize * sizeof(float));
        			}
        			break;

        		case 'd':
        		case 'D':
        			gMeshWidth /= 2;
        			gMeshHeight /= 2;
        			if( 64 >= gMeshWidth )
        			{
        				gMeshWidth = 64;
        				gMeshHeight = 64;
        			}

        			if( false == bOnGpu )
        			{
        				arraySize = gMeshHeight * gMeshWidth * 4;
        				gPosition = (float*) realloc(gPosition, arraySize * sizeof(float));
        			}
        			break;	
        	
        		case 'h':
        		case 'H':
        			bOnGpu = !bOnGpu;
        			break;
        	}
        	break;
    }

    return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullscreen(void) {

    // VARIABLES
    MONITORINFO     mi = { sizeof(MONITORINFO) };

    // CODE
    if( !bFullscreen ) {
        dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
        if( dwStyle & WS_OVERLAPPEDWINDOW) {
            if( GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi)) {
                SetWindowLong(ghwnd, GWL_STYLE, dwStyle & (~WS_OVERLAPPEDWINDOW));
                SetWindowPos(   ghwnd,
                                HWND_TOP,
                                mi.rcMonitor.left,
                                mi.rcMonitor.top,
                                mi.rcMonitor.right - mi.rcMonitor.left,
                                mi.rcMonitor.bottom - mi.rcMonitor.top,
                                SWP_FRAMECHANGED | SWP_NOZORDER);
            }
        }
        ShowCursor(FALSE);
        bFullscreen = true;
    } else {
        SetWindowLong(ghwnd, GWL_STYLE, dwStyle & WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(ghwnd, &wpPrev);
        SetWindowPos( ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
        ShowCursor(TRUE);
        bFullscreen = false;
    }
}

void Initialize(void) {
    // FUNCTIONS
    void Resize(int, int);
    void Uninitialize(void);

    // VARIABLE
    PIXELFORMATDESCRIPTOR   pfd;
    int iPixelIndex = 0;
    
    // CODE
    ghdc = GetDC(ghwnd);
    ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
    pfd.cRedBits = 8;
    pfd.cGreenBits = 8;
    pfd.cBlueBits = 8;
    pfd.cAlphaBits = 8;
    pfd.cDepthBits = 32;
    pfd.cColorBits = 32;

    int devCount;

    cudaResult = cudaGetDeviceCount(&devCount);
    if( cudaSuccess != cudaResult )
    {
    	DestroyWindow(ghwnd);
    }
    else if( 0 == devCount)
    {
    	DestroyWindow(ghwnd);
    }
    else
    {
    	cudaSetDevice(0);
    }

    iPixelIndex = ChoosePixelFormat(ghdc, &pfd);
    if( 0 == iPixelIndex) {
        ERROR_EXIT(gpFile, "ChoosePixelFormat() Failed", ghwnd);
    }

    if( FALSE == SetPixelFormat(ghdc, iPixelIndex, &pfd)) {
        ERROR_EXIT(gpFile, "SetPixelFormat() Failed", ghwnd);
    }

    ghrc = wglCreateContext(ghdc);
    if( NULL == ghrc) {
        ERROR_EXIT(gpFile, "wglCreateContext() Failed", ghwnd);
    }

    if( FALSE == wglMakeCurrent(ghdc, ghrc)) {
        ERROR_EXIT(gpFile, "wglMakeCurrent() Failed", ghwnd);
    }

    GLenum glew_error = glewInit();
    if( GLEW_OK != glew_error) {
        Uninitialize();
        DestroyWindow(ghwnd);
    }

    fprintf(gpFile, "GL_VENDOR: %s\n", glGetString(GL_VENDOR));

    fprintf(gpFile, "GL_RENDERER: %s\n", glGetString(GL_RENDERER));

    fprintf(gpFile, "OpenGL Version: %s\n", glGetString(GL_VERSION));

    fprintf(gpFile, "GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    GLint numExt;

    glGetIntegerv(GL_NUM_EXTENSIONS, &numExt);
    for( int i = 0 ; i < numExt; i++) {
        fprintf(gpFile, "%s\n", glGetStringi(GL_EXTENSIONS, i));
    }

    /****** VERTEX SHADER ******/
    GLchar *vertexShaderSourceCode = 
        "#version 450 core"                         \
        "\n"                                        \
        "in vec4 vPosition;"                        \
        "in vec4 vColor;"                           \
        "uniform mat4 u_mvpMatrix;"                 \
        "out vec4 out_Color;"                       \
        "void main(void)"                           \
        "{"                                         \
        "gl_Position = u_mvpMatrix * vPosition;"    \
        "out_Color = vColor;"                       \
        "}";
    
    vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

    glCompileShader(vertexShaderObject);

    GLint       infoLength = 0;
    GLint       shaderCompileStatus = 0;
    char*       szBuffer = NULL;

    glGetShaderiv(  vertexShaderObject, 
                    GL_COMPILE_STATUS, 
                    &shaderCompileStatus);

    if( GL_FALSE == shaderCompileStatus) {
        glGetShaderiv(  vertexShaderObject, 
                        GL_INFO_LOG_LENGTH, 
                        &infoLength);
        
        if( 0 < infoLength) {
            szBuffer = (char*)malloc(sizeof(char) * infoLength) ;
            
            if( NULL != szBuffer ) {
                GLsizei written;
                glGetShaderInfoLog( vertexShaderObject, 
                                    infoLength, 
                                    &written, 
                                    szBuffer);

                fprintf(gpFile, "Vertex Shader Compilation Log: %s\n", szBuffer);
                free(szBuffer);
                DestroyWindow(ghwnd);
            }
        }
    }

    GLchar *fragmentShaderSourceCode = 
        "#version 450 core"                             \
        "\n"                                            \
        "out vec4 Fragcolor;"                           \
        "in  vec4 out_Color;"                           \
        "void main(void)"                               \
        "{"                                             \
        "Fragcolor = out_Color;"                        \
        "}";

    fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource( fragmentShaderObject, 
                    1, 
                    (const GLchar**)&fragmentShaderSourceCode, 
                    NULL);

    glCompileShader(fragmentShaderObject);

    glGetShaderiv(  fragmentShaderObject, 
                    GL_COMPILE_STATUS, 
                    &shaderCompileStatus);

    if( GL_FALSE == shaderCompileStatus) {
        
        glGetShaderiv(  fragmentShaderObject, 
                        GL_INFO_LOG_LENGTH, 
                        &infoLength);
        if( 0 < infoLength) {
            szBuffer = (char*) malloc(sizeof(char) * infoLength);
            if( NULL != szBuffer) {
                GLsizei written;

                glGetShaderInfoLog( fragmentShaderObject, 
                                    infoLength, 
                                    &written, 
                                    szBuffer);

                fprintf(gpFile, "Fragment Shader Compilation Log: %s\n", szBuffer);
                free(szBuffer);
                DestroyWindow(ghwnd);
            }
        }
    }

    shaderProgramObject = glCreateProgram();

    glAttachShader(shaderProgramObject, fragmentShaderObject);

    glAttachShader(shaderProgramObject, vertexShaderObject);

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
                        &infoLength);

        if( 0 < infoLength) {
            szBuffer = (char *)malloc(sizeof(char) * infoLength) ;
            if( NULL != szBuffer) {
                GLint written;
                glGetProgramInfoLog(    shaderProgramObject, 
                                        infoLength,
                                        &written, 
                                        szBuffer);
                fprintf(gpFile, "Shader Program Link Log: %s\n", szBuffer);
                free(szBuffer);
                DestroyWindow(ghwnd);
            }
        }
    }

    mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, 
                                            "u_mvpMatrix");
    

    glGenVertexArrays(1, &vaoGpu);
    glGenVertexArrays(1, &vaoCpu);
	
	gPosition = (float*)malloc(sizeof(float) * arraySize);

	for (unsigned short i = 0; i < gMeshWidth; i++)
	{
		for (unsigned short j = 0; j < gMeshHeight; j++)
		{
			for (short k = 0; k < 4; k++) 
			{
				gPosition[i * gMeshHeight * 4 + j * 4 + k] = 0.0f;
			}
		}
	}

    glBindVertexArray(vaoCpu);     //   RECORDING CASET
    {
        // VERTEX
        glGenBuffers(1, &vbo_positionCpu);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_positionCpu);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * arraySize, NULL, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(MST_ATTRIBUTES_POSITION, 4, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(MST_ATTRIBUTES_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(0);       // FINISH RECORDING CASET

    glBindVertexArray(vaoGpu);     //   RECORDING CASET
    {
        // VERTEX
        glGenBuffers(1, &vbo_positionGpu);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_positionGpu);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * arraySize, NULL, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(MST_ATTRIBUTES_POSITION, 4, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(MST_ATTRIBUTES_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(0);       // FINISH RECORDING CASET

    cudaResult = cudaGraphicsGLRegisterBuffer(&cudaGraphicsResource, vbo_positionGpu, cudaGraphicsMapFlagsWriteDiscard);
	if (cudaResult != cudaSuccess)
	{
		fprintf(gpFile, "\nGaphics GL registration buffer unsuccessful\n");
		DestroyWindow(ghwnd);
	}

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glClearDepth(1.0f);

    glDepthFunc(GL_LEQUAL);

    glEnable(GL_DEPTH_TEST);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glShadeModel(GL_SMOOTH);

    perspectiveProjectionMatrix = mat4::identity();

    Resize(WIN_WIDTH, WIN_HEIGHT);
}

void Resize(int width, int height) {

    // CODE
    if( height < 1)
        height = 1;

    glViewport(0, 0, (GLsizei) width, (GLsizei) height);

    perspectiveProjectionMatrix = perspective(45.0f, (float)width/(float)height, 1.0f, 100.0f);
}

void Display(void) 
{
	//	Function Declaration
	void launchSineWaveFromCPU(unsigned int meshWidth, unsigned int meshHeight, float time);
	//void launchSineWaveFromCPU(unsigned int meshWidth, unsigned int meshHeight, float time);

    
    // VARIABLES
    mat4 modelViewMatrix;
    mat4 modelViewProjectionMatrix;
    mat4 translateMatrix;

    // CODE
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    LoadIdentity(modelViewMatrix);
    LoadIdentity(modelViewProjectionMatrix);
    LoadIdentity(translateMatrix);

    translateMatrix = vmath::translate(0.0f, 0.0f, -3.0f);

    modelViewMatrix = translateMatrix;

    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

    glUseProgram(shaderProgramObject);

    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);   
    
    
    if( bOnGpu )
    {
    	cudaResult = cudaGraphicsMapResources(1, &cudaGraphicsResource, 0);
    	if( cudaSuccess != cudaResult )
    	{
    		fprintf(gpFile, "Cuda Graphics map resource unsuccessful\n");
    		fflush(gpFile);
    		DestroyWindow(ghwnd);
    	}
		fprintf(gpFile, "Cuda Graphics map resource successful\n");
		fflush(gpFile);
		
		float4* pPosition = NULL;
		size_t numOfBytes;
		cudaResult = cudaGraphicsResourceGetMappedPointer((void**)&pPosition, &numOfBytes, cudaGraphicsResource);
	
		if( cudaSuccess != cudaResult )
		{
			fprintf(gpFile, "Cuda Graphics map resource pointer unsuccessful\n");
    		fflush(gpFile);
    		DestroyWindow(ghwnd);
    	}		
    	fprintf(gpFile, "Cuda Graphics map resource pointer successful\n");
    	fflush(gpFile);
    	
    	launchSineWaveFromCUDAKernel(pPosition, gMeshWidth, gMeshHeight, animationTime);
    	cudaGraphicsUnmapResources(1, &cudaGraphicsResource, 0);

    	glBindBuffer(GL_ARRAY_BUFFER, vbo_positionGpu);
	    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * arraySize, gPosition, GL_DYNAMIC_DRAW);
	    glVertexAttribPointer(MST_ATTRIBUTES_POSITION, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	    glEnableVertexAttribArray(MST_ATTRIBUTES_POSITION);
	    glBindBuffer(GL_ARRAY_BUFFER, 0);

	  	glVertexAttrib3f(MST_ATTRIBUTES_COLOR, 1.0f, 0.5f, 0.0f);

	    glBindVertexArray(vaoGpu);

		glDrawArrays(GL_POINTS, 0, gMeshHeight * gMeshWidth);

	    glBindVertexArray(0);

    }
    else
    {
	    launchSineWaveFromCPU(gMeshWidth, gMeshHeight, animationTime);

	    glBindBuffer(GL_ARRAY_BUFFER, vbo_positionCpu);
	    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * arraySize, gPosition, GL_DYNAMIC_DRAW);
	    glVertexAttribPointer(MST_ATTRIBUTES_POSITION, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	    glEnableVertexAttribArray(MST_ATTRIBUTES_POSITION);
	    glBindBuffer(GL_ARRAY_BUFFER, 0);

	  	glVertexAttrib3f(MST_ATTRIBUTES_COLOR, 1.0f, 0.5f, 0.0f);

	    glBindVertexArray(vaoCpu);

		glDrawArrays(GL_POINTS, 0, gMeshHeight * gMeshWidth);

	    glBindVertexArray(0);
	}

    glUseProgram(0);

    animationTime += 0.001f;

    if( 360.0f < animationTime )
    {
    	animationTime = 0.0f;
    }

    SwapBuffers(ghdc);
}

void launchSineWaveFromCPU( unsigned int meshWidth, unsigned int meshHeight, float time )
{
	for (unsigned int i = 0; i < meshWidth; i++)
	{
		for (unsigned int j = 0; j < meshHeight; j++)
		{
			for (short k = 0; k < 4; k++)
			{
				float u = i / (float)meshWidth;
				float v = j / (float)meshHeight;
				u = u * 2.0 - 1.0;
				v = v * 2.0 - 1.0;
				float frequency = 4.0;
				float w = sin(u * frequency + time) * cos(v * frequency + time) * 0.5;

				if (k == 0)
				{
					gPosition[i * meshHeight  * 4 + j * 4 + k ] = u;
				}
				if (k == 1)
				{
					gPosition[i * meshHeight * 4 + j * 4 + k] = w;
				}
				if (k == 2)
				{
					gPosition[i * meshHeight * 4 + j * 4 + k] = v;
				}
				if (k == 3)
				{
					gPosition[i * meshHeight * 4 + j * 4 + k] = 1.0;
				}
			}
		}
	}
}

void Uninitialize(void) {

    // CODE

    if( vaoCpu ) {
        glDeleteVertexArrays(1, &vaoCpu);
        vaoCpu = 0;
    }

    if( vaoGpu ) {
        glDeleteVertexArrays(1, &vaoGpu);
        vaoGpu = 0;
    }

    if( vbo_positionCpu ) {
        glDeleteBuffers(1, &vbo_positionCpu);
        vbo_positionCpu = 0;
    }

    if( vbo_positionGpu ) {
        glDeleteBuffers(1, &vbo_positionGpu);
        vbo_positionGpu = 0;
    }

    if( cudaGraphicsResource )
    {
    	cudaGraphicsUnregisterResource(cudaGraphicsResource);
    	cudaGraphicsResource = 0;
    }

    if( vbo_color ) {
        glDeleteBuffers(1, &vbo_color);
        vbo_color = 0;
    }

    if( shaderProgramObject) {
        glUseProgram(shaderProgramObject);

        GLsizei shaderCount = 0;

        glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &shaderCount);

        GLuint *pShader = NULL;

        pShader = (GLuint*) malloc(sizeof(GLuint) * shaderCount);

        if( pShader == NULL ) {
            fprintf(gpFile, "ERROR: Error in pShader\n");
        }

        glGetAttachedShaders(shaderProgramObject, shaderCount, &shaderCount, pShader);

        for( GLsizei i = 0; i < shaderCount; i++) {
            glDetachShader(shaderProgramObject, pShader[i]);
            glDeleteShader(pShader[i]);
            pShader[i] = 0;
        }

        free(pShader);

        glDeleteProgram(shaderProgramObject);
        
        shaderProgramObject = 0;

        glUseProgram(0);
    }

    glDeleteProgram(shaderProgramObject);
    shaderProgramObject = 0;

    glUseProgram(0);

    if( bFullscreen ){
        SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(ghwnd, &wpPrev);
        SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
        ShowCursor(TRUE);
        bFullscreen = false;
    }

    if( wglGetCurrentContext() == ghrc) {
        wglMakeCurrent(NULL, NULL);
    }

    if( ghrc ) {
        wglDeleteContext(ghrc);
        ghrc = NULL;
    }

    if( ghdc ) {
        ReleaseDC(ghwnd, ghdc);
        ghdc = NULL;
    }

    if( gpFile ) {
        fclose(gpFile);
        gpFile = NULL;
    }
}
