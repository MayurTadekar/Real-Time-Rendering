// HEADERS

#include	<iostream>
#include	<stdio.h>
#include	<stdlib.h>
#include	<memory.h>

#include	<X11/Xlib.h>
#include	<X11/Xutil.h>
#include	<X11/XKBlib.h>
#include	<X11/keysym.h>

// CHANGES
#include	<GL/gl.h>	// OpenGL APIs
#include	<GL/glx.h>	// Bridiging APIs

// MACROS
#define		WIN_WIDTH	800
#define		WIN_HEIGHT	600

#define		EXIT_ERROR(error)	{						\
						fprintf(stderr,"ERROR: %s\n", error);	\
						Uninitialize();				\
						exit(1);				\
					}

// GLOBAL VARIABLES
bool		bFullscreen = false;
Display*	gpDisplay = NULL;
Window		gWindow;
Colormap	gColormap;
XVisualInfo*	gpXVisualInfo = NULL;
GLXContext	gGLXContext;

// CHANGE
typedef GLXContext(*glxCreateContextAttribsARBProc)(Display*,
													GLXFBConfig,
													GLXContext,
													Bool,
													const int *);
glxCreateContextAttribsARBProc glxCreateContextAttribsARB = NULL;
GLXFBConfig	 gGLXFBConfig;

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

	printf("Found Number of FBConfig = %d\n", numFBConfigs);

	int 	bestFrameBufferConfig = -1;
	int 	worstFrameBufferConfig = -1;
	int 	bestNumberOfSamples = -1;
	int 	worstNumberOfSamples = 999;

	for( int i = 0 ; i < numFBConfigs; ++i) {
		pTempXVisualInfo = glXGetVisualFromFBConfig(gpDisplay,
													pGLXFBConfig[i]);
		if( NULL != pTempXVisualInfo ) {
			int 	sampleBuffers, samples;
			glXGetFBConfigAttrib(	gpDisplay,
									pGLXFBConfig[i],
									GLX_SAMPLE_BUFFERS,
									&sampleBuffers);
			glXGetFBConfigAttrib(	gpDisplay,
									pGLXFBConfig[i],
									GLX_SAMPLES,
									&samples);
			if( bestFrameBufferConfig < 0 || sampleBuffers && samples > bestNumberOfSamples ) {
				bestFrameBufferConfig = i;
				bestNumberOfSamples = samples;
			}

			if( worstFrameBufferConfig < 0 || !sampleBuffers || samples < worstNumberOfSamples ) {
				worstFrameBufferConfig = i;
				worstNumberOfSamples = samples;
			}
			printf("For i = %d then samples = %d, sampleBuffers = %d\n", i, samples, sampleBuffers);
		}
		XFree(pTempXVisualInfo);
	}

	bestGLXFBConfig = pGLXFBConfig[bestFrameBufferConfig];
	gGLXFBConfig = bestGLXFBConfig;

	// FIX: Assign visual info for the best config
	gpXVisualInfo = glXGetVisualFromFBConfig(gpDisplay, gGLXFBConfig);

	XFree(pGLXFBConfig);

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
	void Resize(int, int);


	glxCreateContextAttribsARB = (glxCreateContextAttribsARBProc)glXGetProcAddressARB( (GLubyte*) "glXCreateContextAttribsARB" );

	//gGLXContext = glXCreateContext( gpDisplay, gpXVisualInfo, NULL, GL_TRUE);
	const int attribs[]	= 	{	GLX_CONTEXT_MAJOR_VERSION_ARB,	4,
								GLX_CONTEXT_MINOR_VERSION_ARB,	5,
								GLX_CONTEXT_PROFILE_MASK_ARB,	GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
								None
							};

	gGLXContext = glxCreateContextAttribsARB(	gpDisplay,
												gGLXFBConfig,
												0,
												True,
												attribs);
	if( !gGLXContext ) {

		const int attribs_2[]	= 	{	GLX_CONTEXT_MAJOR_VERSION_ARB,	1,
										GLX_CONTEXT_MINOR_VERSION_ARB,	0,
										None
									};
		gGLXContext = glxCreateContextAttribsARB(	gpDisplay,
													gGLXFBConfig,
													0,
													True,
													attribs_2);
		printf(" Minimum gGLXContext Found\n");
	} else {
		printf(" Core Profile gGLXContext Found\n");
	}

	Bool 	isDirectContext = glXIsDirect(gpDisplay, gGLXContext);

	if( isDirectContext == True ) {
		printf("The Rendering Context is Hardware Rendering Context");
	} else {
		printf("The Rendering Context is Software Rendering Context");
	}

	glXMakeCurrent(gpDisplay, gWindow, gGLXContext);

	glShadeModel(GL_SMOOTH);

	glDepthFunc(GL_LEQUAL);

	glEnable(GL_DEPTH);

	glClearDepth(1.0f);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

	Resize(WIN_WIDTH, WIN_HEIGHT);
}



void Resize(int width, int height) {

	if( height < 1)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei) height);

}

void Draw(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
	}

	if( gWindow )
		XDestroyWindow(gpDisplay, gWindow);

	if( gColormap)
		XFreeColormap(gpDisplay, gColormap);

	if( gpXVisualInfo ) {
		free(gpXVisualInfo);
		gpXVisualInfo = NULL;
	}

	if( gpDisplay ) {
		XCloseDisplay(gpDisplay);
		gpDisplay = NULL;
	}
}
