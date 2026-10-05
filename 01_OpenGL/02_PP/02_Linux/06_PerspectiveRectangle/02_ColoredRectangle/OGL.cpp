// HEADERS

#include	<iostream>
#include	<stdio.h>
#include	<stdlib.h>
#include	<memory.h>
#include	<string.h>

#include	<X11/Xlib.h>
#include	<X11/Xutil.h>
#include	<X11/XKBlib.h>
#include	<X11/keysym.h>

// CHANGES
#include 	<GL/glew.h>
#include	<GL/gl.h>	// OpenGL APIs
#include	<GL/glx.h>	// Bridiging APIs

#include    <GL/vmath/vmath.h>

// MACROS
#define		WIN_WIDTH	800
#define		WIN_HEIGHT	600

#define		EXIT_ERROR(error)	{						\
                        fprintf(stderr,"ERROR: %s\n", error);	\
                        Uninitialize();				\
                        exit(1);				\
                    }

enum
{
    MST_ATTRIBUTES_POSITION = 1,
    MST_ATTRIBUTES_COLOR,
    MST_ATTRIBUTES_NORMAL,
    MST_ATTRIBUTES_TEXCOORD0,
};

// GLOBAL VARIABLES
bool		bFullscreen = false;
Display*	gpDisplay = NULL;
Window		gWindow;
Colormap	gColormap;
XVisualInfo*	gpXVisualInfo = NULL;
GLXContext	gGLXContext;

typedef GLXContext(*glxCreateContextAttribsARBProc)(Display*, 
                                                    GLXFBConfig, 
                                                    GLXContext, 
                                                    Bool, 
                                                    const int *);
glxCreateContextAttribsARBProc glxCreateContextAttribsARB = NULL;
GLXFBConfig	 gGLXFBConfig;

GLuint 	fragmentShaderObject = 0;
GLuint	vertexShaderObject = 0;
GLuint 	shaderProgramObject = 0;

vmath::mat4 perspectiveProjectionMatrix;

GLuint  mMatrixUniform;
GLuint  vMatrixUniform;
GLuint  pMatrixUniform;

GLuint vao;
GLuint vboPosition;
GLuint vboColor;

// FUNCTIONS 

// MAIN
int main(int argc, char **argv, char **envp) {

    // FUNCTION 
    void CreateWindow(void);
    void Uninitialize(void);
    void ToggleFullscreen(void);
    // CHANGE
    void Initialize(void);
    void Resize(int, int);
    void Draw(void);

    // VARIABLES
    int	win_width = 0;
    int	win_height = 0;
    KeySym	keysym;
    XEvent	event;
    // CHANGE
    bool	bDone = false;

    // CODE
    
    CreateWindow();
    
    // CHANGE
    Initialize();


    while(bDone == false) {

        while( XPending(gpDisplay) ) {

            XNextEvent(gpDisplay, &event);

            switch(event.type) {
                case MapNotify:
                    break;

                case KeyPress:
                    keysym = XkbKeycodeToKeysym(gpDisplay, event.xkey.keycode, 0, 0);

                    switch(keysym) {
                        case XK_Escape:
                            bDone = true;
                            break;

                        case XK_F:
                        case XK_f:
                            if( bFullscreen ){
                                ToggleFullscreen();
                                bFullscreen = false;
                            } else {
                                ToggleFullscreen();
                                bFullscreen = true;
                            }
                            break;
                    }
                    break;

                case ButtonPress:
                    switch(event.xbutton.button) {
                        case 1:
                            break;

                        case 2:
                            break;

                        case 3:
                            break;

                        default:
                            break;
                    }
                    // add missing break to avoid fall-through into ConfigureNotify
                    break;

                case ConfigureNotify:
                    win_width = event.xconfigure.width;
                    win_height = event.xconfigure.height;
                    Resize( win_width, win_height);
                    break;

                case Expose:
                    break;

                case 33:
                    bDone = true;
                    break;
            }
        }
        Draw();
    }

    Uninitialize();

    return(0);
}


