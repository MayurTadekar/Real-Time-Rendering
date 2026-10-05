

#import <foundation/foundation.h>
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CVDisplayLink.h>   
#import <OpenGL/gl3.h>                

#import "vmath.h"

using namespace vmath;

enum{
    MST_ATTRIBUTE_POSITION = 0,
    MST_ATTRIBUTE_COLOR,
    MST_ATTRIBUTE_NORMAL,
    MST_ATTRIBUTE_TEXTURE0,
};

CVReturn MyDisplayLinkCallback(CVDisplayLinkRef, const CVTimeStamp*, const CVTimeStamp*, CVOptionFlags, CVOptionFlags*, void*);  
FILE *gpFile = NULL;

@interface AppDelegate:NSObject <NSApplicationDelegate, NSWindowDelegate>
@end

int main(int argc,char* argv[]){

    
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc]init];

    NSApp = [NSApplication sharedApplication];
    
    [NSApp setDelegate:[[AppDelegate alloc]init]];
    
    [NSApp run];
    
    
    [pool release]; 
    return(0);
}

@interface MyOpenGLView:NSOpenGLView
@end


@implementation AppDelegate{
    
    @private
        NSWindow *window;
        
        MyOpenGLView *myOpenGLView;

}


-(void)applicationDidFinishLaunching:(NSNotification*)aNotification{

    
    NSBundle *appBundle = [NSBundle mainBundle];
    
    NSString *appDirPath = [appBundle bundlePath];
    
    NSString *parentDirPath = [appDirPath stringByDeletingLastPathComponent];
    
    NSString *logFileNameWithPath = [NSString stringWithFormat:@"%@/Log.txt",parentDirPath];
    
    const char* pszLogFileNameWithPath = [logFileNameWithPath cStringUsingEncoding:NSASCIIStringEncoding];
    
    gpFile = fopen(pszLogFileNameWithPath,"w");
    if(gpFile == NULL)
    {
        [self release];
        [NSApp terminate:self];
    }
    
    fprintf(gpFile,"Program Started Successfully\n");
    
    NSRect wind_Rect = NSMakeRect( 0.0, 0.0, 800.0, 600.0);
    
    window = [
              [NSWindow alloc]initWithContentRect:wind_Rect
              styleMask:      NSWindowStyleMaskTitled |
                              NSWindowStyleMaskClosable |
                              NSWindowStyleMaskMiniaturizable |
                              NSWindowStyleMaskResizable
              backing:        NSBackingStoreBuffered
              defer:          NO
             ];

    [window setTitle:@"MST: Tesselation Shader"];

    [window center];
    
    myOpenGLView = [[MyOpenGLView alloc] initWithFrame: wind_Rect];

    [window setContentView: myOpenGLView];
    
    [window setDelegate:self];
        
    [window makeKeyAndOrderFront:self];

}

-(void)applicationWillTerminate:(NSNotification*)aNotification{

    if(gpFile)
    {
        fprintf(gpFile,"Program Terminated SuccessFully\n");
        fclose(gpFile);
        gpFile = NULL;
    }

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

    GLuint gVertexShaderObject;

    GLuint tesselationControlShaderObject;
    GLuint tesselationEvaluationShaderObject;

    GLuint gFragmentShaderObject;
    GLuint gShaderProgramObject;

    GLuint vao;
    GLuint vboPosition;
    GLuint mvpMatrixUniform;

    GLuint numberOfSegmentsUniform;
    GLuint numberOfStripsUniform;
    GLuint lineColorUniform;

    unsigned int uiNumberOfSegments;
    mat4 perspectiveProjectionMatrix;
}


-(id)initWithFrame:(NSRect)Frame{
    self = [super initWithFrame:Frame];
    if(self)
    {
        NSOpenGLPixelFormatAttribute attributes[] =
        {
            NSOpenGLPFAOpenGLProfile,NSOpenGLProfileVersion4_1Core,
            NSOpenGLPFAScreenMask, CGDisplayIDToOpenGLDisplayMask(kCGDirectMainDisplay),
            NSOpenGLPFANoRecovery,
            NSOpenGLPFAAccelerated,                     
            NSOpenGLPFAColorSize, 24,
            NSOpenGLPFADepthSize, 24,
            NSOpenGLPFAAlphaSize, 8,
            NSOpenGLPFADoubleBuffer,
            0
        };
        
        NSOpenGLPixelFormat *pixelFormat = [[[NSOpenGLPixelFormat alloc]initWithAttributes:attributes]autorelease];
        
        if(pixelFormat == nil)
        {
            fprintf(gpFile,"Cannot Create Pixel Format Exitting\n");
            [self release];
            [NSApp terminate : self];
        }
        
        NSOpenGLContext *glContext = [[[NSOpenGLContext alloc]initWithFormat:pixelFormat shareContext:nil]autorelease];
        
        [self setPixelFormat:pixelFormat];
        
        [self setOpenGLContext:glContext];
        
    }
    return(self);
}

