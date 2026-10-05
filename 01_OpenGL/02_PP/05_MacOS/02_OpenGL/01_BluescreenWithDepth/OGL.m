#import <Foundation/Foundation.h> // like stdio.h
#import <Cocoa/Cocoa.h> // like Windows.h

#import <QuartzCore/CVDisplayLink.h> // for Core Video
#import <OpenGL/gl3.h>
#import <OpenGL/gl3ext.h>


@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>
@end

@interface GLView : NSOpenGLView
-(void)uninitialize;
@end

// Global Function declarations
CVReturn myDisplayLinkCallback(CVDisplayLinkRef, const CVTimeStamp *, const CVTimeStamp *, CVOptionFlags, CVOptionFlags *, void *);

// GLobal Variable declarations
// Log file related variable
FILE* gpFile = NULL;

// OpenGL Related Variables
GLuint shaderProgramObject = 0;

int main(int argc, char * argv[])
{
    // code
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc]init];

    NSApp = [NSApplication sharedApplication];

    [NSApp setDelegate:[[AppDelegate alloc]init]];

    [NSApp run];

    [pool release];

    return(0);
}

@implementation AppDelegate
{
    @private
    NSWindow *window;
    GLView *glView;
}

-(void)applicationDidFinishLaunching:(NSNotification *)notification
{
    // code
    //create log file
    NSBundle *appBundle=[NSBundle mainBundle];
    NSString *appDirPath=[appBundle bundlePath];
    NSString *parentDirPath=[appDirPath stringByDeletingLastPathComponent];
    NSString *logFileNameWithPath=[NSString stringWithFormat:@"%@/mst_log.txt",parentDirPath];

       const char *pszLogFileNameWithPath=[logFileNameWithPath cStringUsingEncoding:NSASCIIStringEncoding];
    gpFile = fopen(pszLogFileNameWithPath, "w");
    if (gpFile == NULL)
    {
        NSAlert *alert=[[NSAlert alloc]init];
        [alert setAlertStyle:NSAlertStyleCritical];
        [alert setMessageText:@"Log File Creation Failed"];
        [alert addButtonWithTitle:@"EXIT"];
        [alert runModal];
        [alert release];

        [self release];
        [NSApp terminate:self];
    }
    else
    {
        fprintf(gpFile, "Program Started Successfully!!\n");
    }

    NSRect winRect = NSMakeRect(0.0, 0.0, 800.0, 600.0);
    window = [[NSWindow alloc]initWithContentRect:winRect
                        styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable
                        backing:NSBackingStoreBuffered
                        defer:NO];

    [window setTitle:@"MST : MacOS Window"];
    [window center];

    glView = [[GLView alloc]initWithFrame:winRect];

    [window setContentView:glView];
    [window setDelegate:self];
    [window makeKeyAndOrderFront:self];
    
}

-(void)applicationWillTerminate:(NSNotification *)notification
{
    // code
    if(gpFile)
    {
        fprintf(gpFile, "App Will Terminate...\n");
        fclose(gpFile);
        gpFile = NULL;
    }
}

-(BOOL)windowShouldClose:(NSWindow *)aWindow
{
    [glView uninitialize];
    return(YES);
}

-(void)windowWillClose:(NSNotification *)notification
{
    // code
    [NSApp terminate:self];
}

-(void)dealloc
{
    // code
    [glView release];

    [window release];

    [super dealloc];
}

@end