// CreateWindow
void CreateWindow(void) {

    // FUNCTION
    void Uninitialize(void);

    // VARIABLES
    XSetWindowAttributes 	winAttribs;
    
    //	Changes
    GLXFBConfig 	*pGLXFBConfig = NULL;
    GLXFBConfig 	bestGLXFBConfig;
    XVisualInfo 	*pTempXVisualInfo;
    int 			numFBConfigs = 0;
    //	-------

    int		defaultScreen;
    int		styleMask;
    Status		status;

    static int frameBufferAttributes[] = { 	//	CHANGES
                                            GLX_X_RENDERABLE, 	True,
                                            GLX_DRAWABLE_TYPE, 	GLX_WINDOW_BIT,
                                            GLX_RENDER_TYPE,	GLX_RGBA_BIT,
                                            GLX_X_VISUAL_TYPE,	GLX_TRUE_COLOR,
                                            //	-------
                                            GLX_DOUBLEBUFFER, 	True,
                                            GLX_RED_SIZE, 		8,
                                            GLX_GREEN_SIZE, 	8,
                                            GLX_BLUE_SIZE, 		8,	
                                            GLX_ALPHA_SIZE, 	8,
                                            GLX_STENCIL_SIZE, 	8,
                                            GLX_DEPTH_SIZE, 	24,
                                            // V4L recommends bitsize of depth to be kept around 24
                                            None 			
                                            // Here we can use '0' as well 	
                                         };

    // CODE

    gpDisplay = XOpenDisplay(NULL);
    if( NULL == gpDisplay) 
        EXIT_ERROR("Display cannot be opened");

    defaultScreen = XDefaultScreen(gpDisplay);

    /*
    gpXVisualInfo = glXChooseVisual(gpDisplay, defaultScreen, frameBufferAttributes);
    if( NULL == gpXVisualInfo) 
        EXIT_ERROR("Memory cannot be allocated to XVisualInfo");
    */

    pGLXFBConfig = glXChooseFBConfig(	gpDisplay, 
                                        XDefaultScreen(gpDisplay), 
                                        frameBufferAttributes,
                                        &numFBConfigs
                                    );

    if (!pGLXFBConfig || numFBConfigs == 0) {
        EXIT_ERROR("Failed to get any GLXFBConfig");
    }

    printf("Found Number of FBConfig = %d\n", numFBConfigs);

    int 	bestFrameBufferConfig = -1;
    int 	worstFrameBufferConfig = -1;
    int 	bestNumberOfSamples = -1;
    int 	worstNumberOfSamples = 999;

    for( int i = 0 ; i < numFBConfigs; ++i) {
        pTempXVisualInfo = glXGetVisualFromFBConfig(gpDisplay, 
                                                    pGLXFBConfig[i]);
        if( NULL != pTempXVisualInfo ) {
            int 	sampleBuffers = 0, samples = 0;
            glXGetFBConfigAttrib(	gpDisplay,
                                    pGLXFBConfig[i],
                                    GLX_SAMPLE_BUFFERS,
                                    &sampleBuffers);
            glXGetFBConfigAttrib(	gpDisplay,
                                    pGLXFBConfig[i],
                                    GLX_SAMPLES,
                                    &samples);
            if( bestFrameBufferConfig < 0 || (sampleBuffers && samples > bestNumberOfSamples) ) {
                bestFrameBufferConfig = i;
                bestNumberOfSamples = samples;
            }

            if( worstFrameBufferConfig < 0 || (!sampleBuffers || samples < worstNumberOfSamples) ) {
                worstFrameBufferConfig = i;
                worstNumberOfSamples = samples;
            }
            printf("For i = %d then samples = %d, sampleBuffers = %d\n", i, samples, sampleBuffers);
        }
        if (pTempXVisualInfo)
            XFree(pTempXVisualInfo);
    }

    if (bestFrameBufferConfig < 0) {
        XFree(pGLXFBConfig);
        EXIT_ERROR("No suitable FBConfig found");
    }

    bestGLXFBConfig = pGLXFBConfig[bestFrameBufferConfig];
    gGLXFBConfig = bestGLXFBConfig;

    XFree(pGLXFBConfig);

    gpXVisualInfo = glXGetVisualFromFBConfig(	gpDisplay,
                                                gGLXFBConfig);
    if( NULL == gpXVisualInfo) 
        EXIT_ERROR("Memory cannot be allocated to XVisualInfo");

    winAttribs.border_pixel = 0;
    winAttribs.background_pixmap = 0;
    winAttribs.background_pixel = BlackPixel(gpDisplay,
                         defaultScreen);
                    
    winAttribs.colormap = XCreateColormap(	gpDisplay,
                        RootWindow(gpDisplay, gpXVisualInfo->screen),
                        gpXVisualInfo->visual,
                        AllocNone);

    winAttribs.event_mask = ExposureMask | StructureNotifyMask | VisibilityChangeMask | PointerMotionMask | KeyPressMask | ButtonPressMask;
    gColormap = winAttribs.colormap;

    styleMask = CWBackPixel | CWBorderPixel | CWEventMask | CWColormap;

    gWindow = XCreateWindow( gpDisplay,
                 RootWindow(gpDisplay, gpXVisualInfo->screen),
                 0,
                 0,
                 WIN_WIDTH,
                 WIN_HEIGHT,
                 0,
                 gpXVisualInfo->depth,
                 InputOutput,
                 gpXVisualInfo->visual,
                 styleMask,
                 &winAttribs);

    if( 0 == gWindow) 
        EXIT_ERROR("Window Cannot be created");

    XStoreName(gpDisplay, gWindow, "Window");

    Atom windowDeleteManager = XInternAtom(gpDisplay, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(gpDisplay, gWindow, &windowDeleteManager, 1);

    XMapWindow(gpDisplay, gWindow);
}

// ToggleFullscreen


void ToggleFullscreen(void) {
    
    // VARIABLES
    Atom	wm_state = XInternAtom(gpDisplay, "_NET_WM_STATE", False);
    Atom	fullscreen = XInternAtom(gpDisplay, "_NET_WM_STATE_FULLSCREEN", False);
    XEvent	event = {0};

    // CODE
    event.type = ClientMessage;
    event.xclient.window = gWindow;
    event.xclient.message_type = wm_state;
    event.xclient.format = 32;
    event.xclient.data.l[0] = bFullscreen ? 0 : 1;
    event.xclient.data.l[1] = fullscreen;

    XSendEvent( gpDisplay,
            RootWindow(gpDisplay, gpXVisualInfo->screen),
            False,
            StructureNotifyMask,
            &event);
}

void Initialize(void) {

    // FUNCTION DECLARATION
    void 	Resize(int, int);
    void 	InitProgramObject(void);
    void    initializeDrawingObjects();
    void Uninitialize(void);


    // get pointer to glXCreateContextAttribsARB if available
    glxCreateContextAttribsARB = (glxCreateContextAttribsARBProc)glXGetProcAddressARB( (const GLubyte*) "glXCreateContextAttribsARB" );

    int contextAttribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
        GLX_CONTEXT_MINOR_VERSION_ARB, 5,
        GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        None
    };
    
    // Try creating modern context if function present
    if (glxCreateContextAttribsARB) {
        gGLXContext = glxCreateContextAttribsARB(gpDisplay, gGLXFBConfig, 0, True, contextAttribs);
    
        // If failed, try with OpenGL 3.3
        if (!gGLXContext) {
            contextAttribs[1] = 3;
            contextAttribs[3] = 3;
            gGLXContext = glxCreateContextAttribsARB(gpDisplay, gGLXFBConfig, 0, True, contextAttribs);
        }
    
        // If still failed, try compatibility profile (older)
        if (!gGLXContext) {
            contextAttribs[1] = 3;
            contextAttribs[3] = 0;
            contextAttribs[4] = GLX_CONTEXT_PROFILE_MASK_ARB;
            contextAttribs[5] = GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
            gGLXContext = glxCreateContextAttribsARB(gpDisplay, gGLXFBConfig, 0, True, contextAttribs);
        }
    } else {
        // Fallback: create a legacy context using glXCreateContext
        gGLXContext = glXCreateContext(gpDisplay, gpXVisualInfo, 0, GL_TRUE);
    }
    
    if (!gGLXContext) {
        fprintf(stderr, "Failed to create OpenGL context\n");
        Uninitialize();
        exit(1);
    }

    Bool 	isDirectContext = glXIsDirect(gpDisplay, gGLXContext);

    if( isDirectContext == True ) {
        printf("The Rendering Context is Hardware Rendering Context\n");
    } else {
        printf("The Rendering Context is Software Rendering Context\n");	
    }

    if (!glXMakeCurrent(gpDisplay, gWindow, gGLXContext)) {
        fprintf(stderr, "glXMakeCurrent() failed\n");
        Uninitialize();
        exit(1);
    }

    InitProgramObject();

    initializeDrawingObjects();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glDepthFunc(GL_LEQUAL);

    glEnable(GL_DEPTH_TEST);

    glClearDepth(1.0f);


    Resize(WIN_WIDTH, WIN_HEIGHT);
}

