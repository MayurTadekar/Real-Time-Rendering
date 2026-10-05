//
//  OGL.mm
//  
//
//  Created by Mayur Tadekar on 23/04/26.
//



#import        <foundation/foundation.h>
#import        <Cocoa/Cocoa.h>
#import        <QuartzCore/CVDisplayLink.h>
#import        <OpenGL/gl3.h>

#import        "vmath.h"

using namespace vmath;

enum
{
    MST_ATTRIBUTES_POSITION = 0,
    MST_ATTRIBUTES_COLOR,
    MST_ATTRIBUTES_NORMAL,
    MST_ATTRIBUTES_TEXTURE0
};

CVReturn myDisplayLinkCallback( CVDisplayLinkRef displayLink,
                                const CVTimeStamp* now,
                                const CVTimeStamp* outputTime,
                                CVOptionFlags flagIn,
                                CVOptionFlags* flagOut,
                               void* displayLinkContext);

FILE    *gpFile = NULL;

@interface AppDelegate:NSObject <NSApplicationDelegate, NSWindowDelegate>
@end


@interface MyOpenGLView:NSOpenGLView
-(void)uninitialize;
@end

int main(int argc, char **argv) {
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    NSApp = [NSApplication sharedApplication];

    [NSApp setDelegate: [[AppDelegate alloc]init]];

    [NSApp run];

    [pool release];

    return(0);
}

@implementation AppDelegate {
    @private
        NSWindow *window;
        MyOpenGLView *myOpenGLView;
}

-(void)applicationDidFinishLaunching:(NSNotification *) notification
{
    NSBundle *appBundle = [NSBundle mainBundle];

    NSString *appDirPath = [appBundle bundlePath];

    NSString *parentDirPath = [appDirPath stringByDeletingLastPathComponent];

    NSString *logFileNameWithPath = [NSString stringWithFormat:@"%@/log.txt", parentDirPath];

    const char* pszLogFileNameWithPath = [logFileNameWithPath cStringUsingEncoding:NSASCIIStringEncoding];

    gpFile = fopen(pszLogFileNameWithPath, "w");
    if( NULL == gpFile )
    {
        [self release];
        [NSApp terminate:self];
    }

    fprintf(gpFile, "Program Started\n");

    NSRect windRect = NSMakeRect(0.0f, 0.0f, 800.0f, 600.0f);


    window =     [
                    [NSWindow alloc]initWithContentRect:windRect
                    styleMask:         NSWindowStyleMaskTitled        |
                                    NSWindowStyleMaskClosable    |
                                    NSWindowStyleMaskMiniaturizable    |
                                    NSWindowStyleMaskResizable
                    backing:        NSBackingStoreBuffered
                    defer:            NO
                ];

    [window setTitle:@"MST: Perspective Colored Triangle"];

    [window center];

    myOpenGLView = [[MyOpenGLView alloc] initWithFrame: windRect];

    [window setContentView: myOpenGLView];

    [window setDelegate:self];

    [window makeKeyAndOrderFront:self];
}

-(void)applicationWillTerminate:(NSNotification*)aNotification{
    if(gpFile)
    {
        fprintf(gpFile, "Program Terminated\n");
        fclose(gpFile);
        gpFile = NULL;
    }
}

-(BOOL)windowShouldClose:(NSWindow*) windowWillClose{
    [myOpenGLView uninitialize];
    return(YES);
}

-(void)windowWillClose:(NSNotification*)aNotification{
    [NSApp terminate:self];
}

-(void)dealloc{
    [myOpenGLView release];
    [window release];

    [super dealloc];
}

@end

@implementation MyOpenGLView{
    @private
        CVDisplayLinkRef displayLink;

        GLuint    gVertexShaderObject;
        GLuint    gFragmentShaderObject;
        GLuint    gShaderProgramObject;

        GLuint    vao;
        GLuint    vbo_Position;
        GLuint    vbo_Color;
        GLuint    mvpMatrixUniform;
        mat4    perspectiveProjectionMatrix;
}