-(CVReturn)getFrameForTime:(const CVTimeStamp*)outputTime
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc]init];
    
    [self drawView];
    
    [pool release];
    
    return(kCVReturnSuccess);
    
}

-(void)prepareOpenGL
{
    [super prepareOpenGL];
    
    [[self openGLContext]makeCurrentContext];
    
    GLint swapint = 1;
    
    [[self openGLContext]setValues:&swapint forParameter:NSOpenGLCPSwapInterval];
    
    
    gVertexShaderObject  = glCreateShader(GL_VERTEX_SHADER);

    const GLchar* vertexShaderSourceCode =
        "#version 410 core" \
        "\n" \
        "in vec2 vPosition;" \
        "void main(void)" \
        "{" \
        "   gl_Position = vec4(vPosition,0.0,1.0);" \
        "}";

    glShaderSource(gVertexShaderObject,
                    1,
                    (const GLchar**)&vertexShaderSourceCode,
                    NULL);  

    glCompileShader(gVertexShaderObject);
    GLint infoLogLength = 0;
    GLint shaderCompileStatus = 0;
    char *szBuffer = NULL;

    glGetShaderiv(gVertexShaderObject,
                GL_COMPILE_STATUS,
                &shaderCompileStatus);


    if(shaderCompileStatus == GL_FALSE){
        
        glGetShaderiv(gVertexShaderObject,
                        GL_INFO_LOG_LENGTH,
                        &infoLogLength);  


        if(infoLogLength > 0){

            szBuffer = (char*)malloc(sizeof(char)*infoLogLength);

            if(szBuffer != NULL){

                GLsizei written;

                glGetShaderInfoLog(gVertexShaderObject,
                                infoLogLength,
                                &written,
                                szBuffer);

                fprintf(gpFile,"vertex shader compilation log : %s\n",szBuffer);

                free(szBuffer);

                [self release];
                [NSApp terminate:self];
            }

        }

    }

    
    tesselationControlShaderObject  = glCreateShader(GL_TESS_CONTROL_SHADER);

    const GLchar* tessControlShaderSourceCode =
        "#version 410 core" \
        "\n" \
        "layout(vertices=4)out;" \
        "uniform int numberofsegments;" \
        "uniform int numberofstrips;" \
        "void main(void)" \
        "{" \
        "   gl_out[gl_InvocationID].gl_Position=gl_in[gl_InvocationID].gl_Position;" \
        "   gl_TessLevelOuter[0] = float(numberofstrips);" \
        "   gl_TessLevelOuter[1] = float(numberofsegments);" \
        "}";

    glShaderSource(tesselationControlShaderObject,
                    1,
                    (const GLchar**)&tessControlShaderSourceCode,
                    NULL);  

    glCompileShader(tesselationControlShaderObject);
    infoLogLength = 0;
    shaderCompileStatus = 0;
    szBuffer = NULL;

    glGetShaderiv(tesselationControlShaderObject,
                GL_COMPILE_STATUS,
                &shaderCompileStatus);


    if(shaderCompileStatus == GL_FALSE){
        
        glGetShaderiv(tesselationControlShaderObject,
                        GL_INFO_LOG_LENGTH,
                        &infoLogLength);


        if(infoLogLength > 0){

            szBuffer = (char*)malloc(sizeof(char)*infoLogLength);

            if(szBuffer != NULL){

                GLsizei written;

                glGetShaderInfoLog(tesselationControlShaderObject,
                                infoLogLength,
                                &written,
                                szBuffer);

                fprintf(gpFile,"tesselation control shader compilation log : %s\n",szBuffer);

                free(szBuffer);

                [self release];
                [NSApp terminate:self];
            }

        }

    }

    
    tesselationEvaluationShaderObject  = glCreateShader(GL_TESS_EVALUATION_SHADER);

    const GLchar* tessEvaluationShaderSourceCode =
        "#version 410 core" \
        "\n" \
        "layout(isolines)in;" \
        "uniform mat4 u_mvpMatrix;" \
        "void main(void)" \
        "{" \
        "   float tessCoord = gl_TessCoord.x;" \
        "   vec3 p0 = gl_in[0].gl_Position.xyz;" \
        "   vec3 p1 = gl_in[1].gl_Position.xyz;" \
        "   vec3 p2 = gl_in[2].gl_Position.xyz;" \
        "   vec3 p3 = gl_in[3].gl_Position.xyz;" \
        "   vec3 p = p0*(1.0-tessCoord)*(1.0-tessCoord)*(1.0-tessCoord)+p1*3.0*tessCoord*(1.0-tessCoord)*(1.0-tessCoord)+p2*3.0*tessCoord*tessCoord*(1.0-tessCoord)*(1.0-tessCoord)+p3*tessCoord*tessCoord*tessCoord;" \
        "   gl_Position = u_mvpMatrix * vec4(p,1.0);" \
        "}";

    glShaderSource(tesselationEvaluationShaderObject,
                    1,
                    (const GLchar**)&tessEvaluationShaderSourceCode,
                    NULL);  

    glCompileShader(tesselationEvaluationShaderObject);
    infoLogLength = 0;
    shaderCompileStatus = 0;
    szBuffer = NULL;

    glGetShaderiv(tesselationEvaluationShaderObject,
                GL_COMPILE_STATUS,
                &shaderCompileStatus);


    if(shaderCompileStatus == GL_FALSE){
        
        glGetShaderiv(tesselationEvaluationShaderObject,
                        GL_INFO_LOG_LENGTH,
                        &infoLogLength);


        if(infoLogLength > 0){

            szBuffer = (char*)malloc(sizeof(char)*infoLogLength);

            if(szBuffer != NULL){

                GLsizei written;

                glGetShaderInfoLog(tesselationEvaluationShaderObject,
                                infoLogLength,
                                &written,
                                szBuffer);

                fprintf(gpFile,"tesselation evaluation shader compilation log : %s\n",szBuffer);

                free(szBuffer);

                [self release];
                [NSApp terminate:self];
            }

        }

    }

    
    gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

    const GLchar* fragmentShaderSourceCode = 
        "#version 410 core" \
        "\n" \
        "out vec4 FragColor;" \
        "uniform vec4 lineColor;" \
        "void main(void)" \
        "{" \
        "   FragColor = lineColor;" \
        "}";

    glShaderSource(gFragmentShaderObject,1,(const char**)&fragmentShaderSourceCode,NULL);

    glCompileShader(gFragmentShaderObject);
    infoLogLength = 0;
    szBuffer = NULL;
    shaderCompileStatus = 0;

    glGetShaderiv(gFragmentShaderObject,
                GL_COMPILE_STATUS,
                &shaderCompileStatus);

    if(shaderCompileStatus == GL_FALSE){
        
        glGetShaderiv(gFragmentShaderObject,
                        GL_INFO_LOG_LENGTH,
                        &infoLogLength);


        if(infoLogLength > 0){

            szBuffer = (char*)malloc(sizeof(char)*infoLogLength);

            if(szBuffer != NULL){

                GLsizei written;

                glGetShaderInfoLog(gFragmentShaderObject,
                                infoLogLength,
                                &written,
                                szBuffer);

                fprintf(gpFile,"Fragment shader compilation log : %s\n",szBuffer);

                free(szBuffer);

                [self release];
                [NSApp terminate:self];
            }

        }

    }

    gShaderProgramObject = glCreateProgram();

    glAttachShader(gShaderProgramObject,
                    gVertexShaderObject);

    glAttachShader(gShaderProgramObject,
                    tesselationControlShaderObject);

    glAttachShader(gShaderProgramObject,
                    tesselationEvaluationShaderObject);

    glAttachShader(gShaderProgramObject,
                    gFragmentShaderObject);

    glBindAttribLocation(gShaderProgramObject,MST_ATTRIBUTE_POSITION,"vPosition");

    glLinkProgram(gShaderProgramObject);
    GLint ShaderProgramLinkStatus = 0;

    glGetProgramiv(gShaderProgramObject,
                GL_LINK_STATUS,
                &ShaderProgramLinkStatus);

    if(ShaderProgramLinkStatus == GL_FALSE){
        
        glGetShaderiv(gShaderProgramObject,
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
    numberOfSegmentsUniform = glGetUniformLocation(gShaderProgramObject,"numberofsegments");
    numberOfStripsUniform = glGetUniformLocation(gShaderProgramObject,"numberofstrips");
    lineColorUniform = glGetUniformLocation(gShaderProgramObject,"lineColor");

    const GLfloat vertices[] = {

        -1.0f,-1.0f,
        -0.5f,1.0f,
        0.5f,-1.0f,
        1.0f,1.0f,

    };


    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);

    glGenBuffers(1,&vboPosition);
    glBindBuffer(GL_ARRAY_BUFFER,vboPosition);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);

    glVertexAttribPointer(MST_ATTRIBUTE_POSITION,2,GL_FLOAT,GL_FALSE,0,NULL);


    glEnableVertexAttribArray(MST_ATTRIBUTE_POSITION);

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    glClearDepth(1.0f);

    glEnable(GL_DEPTH_TEST);

    glDepthFunc(GL_LEQUAL);

    glEnable(GL_CULL_FACE);

    glClearColor(0.0f,0.0f,0.0f,0.0f);

    uiNumberOfSegments = 1;
    perspectiveProjectionMatrix = mat4::identity();
    
    fprintf(gpFile, "OpenGL Version                           : %s\n", glGetString(GL_VERSION));
    fprintf(gpFile, "Graphics Library Shading Language(GLSL)  : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    
    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    
    CVDisplayLinkSetOutputCallback(displayLink, &MyDisplayLinkCallback, self);
    
    CGLContextObj cglContext = (CGLContextObj)[[self openGLContext]CGLContextObj];
    
    CGLPixelFormatObj cglPixelFormat =(CGLPixelFormatObj)[[self pixelFormat]CGLPixelFormatObj];
    
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
    
    CVDisplayLinkStart(displayLink);
    
}

-(void)reshape
{
    [super reshape];
    
    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
    NSRect rect = [self bounds];
    
    if(rect.size.height < 0){
        rect.size.height = 1;
    }
    
    glViewport(0, 0, (GLsizei)rect.size.width, (GLsizei)rect.size.height);

    perspectiveProjectionMatrix = vmath::perspective(45.0f,(GLfloat)rect.size.width / (GLfloat)rect.size.height,0.1f,100.0f);
    
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
}

-(void)drawView
{
    [[self openGLContext]makeCurrentContext];
    
    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(gShaderProgramObject);


    mat4 modelViewMatrix = mat4::identity();

    mat4 modelViewProjectionMatrix = mat4::identity();

    mat4 translateMatrix;

    translateMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
    modelViewMatrix = translateMatrix;
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;


    
    glUniformMatrix4fv(mvpMatrixUniform, 
        1,
        GL_FALSE,
        modelViewProjectionMatrix);

    glUniform1i(numberOfSegmentsUniform,uiNumberOfSegments);
    glUniform1i(numberOfStripsUniform,1);
    glUniform4fv(lineColorUniform,1,vmath::vec4(1.0f,1.0f,0.0f,1.0f));

    
    glBindVertexArray(vao);

    glPatchParameteri(GL_PATCH_VERTICES,4);
                
    glDrawArrays(GL_PATCHES,
        0,
        4);

    glBindVertexArray(0);

    glUseProgram(0);
        
    CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);                 
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);

}