void 	InitProgramObject(void) {

    void 	Uninitialize(void);

    //	CODE
    // Ensure GLEW works with core profiles
    glewExperimental = GL_TRUE;
    GLenum glew_error = glewInit();
    if( GLEW_OK != glew_error ){
        fprintf(stderr, "GLEW Cannot be Initialized: %s\n", glewGetErrorString(glew_error));
        Uninitialize();
        exit(1);
    }

    // Read vertex shader file
    GLchar	*vertexShaderSource = NULL;					
    FILE *file_vertex = fopen("vertexshader.vs", "r");
    if (!file_vertex) {
        fprintf(stderr, "Failed to open vertexshader.vs\n");
        Uninitialize();
        exit(1);
    }
    fseek(file_vertex, 0, SEEK_END);
    long vlen = ftell(file_vertex);
    fseek(file_vertex, 0, SEEK_SET);
    vertexShaderSource = (GLchar*) malloc(vlen + 1);
    if (vertexShaderSource && vlen > 0) {
        fread(vertexShaderSource, 1, vlen, file_vertex);
    }
    vertexShaderSource[vlen] = '\0';
    fclose(file_vertex);
        
    vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
    if( 0 == vertexShaderObject ) {
        fprintf(stderr, "Vertex shader not created\n");
        Uninitialize();
        exit(1);
    }
    glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSource, NULL);
    glCompileShader(vertexShaderObject);

    // Check vertex shader compile status
    GLint status = 0;
    glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);
    if (!status) {
        GLint logLen = 0;
        glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &logLen);
        if (logLen > 0) {
            char *log = (char*)malloc(logLen);
            glGetShaderInfoLog(vertexShaderObject, logLen, NULL, log);
            fprintf(stderr, "Vertex shader compile error: %s\n", log);
            free(log);
        }
        Uninitialize();
        exit(1);
    }
    
    // Read fragment shader file
    GLchar	*fragmentShaderSource = NULL; 
    FILE *file_fragment = fopen("fragmentshader.fs", "r");
    if (!file_fragment) {
        fprintf(stderr, "Failed to open fragmentshader.fs\n");
        Uninitialize();
        exit(1);
    }
    fseek(file_fragment, 0, SEEK_END);
    long flen = ftell(file_fragment);
    fseek(file_fragment, 0, SEEK_SET);
    fragmentShaderSource = (GLchar*) malloc(flen + 1);
    if (fragmentShaderSource && flen > 0) {
        fread(fragmentShaderSource, 1, flen, file_fragment);
    }
    fragmentShaderSource[flen] = '\0';
    fclose(file_fragment);

    fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    if (0 == fragmentShaderObject) {
        fprintf(stderr, "Fragment shader not created\n");
        Uninitialize();
        exit(1);
    }
    glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSource, NULL);
    glCompileShader(fragmentShaderObject);

    // Check fragment shader compile status
    status = 0;
    glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);
    if (!status) {
        GLint logLen = 0;
        glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &logLen);
        if (logLen > 0) {
            char *log = (char*)malloc(logLen);
            glGetShaderInfoLog(fragmentShaderObject, logLen, NULL, log);
            fprintf(stderr, "Fragment shader compile error: %s\n", log);
            free(log);
        }
        Uninitialize();
        exit(1);
    }
    
    shaderProgramObject = glCreateProgram();
    if (0 == shaderProgramObject) {
        fprintf(stderr, "Failed to create shader program\n");
        Uninitialize();
        exit(1);
    }
    glAttachShader(shaderProgramObject, vertexShaderObject);
    glAttachShader(shaderProgramObject, fragmentShaderObject);
    
    glBindAttribLocation(   shaderProgramObject, 
                            MST_ATTRIBUTES_POSITION, 
                            "vPosition");
    
    glBindAttribLocation(   shaderProgramObject, 
                            MST_ATTRIBUTES_COLOR, 
                            "vColor");
    
    glLinkProgram(shaderProgramObject);

    // Check link status
    GLint linkStatus = 0;
    glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &linkStatus);
    if (!linkStatus) {
        GLint logLen = 0;
        glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &logLen);
        if (logLen > 0) {
            char *log = (char*)malloc(logLen);
            glGetProgramInfoLog(shaderProgramObject, logLen, NULL, log);
            fprintf(stderr, "Shader program link error: %s\n", log);
            free(log);
        }
        Uninitialize();
        exit(1);
    }

    // We can detach and delete shaders after successful link
    glDetachShader(shaderProgramObject, vertexShaderObject);
    glDetachShader(shaderProgramObject, fragmentShaderObject);
    glDeleteShader(vertexShaderObject);
    glDeleteShader(fragmentShaderObject);
    vertexShaderObject = 0;
    fragmentShaderObject = 0;

    free(vertexShaderSource);
    free(fragmentShaderSource);
}