-(id) initWithFrame:(NSRect)Frame{
    self = [super initWithFrame:Frame];

    if(self)
    {
        NSOpenGLPixelFormatAttribute attributes[] =
        {
            NSOpenGLPFAOpenGLProfile,     NSOpenGLProfileVersion4_1Core,
            NSOpenGLPFAScreenMask,         CGDisplayIDToOpenGLDisplayMask(kCGDirectMainDisplay),
            NSOpenGLPFANoRecovery,
            NSOpenGLPFAAccelerated,
            NSOpenGLPFAColorSize,     24,
            NSOpenGLPFADepthSize,     24,
            NSOpenGLPFAAlphaSize,    8,
            NSOpenGLPFADoubleBuffer,
            0
        };

        NSOpenGLPixelFormat *pixelFormat = [[[NSOpenGLPixelFormat alloc]initWithAttributes:attributes]autorelease];

        if( nil == pixelFormat )
        {
            fprintf(gpFile, "Cannot Create Pixel Format\n");
            [self release];
            [NSApp terminate : self];
        }

        NSOpenGLContext *glContext = [[[NSOpenGLContext alloc]initWithFormat:pixelFormat shareContext:nil]autorelease];
        if(nil == glContext)
        {
            fprintf(gpFile, "NSOpenGL glContext cannot be created");
            [self uninitialize];
            [self release];
            [NSApp terminate:self];
        }


        [self setPixelFormat:pixelFormat];

        [self setOpenGLContext:glContext];

        [self setWantsLayer: YES];
        NSColor *backgroundColor = [NSColor blackColor];
        struct CGColor *bgColor = [backgroundColor CGColor];
        [[self layer] setBackgroundColor:bgColor];

    }

    return(self);
}

-(CVReturn)getFrameForTime:(const CVTimeStamp*)outputTime{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc]init];

    [self drawView];

    [pool release];

    return(kCVReturnSuccess);
}

-(void)prepareOpenGL{
    [super prepareOpenGL];

    [[self openGLContext]makeCurrentContext];

    GLint swapInterval = 1;

    [[self openGLContext]setValues:&swapInterval forParameter:NSOpenGLCPSwapInterval];

    int result = [self initialize];
    if(0 != result)
    {
        fprintf(gpFile, "initialized() Failed\n");
        [self uninitialize];
        [self release];
        [NSApp terminate:self];
    }

    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);

    CVDisplayLinkSetOutputCallback(    displayLink,
                                    &myDisplayLinkCallback,
                                    self);
    
    CGLContextObj cglContext = (CGLContextObj) [[self openGLContext]CGLContextObj];

    CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[[self pixelFormat]CGLPixelFormatObj];

    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);

    CVDisplayLinkStart(displayLink);
}

