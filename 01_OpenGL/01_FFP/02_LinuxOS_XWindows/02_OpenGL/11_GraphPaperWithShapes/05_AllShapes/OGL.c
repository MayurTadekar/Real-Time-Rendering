/* S.G.M.P */


// Standard Header Files
#include <stdio.h>	// for printf()
#include <stdlib.h>	// for exit()
#include <memory.h>	// for memset()

// XLib Header Files
#include <X11/Xlib.h>	// analogous to windows.h
#include <X11/Xutil.h>	// for XVisualInfo	
#include <X11/XKBlib.h>	// 
#include <X11/keysym.h>	// 

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#define _USE_MATH_DEFINES	// for M_PI
#include <math.h>		// for M_PI

// Marcos
#define WIN_WIDTH	800
#define	WIN_HEIGHT	600

#define WINDOW_TITLE	"Mayur S Tadekar - XWindow - 11_GraphPaperWithShapes - 05_AllShapes"

// Global Variables
Display *gpDisplay = NULL;
XVisualInfo *visualInfo;
Window	window;
Colormap colormap;
Screen *screen = NULL;
int screen_width, screen_height;
FILE *gpFile = NULL;

Bool bFullscreen = False;
Bool bActiveWindow = False;

// OpenGL Related Functions
GLXContext glXContext = NULL;

int screenWidth = 0;
int screenHeight = 0;

Bool graphShapeToggle = True;	
Bool triangleShapeToggle = False;
Bool squareShapeToggle = False;
Bool circleShapeToggle = False;