void initializeDrawingObjects()
{
    mMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_mMatrix");
    vMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_vMatrix");
    pMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_pMatrix");

    const GLfloat rectangleVertices[] = {
        1.0f, -1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,

        1.0f, -1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f
    };

    const GLfloat rectangleColor[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,

        1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &vao);
    
    glBindVertexArray(vao);
    {
        glGenBuffers(1, &vboPosition);
        glBindBuffer(GL_ARRAY_BUFFER, vboPosition);
        {
            glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);
            glVertexAttribPointer(MST_ATTRIBUTES_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(MST_ATTRIBUTES_POSITION);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &vboColor);
        glBindBuffer(GL_ARRAY_BUFFER, vboColor);
        {
            glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleColor), rectangleColor, GL_STATIC_DRAW);
            glVertexAttribPointer(MST_ATTRIBUTES_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(MST_ATTRIBUTES_COLOR);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(0);
}

void Resize(int width, int height) {

    if( height < 1) 
        height = 1;

    glViewport(0, 0, (GLsizei)width, (GLsizei) height);

    perspectiveProjectionMatrix = vmath::perspective(45.0f, (float)width/(float)height, 1.0f, 100.0f);

}

void Draw(void) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vmath::mat4 modelMatrix;
    vmath::mat4 viewMatrix;

    modelMatrix = vmath::mat4::identity();
    viewMatrix = vmath::mat4::identity();

    modelMatrix = vmath::translate(0.0f, 0.0f, -3.0f);

    glUseProgram(shaderProgramObject);
    {
        glUniformMatrix4fv(mMatrixUniform, 1, GL_FALSE, modelMatrix);
        glUniformMatrix4fv(vMatrixUniform, 1, GL_FALSE, viewMatrix);
        glUniformMatrix4fv(pMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

        glBindVertexArray(vao);
        {
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        glBindVertexArray(0);
    }
    glUseProgram(0);

    glXSwapBuffers(gpDisplay,gWindow);
}

// Uninitialize
void Uninitialize(void) {

    // VARIABLES
    GLXContext currentContext; 

    currentContext = glXGetCurrentContext();
    
    // CODE
    if( currentContext == gGLXContext) {
        glXMakeCurrent( gpDisplay, 0, 0);
    }

    if( gGLXContext ) {
        glXDestroyContext(gpDisplay, gGLXContext);
        gGLXContext = NULL;
    }

    if( shaderProgramObject ) {
        glUseProgram(0);
        glDeleteProgram(shaderProgramObject);
        shaderProgramObject = 0;
    }

    if( vertexShaderObject ) {
        glDeleteShader(vertexShaderObject);
        vertexShaderObject = 0;
    }

    if( fragmentShaderObject ) {
        glDeleteShader(fragmentShaderObject);
        fragmentShaderObject = 0;
    }

    if( gWindow )
        XDestroyWindow(gpDisplay, gWindow);

    if( gColormap) 
        XFreeColormap(gpDisplay, gColormap);

    if( gpXVisualInfo ) {
        // gpXVisualInfo was allocated by glXGetVisualFromFBConfig -> free with XFree
        XFree(gpXVisualInfo);
        gpXVisualInfo = NULL;
    }

    if( gpDisplay ) {
        XCloseDisplay(gpDisplay);
        gpDisplay = NULL;
    }
}
