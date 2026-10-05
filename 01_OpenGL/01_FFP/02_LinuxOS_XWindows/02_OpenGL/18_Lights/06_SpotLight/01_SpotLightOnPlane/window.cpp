

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

#define MAX_X	50
#define	DELTA_X (1.0f / 10.0f)

#define MAX_Z	50
#define	DELTA_Z (1.0f / 10.0f)

#define MAX_STRIPS	50 * 10

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

GLfloat angleCutOff = 25.0f;
GLfloat angleExponent = 20.0f;

int axis = 1;

int gbLight = 0;
int gbRotate = 0;

GLfloat lightAmbient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPosition[] = { 0.0f, 20.0f, 0.0f, 1.0f };
GLfloat lightDirection[] = { 0.0f, -1.0f, 0.0f};

GLfloat materialAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat materialDiffuse[] = { 0.5f, 0.2f, 0.7f, 1.0f };
GLfloat materialSpecular[] = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat materialShininess[] = { 128.0f };

GLfloat floorColor[MAX_STRIPS][3];

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
							if(gbLight)
							{
								gbLight = 0;
								glDisable(GL_LIGHTING);
							}
							else
							{
								gbLight = 1;
								glEnable(GL_LIGHTING);
							}
							break;

						case XK_W:
						case XK_w:
							lightPosition[2] = lightPosition[2] - 0.2f;
							break;

						case XK_S:
						case XK_s:
								lightPosition[2] = lightPosition[2] + 0.2f;
							break;

						case XK_A:
						case XK_a:
								lightPosition[0] = lightPosition[0] - 0.2f;
							break;

						case XK_D:
						case XK_d:
								lightPosition[0] = lightPosition[0] + 0.2f;
							break;

						case XK_R:
						case XK_r:
							lightPosition[0] = 0.0f;
							lightPosition[1] = 20.0f;
							lightPosition[2] = 0.0f;
							lightPosition[3] = 1.0f;
							break;

						case XK_1:
							angleCutOff -= 0.5f;
							break;

						case XK_2:
							angleCutOff += 0.5f;
							break;

						case XK_3:
							angleExponent -= 0.5f;
							break;

						case XK_4:
							angleExponent += 0.5f;
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

	for(int le = 0; le < MAX_STRIPS; ++le)
	{
		float random_r = rand() % 2000 / 2000.0f;
		float random_g = rand() % 2000 / 2000.0f;
		float random_b = rand() % 2000 / 2000.0f;

		floorColor[le][0] = random_r;
		floorColor[le][1] = random_g;
		floorColor[le][2] = random_b;
	}

	// Light Related Functions
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightDirection);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, angleCutOff);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, angleExponent);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0f);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0f);

	glEnable(GL_LIGHT0);

	Resize(WIN_WIDTH, WIN_HEIGHT);
}



void Resize(int width, int height) {

	if( height < 1)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(35.0f, (GLfloat)width / (GLfloat)height, 0.1f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Draw(void) {

	// Function Declaration
	void DrawFloor(void);

	// CODE

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	gluLookAt( 	0.0f, 17.0f, 40.0f,
				0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f);

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, angleCutOff);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, angleExponent);

	DrawFloor();

	glXSwapBuffers(gpDisplay, gWindow);
}

void DrawFloor(void)
{
	// function declartion
	void DrawQuad(float, float);

	// first strip
	glPushMatrix();
	{
		int le = 0;

		for (float MultipleX = -MAX_X/2.0f; MultipleX <= MAX_X/2.0f; MultipleX += DELTA_X)
		{
			materialDiffuse[0] = floorColor[le][0];
			materialDiffuse[1] = floorColor[le][1];
			materialDiffuse[2] = floorColor[le][2];
			materialDiffuse[3] = 1.0f;
			le++;
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuse);

			for (float MultipleZ = -MAX_Z/2.0f; MultipleZ <= MAX_Z/2.0f; MultipleZ += DELTA_Z)
			{
				DrawQuad(MultipleX, MultipleZ);
			}
		}
	}
	glPopMatrix();
}

void DrawQuad(float x_pos, float z_pos)
{
	glBegin(GL_QUADS);
	{
		glNormal3f(0.0f, 1.0f, 0.0f);

		glVertex3f(DELTA_X + x_pos, 0.0f, -DELTA_Z+ z_pos);
		glVertex3f(-DELTA_X+ x_pos, 0.0f, -DELTA_Z + z_pos);
		glVertex3f(-DELTA_X+ x_pos, 0.0f, DELTA_Z + z_pos);
		glVertex3f(DELTA_X+ x_pos, 0.0f, DELTA_Z + z_pos);
	}
	glEnd();
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
