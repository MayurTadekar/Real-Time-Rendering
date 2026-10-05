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

// Audio Headers
#include <AL/al.h>
#include <AL/alc.h>
#include <mpg123.h>

#define _USE_MATH_DEFINES
#include <math.h>	// for M_PI

// Marcos
#define WIN_WIDTH	800
#define	WIN_HEIGHT	600

#define WINDOW_TITLE	"Mayur S Tadekar - XWindow - 12_DynamicDeathlyHallows"

#define VFP(complete, percent) (complete * percent / 100.0)

#define CENTROID_X (((0.02f * 20) + (-0.02f * 20)) / 3)
#define CENTROID_Y (((0.02f * 15) + (-0.02f * 15) + (-0.02f * 15)) / 3)

// Audio settings
#define MP3_FILE_PATH "deathly_hallows.mp3"  // Change to your MP3 file path


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

GLfloat fCloackAndStoneAndWandSpinning = 0.0f;

GLfloat fCloackTranslateFactor = 100.0f;
GLfloat fStoneTranslateFactor = 100.0f;
GLfloat fWandTranslateFactor = 100.0f;

int iDeathlyHollowsAnimateFlag = 0;
Bool startAnimation = False; 

// OpenAL Audio Variables
ALCdevice* audioDevice = NULL;
ALCcontext* audioContext = NULL;
ALuint audioBuffer = 0;
ALuint audioSource = 0;
Bool isAudioInitialized = False;
Bool isAudioPlaying = False;


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
	void playAudio(void);

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

						case 'p':
						case 'P':
							if (startAnimation == False)
							{
								startAnimation = True;
								fprintf(gpFile, "Animation Started\n");
							}
							iDeathlyHollowsAnimateFlag = 1;
							fprintf(gpFile, "Animation Started\n");

							// Toggle Audio
							playAudio();
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
	Bool initAudio(void);
	Bool loadMP3File(const char* filePath);

	// Code
	glXContext = glXCreateContext( gpDisplay, visualInfo, NULL, GL_TRUE);
	if(NULL == glXContext)
	{
		return(-1);
	}

	glXMakeCurrent(gpDisplay, window, glXContext);

	// Initialize audio system
    if (initAudio())
    {
        // Load MP3 file
        if (!loadMP3File(MP3_FILE_PATH))
        {
            fprintf(gpFile, "Failed to load MP3 file: %s\n", MP3_FILE_PATH);
        }
        else
        {
            fprintf(gpFile, "Successfully loaded MP3 file: %s\n", MP3_FILE_PATH);
        }
    }

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	resize(WIN_WIDTH, WIN_HEIGHT);

	return(0);
}