@implementation GLView
{
    @private
    CVDisplayLinkRef displayLink;
}
-(id)initWithFrame:(NSRect)frame
{
    //code
    self = [super initWithFrame:frame];

    if(self)
    {
        [[self window]setContentView:self];

        // NSOpenGL Pixel format attributes
        NSOpenGLPixelFormatAttribute attributes[] = {    NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core,
                                                        NSOpenGLPFAScreenMask, CGDisplayIDToOpenGLDisplayMask(kCGDirectMainDisplay),
                                                        NSOpenGLPFANoRecovery,
                                                        NSOpenGLPFAAccelerated,
                                                        NSOpenGLPFAColorSize, 24,
                                                        NSOpenGLPFADepthSize, 24,
                                                        NSOpenGLPFAAlphaSize, 8,
                                                        NSOpenGLPFADoubleBuffer,
                                                        0
                                                    };

        // Create NSOpenGL Pixel Format
        NSOpenGLPixelFormat * pixelFormat = [[[NSOpenGLPixelFormat alloc]initWithAttributes : attributes]autorelease];
        if(pixelFormat == nil)
        {
            fprintf(gpFile, "NSOpenGL Pixel Format cannot be created");
            [self uninitialize];
            [self release];
            [NSApp terminate:self];
        }

        // Create NSOpenGL Context
        NSOpenGLContext *glContext = [[[NSOpenGLContext alloc]initWithFormat:pixelFormat shareContext:nil]autorelease];
        if(glContext == nil)
        {
            fprintf(gpFile, "NSOpenGL glContext cannot be created");
            [self uninitialize];
            [self release];
            [NSApp terminate:self];
        }

        // Set Views Pixel Format with our created Pixel Format
        [self setPixelFormat:pixelFormat];

        // Set View's OpenGL Context with our created context
        [self setOpenGLContext:glContext];

        [self setWantsLayer:YES];
        NSColor *backgroundColor = [NSColor blackColor];
        struct CGColor *bgColor = [backgroundColor CGColor];
        [[self layer]setBackgroundColor:bgColor];
    }

    return(self);
}

-(CVReturn)getFrameForTime:(const CVTimeStamp *)outputTime
{
    // code
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc]init];

    [self drawView];

    [pool release];

    return(kCVReturnSuccess);
}

-(void)prepareOpenGL
{
    // code
    [super prepareOpenGL];

    // Make the OpenGL Context current context
    [[self openGLContext]makeCurrentContext];

    // Set Swap Interval to One to snyc arrivals of buffers in double / triple buffering to avoid screen tearing
    GLint swapInterval = 1;
    [[self openGLContext]setValues:&swapInterval forParameter:NSOpenGLCPSwapInterval];

    // Now call Initialize here
    int result = [self initialize];
    if(result != 0)
    {
        fprintf(gpFile, "Initialize Failed");
        [self uninitialize];
        [self release];
        [NSApp terminate:self];
    }

    // Start the display link to create the seperate rendering thread
    // Create the Display Link
    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);

    // Set The Callback function of this Display link
    CVDisplayLinkSetOutputCallback(    displayLink,
                                    &myDisplayLinkCallback,
                                    self);

    //
    CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[[self pixelFormat]CGLPixelFormatObj];

    // Convert NSOpenGLContext to CGLContext
    CGLContextObj cglContext = (CGLContextObj)[[self openGLContext]CGLContextObj];

    //Set above CGLPixelFormat and CGLContext as Current display's CGLContext and CGLPixelFormat for this display link
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);

    // Start the display link (start the new rendering thread)
    CVDisplayLinkStart(displayLink);
}

-(void)reshape
{
    // code
    [super reshape];

    [[self openGLContext]makeCurrentContext];
    
    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);

    NSRect viewRect = [self bounds];
    int width = (int)viewRect.size.width;
    int height = (int)viewRect.size.height;

    [self resize:width :height];

    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
}

-(void)drawView
{
    // code
    [[self openGLContext]makeCurrentContext];

    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);

    [self display];

    [self myUpdate];

    // similar to SwapBuffers()
    CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);

    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
}

-(void)drawRect:(NSRect)dirtyRect
{
    // code
    // to avoid flickerring
    [self drawView];
}

-(BOOL)acceptsFirstResponder
{
    // code
    [[self window]makeFirstResponder:self];
    
    return(YES);
}

-(void)keyDown:(NSEvent *)event
{
    // code
    int key = (int)[[event characters]characterAtIndex:0];

    switch(key)
    {
        case 27:
        [self uninitialize];
        [self release];
        [NSApp terminate:self];
        break;

        case 'F':
        case 'f':
            [[self window]toggleFullScreen:self];
            break;

        default:
            break;
    }
}

-(void)mouseDown:(NSEvent *)event
{
    // code
}