// Entry Point Function
int main(void)
{
	// Function Declarations
	int initialize(void);
	void toggleFullscreen(void);
	void uninitialize(void);
	void display(void);
	void update(void);
	void resize(int, int);


	// Local Variables
	int defaultScreen;
	int defaultDepth;
	XSetWindowAttributes windowAttributes;
	Atom windowManagerDeleteAtom;
	XEvent event;
	KeySym keysym;
	char keys[26];	// though we need only 0th index,  Conventionally, the Array size is equal to number of alphabets ie, either 26 or 52
	Bool bDone = False;
	int frameBufferAttributes[] = 
	{
		GLX_DOUBLEBUFFER,
		GLX_RGBA,
		GLX_RED_SIZE, 8,
		GLX_GREEN_SIZE, 8,
		GLX_BLUE_SIZE, 8,
		GLX_ALPHA_SIZE, 8,
		None
	};

	// Code

	if( NULL != (gpFile = fopen("log.txt", "w")))
      	{
        	fprintf(gpFile, "window started successfully\n");
      	}
      	else
      	{
        	printf("Logfile cannot be created\n");
          	exit(0);
      	}

	gpDisplay = XOpenDisplay(NULL);	// NULL ==> Display's Name
	if(NULL == gpDisplay)	// Error Checking
	{
		fprintf(gpFile, "ERROR: XOpenDisplay() Failed to connect with XServer\n");
		uninitialize();
		exit(EXIT_FAILURE);		
	}
	
	// Create the Default Screen Object
	defaultScreen =  XDefaultScreen(gpDisplay);	

	// Get Default Depth
	defaultDepth = XDefaultDepth(gpDisplay, defaultScreen);

	// Get VisualInfo
	// memset((void*)&visualInfo, 0, sizeof(XVisualInfo));
	// status = XMatchVisualInfo(gpDisplay, defaultScreen, defaultDepth, TrueColor, &visualInfo);
	visualInfo = glXChooseVisual(gpDisplay, defaultScreen, frameBufferAttributes);
	if(NULL == visualInfo)
	{
		printf("ERROR: glXChooseVisual() Failed\n");
		uninitialize();
		exit(EXIT_FAILURE);
	}

	// Set Window Attributes
	memset((void*)&windowAttributes, 0, sizeof(XSetWindowAttributes));
	windowAttributes.border_pixel = 0;
	windowAttributes.background_pixmap = 0;
	windowAttributes.background_pixel = XBlackPixel(gpDisplay, visualInfo->screen);	//defaultScreen | visualInfo.screen
	windowAttributes.colormap = XCreateColormap(gpDisplay,
						    XRootWindow(gpDisplay, visualInfo->screen),
						    visualInfo->visual,
						    AllocNone);
	windowAttributes.event_mask = KeyPressMask | ButtonPressMask | FocusChangeMask | StructureNotifyMask | ExposureMask; 
	
	// Create Window
	window = XCreateWindow(	gpDisplay,
				XRootWindow(gpDisplay, visualInfo->screen),
				0,
				0,
				WIN_WIDTH,
				WIN_HEIGHT,
				0,
				visualInfo->depth,
				InputOutput,
				visualInfo->visual,
				CWBorderPixel | CWBackPixel | CWEventMask | CWColormap,
				&windowAttributes);

	if(!window)
	{
		fprintf(gpFile, "ERROR: XCreateWindow() Failed\n");
		uninitialize();
		exit(EXIT_FAILURE);
	}
	
	// Create Atom for window manager to destroy the window
	windowManagerDeleteAtom = XInternAtom(gpDisplay, "WM_DELETE_WINDOW", True);

	XSetWMProtocols(gpDisplay, window, &windowManagerDeleteAtom, 1);

	// Set Window Title
	XStoreName(gpDisplay, window, WINDOW_TITLE);

	// Map the window to show it
	XMapWindow(gpDisplay, window);

		// // Centering of Window
	// screen = XScreenOfDisplay(gpDisplay, defaultScreen);
	// screen_width = XWidthOfScreen(screen);
	// screen_height = XHeightOfScreen(screen);

	// // Changing the x and y in create window (like we did in win32) because the initial position of the window is governed by window manager. So let the window manager decide the position, then later we will center it by using XMoveWindow() Xlib API
	// XMoveWindow(gpDisplay, 
	// 	    window,
	// 	    (screen_width/2) - (WIN_WIDTH/2),
	// 	    (screen_height/2) - (WIN_HEIGHT/2));

	int iResult = initialize();
	if(0 > iResult)
	{
		uninitialize();
		exit(EXIT_FAILURE);
	}


	// Message Loop
	while(False == bDone)
	{
		while(XPending(gpDisplay))
		{
			XNextEvent(gpDisplay, &event);
		
			switch(event.type)
			{
				case 33:
					bDone = True;
					break;

				case MapNotify:
					break;
	
				case FocusIn:
					bActiveWindow = True;
					break;
	
				case FocusOut:
					bActiveWindow = False;
					break;

				case ConfigureNotify:
					screenWidth = event.xconfigure.width;
					screenHeight = event.xconfigure.height;
					resize(event.xconfigure.width, event.xconfigure.height);
					break;
	
				case KeyPress:
	
					// For KeyCode KeyPress
					keysym = XkbKeycodeToKeysym(gpDisplay, event.xkey.keycode, 0, 0);
	
					switch(keysym)
					{
						case XK_Escape:
							bDone = True;
							break;

						default:
							break;
					}
					
					// For Alphabetic String
					XLookupString(&event.xkey, keys, sizeof(keys), NULL, NULL);
	
					switch(keys[0])
					{
	
						case 'f':
						case 'F':
							if(bFullscreen == False)
							{
								toggleFullscreen();
								bFullscreen = True;
							}
							else
							{
								toggleFullscreen();
								bFullscreen = False;
							}
							break;

						case 'g':
						case 'G':
							graphShapeToggle = (True == graphShapeToggle ? False : True);
							break;

						case 't':
						case 'T':
							triangleShapeToggle = (True == triangleShapeToggle ? False : True);
							break;

						case 's':
						case 'S':
							squareShapeToggle = (True == squareShapeToggle ? False : True);
							break;

						case 'c':
						case 'C':
							circleShapeToggle = (True == circleShapeToggle ? False : True);
							break;

						default:
							break;
					}
					break;
	
				case ButtonPress:
					break;
	
				case Expose:
					break;
				
				default:
					break;
			}
		}
	
		if(True == bActiveWindow)
		{
			display();
			update();
		}
	}

	uninitialize();

	return(0);
}

int initialize(void)
{
	// Function Declaration
        void resize(int, int);

	// Code
	glXContext = glXCreateContext( gpDisplay, visualInfo, NULL, GL_TRUE);
	if(NULL == glXContext)
	{
		return(-1);
	}

	glXMakeCurrent(gpDisplay, window, glXContext);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_POINT_SMOOTH);
	glPointSize(10.0f);
	
	resize(WIN_WIDTH, WIN_HEIGHT);

	return(0);
}