void resize(int width, int height)
{
	// Code
	if(height <= 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}

void display(void)
{
	// Function Declaration
	void DrawInvisibilityCloak(void);
	void DrawElderWand(void);
	void DrawResurrectionStone(void);

	// Code
	glClear(GL_COLOR_BUFFER_BIT);

	glLoadIdentity();

	gluLookAt(0.0f, 0.0f, 1.0f,
			  0.0f, 0.0f, 0.0f,
			  0.0f, 1.0f, 0.0f);

	// Draw InvisibilityCloak
	DrawInvisibilityCloak();

	// Draw ElderWand
	DrawElderWand();

	// Draw ResurrectionStone
	DrawResurrectionStone();

	glXSwapBuffers(gpDisplay, window);
}

void update(void)
{
	// Code

	if( startAnimation == False)
	{
		return;
	}

	static Bool isAnimating = True;

	if (fCloackTranslateFactor > 0.0f)
		fCloackTranslateFactor -= 0.28f;

	if (fStoneTranslateFactor > 0.0f)
		fStoneTranslateFactor -= 0.28f;

	if (fWandTranslateFactor > 0.0f)
		fWandTranslateFactor -= 0.28f;

	// Spin untill all three objects are translated to the center of the screen and then stop when the angle is 0.0f
	if (fCloackTranslateFactor <= 0.0f && fStoneTranslateFactor <= 0.0f && fWandTranslateFactor <= 0.0f)
	{
		if (fCloackAndStoneAndWandSpinning <= 360.0f && isAnimating == True)
			fCloackAndStoneAndWandSpinning += 1.28f;
		else
		{
			isAnimating = False;
			fCloackAndStoneAndWandSpinning = 0.0f;
		}
	}
	else
	{
		fCloackAndStoneAndWandSpinning += 1.28f;
		if (fCloackAndStoneAndWandSpinning >= 360.0f)
			fCloackAndStoneAndWandSpinning = 0.0f;
	}
}

void DrawInvisibilityCloak(void)
{
	// Code
	// glLoadIdentity();

	// Set Translation
	glTranslatef(VFP(-2.0f, fCloackTranslateFactor),
				 VFP(-2.0f, fCloackTranslateFactor),
				 -1.0f);

	// Set Rotation
	glRotatef(fCloackAndStoneAndWandSpinning, 0.0f, 1.0f, 0.0f);

	glBegin(GL_LINE_LOOP);
	{
		glColor3f(255.0f / 255.0f, 191.0f / 255.0f, 0.0f / 255.0f);

		glVertex3f(0.0f, 0.02f * 15, 0.0f);
		glVertex3f(0.02f * 17.5, -0.02f * 15, 0.0f);
		glVertex3f(-0.02f * 17.5, -0.02f * 15, 0.0f);
	}
	glEnd();

	// Reset Rotation
	glRotatef(-fCloackAndStoneAndWandSpinning, 0.0f, 1.0f, 0.0f);
	glTranslatef(VFP(2.0f, fCloackTranslateFactor),
				 VFP(2.0f, fCloackTranslateFactor),
				 1.0f);
}

void DrawElderWand(void)
{
	
	// Code

	// Set Translation
	glTranslatef(0.0f,
				 VFP(2.0f, fWandTranslateFactor),
				 -1.0f);

	glBegin(GL_LINES);
	{
		glColor3f(255.0f / 255.0f, 191.0f / 255.0f, 0.0f / 255.0f);

		glVertex3f(0.0f, 0.02f * 15, 0.0f);
		glVertex3f(0.0f, -0.02f * 15, 0.0f);
	}
	glEnd();

	// Reset Translation
	glTranslatef(0.0f,
				 VFP(-2.0f, fWandTranslateFactor),
				 1.0f);
}

void DrawResurrectionStone(void)
{
	// Code

	// Set Translation
	glTranslatef(VFP(2.0f, fStoneTranslateFactor),
				 VFP(-2.0f, fStoneTranslateFactor),
				 -1.0f);

	// Set Rotation
	glRotatef(fCloackAndStoneAndWandSpinning, 0.0f, 1.0f, 0.0f);

	glBegin(GL_LINE_LOOP);
	{
		for (int le = 0; le < 36; ++le)
		{
			float angle = M_PI * 2.0f * le / 36;

			glVertex3f((CENTROID_Y - -0.02f * 15) * sinf(angle) + CENTROID_X,
					   (CENTROID_Y - -0.02f * 15) * cosf(angle) + CENTROID_Y,
					   0.0f);
		}
	}
	glEnd();

	// Reset Rotation
	glRotatef(-fCloackAndStoneAndWandSpinning, 0.0f, 1.0f, 0.0f);

	// Reset Translation
	glTranslatef(VFP(-2.0f, fStoneTranslateFactor),
				 VFP(2.0f, fStoneTranslateFactor),
				 1.0f);
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
	// Function Declaration
	void uninitializeAudio(void);

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

	uninitializeAudio();
}

Bool initAudio(void)
{
    // Initialize OpenAL
    audioDevice = alcOpenDevice(NULL);
    if (!audioDevice)
    {
        fprintf(gpFile, "ERROR: Failed to open audio device\n");
        return False;
    }

    audioContext = alcCreateContext(audioDevice, NULL);
    if (!audioContext)
    {
        fprintf(gpFile, "ERROR: Failed to create audio context\n");
        alcCloseDevice(audioDevice);
        return False;
    }

    if (!alcMakeContextCurrent(audioContext))
    {
        fprintf(gpFile, "ERROR: Failed to make audio context current\n");
        alcDestroyContext(audioContext);
        alcCloseDevice(audioDevice);
        return False;
    }

    // Initialize mpg123
    mpg123_init();

    // Create audio buffer and source
    alGenBuffers(1, &audioBuffer);
    alGenSources(1, &audioSource);

    isAudioInitialized = True;
    return True;
}

Bool loadMP3File(const char* filePath)
{
    if (!isAudioInitialized)
        return False;

    mpg123_handle* mh = NULL;
    int err = MPG123_OK;
    unsigned char* buffer = NULL;
    size_t buffer_size = 0;
    size_t done = 0;
    int channels = 0, encoding = 0;
    long rate = 0;
    unsigned char* audio_data = NULL;
    size_t audio_data_size = 0;
    size_t audio_data_capacity = 0;
    ALenum format = 0;
    ALuint frequency = 0;
    
    // Open the MP3 file with mpg123
    mh = mpg123_new(NULL, &err);
    if (mh == NULL)
    {
        fprintf(gpFile, "ERROR: Unable to create mpg123 handle\n");
        return False;
    }

    if (mpg123_open(mh, filePath) != MPG123_OK)
    {
        fprintf(gpFile, "ERROR: Unable to open MP3 file: %s\n", filePath);
        mpg123_delete(mh);
        return False;
    }

    // Get the audio format details
    if (mpg123_getformat(mh, &rate, &channels, &encoding) != MPG123_OK)
    {
        fprintf(gpFile, "ERROR: Could not get audio format\n");
        mpg123_close(mh);
        mpg123_delete(mh);
        return False;
    }

    // Set format based on number of channels and encoding
    if (channels == 1)
        format = AL_FORMAT_MONO16;
    else
        format = AL_FORMAT_STEREO16;
    
    frequency = (ALuint)rate;
    
    // Decode the entire file into memory
    buffer_size = mpg123_outblock(mh);
    buffer = (unsigned char*)malloc(buffer_size * sizeof(unsigned char));
    
    while (mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK)
    {
        if (audio_data_size + done > audio_data_capacity)
        {
            audio_data_capacity = audio_data_capacity == 0 ? buffer_size * 4 : audio_data_capacity * 2;
            audio_data = (unsigned char*)realloc(audio_data, audio_data_capacity);
        }
        
        memcpy(audio_data + audio_data_size, buffer, done);
        audio_data_size += done;
    }
    
    // Load the decoded audio data into the OpenAL buffer
    alBufferData(audioBuffer, format, audio_data, audio_data_size, frequency);
    
    // Free resources
    free(buffer);
    free(audio_data);
    mpg123_close(mh);
    mpg123_delete(mh);
    
    // Set up source properties
    alSourcei(audioSource, AL_BUFFER, audioBuffer);
    alSourcef(audioSource, AL_GAIN, 1.0f);
    alSourcef(audioSource, AL_PITCH, 1.0f);
    alSourcei(audioSource, AL_LOOPING, AL_FALSE); // AL_FALSE for one-time playback
    
    return True;
}

void playAudio(void)
{
    if (!isAudioInitialized)
        return;
    
    ALint source_state;
    alGetSourcei(audioSource, AL_SOURCE_STATE, &source_state);
    
    if (source_state != AL_PLAYING)
    {
        alSourcePlay(audioSource);
        isAudioPlaying = True;
        fprintf(gpFile, "Audio Playback Started\n");
    }
    else
    {
        alSourcePause(audioSource);
        isAudioPlaying = False;
        fprintf(gpFile, "Audio Playback Paused\n");
    }
}

void uninitializeAudio(void)
{
    if (!isAudioInitialized)
        return;
    
    // Stop playback
    if (isAudioPlaying)
    {
        alSourceStop(audioSource);
        isAudioPlaying = False;
    }
    
    // Delete source and buffer
    alDeleteSources(1, &audioSource);
    alDeleteBuffers(1, &audioBuffer);
    
    // Clean up OpenAL context and device
    alcMakeContextCurrent(NULL);
    if (audioContext)
    {
        alcDestroyContext(audioContext);
        audioContext = NULL;
    }
    
    if (audioDevice)
    {
        alcCloseDevice(audioDevice);
        audioDevice = NULL;
    }
    
    // Shut down mpg123
    mpg123_exit();
    
    isAudioInitialized = False;
}



