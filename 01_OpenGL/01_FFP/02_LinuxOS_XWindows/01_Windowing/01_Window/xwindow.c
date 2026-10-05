/* S.G.M.P */


// Standard Header Files
#include <stdio.h>	// for printf()
#include <stdlib.h>	// for exit()
#include <memory.h>	// for memset()

// XLib Header Files
#include <X11/Xlib.h>	// analogous to windows.h
#include <X11/Xutil.h>	//	

// Marcos
#define WIN_WIDTH	800
#define	WIN_HEIGHT	600

// Global Variables
Display *gpDisplay = NULL;
XVisualInfo visualInfo;
Window	window;
Colormap colormap;

// Entry Point Function
int main(void)
{
	// Function Declarations
	void uninitialize(void);

	// Local Variables
	int defaultScreen;
	int defaultDepth;
	Status status = 0;
	XSetWindowAttributes windowAttributes;
	Atom windowManagerDeleteAtom;
	XEvent event;

	// Code
	gpDisplay = XOpenDisplay(NULL);	// NULL ==> Display's Name
	if(NULL == gpDisplay)	// Error Checking
	{
		printf("ERROR: XOpenDisplay() Failed to connect with XServer\n");
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
		printf("ERROR: XCreateWindow() Failed\n");
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

	// Message Loop
	while(1)
	{
		XNextEvent(gpDisplay, &event);
		
		switch(event.type)
		{
			case 33:
				uninitialize();
				exit(EXIT_SUCCESS);
		
			default:
				break;
		}
	}

	uninitialize();
	return(0);
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
}