-(int)initialize
{
    // code
    [self printGLInfo];

    // VERTEX SHADER
    const GLchar * vertexShaderSourcecode =
    "#version 410 core\n" \
    "void main(void)\n" \
    "{\n" \
    "}\n";
    // Here we write Shader Sourcecode

    GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
    // This function creates a shader object according to shader type

    glShaderSource(vertexShaderObject, 1, (const GLchar **)&vertexShaderSourcecode, NULL);
    // This function gives shader sourcecode to shader object
    // 1st parameter = to which object
    // 2nd parameter = no of shaders (Array size)
    // 3rd parameter = Address of shader array
    // 4th parameter = array of sizes of shader in array

    glCompileShader(vertexShaderObject);
    // This function compiles the shader

    GLint status = 0;
    GLint infoLogLength = 0;
    GLchar * szInfoLog = NULL;

    glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);
    // This function fills the status variable with state of compilation
    // 1st parameter = Shader object
    // 2nd parameter = what info you want
    // 3rd parameter = out parameter

    if (status == GL_FALSE)
    {
        glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
        // This function fills the status variable with log length
        // 1st parameter = Shader object
        // 2nd parameter = what info you want
        // 3rd parameter = out parameter

        if (infoLogLength > 0) // as their are some error which don't any string
        {
            szInfoLog = (GLchar *)malloc(infoLogLength * sizeof(GLchar)); // Here we allocate memory to szInfoLog to store error
            if (szInfoLog != NULL)
            {
                glGetShaderInfoLog(vertexShaderObject, infoLogLength, NULL, szInfoLog);
                // This function fills error in szInfoLog
                // 1st parameter = which shader
                // 2nd parameter = length of log
                // 3rd parameter = Actual returned length of string of error
                // 4th paramter = out parameter


                fprintf(gpFile, "Vertex Shader compilation log : %s\n", szInfoLog);

                free(szInfoLog);
                szInfoLog = NULL;
            }
        }

        [self uninitialize];
    }
    

    // FRAGMENT SHADER
    const GLchar * fragmentShaderSourceCode =
    "#version 410 core\n" \
    "void main(void)\n" \
    "{\n" \
    "}";
    // Here we write fragment Sourcecode

    GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    // This function creates a shader object according to shader type

    glShaderSource(fragmentShaderObject, 1, (const GLchar **)&fragmentShaderSourceCode, NULL);
    // This function gives shader sourcecode to shader object
    // 1st parameter = to which object
    // 2nd parameter = no of shaders (Array size)
    // 3rd parameter = Address of shader array
    // 4th parameter = array of sizes of shader in array

    glCompileShader(fragmentShaderObject);
    // This function compiles the shader

    status = 0;
    infoLogLength = 0;
    szInfoLog = NULL;


    glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);
    // This function fills the status variable with state of compilation
    // 1st parameter = Shader object
    // 2nd parameter = what info you want
    // 3rd parameter = out parameter


    if (status == GL_FALSE)
    {
        glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
        // This function fills the status variable with log length
        // 1st parameter = Shader object
        // 2nd parameter = what info you want
        // 3rd parameter = out parameter


        if (infoLogLength > 0) // as their are some error which don't any string
        {
            szInfoLog = (GLchar *)malloc(infoLogLength * sizeof(GLchar)); // Here we allocate memory to szInfoLog to store error
            if (szInfoLog != NULL)
            {
                glGetShaderInfoLog(fragmentShaderObject, infoLogLength, NULL, szInfoLog);
                // This function fills error in szInfoLog
                // 1st parameter = which shader
                // 2nd parameter = length of log
                // 3rd parameter = Actual returned length of string of error (Out Parameter)
                // 4th paramter = out parameter

                fprintf(gpFile, "Fragment Shader compilation log : %s\n", szInfoLog);

                free(szInfoLog);
                szInfoLog = NULL;
            }
        }

        [self uninitialize];
    }

    // Create, Attach, Link Shader Program Object
    shaderProgramObject = glCreateProgram(); // Here we create Shader Program Object
    
    glAttachShader(shaderProgramObject, vertexShaderObject); // Attaching Shader object
    glAttachShader(shaderProgramObject, fragmentShaderObject); // Attaching Shader object
    
    glLinkProgram(shaderProgramObject); // Linking the Shader Program Object

    status = 0;
    infoLogLength = 0;
    szInfoLog = NULL;

    glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &status);
    // This function fills the status variable with state of linking
    // 1st parameter = Shader Program object
    // 2nd parameter = what info you want
    // 3rd parameter = out parameter

    if (status == GL_FALSE)
    {
        glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);
        // This function fills the status variable with log length
        // 1st parameter = Shader Program object
        // 2nd parameter = what info you want
        // 3rd parameter = out parameter

        if (infoLogLength > 0)
        {
            szInfoLog = (GLchar *)malloc(infoLogLength * sizeof(GLchar)); // here we allocate memory to szInfoLog to store the error

            if (szInfoLog != NULL)
            {
                glGetProgramInfoLog(shaderProgramObject, infoLogLength, NULL, szInfoLog);
                // This function fills error in szInfoLog
                // 1st parameter = which shader program object
                // 2nd parameter = length of log
                // 3rd parameter = Actual returned length of string of error (Out Parameter)
                // 4th paramter = out parameter

                fprintf(gpFile, "Shader Program Link Log : %s\n", szInfoLog);

                free(szInfoLog);
                szInfoLog = NULL;
            }
        }
        
        [self uninitialize];
    }
    
    // Depth related Function call
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // From here Onward OpenGL Code starts
    // Tell OpenGL to Choose the Color to Clear the Screen
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

    // Warmup resize
    NSRect viewRect = [self bounds];
    int width = (int)viewRect.size.width;
    int height = (int)viewRect.size.height;

    [self resize:width :height];

    return(0);
}

