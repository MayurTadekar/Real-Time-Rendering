

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

#define		_USE_MATH_DEFINES
#include	<cmath>

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

GLUquadric* mst_Quadric = NULL;

int axis = 1;
int iAnimate = 0;
int iLight = 0;

GLfloat		light_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f};
GLfloat		light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f};
GLfloat		light_specular[] = { 0.1f, 0.1f, 0.1f, 0.1f};
GLfloat		light_position[] = { 0.0f, 0.0f, 0.0f, 1.0f};

GLfloat materialAmbient[] = {	0.0215,0.1745,0.0215,1.0f,
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

GLfloat materialDiffuse[] = { 	0.07568,0.61424,0.07568,1.0f,
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

GLfloat materialSpecular[] = { 	0.633,0.727811,0.633,1.0f,
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

GLfloat materialShininess[] = 	{	0.6 * 128,
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

						case XK_X:
						case XK_x:
							axis = 1;
							break;

						case XK_Y:
						case XK_y:
							axis = 2;
							break;

						case XK_Z:
						case XK_z:
							axis = 3;
							break;

						case XK_a:
						case XK_A:
							iAnimate = !iAnimate;
							break;

						case XK_l:
						case XK_L:
							iLight = !iLight;
							if(iLight)
								glEnable(GL_LIGHTING);
							else
								glDisable(GL_LIGHTING);
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

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    mst_Quadric = gluNewQuadric();

	glLightfv( GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv( GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv( GL_LIGHT0, GL_SPECULAR, light_specular);

	glEnable(GL_LIGHT0);
	// glEnable(GL_LIGHTING);

	Resize(WIN_WIDTH, WIN_HEIGHT);
}



void Resize(int width, int height) {

	if( height < 1)
		height = 1;

	glViewport(0, 0, (GLsizei) width, (GLsizei)height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Maintain aspect ratio so spheres look spherical
    GLfloat orthoSize = 100.0f;
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    if(width >= height)
    {
        glOrtho(-orthoSize * aspect, orthoSize * aspect, -orthoSize, orthoSize, -orthoSize, orthoSize);
    }
    else
    {
        glOrtho(-orthoSize, orthoSize, -orthoSize / aspect, orthoSize / aspect, -orthoSize, orthoSize);
    }

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Draw(void) {

	// VARIABLE
	static float index = 0.0f;
	float angle = 0.0f;

	// CODE
	angle = 2.0f * M_PI * index / 72;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	gluLookAt(	60.0f, 35.0f, 1.0f,
				60.0f, 35.0f, 0.0f,
				0.0f, 1.0f, 0.0f);

	switch(axis)
	{
		case 1:
			light_position[0] = 75.0f;
			light_position[1] = 150.0f * cosf(angle) + 4.0f;
			light_position[2] = 150.0f * sinf(angle);
			light_position[3] = 1.0f;
			break;

		case 2:
			light_position[0] = 150.0f * cosf(angle) + 4.0f;
			light_position[1] = 0.0f;
			light_position[2] = 150.0f * sinf(angle);
			light_position[3] = 1.0f;
			break;

		case 3:
			light_position[0] = 150.0f * sinf(angle);
			light_position[1] = 150.0f * cosf(angle) + 4.0f;
			light_position[2] = 0.0f;
			light_position[3] = 1.0f;
			break;
	}

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	int materialIndex = 0;
	for( int ole = 0 ; ole < 6; ole++)
	{
		for( int ile = 0 ; ile < 4; ile++)
		{
			glPushMatrix();
			{
				glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, &materialSpecular[4 * materialIndex] );
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, &materialAmbient[4 * materialIndex] );
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, &materialDiffuse[4 * materialIndex] );
				glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, materialShininess[materialIndex] );

				glTranslatef(ole * 25.0f, ile * 25.0f, 0.0f);

				/* SPHERE */
				gluSphere(mst_Quadric, 10.0f, 50, 50);
				materialIndex++;
			}
			glPopMatrix();
		}
	}

	if(iAnimate)
		index += 1.0f;

	if( index >= 72.0f )
		index = 0.0f;

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
