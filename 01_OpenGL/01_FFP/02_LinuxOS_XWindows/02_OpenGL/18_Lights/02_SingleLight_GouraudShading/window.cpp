

// HEADERS

#include	<iostream>
#include	<stdio.h>
#include	<stdlib.h>
#include	<memory.h>

#include	<X11/Xlib.h>
#include	<X11/Xutil.h>
#include	<X11/XKBlib.h>
#include	<X11/keysym.h>

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

GLfloat		material_ambient[] = 	{ 0.1f, 0.1f, 0.1f, 1.0f};
GLfloat		material_specular[] = 	{ 1.0f, 1.0f, 1.0f, 1.0f};
GLfloat		material_diffuse[] = 	{ 1.0f, 1.0f, 1.0f, 1.0f};
GLfloat		material_shininess = 128.0f;


GLfloat		light_specular[] = 	{ 0.1f, 0.1f, 0.1f, 1.0f};
GLfloat		light_ambient[] = 	{ 1.0f, 1.0f, 1.0f, 1.0f};
GLfloat		light_diffuse[] = 	{ 1.0f, 1.0f, 1.0f, 1.0f};
GLfloat		light_position[] = 	{ 100.0f, 100.0f, 100.0f, 1.0f};

GLUquadric* mst_Quadric = NULL;
bool bLight = false;

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
							
						case XK_L:
						case XK_l:
						        if(true == bLight)
						        	glDisable(GL_LIGHTING);
						        else
						                glEnable(GL_LIGHTING);
						        bLight != bLight;
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

	static int frameBufferAttributes[] = { 	GLX_DOUBLEBUFFER, True,
						GLX_RGBA,
						GLX_RED_SIZE, 	8,
						GLX_GREEN_SIZE, 8,
						GLX_BLUE_SIZE, 	8,
						GLX_ALPHA_SIZE, 8,
						GLX_DEPTH_SIZE, 24,
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

	glEnable(GL_DEPTH);

	glClearDepth(1.0f);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_diffuse);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material_shininess);

	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glEnable(GL_LIGHT0);


        mst_Quadric = gluNewQuadric();

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

	glLoadIdentity();

	gluLookAt( 	0.0f, 0.0f, 5.0f,
			0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f);

	glPushMatrix();
	{
		glRotatef(spin, 0.0f, 1.0f, 1.0f);
		// glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		/* SUN */
		gluSphere(mst_Quadric, 0.75f, 20, 20);
	}
	glPopMatrix();

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