-(int)initialize
{
    gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

    const GLchar* vertexShaderSourceCode =
    "#version 410 core"                            \
    "\n"                                        \
    "in vec4 vPosition;"                        \
    "in vec3 vColor;"                        \
    "out vec3 out_vColor;"                        \
    "uniform mat4 u_mvpMatrix;"                    \
    "void main(void)"                            \
    "{    "                                        \
    "    gl_Position = u_mvpMatrix * vPosition;"    \
    "   out_vColor = vColor;\n"
    "}    ";

    glShaderSource(    gVertexShaderObject,
                    1,
                    (const GLchar**) &vertexShaderSourceCode,
                    NULL);

    glCompileShader(gVertexShaderObject);

    GLint infoLogLength = 0;
    GLint shaderCompileStatus = 0;
    char *szBuffer = NULL;

    glGetShaderiv(    gVertexShaderObject,
                    GL_COMPILE_STATUS,
                    &shaderCompileStatus);

    if(shaderCompileStatus == GL_FALSE){
        
        glGetShaderiv(    gVertexShaderObject,
                        GL_INFO_LOG_LENGTH,
                        &infoLogLength);

        if(infoLogLength > 0){

            szBuffer = (char*)malloc(sizeof(char)*infoLogLength);

            if(szBuffer != NULL){

                GLsizei written;

                glGetShaderInfoLog(    gVertexShaderObject,
                                    infoLogLength,
                                    &written,
                                    szBuffer);

                fprintf(gpFile,"Vertex Shader Compilation Log : %s\n",szBuffer);

                free(szBuffer);

            }
        }
        [self uninitialize];
        [self release];
        [NSApp terminate:self];
    }

    gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

    const GLchar* fragmentShaderSourceCode =
    "#version 410 core"                                \
    "\n"                                            \
    "out vec4 FragColor;"                            \
    "in vec3 out_vColor;"                        \
    "void main(void)"                                \
    "{    "                                            \
    "    FragColor = vec4(out_vColor, 1.0f);"    \
    "}    ";

    glShaderSource(    gFragmentShaderObject,
                    1,
                    (const GLchar**) &fragmentShaderSourceCode,
                    NULL);

    glCompileShader(gFragmentShaderObject);

    infoLogLength = 0;
    shaderCompileStatus = 0;
    szBuffer = NULL;

    glGetShaderiv(    gFragmentShaderObject,
                    GL_COMPILE_STATUS,
                    &shaderCompileStatus);

    if(shaderCompileStatus == GL_FALSE){
        
        glGetShaderiv(    gFragmentShaderObject,
                        GL_INFO_LOG_LENGTH,
                        &infoLogLength);

        if(infoLogLength > 0){

            szBuffer = (char*)malloc(sizeof(char)*infoLogLength);

            if(szBuffer != NULL){

                GLsizei written;

                glGetShaderInfoLog(    gFragmentShaderObject,
                                    infoLogLength,
                                    &written,
                                    szBuffer);

                fprintf(gpFile,"Fragment Shader Compilation Log : %s\n",szBuffer);

                free(szBuffer);

            }
        }
        [self uninitialize];
        [self release];
        [NSApp terminate:self];
    }

    gShaderProgramObject = glCreateProgram();

    glAttachShader(    gShaderProgramObject,
                    gVertexShaderObject);

    glAttachShader(    gShaderProgramObject,
                    gFragmentShaderObject);

    glBindAttribLocation(    gShaderProgramObject,
                            MST_ATTRIBUTES_POSITION,
                            "vPosition");

    glBindAttribLocation(    gShaderProgramObject,
                            MST_ATTRIBUTES_COLOR,
                            "vColor");

    glLinkProgram(gShaderProgramObject);

    GLint shaderProgramLinkStatus = 0;

    glGetProgramiv(    gShaderProgramObject,
                    GL_LINK_STATUS,
                    &shaderProgramLinkStatus);

    if(shaderProgramLinkStatus == GL_FALSE){
        
        glGetShaderiv(    gShaderProgramObject,
                        GL_INFO_LOG_LENGTH,
                        &infoLogLength);

        if(infoLogLength > 0){

            szBuffer = (char*)malloc(sizeof(char)*infoLogLength);

            if(szBuffer != NULL){

                GLsizei written;

                glGetProgramInfoLog(gShaderProgramObject,
                                    infoLogLength,
                                    &written,
                                    szBuffer);

                fprintf(gpFile,"Shader Program Link log : %s\n",szBuffer);

                free(szBuffer);

                [self release];
                [NSApp terminate:self];
            }
        }
    }

    mvpMatrixUniform = glGetUniformLocation(gShaderProgramObject,"u_mvpMatrix");

    const GLfloat rectVertices[] = {

        -1.0f,1.0f,0.0f,
        -1.0f,-1.0f,0.0f,
        1.0f,-1.0f,0.0f,

        -1.0f,1.0f,0.0f,
        1.0f,-1.0f,0.0f,
        1.0f,1.0f,0.0f
    };

    const GLfloat rectColor[] = {

        1.0f,0.0f,0.0f,
        0.0f,1.0f,0.0f,
        0.0f,0.0f,1.0f,

        1.0f,0.0f,0.0f,
        0.0f,0.0f,1.0f,
        1.0f,1.0f,0.0f

    };


    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);
    {
        glGenBuffers(1,&vbo_Position);
        glBindBuffer(GL_ARRAY_BUFFER,vbo_Position);
        glBufferData(GL_ARRAY_BUFFER,sizeof(rectVertices),rectVertices,GL_STATIC_DRAW);

        glVertexAttribPointer(    MST_ATTRIBUTES_POSITION,
                                3,
                                GL_FLOAT,
                                GL_FALSE,
                                0,
                                NULL);


        glEnableVertexAttribArray(MST_ATTRIBUTES_POSITION);

        glBindBuffer(GL_ARRAY_BUFFER,0);

        glGenBuffers(1,&vbo_Color);
        glBindBuffer(GL_ARRAY_BUFFER,vbo_Color);
        glBufferData(GL_ARRAY_BUFFER,sizeof(rectColor),rectColor,GL_STATIC_DRAW);

        glVertexAttribPointer(    MST_ATTRIBUTES_COLOR,
                                3,
                                GL_FLOAT,
                                GL_FALSE,
                                0,
                                NULL);


        glEnableVertexAttribArray(MST_ATTRIBUTES_COLOR);

        glBindBuffer(GL_ARRAY_BUFFER,0);
    }
    glBindVertexArray(0);

    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    fprintf(gpFile, "OpenGL Version: %s\n", glGetString(GL_VERSION));
    fprintf(gpFile, "Graphics Library Shading Language(GLSL): %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    perspectiveProjectionMatrix = mat4::identity();

    NSRect viewRect = [self bounds];
    int width = (int)viewRect.size.width;
    int height = (int)viewRect.size.height;

    [self resize:width :height];

    return(0);
}

-(void)reshape{
    [super reshape];

    [[self openGLContext] makeCurrentContext];

    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);

    NSRect viewRect = [self bounds];
    int width = (int)viewRect.size.width;
    int height = (int)viewRect.size.height;

    [self resize:width :height];

    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
}

