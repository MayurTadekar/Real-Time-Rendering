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

// Marcos
#define WIN_WIDTH	800
#define	WIN_HEIGHT	600

// Global Variables
Display *gpDisplay = NULL;
XVisualInfo visualInfo;
Window	window;
Colormap colormap;
Screen *screen = NULL;
int screen_width, screen_height;
Bool bFullscreen = False;
FILE *gpFile = NULL;

// Entry Point Function
int main(void)
{
	// Function Declarations
	void toggleFullscreen(void);
	void uninitialize(void);


	// Local Variables
	int defaultScreen;
	int defaultDepth;
	Status status = 0;
	XSetWindowAttributes windowAttributes;
	Atom windowManagerDeleteAtom;
	XEvent event;
	KeySym keysym;
	char keys[26];	// though we need only 0th index,  Conventionally, the Array size is equal to number of alphabets ie, either 26 or 52

	// Code

	if( NULL != (gpFile = fopen("logfile.txt", "w")))
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
	memset((void*)&visualInfo, 0, sizeof(XVisualInfo));
	status = XMatchVisualInfo(gpDisplay, defaultScreen, defaultDepth, TrueColor, &visualInfo);
	if(0 == status)
	{
		printf("ERROR: XMatchVisualInfo() Failed\n");
		uninitialize();
		exit(EXIT_FAILURE);
	}

	// Set Window Attributes
	memset((void*)&windowAttributes, 0, sizeof(XSetWindowAttributes));
	windowAttributes.border_pixel = 0;
	windowAttributes.background_pixmap = 0;
	windowAttributes.background_pixel = XBlackPixel(gpDisplay, visualInfo.screen);	//defaultScreen | visualInfo.screen
	windowAttributes.colormap = XCreateColormap(gpDisplay,
						    XRootWindow(gpDisplay, visualInfo.screen),
						    visualInfo.visual,
						    AllocNone);
	windowAttributes.event_mask = KeyPressMask | ButtonPressMask | FocusChangeMask | StructureNotifyMask | ExposureMask; 
	
	// Create Window
	window = XCreateWindow(	gpDisplay,
				XRootWindow(gpDisplay, visualInfo.screen),
				0,
				0,
				WIN_WIDTH,
				WIN_HEIGHT,
				0,
				visualInfo.depth,
				InputOutput,
				visualInfo.visual,
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
	XStoreName(gpDisplay, window, "Mayur S Tadekar - XWindow - First Window");

	// Map the window to show it
	XMapWindow(gpDisplay, window);

	// Centering of Window
	screen = XScreenOfDisplay(gpDisplay, defaultScreen);
	screen_width = XWidthOfScreen(screen);
	screen_height = XHeightOfScreen(screen);

	// Changing the x and y in create window (like we did in win32) because the initial position of the window is governed by window manager. So let the window manager decide the position, then later we will center it by using XMoveWindow() Xlib API
	XMoveWindow(gpDisplay, 
		    window,
		    (screen_width/2) - (WIN_WIDTH/2),
		    (screen_height/2) - (WIN_HEIGHT/2));

	// Message Loop
	while(1)
	{
		XNextEvent(gpDisplay, &event);
		
		switch(event.type)
		{
			case 33:
				uninitialize();
				exit(EXIT_SUCCESS);
		
			case MapNotify:
				break;

			case FocusIn:

				break;

			case FocusOut:
				break;

			case ConfigureNotify:
				break;

			case KeyPress:

				// For KeyCode KeyPress
				keysym = XkbKeycodeToKeysym(gpDisplay, event.xkey.keycode, 0, 0);

				switch(keysym)
				{
					case XK_Escape:
						uninitialize();
						exit(EXIT_SUCCESS);			
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

	uninitialize();

	return(0);
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
		   RootWindow(gpDisplay, visualInfo.screen),
		   False,
		   SubstructureNotifyMask,
		   &event);
}

void uninitialize(void)
{
	// Code

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



