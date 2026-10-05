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

// Marcos
#define WIN_WIDTH	800
#define	WIN_HEIGHT	600

#define WINDOW_TITLE	"Mayur S Tadekar - XWindow - 10_Patterns"

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
	// Variable Definition
	GLfloat x_center = 0.3f * 3.0f / 2.0f;
	GLfloat y_center = 0.3f * 3.0f / 2.0f;

	// Code
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	//	रांगोळी चे टिपके 
	glLoadIdentity();
	glTranslatef(-2.5f, 1.0f, -6.0f);
	glBegin(GL_POINTS);
	{
		for(int ole = 0; ole < 4; ++ole)
		{
			for(int ile = 0; ile < 4; ++ile)
			{
				glVertex3f(	0.3f * ile - x_center, 
							0.3f * ole - y_center, 
							0.0f);
			}
		}
	}
	glEnd();

	//	Lines ची रांगोळी  
	glLoadIdentity();
	glTranslatef(0.0f, 1.0f, -6.0f);
	
	for(int ole = 0; ole < 3; ++ole)
	{
		glBegin(GL_LINE_STRIP);
		{		
			for(int ile = 0; ile < 4; ++ile)
			{
				glVertex3f(	0.3f * ile - x_center, 
							-0.3f * ole + y_center, 
							0.0f);

				if(ile != 3 && (ole+1) != 4)
					glVertex3f(	0.3f * ile - x_center, 
								-0.3f * (ole+1) + y_center, 
								0.0f);
			}
		}
		glEnd();

		glBegin(GL_LINE_STRIP);
		{		
			for(int ile = 0; ile < 4; ++ile)
			{
				glVertex3f(	0.3f * ile - x_center, 
							-0.3f * ole + y_center, 
							0.0f);
			}
		}
		glEnd();
	}

	//	Lines ने बनलेले box रांगोळी  
	glLoadIdentity();
	glTranslatef(2.5f, 1.0f, -6.0f);
	
	for(int ole = 0; ole < 4; ++ole)
	{
		glBegin(GL_LINE_STRIP);
		{		
			for(int ile = 0; ile < 4; ++ile)
			{
				glVertex3f(	0.3f * ile - x_center, 
							-0.3f * ole + y_center, 
							0.0f);
			}
		}
		glEnd();

		glBegin(GL_LINE_STRIP);
		{		
			for(int ile = 0; ile < 4; ++ile)
			{
				glVertex3f(	0.3f * ole - x_center, 
							-0.3f * ile + y_center, 
							0.0f);
			}
		}
		glEnd();
	}

	//	Lines + Box ची रांगोळी  
	glLoadIdentity();
	glTranslatef(-2.5f, -1.0f, -6.0f);
	
	for(int ole = 0; ole < 4; ++ole)
	{
		if(ole < 3)
		{
			glBegin(GL_LINE_STRIP);
			{		
				for(int ile = 0; ile < 4; ++ile)
				{
					glVertex3f(	0.3f * ile - x_center, 
								-0.3f * ole + y_center, 
								0.0f);

					glVertex3f(	0.3f * ile - x_center, 
								-0.3f * (ole+1) + y_center, 
								0.0f);
				}
			}
			glEnd();
		}

		glBegin(GL_LINE_STRIP);
		{		
			for(int ile = 0; ile < 4; ++ile)
			{
				glVertex3f(	0.3f * ile - x_center, 
							-0.3f * ole + y_center, 
							0.0f);
			}
		}
		glEnd();
	}


	//	Lines Connection ची रांगोळी 
	glLoadIdentity();
	glTranslatef(0.0f, -1.0f, -6.0f);
	glBegin(GL_LINES);
	{
		// Line 01
		glVertex3f(	0.0f - x_center,
					0.0f + y_center,
					0.0f);

		glVertex3f(	0.9f - x_center,
					0.0f + y_center,
					0.0f);

		// Line 02
		glVertex3f(	0.0f - x_center,
					0.0f + y_center,
					0.0f);

		glVertex3f(	0.9f - x_center,
					-0.3f + y_center,
					0.0f);

		// Line 03
		glVertex3f(	0.0f - x_center,
					0.0f + y_center,
					0.0f);

		glVertex3f(	0.9f - x_center,
					-0.6f + y_center,
					0.0f);

		// Line 04
		glVertex3f(	0.0f - x_center,
					0.0f + y_center,
					0.0f);

		glVertex3f(	0.9f - x_center,
					-0.9f + y_center,
					0.0f);

		// Line 05
		glVertex3f(	0.0f - x_center,
					0.0f + y_center,
					0.0f);

		glVertex3f(	0.6f - x_center,
					-0.9f + y_center,
					0.0f);

		// Line 06
		glVertex3f(	0.0f - x_center,
					0.0f + y_center,
					0.0f);

		glVertex3f(	0.3f - x_center,
					-0.9f + y_center,
					0.0f);

		// Line 07
		glVertex3f(	0.0f - x_center,
					0.0f + y_center,
					0.0f);

		glVertex3f(	0.0f - x_center,
					-0.9f + y_center,
					0.0f);

		// Line 08
		glVertex3f(	0.9f - x_center,
					0.0f + y_center,
					0.0f);

		glVertex3f(	0.9f - x_center,
					-0.9f + y_center,
					0.0f);

		// Line 08
		glVertex3f(	0.0f - x_center,
					-0.9f + y_center,
					0.0f);

		glVertex3f(	0.9f - x_center,
					-0.9f + y_center,
					0.0f);
	}
	glEnd();

	//	Lines ने बनलेले box रांगोळी and Color 
	glLoadIdentity();
	glTranslatef(2.5f, -1.0f, -6.0f);

	for(int ole = 0; ole < 4; ++ole)
	{
		glBegin(GL_TRIANGLE_STRIP);
		{		
			if(ole < 3)
			{
				for(int ile = 0; ile < 4; ++ile)
				{
					switch(ole)
					{
					case 2:
						glColor3f(1.0f, 0.0f, 0.0f);
						break;

					case 1:
						glColor3f(0.0f, 1.0f, 0.0f);
						break;

					case 0:
						glColor3f(0.0f, 0.0f, 1.0f);
						break;
					}

					glVertex3f(	-0.3f * ole + y_center, 
								0.3f * ile - x_center, 
								0.0f);

					glVertex3f(	-0.3f * (ole+1) + y_center, 
								0.3f * ile - x_center, 
								0.0f);
				}
			}
				
			glColor3f(1.0f, 1.0f, 1.0f);			
		}
		glEnd();
	}
	
	for(int ole = 0; ole < 4; ++ole)
	{
		glBegin(GL_LINE_STRIP);
		{		
			for(int ile = 0; ile < 4; ++ile)
			{
				glVertex3f(	0.3f * ile - x_center, 
							-0.3f * ole + y_center, 
							0.0f);
			}
		}
		glEnd();

		glBegin(GL_LINE_STRIP);
		{		
			for(int ile = 0; ile < 4; ++ile)
			{
				glVertex3f(	0.3f * ole - x_center, 
							-0.3f * ile + y_center, 
							0.0f);
			}
		}
		glEnd();
	}

	glXSwapBuffers(gpDisplay, window);
}

void update(void)
{
	// Code
	// Currently, no updates are required
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



