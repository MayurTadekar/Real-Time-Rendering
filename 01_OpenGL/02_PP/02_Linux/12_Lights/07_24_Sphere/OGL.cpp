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

#include    "Sphere.h"

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

GLuint lKeyUniform;

GLuint lightDiffuseUniform;
GLuint lightAmbientUniform;
GLuint lightSpecularUniform;
GLuint lightPositionUniform;

GLuint materialDiffuseUniform;
GLuint materialAmbientUniform;
GLuint materialSpecularUniform;
GLuint materialShininessUniform;

GLuint vaoSphere;
GLuint vboSpherePosition;
GLuint vboSphereNormal;
GLuint vboSphereElements;

int bAnimation = 0;

float angle = 0.0f;

float   sphere_vertices[1146];
float   sphere_normals[1146];
float   sphere_textures[764];
unsigned short  sphere_elements[2280];

GLuint  gNumVertices;
GLuint  gNumElements;

int bLight = 0;

int axis = 1;

GLfloat     materialAmbient[] = {	0.0215,0.1745,0.0215,1.0f,
                                    0.135,0.2225,0.1575,1.0f,
                                    0.05375,0.05,0.06625,1.0f,
                                    0.25,0.20725,0.20725,1.0f,
                                    0.1745,0.01175,0.01175,1.0f,
                                    0.1,0.18725,0.1745,1.0f,
                                    0.329412,0.223529,0.027451,1.0f,
                                    0.2125,0.1275,0.054,1.0f,
                                    0.25,0.25,0.25,1.0f,
                                    0.19125,0.0735,0.0225,1.0f,
                                    0.24725,0.1995,0.0745,1.0f,
                                    0.19225,0.19225,0.19225,1.0f,
                                    0.0,0.0,0.0,1.0f,
                                    0.0,0.1,0.06,1.0f,
                                    0.0,0.0,0.0,1.0f,
                                    0.0,0.0,0.0,1.0f,
                                    0.0,0.0,0.0,1.0f,
                                    0.0,0.0,0.0,1.0f,
                                    0.02,0.02,0.02,1.0f,
                                    0.0,0.05,0.05,1.0f,
                                    0.0,0.05,0.0,1.0f,
                                    0.05,0.0,0.0,1.0f,
                                    0.05,0.05,0.05,1.0f,
                                    0.05,0.05,0.0,1.0f
                                };

GLfloat     materialDiffuse[] = { 	0.07568,0.61424,0.07568,1.0f, 
                                    0.54,0.89,0.63,1.0f,
                                    0.18275,0.17,0.22525,1.0f,
                                    1.0,0.829,0.829,1.0f,
                                    0.61424,0.04136,0.04136,1.0f, 
                                    0.396,0.74151,0.69102,1.0f,
                                    0.780392,0.568627,0.113725,1.0f,
                                    0.714,0.4284,0.18144,1.0f,
                                    0.4,0.4,0.4,1.0f,
                                    0.7038,0.27048,0.0828,1.0f,
                                    0.75164,0.60648,0.22648,1.0f,
                                    0.50754,0.50754,0.50754,1.0f,
                                    0.01,0.01,0.01,1.0f,
                                    0.0,0.50980392,0.50980392,1.0f,
                                    0.1,0.35,0.1,1.0f,
                                    0.5,0.0,0.0,1.0f,
                                    0.55,0.55,0.55,1.0f,
                                    0.5,0.5,0.0,1.0f,
                                    0.01,0.01,0.01,1.0f,
                                    0.4,0.5,0.5,1.0f,
                                    0.4,0.5,0.4,1.0f,
                                    0.5,0.4,0.4,1.0f,
                                    0.5,0.5,0.5,1.0f,
                                    0.5,0.5,0.4,1.0f
                                };

