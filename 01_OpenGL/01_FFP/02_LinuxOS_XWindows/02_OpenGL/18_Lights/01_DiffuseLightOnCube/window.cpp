

// HEADERS
#include	<iostream>
#include	<stdio.h>
#include	<stdlib.h>
#include	<memory.h>

#include	<X11/Xlib.h>
#include	<X11/Xutil.h>
#include	<X11/XKBlib.h>
#include	<X11/keysym.h>

#include	<GL/gl.h>
#include	<GL/glu.h>
#include	<GL/glx.h>

// MACROS
#define		WIN_WIDTH	800
#define		WIN_HEIGHT	600
#define		ERROR_EXIT(error)	{						\
						fprintf(stderr, "ERROR: %s\n", error);	\
						Uninitialize();				\
						exit(1);				\
					}

// GLOBAL VARIABLES
bool		bFullscreen = false;
Display		*gpDisplay = NULL;
XVisualInfo	*gpXVisualInfo = NULL;
Window		gWindow;
Colormap	gColormap;
GLXContext	gGLXContext;

bool	bLight = false;
GLfloat	light_position[] = { 0.0f, 0.0f, 10.0f, 1.0f};
GLfloat	light_diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f};
GLfloat	light_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f};

// FUNCTIONS

int main(int argc, char **argv, char **envp) {

	// FUNCTIONS
	void Draw(void);
	void Initialize(void);
	void Resize(int, int);
	void CreateWindow(void);
	void Uninitialize(void);
	void ToggleFullscreen(void);

	// VARIABLES
	int	win_width = 0;
	int	win_height = 0;
	KeySym	keysym;
	XEvent	event;
	bool	bDone = false;

	// CODE

	CreateWindow();

	Initialize();

	while( !bDone ) {
		while(XPending(gpDisplay) ) {
			XNextEvent(gpDisplay, &event);

			switch( event.type ) {
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
							ToggleFullscreen();

							bFullscreen = !bFullscreen;
							break;

						case XK_L:
						case XK_l:
							if( bLight )
								glDisable(GL_LIGHTING);
							else
								glEnable(GL_LIGHTING);
							bLight = !bLight;
						}
					break;

				case ButtonPress:
					break;

				case ConfigureNotify:
					win_width = event.xconfigure.width;
					win_height = event.xconfigure.height;
					Resize(win_width, win_height);
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

void CreateWindow(void) {

	// FUNCTION
	void Uninitialize(void);

	// VARIABLE
	XSetWindowAttributes	winAttribs;
	int	defaultScreen;
	int	styleMask;
	static	int	frameBufferAttributes[] = { 	GLX_DOUBLEBUFFER,
							GLX_RGBA,
							GLX_RED_SIZE,	8,
							GLX_GREEN_SIZE,	8,
							GLX_BLUE_SIZE,	8,
							GLX_ALPHA_SIZE,	8,
							GLX_DEPTH_SIZE,	24,
						};

	// CODE

	gpDisplay = XOpenDisplay(NULL);
	if ( NULL == gpDisplay)
		ERROR_EXIT("Cannot open valid Display");

	defaultScreen = XDefaultScreen(gpDisplay);

	gpXVisualInfo = glXChooseVisual(gpDisplay, defaultScreen, frameBufferAttributes);
	if( NULL == gpXVisualInfo)
		ERROR_EXIT("Cannot find valid visual");

	winAttribs.border_pixel = 0;
	winAttribs.background_pixmap = 0;
	winAttribs.background_pixel = BlackPixel(gpDisplay,
						 defaultScreen);

	winAttribs.colormap = XCreateColormap(	gpDisplay,
						RootWindow(gpDisplay, gpXVisualInfo->screen),
						gpXVisualInfo->visual,
						AllocNone);
	winAttribs.event_mask = ExposureMask | StructureNotifyMask | VisibilityChangeMask | PointerMotionMask | ButtonPressMask | KeyPressMask;

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

	if( 0 == gWindow )
		ERROR_EXIT("Cannot create window");

	XStoreName(gpDisplay, gWindow, "Cube");

	Atom windowDeleteManager = XInternAtom(gpDisplay, "WM_DELETE_MANAGER", True);
	XSetWMProtocols(gpDisplay, gWindow, &windowDeleteManager, 1);

	XMapWindow(gpDisplay, gWindow);
}

void Initialize(void) {

	// FUNCTION
	void Resize(int, int);

	// CODE

	gGLXContext = glXCreateContext(gpDisplay, gpXVisualInfo, NULL, True);

	glXMakeCurrent(gpDisplay, gWindow, gGLXContext);

	glDepthFunc(GL_LEQUAL);

	glEnable(GL_DEPTH_TEST);

	glClearDepth(1.0f);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glShadeModel(GL_SMOOTH);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

	glEnable(GL_LIGHT0);

	Resize(WIN_WIDTH, WIN_HEIGHT);
}

void ToggleFullscreen(void) {

	// VARIABLE
	Atom	wm_state = XInternAtom(gpDisplay, "_NET_WM_STATE", False);
	Atom	fullscreen = XInternAtom(gpDisplay, "_NET_WM_STATE_FULLSCREEN", False);
	XEvent 	event = {0};

	// CODE
	event.type = ClientMessage;
	event.xclient.message_type = wm_state;
	event.xclient.window = gWindow;
	event.xclient.format = 32;
	event.xclient.data.l[0] = bFullscreen ? 0 : 1;
	event.xclient.data.l[1] = fullscreen;

	XSendEvent(	gpDisplay,
			RootWindow(gpDisplay, gpXVisualInfo->screen),
			False,
			StructureNotifyMask,
			&event);
}

void Resize(int width, int height) {

	// CODE

	if( height < 1)
		height = 1;

	glViewport( 0, 0, (GLsizei) width, (GLsizei) height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 1.0f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Draw(void) {

	// VARIABLE
	static float spin = 0.0f;

	// CODE
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	gluLookAt( 0.0f, 0.0f, 20.0f,
		   0.0f, 0.0f, 0.0f,
		   0.0f, 1.0f, 0.0f);

	glRotatef( spin, 1.0f, 1.0f, 1.0f);

	glBegin(GL_QUADS);
	{
		// FRONT FACE
		glNormal3f( 0.0f, 0.0f, 1.0f);
		glVertex3f(-3.0f, 3.0f, 3.0f);
		glVertex3f(-3.0f,-3.0f, 3.0f);
		glVertex3f( 3.0f,-3.0f, 3.0f);
		glVertex3f( 3.0f, 3.0f, 3.0f);

		// RIGHT FACE
		glNormal3f( 1.0f, 0.0f, 0.0f);
		glVertex3f( 3.0f, 3.0f, 3.0f);
		glVertex3f( 3.0f,-3.0f, 3.0f);
		glVertex3f( 3.0f,-3.0f,-3.0f);
		glVertex3f( 3.0f, 3.0f,-3.0f);

		// BACK FACE
		glNormal3f( 0.0f, 0.0f,-1.0f);
		glVertex3f( 3.0f, 3.0f,-3.0f);
		glVertex3f( 3.0f,-3.0f,-3.0f);
		glVertex3f(-3.0f,-3.0f,-3.0f);
		glVertex3f(-3.0f, 3.0f,-3.0f);

		// LEFT FACE
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glVertex3f(-3.0f, 3.0f,-3.0f);
		glVertex3f(-3.0f,-3.0f,-3.0f);
		glVertex3f(-3.0f,-3.0f, 3.0f);
		glVertex3f(-3.0f, 3.0f, 3.0f);

		// TOP FACE
		glNormal3f( 0.0f, 1.0f, 0.0f);
		glVertex3f( 3.0f, 3.0f, 3.0f);
		glVertex3f( 3.0f, 3.0f,-3.0f);
		glVertex3f(-3.0f, 3.0f,-3.0f);
		glVertex3f(-3.0f, 3.0f, 3.0f);

		//  BASE FACE
		glNormal3f( 0.0f,-1.0f, 0.0f);
		glVertex3f( 3.0f,-3.0f, 3.0f);
		glVertex3f( 3.0f,-3.0f,-3.0f);
		glVertex3f(-3.0f,-3.0f,-3.0f);
		glVertex3f(-3.0f,-3.0f, 3.0f);


	}
	glEnd();

	spin += 1.0f;

	if( spin >= 360.0f)
		spin = 0.0f;

	glXSwapBuffers(gpDisplay, gWindow);
}

void Uninitialize(void) {

	// CODE

	if( glXGetCurrentContext() == gGLXContext) {
		glXMakeCurrent(gpDisplay, 0, 0);
	}

	if( gGLXContext ) {
		glXDestroyContext(gpDisplay, gGLXContext);
	}

	if( gColormap ) {
		XFreeColormap(gpDisplay, gColormap);
	}

	if( gWindow ) {
		XDestroyWindow( gpDisplay, gWindow );
	}

	if( gpXVisualInfo ) {
		free(gpXVisualInfo);
		gpXVisualInfo = NULL;
	}

	if( gpDisplay ) {
		XCloseDisplay(gpDisplay);
		gpDisplay = NULL;
	}
}