void resize(int width, int height)
{
	// Code
	if(height <= 0)
		height = 1;

	glViewport(0, 0, (GLsizei) width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}

void display(void)
{
	// Function Declaration
	void drawGraph(void);
	void triangle(void);
	void square(void);
	void circle(void);

	// Code
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt(	0.0f, 0.0f, 1.0f,
				0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f);

	// Draw Graph
	if(graphShapeToggle)
		drawGraph();

	// Draw Triangle
	if(triangleShapeToggle)
		triangle();

	// Draw Square
	if(squareShapeToggle)
		square();

	// Draw Circle
	if(circleShapeToggle)
		circle();	

	glXSwapBuffers(gpDisplay, window);
}

void update(void)
{
	// Code
	// Currently, no updates are required
}

void triangle(void)
{
	// Code
	glBegin(GL_LINE_LOOP);
	{
		glColor3f(1.0f, 1.0f, 0.0f);

		glVertex3f(0.0f, 0.02f * 10, 0.0f);
		glVertex3f(0.02f * 10, -0.02f * 10, 0.0f);
		glVertex3f(-0.02f * 10, -0.02f * 10, 0.0f);
	}
	glEnd();
}

void square(void)
{
	// Code
	glBegin(GL_LINE_LOOP);
	{
		glColor3f(1.0f, 1.0f, 0.0f);

		glVertex3f(0.02f * 10, 0.02f * 10, 0.0f);
		glVertex3f(0.02f * 10, -0.02f * 10, 0.0f);
		glVertex3f(-0.02f * 10, -0.02f * 10, 0.0f);
		glVertex3f(-0.02f * 10, 0.02f * 10, 0.0f);
	}
	glEnd();
}

void circle(void)
{
	// Variable Declaration
	GLfloat radius = 0.02f * 10;

	// Code
	glBegin(GL_LINE_LOOP);
	{
		glColor3f(1.0f, 1.0f, 0.0f);

		for(int le = 0; le <= 36; ++le)
		{
			GLfloat angle = 2.0f * M_PI * le / 36.0f;

			glVertex3f( radius * sinf(angle),
						radius * cosf(angle),
						0.0f);
		}
		
	}
	glEnd();
}
void drawGraph(void)
{
	// Code

	for(int le = 1; le <= 20; ++le)
	{

		if(le%5 == 0)
			glLineWidth(3.0f);
		else
			glLineWidth(1.0f);

		glBegin(GL_LINES);
		{
			glColor3f(0.0f, 0.0f, 0.8f);

			glVertex3f(-1.0f, 0.02f * le, 0.0f);
			glVertex3f(1.0f, 0.02f * le, 0.0f);

			glVertex3f(-1.0f, -0.02f * le, 0.0f);
			glVertex3f(1.0f, -0.02f * le, 0.0f);
		}
		glEnd();
	}


	for(int le = 1; le <= 40; ++le)
	{

		if(le%5 == 0)
			glLineWidth(3.0f);
		else
			glLineWidth(1.0f);

		glBegin(GL_LINES);
		{
			glColor3f(0.0f, 0.0f, 0.8f);

			glVertex3f(0.02f * le, -1.0f, 0.0f);
			glVertex3f(0.02f * le, 1.0f, 0.0f);

			glVertex3f(-0.02f * le, -1.0f, 0.0f);
			glVertex3f(-0.02f * le, 1.0f, 0.0f);
		}
		glEnd();
	}

	glLineWidth(3.0f);
	glBegin(GL_LINES);
	{
		
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-1.0f, 0.0f, 0.0f);

		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, -1.0f, 0.0f);
	}
	glEnd();
}


void toggleFullscreen(void)
{
	// Variable Declaration
	Atom windowManagerNormalStateAtom = XInternAtom(gpDisplay, "_NET_WM_STATE", False);
	Atom windowManagerFullscreenStateAtom = XInternAtom(gpDisplay, "_NET_WM_STATE_FULLSCREEN", False);
	XEvent event;

	// Code
	fprintf(gpFile, "toggleFullscreen() called\n");
	memset((void*)&event, 0, sizeof(XEvent));
	
	event.type = ClientMessage;
	event.xclient.window = window;
	event.xclient.message_type = windowManagerNormalStateAtom;
	event.xclient.format = 32;
	event.xclient.data.l[0] = bFullscreen ? 0 : 1;
	event.xclient.data.l[1] = windowManagerFullscreenStateAtom;
	
	XSendEvent(gpDisplay,
		   RootWindow(gpDisplay, visualInfo->screen),
		   False,
		   SubstructureNotifyMask,
		   &event);
}

void uninitialize(void)
{
	// Code

	GLXContext currentContext = glXGetCurrentContext();
	if(currentContext == glXContext)
	{
		glXMakeCurrent(gpDisplay, None, NULL);
	}

	if(glXContext)
	{
		glXDestroyContext(gpDisplay, glXContext);
		glXContext = NULL;
	}

	if(visualInfo)
	{
		free(visualInfo);
		visualInfo = NULL;
	}


	if(window)
	{
		XDestroyWindow(gpDisplay, window);
	}

	if(colormap)
	{
		XFreeColormap(gpDisplay, colormap);
	}

	if(gpDisplay)
	{
		XCloseDisplay(gpDisplay);
		gpDisplay = NULL;
	}

	if(gpFile)
	{
		fprintf(gpFile, "program terminated successfully\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}