GLfloat     materialSpecular[] = { 	0.633,0.727811,0.633,1.0f,
                                    0.316228,0.316228,0.316228,1.0f,
                                    0.332741,0.328634,0.346435,1.0f,
                                    0.296648,0.296648,0.296648,1.0f,
                                    0.727811,0.626959,0.626959,1.0f,
                                    0.297254,0.30829,0.306678,1.0f,
                                    0.992157,0.941176,0.807843,1.0f,
                                    0.393548,0.271906,0.166721,1.0f,
                                    0.774597,0.774597,0.774597,1.0f,
                                    0.256777,0.137622,0.086014,1.0f,
                                    0.628281,0.555802,0.366065,1.0f,
                                    0.508273,0.508273,0.508273,1.0f,
                                    0.50,0.50,0.50,1.0f,
                                    0.50196078,0.50196078,0.50196078,1.0f,
                                    0.45,0.55,0.45,1.0f,
                                    0.7,0.6,0.6,1.0f,
                                    0.70,0.70,0.70,1.0f,
                                    0.60,0.60,0.50,1.0f,
                                    0.4,0.4,0.4,1.0f,
                                    0.04,0.7,0.7,1.0f,
                                    0.04,0.7,0.04,1.0f,
                                    0.7,0.04,0.04,1.0f,
                                    0.7,0.7,0.7,1.0f,
                                    0.7,0.7,0.04,1.0f
                                };

GLfloat     materialShininess[] = 	{	0.6 * 128,
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
                                        0.078125 * 128
                                    };

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
    void Update(void);

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

                        case XK_l:
                        case XK_L:
                            if(1 == bLight)
                                bLight = 0;
                            else
                                bLight = 1;
                            break;

                        case XK_a:
                        case XK_A:
                            if( 1 == bAnimation)
                            {
                                bAnimation = 0;
                            }
                            else
                            {
                                bAnimation = 1;
                            }
                            break;

                        case XK_x:
                        case XK_X:
                            axis = 1;
                            break;

                        case XK_y:
                        case XK_Y:
                            axis = 2;
                            break;

                        case XK_z:
                        case XK_Z:
                            axis = 3;
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
        Update();
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
    void    initializeSphere();
    void    Uninitialize(void);


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

    initializeSphere();

    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);

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
                            MST_ATTRIBUTES_NORMAL, 
                            "vNormal");
    
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


    mMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_mMatrix");
    vMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_vMatrix");
    pMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_pMatrix");
    
    lKeyUniform = glGetUniformLocation(shaderProgramObject, "u_lKey");
    
    lightAmbientUniform = glGetUniformLocation(shaderProgramObject, "u_lightAmbient");
    lightDiffuseUniform = glGetUniformLocation(shaderProgramObject, "u_lightDiffuse");
    lightSpecularUniform = glGetUniformLocation(shaderProgramObject, "u_lightSpecular");
    lightPositionUniform = glGetUniformLocation(shaderProgramObject, "u_lightPosition");

    materialAmbientUniform = glGetUniformLocation(shaderProgramObject, "u_materialAmbient");
    materialDiffuseUniform = glGetUniformLocation(shaderProgramObject, "u_materialDiffuse");
    materialSpecularUniform = glGetUniformLocation(shaderProgramObject, "u_materialSpecular");
    materialShininessUniform = glGetUniformLocation(shaderProgramObject, "u_materialShininess");

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

void initializeSphere()
{

	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);

	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();

    glGenVertexArrays(1, &vaoSphere);
    
    glBindVertexArray(vaoSphere);
    {
        glGenBuffers(1, &vboSpherePosition);
        glBindBuffer(GL_ARRAY_BUFFER, vboSpherePosition);
        {
            glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);

            glVertexAttribPointer(MST_ATTRIBUTES_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

            glEnableVertexAttribArray(MST_ATTRIBUTES_POSITION);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &vboSphereNormal);
        glBindBuffer(GL_ARRAY_BUFFER, vboSphereNormal);
        {
            glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);

            glVertexAttribPointer(MST_ATTRIBUTES_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);

            glEnableVertexAttribArray(MST_ATTRIBUTES_NORMAL);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &vboSphereElements);
        glBindBuffer(GL_ARRAY_BUFFER, vboSphereElements);
        {
            glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(0);
}