-(void)drawRect:(NSRect)DirtyRect{
    [self drawView];

}

-(BOOL)acceptsFirstResponder
{

    [[self window]becomeFirstResponder];

    return YES;

}

-(void)keyDown:(NSEvent*)theEvent{

    int key = [[theEvent characters] characterAtIndex:0];

    switch(key){
        case 27:
            [self release];
            [NSApp terminate:self];
        break;
            
        case 'F':
        case 'f':

            [[self window] toggleFullScreen:self];

        break;

        case NSUpArrowFunctionKey:
            uiNumberOfSegments++;
            if(uiNumberOfSegments >= 30){
                uiNumberOfSegments = 30;
            }
        break;

        case NSDownArrowFunctionKey:
            uiNumberOfSegments--;
            if(uiNumberOfSegments <= 0){
                uiNumberOfSegments = 1;
            }
        break;

    }

}

-(void)mouseDown:(NSEvent*)theEvent{

}

-(void)rightMouseDown:(NSEvent*)theEvent{

}

-(void)otherMouseDown:(NSEvent*)theEvent{

}


-(void)dealloc{
    CVDisplayLinkStop(displayLink);
    
    CVDisplayLinkRelease(displayLink);

    
    if(vao){
        glDeleteVertexArrays(1,&vao);
        vao = 0;
    }

    if(vboPosition){
        glDeleteBuffers(1,&vboPosition);
        vboPosition = 0;
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

@end

CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime, CVOptionFlags flagIn, CVOptionFlags* flagsOut, void* displayLinkContext)
{
    CVReturn result = [(MyOpenGLView*)displayLinkContext getFrameForTime:outputTime];
    
    return(result);
    
}

