-(void)resize:(int)width :(int)height
{

    if( height < 0 )
    {
        height = 1;
    }

    glViewport(0, 0, (GLsizei)width, (GLsizei)height);

    perspectiveProjectionMatrix = vmath::perspective(  45.0f,
                                                        (GLfloat)width/(GLfloat)height,
                                                        0.1f,
                                                        100.0f);
}

-(void)drawView{
    [[self openGLContext]makeCurrentContext];

    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    glUseProgram(gShaderProgramObject);

    mat4 modelViewMatrix = mat4::identity();

    modelViewMatrix = vmath::translate(0.0f, 0.0f, -3.0f);

    mat4 modelViewProjectionMatrix = mat4::identity();

    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    glBindVertexArray(vao);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);

    glUseProgram(0);

    CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);

    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
}

-(void)drawRect:(NSRect)DirtyRect{
    [[self openGLContext]makeCurrentContext];

    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);

    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
}

-(BOOL)acceptsFirstResponder{
    [[self window]becomeFirstResponder];

    return(YES);
}

-(void)keyDown:(NSEvent*)theEvent{
    int key = (int)[[theEvent characters] characterAtIndex:0];

    switch(key)
    {
        case 27:
            [self release];
            [NSApp terminate:self];
            break;

        case 'F':
        case 'f':
            [[self window] toggleFullScreen:self];
            break;
    }
}

-(void)mouseDown:(NSEvent*)theEvent
{
    //    No Code
}

-(void)rightMouseDown:(NSEvent*)theEvent{
    // No Code
}

-(void)otherMouseDown:(NSEvent*)theEvent{
    //    No Code
}

-(void)dealloc{
    CVDisplayLinkStop(displayLink);
    CVDisplayLinkRelease(displayLink);

    if(vao){
        glDeleteVertexArrays(1,&vao);
        vao = 0;
    }

    if(vbo_Position){
        glDeleteBuffers(1,&vbo_Position);
        vbo_Position = 0;
    }

    if(gShaderProgramObject){
        glUseProgram(gShaderProgramObject);

        GLsizei ShaderCount;


        glGetProgramiv(gShaderProgramObject,GL_ATTACHED_SHADERS,&ShaderCount);


        GLuint *pShaders = NULL;

        pShaders = (GLuint*)malloc(sizeof(GLuint)*ShaderCount);

        if(pShaders == NULL){
            printf("Error in pShaders\n");
        }

        glGetAttachedShaders(gShaderProgramObject,ShaderCount,&ShaderCount,pShaders);

        for(GLsizei i = 0;i < ShaderCount;i++){
            glDetachShader(gShaderProgramObject,pShaders[i]);
            glDeleteShader(pShaders[i]);
            pShaders[i] = 0;
        }

        free(pShaders);
        glDeleteProgram(gShaderProgramObject);

        gShaderProgramObject = 0;
        glUseProgram(0);
    }


    glDeleteProgram(gShaderProgramObject);

    gShaderProgramObject = 0;

    glUseProgram(0);

    [super dealloc];
}

-(void)uninitialize
{
    // code
    // Free vb_position
    if (vbo_Position)
    {
        glDeleteBuffers(1, &vbo_Position);
        vbo_Position = 0;
    }

    if (vao)
    {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }

    // Detach, Delete Shader objects and Delete Shader Program object
    if (gShaderProgramObject)
    {
        glUseProgram(gShaderProgramObject); 

        GLint numShader;

        glGetProgramiv(gShaderProgramObject, GL_ATTACHED_SHADERS, &numShader);
        

        if (numShader > 0)
        {
            GLuint * pShaders = (GLuint *)malloc(numShader * sizeof(GLuint)); 
            if (pShaders != NULL)
            {
                glGetAttachedShaders(gShaderProgramObject, numShader, NULL, pShaders);
                

                for (GLint i = 0; i < numShader; i++)
                {
                    glDetachShader(gShaderProgramObject, pShaders[i]); 

                    glDeleteShader(pShaders[i]); 
                    pShaders[i] = 0; 
                }

                free(pShaders);
                pShaders = NULL;
            }       
        }

        glUseProgram(0);

        glDeleteProgram(gShaderProgramObject);
    }
}

@end

CVReturn myDisplayLinkCallback( CVDisplayLinkRef displayLink,
                                const CVTimeStamp* now,
                                const CVTimeStamp* outputTime,
                                CVOptionFlags flagIn,
                                CVOptionFlags* flagOut,
                                void* displayLinkContext) {
    
    CVReturn result = [(MyOpenGLView*)displayLinkContext getFrameForTime:outputTime];

    return(result);
}