void Resize(int width, int height) {

    if( height < 1)
        height = 1;

    glViewport(0, 0, (GLsizei)width, (GLsizei) height);

    perspectiveProjectionMatrix = vmath::perspective(35.0f, (float)width/(float)height, 1.0f, 100.0f);

}

void Draw(void) 
{

    void DrawSphere(vmath::mat4 viewMatrix);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vmath::mat4 viewMatrix = vmath::mat4::identity();

    viewMatrix = vmath::lookat( vmath::vec3(2.5f * 2.0f, 1.5f * 2.0f, 12.0f),
                                vmath::vec3(2.5f * 2.0f, 1.5f * 2.0f, 0.0f),
                                vmath::vec3(0.0f, 1.0f, 0.0f));

    DrawSphere(viewMatrix);

    glXSwapBuffers(gpDisplay,gWindow);
}

void DrawSphere(vmath::mat4 viewMatrix)
{

    vmath::mat4 modelMatrix;

    for(int le = 0; le < 24; ++le)
    {

        modelMatrix = vmath::mat4::identity();

        modelMatrix *= vmath::translate(le / 4 * 2.0f, le % 4 * 2.0f, 0.0f);
        modelMatrix *= vmath::rotate(angle, vmath::vec3(0.0f, 1.0f, 0.0f));

        glUseProgram(shaderProgramObject);
        {
            glUniformMatrix4fv(mMatrixUniform, 1, GL_FALSE, modelMatrix);
            glUniformMatrix4fv(vMatrixUniform, 1, GL_FALSE, viewMatrix);
            glUniformMatrix4fv(pMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
            
            if(1 == bLight)
            {
                glUniform1i(lKeyUniform, 1);

                glUniform3f(lightAmbientUniform, 0.1f, 0.1f, 0.1f);
                glUniform3f(lightDiffuseUniform, 1.0f, 1.0f, 1.0f);
                glUniform3f(lightSpecularUniform, 1.0f, 1.0f, 1.0f);
                
                if(1 == axis)
                {
                    GLfloat light_position[] = {
                                                    0.0f, 
                                                    20.0f * sinf(vmath::radians(angle)), 
                                                    20.0f * cosf(vmath::radians(angle)), 
                                                    1.0f
                                                };
                    glUniform4fv(lightPositionUniform, 1, (const GLfloat*)light_position);
                }
                else if(2 == axis)
                {
                    GLfloat light_position[] = {
                                                    20.0f * sinf(vmath::radians(angle)), 
                                                    0.0f, 
                                                    20.0f * cosf(vmath::radians(angle)), 
                                                    1.0f
                                                };
                    glUniform4fv(lightPositionUniform, 1, (const GLfloat*)light_position);
                }
                else if(3 == axis)
                {
                    GLfloat light_position[] = {
                                                    20.0f * sinf(vmath::radians(angle)), 
                                                    20.0f * cosf(vmath::radians(angle)), 
                                                    0.0f, 
                                                    1.0f
                                                };
                    glUniform4fv(lightPositionUniform, 1, (const GLfloat*)light_position);
                }
                
                
                glUniform3fv(materialAmbientUniform, 1, &materialAmbient[le * 4]);
                glUniform3fv(materialDiffuseUniform, 1, &materialDiffuse[le * 4]);
                glUniform3fv(materialSpecularUniform, 1, &materialSpecular[le * 4]);
                glUniform1f(materialShininessUniform, materialShininess[le]);
            }
            else
            {
                glUniform1i(lKeyUniform, 0);
            }

            glBindVertexArray(vaoSphere);
            {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboSphereElements);
                glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
            }
            glBindVertexArray(0);
        }
        glUseProgram(0);

    }

}

void Update()
{
    // Code

    if(1 == bAnimation)
        angle += 1.0f;

    if(360.0f > angle)
    {
        angle -= 360.0f;
    }
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