-(void)printGLInfo
{
    // code
    // Print OpenGL Information
    fprintf(gpFile, "**************************************************************\n");
    fprintf(gpFile, "OpenGL information\n");
    fprintf(gpFile, "**************************************************************\n");
    fprintf(gpFile, "OpenGL Vendor : %s \n", glGetString(GL_VENDOR));
    fprintf(gpFile, "OpenGL Renderer : %s \n", glGetString(GL_RENDERER));
    fprintf(gpFile, "OpenGL Version : %s \n", glGetString(GL_VERSION));
    fprintf(gpFile, "GLSL version : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    fprintf(gpFile, "**************************************************************\n");
}

-(void)resize:(int)width :(int)height
{
    // If Height by accident becomes 0, then make it 1
    if (height <= 0)
    {
        height = 1;
    }

    // Set The ViewPort
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}

-(void)display
{
    // code
    // Clear OpenGL Buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // use shader program object
    glUseProgram(shaderProgramObject);

    // Unuse Shader program object
    glUseProgram(0);
}

-(void)myUpdate
{

}

-(void)uninitialize
{
    // code
    // Detach, Delete Shader objects and Delete Shader Program object
    if (shaderProgramObject)
    {
        glUseProgram(shaderProgramObject); 

        GLint numShader;

        glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &numShader);
        

        if (numShader > 0)
        {
            GLuint * pShaders = (GLuint *)malloc(numShader * sizeof(GLuint)); 
            if (pShaders != NULL)
            {
                glGetAttachedShaders(shaderProgramObject, numShader, NULL, pShaders);
                

                for (GLint i = 0; i < numShader; i++)
                {
                    glDetachShader(shaderProgramObject, pShaders[i]); 

                    glDeleteShader(pShaders[i]); 
                    pShaders[i] = 0; 
                }

                free(pShaders);
                pShaders = NULL;
            }
        }

        glUseProgram(0);

        glDeleteProgram(shaderProgramObject);
    }
}

-(void)dealloc
{
    // code
    CVDisplayLinkStop(displayLink);
    CVDisplayLinkRelease(displayLink);
    [super dealloc];
}

@end

CVReturn myDisplayLinkCallback(    CVDisplayLinkRef displayLinkRef,
                                const CVTimeStamp * current,
                                const CVTimeStamp * output,
                                CVOptionFlags inputFlags,
                                CVOptionFlags * outputFlags,
                                void * view)
{
    CVReturn result = [(GLView *)view getFrameForTime:output];

    return(result);
}
