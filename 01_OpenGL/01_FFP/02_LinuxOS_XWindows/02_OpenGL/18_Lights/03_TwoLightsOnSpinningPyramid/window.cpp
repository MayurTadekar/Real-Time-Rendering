


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
#include	<GL/glu.h>
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

GLfloat		light_1_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f};
GLfloat		light_1_diffuse[] = { 1.0f, 0.0f, 0.0f, 1.0f};
GLfloat		light_1_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f};
GLfloat		light_1_position[] = { 10.0f, 0.0f, 0.0f, 1.0f};

GLfloat		light_2_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f};
GLfloat		light_2_diffuse[] = { 0.0f, 0.0f, 1.0f, 1.0f};
GLfloat		light_2_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f};
GLfloat		light_2_position[] = { -10.0f, 0.0f, 0.0f, 1.0f};

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
	bool	bLight = false;

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
						      if(bLight)
						          glDisable(GL_LIGHTING);
						      else
						          glEnable(GL_LIGHTING);
						      bLight = !bLight;
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
	int		defaultScreen;
	int		styleMask;
	Status		status;

	static int frameBufferAttributes[] = {
                        GLX_DOUBLEBUFFER, True,
                        GLX_RGBA,
						GLX_RED_SIZE, 	8,
						GLX_GREEN_SIZE, 8,
						GLX_BLUE_SIZE, 	8,
						GLX_ALPHA_SIZE, 8,
						GLX_DEPTH_SIZE, 24,
						None
					 };

	// CODE

	gpDisplay = XOpenDisplay(NULL);
	if( NULL == gpDisplay)
		EXIT_ERROR("Display cannot be opened");

	defaultScreen = XDefaultScreen(gpDisplay);
/*
	gpXVisualInfo = (XVisualInfo*) malloc(sizeof(XVisualInfo));
	if( NULL == gpXVisualInfo)
		EXIT_ERROR("Memory cannot be allocated to XVisualInfo");*/

	gpXVisualInfo = glXChooseVisual(gpDisplay, defaultScreen, frameBufferAttributes);
	if( NULL == gpXVisualInfo)
		EXIT_ERROR("Memory cannot be allocated to XVisualInfo");

	/*status = XMatchVisualInfo(gpDisplay, defaultScreen, defaultDepth, TrueColor, gpXVisualInfo);
	if( 0 == status)
		EXIT_ERROR("Suitable Match cannot be found of XVisualInfo");
*/
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

	gGLXContext = glXCreateContext( gpDisplay, gpXVisualInfo, NULL, GL_TRUE);

	glXMakeCurrent(gpDisplay, gWindow, gGLXContext);

	glShadeModel(GL_SMOOTH);

	glDepthFunc(GL_LEQUAL);

	glEnable(GL_DEPTH_TEST);

	glClearDepth(1.0f);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_1_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_1_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_1_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_1_position);

	glLightfv(GL_LIGHT1, GL_AMBIENT, light_2_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_2_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_2_specular);
	glLightfv(GL_LIGHT1, GL_POSITION, light_2_position);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	glEnable(GL_LIGHTING);

	Resize(WIN_WIDTH, WIN_HEIGHT);
}



void Resize(int width, int height) {

	if( height < 1)
		height = 1;


	glViewport(0, 0, (GLsizei)width, (GLsizei) height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective( 45.0f, (GLfloat)width/(GLfloat)height, 1.0f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Draw(void) {

	// VARIABLE
	static float spin = 0.0f;

	// CODE
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
  
	glTranslatef(0.0f, 0.0f, -5.0f);
	glRotatef(spin, 0.0f, 1.0f, 0.0f);

	glBegin(GL_TRIANGLES);
	{
		// FRONT
                glNormal3f(0.0f, 0.447f, 0.894f);
		glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-1.0f, -1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, 1.0f);

		// RIGHT
		glNormal3f(0.894f, 0.447f, 0.0f);
		glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f,-1.0f);

		// BACK
		glNormal3f(0.0f, 0.447f, -0.894f);
		glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(1.0f, -1.0f,-1.0f);
		glVertex3f(-1.0f, -1.0f,-1.0f);

		// LEFT
		glNormal3f(-0.894f, 0.447f, 0.0f);
		glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-1.0f, -1.0f,-1.0f);
		glVertex3f(-1.0f, -1.0f, 1.0f);

	}
	glEnd();

	spin += 1.0f;

	if( spin > 360.0f)
		spin = 0.0f;

	glXSwapBuffers(gpDisplay, gWindow);
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
