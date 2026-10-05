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

#define     FIND_DISTANCE(x1, y1, x2, y2)   {  (float) sqrt( ( (x2-x1) * (x2-x1) ) + ( (y2-y1) * (y2-y1) )  ) }

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

    [window setTitle:@"MST: Graph Paper With Shapes"];

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

enum    {
    RED_HOR_LINE = 0,
    BLUE_HOR_LINE,
    GREEN_VERT_LINE,
    BLUE_VERT_LINE,
};


enum {
    TRIANGLE = 0,
    SQUARE,
    CIRCLE,
    CIRCLE_2,
};

@implementation MyOpenGLView{
    @private
        CVDisplayLinkRef displayLink;

        GLuint    gVertexShaderObject;
        GLuint    gFragmentShaderObject;
        GLuint    gShaderProgramObject;

        GLuint      vao_shape[4];
        GLuint      vbo_shape_position[4];
        GLuint      vbo_shape_color[4];

        GLuint      vao[4];
        GLuint      vbo_position[4];
        GLuint      vbo_color[4];

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

    //  GRAPH
    {
        //  RED_LINE
        {
            const GLfloat red_line[] = {
                5.0f, 0.0f, 0.0f,
                -5.0f, 0.0f, 0.0f,
            };
            
            const GLfloat red_line_color[] = {
                1.0f, 0.0f, 0.0f,
                1.0f, 0.0f, 0.0f,
            };

            glGenVertexArrays(1, &vao[RED_HOR_LINE]);

            glBindVertexArray(vao[RED_HOR_LINE]);     //   RECORDING CASET
            {
                // VERTEX
                glGenBuffers(1, &vbo_position[RED_HOR_LINE]);
                glBindBuffer(GL_ARRAY_BUFFER, vbo_position[RED_HOR_LINE]);
                glBufferData(GL_ARRAY_BUFFER, sizeof(red_line), red_line, GL_STATIC_DRAW);
                glVertexAttribPointer(MST_ATTRIBUTES_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
                glEnableVertexAttribArray(MST_ATTRIBUTES_POSITION);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                // COLOR
                glGenBuffers(1, &vbo_color[RED_HOR_LINE]);
                glBindBuffer(GL_ARRAY_BUFFER, vbo_color[RED_HOR_LINE]);
                glBufferData(GL_ARRAY_BUFFER, sizeof(red_line_color), red_line_color, GL_STATIC_DRAW);
                glVertexAttribPointer(MST_ATTRIBUTES_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
                glEnableVertexAttribArray(MST_ATTRIBUTES_COLOR);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            
            }
            glBindVertexArray(0);       // FINISH RECORDING CASET

        }

        //  BLUE_HOR_LINE
        {

            GLfloat *blue_hor_line = (GLfloat*) calloc( 40 * 3, sizeof(GLfloat) );
            GLfloat *blue_hor_line_color = (GLfloat*) calloc( 40 * 3, sizeof(GLfloat) );

            for(int i = 0 ; i < 20; i++) {
                //  Vertex
                *(blue_hor_line + (i*2 + 0) * 3 + 0) = -5.0f;
                *(blue_hor_line + (i*2 + 0) * 3 + 1) = 0.2f * i - (0.2f * 10);
                *(blue_hor_line + (i*2 + 0) * 3 + 2) = 0.0f;
                //  Color
                *(blue_hor_line_color + (i*2 + 0) * 3 + 0) = 0.0f;
                *(blue_hor_line_color + (i*2 + 0) * 3 + 1) = 0.0f;
                *(blue_hor_line_color + (i*2 + 0) * 3 + 2) = (i%5 == 0 ? 1.0f : 0.6f);

                if(i == 10)
                {
                    *(blue_hor_line_color + (i*2 + 0) * 3 + 0) = 1.0f;
                    *(blue_hor_line_color + (i*2 + 0) * 3 + 1) = 0.0f;
                    *(blue_hor_line_color + (i*2 + 0) * 3 + 2) = 0.0f;
                }
                
                //  Vertex
                *(blue_hor_line + (i*2 + 1) * 3 + 0) = 5.0f;
                *(blue_hor_line + (i*2 + 1) * 3 + 1) = 0.2f * i - (0.2f * 10);
                *(blue_hor_line + (i*2 + 1) * 3 + 2) = 0.0f;
                //  Color
                *(blue_hor_line_color + (i*2 + 1) * 3 + 0) = 0.0f;
                *(blue_hor_line_color + (i*2 + 1) * 3 + 1) = 0.0f;
                *(blue_hor_line_color + (i*2 + 1) * 3 + 2) = (i%5 == 0 ? 1.0f : 0.6f);

                if(i == 10)
                {
                    *(blue_hor_line_color + (i*2 + 1) * 3 + 0) = 1.0f;
                    *(blue_hor_line_color + (i*2 + 1) * 3 + 1) = 0.0f;
                    *(blue_hor_line_color + (i*2 + 1) * 3 + 2) = 0.0f;
                }
            } 

            glGenVertexArrays(1, &vao[BLUE_HOR_LINE]);
            
            glBindVertexArray(vao[BLUE_HOR_LINE]);
            {
                // Vertices
                glGenBuffers(1, &vbo_position[BLUE_HOR_LINE]);
                glBindBuffer(GL_ARRAY_BUFFER, vbo_position[BLUE_HOR_LINE]);
                glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * (40 * 3), blue_hor_line, GL_STATIC_DRAW);
                glVertexAttribPointer(MST_ATTRIBUTES_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
                glEnableVertexAttribArray(MST_ATTRIBUTES_POSITION);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                // Color
                glGenBuffers(1, &vbo_color[BLUE_HOR_LINE]);
                glBindBuffer(GL_ARRAY_BUFFER, vbo_color[BLUE_HOR_LINE]);
                glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 40 * 3, blue_hor_line_color, GL_STATIC_DRAW);
                glVertexAttribPointer(MST_ATTRIBUTES_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
                glEnableVertexAttribArray(MST_ATTRIBUTES_COLOR);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

            }
            glBindVertexArray(0);

            free(blue_hor_line_color);
            free(blue_hor_line);
        }
            
        //  GREEN_LINE
        {

            const GLfloat green_line[] = {
                0.0f, 3.0f, 0.0f,
                0.0f,-3.0f, 0.0f,
            };
            
            const GLfloat green_line_color[] = {
                0.0f, 1.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
            };

            glGenVertexArrays(1, &vao[GREEN_VERT_LINE]);

            glBindVertexArray(vao[GREEN_VERT_LINE]);     //   RECORDING CASET
            {
                // VERTEX
                glGenBuffers(1, &vbo_position[GREEN_VERT_LINE]);
                glBindBuffer(GL_ARRAY_BUFFER, vbo_position[GREEN_VERT_LINE]);
                glBufferData(GL_ARRAY_BUFFER, sizeof(green_line), green_line, GL_STATIC_DRAW);
                glVertexAttribPointer(MST_ATTRIBUTES_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
                glEnableVertexAttribArray(MST_ATTRIBUTES_POSITION);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                // COLOR
                glGenBuffers(1, &vbo_color[GREEN_VERT_LINE]);
                glBindBuffer(GL_ARRAY_BUFFER, vbo_color[GREEN_VERT_LINE]);
                glBufferData(GL_ARRAY_BUFFER, sizeof(green_line_color), green_line_color, GL_STATIC_DRAW);
                glVertexAttribPointer(MST_ATTRIBUTES_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
                glEnableVertexAttribArray(MST_ATTRIBUTES_COLOR);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            
            }
            glBindVertexArray(0);       // FINISH RECORDING CASET

        }

        //  BLUE_VERT_LINE
        {

            GLfloat *blue_vert_line = (GLfloat*) calloc( 60 * 3, sizeof(GLfloat) );
            GLfloat *blue_vert_line_color = (GLfloat*) calloc( 60 * 3, sizeof(GLfloat) );

            for(int i = 0 ; i < 30; i++) {
                //  Vertex
                *(blue_vert_line + (i*2 + 0) * 3 + 1) = 3.0f;
                *(blue_vert_line + (i*2 + 0) * 3 + 0) = 0.25f * i - (0.25 * 15);
                *(blue_vert_line + (i*2 + 0) * 3 + 2) = 0.0f;
                //  Color
                *(blue_vert_line_color + (i*2 + 0) * 3 + 0) = 0.0f;
                *(blue_vert_line_color + (i*2 + 0) * 3 + 1) = 0.0f;
                *(blue_vert_line_color + (i*2 + 0) * 3 + 2) = (i%5 == 0 ? 1.0f : 0.6f);
                
                if(i == 15)
                {
                    *(blue_vert_line_color + (i*2 + 0) * 3 + 0) = 0.0f;
                    *(blue_vert_line_color + (i*2 + 0) * 3 + 1) = 1.0f;
                    *(blue_vert_line_color + (i*2 + 0) * 3 + 2) = 0.0f;
                }
                
                //  Vertex
                *(blue_vert_line + (i*2 + 1) * 3 + 1) =-3.0f;
                *(blue_vert_line + (i*2 + 1) * 3 + 0) = 0.25f * i - (0.25 * 15);
                *(blue_vert_line + (i*2 + 1) * 3 + 2) = 0.0f;
                //  Color
                *(blue_vert_line_color + (i*2 + 1) * 3 + 0) = 0.0f;
                *(blue_vert_line_color + (i*2 + 1) * 3 + 1) = 0.0f;
                *(blue_vert_line_color + (i*2 + 1) * 3 + 2) = (i%5 == 0 ? 1.0f : 0.6f);
                
                if(i == 15)
                {
                    *(blue_vert_line_color + (i*2 + 1) * 3 + 0) = 0.0f;
                    *(blue_vert_line_color + (i*2 + 1) * 3 + 1) = 1.0f;
                    *(blue_vert_line_color + (i*2 + 1) * 3 + 2) = 0.0f;
                }

            } 

            glGenVertexArrays(1, &vao[BLUE_VERT_LINE]);
            
            glBindVertexArray(vao[BLUE_VERT_LINE]);
            {
                // Vertices
                glGenBuffers(1, &vbo_position[BLUE_VERT_LINE]);
                glBindBuffer(GL_ARRAY_BUFFER, vbo_position[BLUE_VERT_LINE]);
                glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * (60 * 3), blue_vert_line, GL_STATIC_DRAW);
                glVertexAttribPointer(MST_ATTRIBUTES_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
                glEnableVertexAttribArray(MST_ATTRIBUTES_POSITION);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                // Color
                glGenBuffers(1, &vbo_color[BLUE_VERT_LINE]);
                glBindBuffer(GL_ARRAY_BUFFER, vbo_color[BLUE_VERT_LINE]);
                glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 60 * 3, blue_vert_line_color, GL_STATIC_DRAW);
                glVertexAttribPointer(MST_ATTRIBUTES_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
                glEnableVertexAttribArray(MST_ATTRIBUTES_COLOR);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

            }
            glBindVertexArray(0);

            free(blue_vert_line_color);
            free(blue_vert_line);
        }

    }

    //  TRIANGLE   
    const GLfloat triangle[] = {
        0.0f, 0.25f * 4, 0.0f,
        -0.25f * 4, -0.25f * 4, 0.0f,
        0.25f * 4, -0.25f * 4, 0.0f,
    };
    
    const GLfloat triangle_color[] = {
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
    };

    glGenVertexArrays(1, &vao_shape[TRIANGLE]);

    glBindVertexArray(vao_shape[TRIANGLE]);     //   RECORDING CASET
    {
        // VERTEX
        glGenBuffers(1, &vbo_shape_position[TRIANGLE]);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_shape_position[TRIANGLE]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
        glVertexAttribPointer(MST_ATTRIBUTES_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(MST_ATTRIBUTES_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // COLOR
        glGenBuffers(1, &vbo_shape_color[TRIANGLE]);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_shape_color[TRIANGLE]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_color), triangle_color, GL_STATIC_DRAW);
        glVertexAttribPointer(MST_ATTRIBUTES_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(MST_ATTRIBUTES_COLOR);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    }
    glBindVertexArray(0);       // FINISH RECORDING CASET

    //  SQUARE   
    {
        const GLfloat square[] = {
            -0.25f * 4,  0.25f * 4, 0.0f,
            -0.25f * 4, -0.25f * 4, 0.0f,
             0.25f * 4, -0.25f * 4, 0.0f,
             0.25f * 4,  0.25f * 4, 0.0f
        };
        
        const GLfloat square_color[] = {
            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f
        };

        glGenVertexArrays(1, &vao_shape[SQUARE]);

        glBindVertexArray(vao_shape[SQUARE]);     //   RECORDING CASET
        {
            // VERTEX
            glGenBuffers(1, &vbo_shape_position[SQUARE]);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_shape_position[SQUARE]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_STATIC_DRAW);
            glVertexAttribPointer(MST_ATTRIBUTES_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(MST_ATTRIBUTES_POSITION);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // COLOR
            glGenBuffers(1, &vbo_shape_color[SQUARE]);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_shape_color[SQUARE]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(square_color), square_color, GL_STATIC_DRAW);
            glVertexAttribPointer(MST_ATTRIBUTES_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(MST_ATTRIBUTES_COLOR);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        }
        glBindVertexArray(0);       // FINISH RECORDING CASET

    }

    //  CIRCLE   
    {
        GLfloat circle[33 * 3];        
        GLfloat circle_color[33 * 3];

        GLfloat     a = FIND_DISTANCE( triangle[3], triangle[4], triangle[6], triangle[7] );
        GLfloat     b = FIND_DISTANCE( triangle[6], triangle[7], triangle[0], triangle[1] );
        GLfloat     c = FIND_DISTANCE( triangle[0], triangle[1], triangle[3], triangle[4] );

        GLfloat center_X = ( (a * triangle[0]) + (b * triangle[3]) + ( c * triangle[6]) ) / ( a + b + c);
        GLfloat center_Y = ( (a * triangle[1]) + (b * triangle[4]) + ( c * triangle[7]) ) / ( a + b + c);

        for( int i = 0 ; i <= 32; i++) {

            float angle = 2.0f * M_PI * i / 32;

            circle[ i*3 + 0 ] = (center_Y + triangle[1]) * sinf(angle);
            circle[ i*3 + 1 ] = ( center_Y + triangle[1]) * cosf(angle) + center_Y;
            circle[ i*3 + 2 ] = 0.0f;

            circle_color[ i*3 + 0 ] = 1.0f;
            circle_color[ i*3 + 1 ] = 1.0f;
            circle_color[ i*3 + 2 ] = 0.0f;
        }

        glGenVertexArrays(1, &vao_shape[CIRCLE]);

        glBindVertexArray(vao_shape[CIRCLE]);     //   RECORDING CASET
        {
            // VERTEX
            glGenBuffers(1, &vbo_shape_position[CIRCLE]);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_shape_position[CIRCLE]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(circle), circle, GL_STATIC_DRAW);
            glVertexAttribPointer(MST_ATTRIBUTES_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(MST_ATTRIBUTES_POSITION);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // COLOR
            glGenBuffers(1, &vbo_shape_color[CIRCLE]);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_shape_color[CIRCLE]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(circle_color), circle_color, GL_STATIC_DRAW);
            glVertexAttribPointer(MST_ATTRIBUTES_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(MST_ATTRIBUTES_COLOR);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        }
        glBindVertexArray(0);       // FINISH RECORDING CASET

    }

    //  CIRCLE_2   
    {
        GLfloat circle[33 * 3];        
        GLfloat circle_color[33 * 3];

        
        for( int i = 0 ; i <= 32; i++) {

            float angle = 2.0f * M_PI * i / 32;

            circle[ i*3 + 0 ] = 1.414f * sinf(angle);
            circle[ i*3 + 1 ] = 1.414f * cosf(angle);
            circle[ i*3 + 2 ] = 0.0f;

            circle_color[ i*3 + 0 ] = 1.0f;
            circle_color[ i*3 + 1 ] = 1.0f;
            circle_color[ i*3 + 2 ] = 0.0f;
        }

        glGenVertexArrays(1, &vao_shape[CIRCLE_2]);

        glBindVertexArray(vao_shape[CIRCLE_2]);     //   RECORDING CASET
        {
            // VERTEX
            glGenBuffers(1, &vbo_shape_position[CIRCLE_2]);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_shape_position[CIRCLE_2]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(circle), circle, GL_STATIC_DRAW);
            glVertexAttribPointer(MST_ATTRIBUTES_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(MST_ATTRIBUTES_POSITION);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // COLOR
            glGenBuffers(1, &vbo_shape_color[CIRCLE_2]);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_shape_color[CIRCLE_2]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(circle_color), circle_color, GL_STATIC_DRAW);
            glVertexAttribPointer(MST_ATTRIBUTES_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(MST_ATTRIBUTES_COLOR);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        }
        glBindVertexArray(0);       // FINISH RECORDING CASET

    }

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

    modelViewMatrix = vmath::translate(0.0f, 0.0f, -5.0f);

    mat4 modelViewProjectionMatrix = mat4::identity();

    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    //  GRAPH
    {

        glBindVertexArray(vao[BLUE_HOR_LINE]);
        glDrawArrays(GL_LINES, 0, 40);
        glBindVertexArray(0);
        
        glBindVertexArray(vao[GREEN_VERT_LINE]);
        glDrawArrays(GL_LINES, 0, 2);
        glBindVertexArray(0);
        
        glBindVertexArray(vao[BLUE_VERT_LINE]);
        glDrawArrays(GL_LINES, 0, 60);
        glBindVertexArray(0);

    }

    //  SHAPE   
    {
        glBindVertexArray(vao_shape[TRIANGLE]);
        glDrawArrays(GL_LINE_LOOP, 0, 3);
        glBindVertexArray(0);
        
        glBindVertexArray(vao_shape[SQUARE]);
        glDrawArrays(GL_LINE_LOOP, 0, 4);
        glBindVertexArray(0);
        
        glBindVertexArray(vao_shape[CIRCLE]);
        glDrawArrays(GL_LINE_LOOP, 0, 33);
        glBindVertexArray(0);
        
        glBindVertexArray(vao_shape[CIRCLE_2]);
        glDrawArrays(GL_LINE_LOOP, 0, 33);
        glBindVertexArray(0);
    }

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

    if (vao_shape)
    {
        glDeleteBuffers(4, vao_shape);
    }

    if (vbo_shape_position)
    {
        glDeleteBuffers(4, vbo_shape_position);
    }

    if (vbo_shape_color)
    {
        glDeleteBuffers(4, vbo_shape_color);
    }

    if (vao)
    {
        glDeleteBuffers(4, vao);
    }

    if (vbo_position)
    {
        glDeleteBuffers(4, vbo_position);
    }

    if (vbo_color)
    {
        glDeleteBuffers(4, vbo_color);
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
    if (vao_shape)
    {
        glDeleteBuffers(4, vao_shape);
    }

    if (vbo_shape_position)
    {
        glDeleteBuffers(4, vbo_shape_position);
    }

    if (vbo_shape_color)
    {
        glDeleteBuffers(4, vbo_shape_color);
    }

    if (vao)
    {
        glDeleteBuffers(4, vao);
    }

    if (vbo_position)
    {
        glDeleteBuffers(4, vbo_position);
    }

    if (vbo_color)
    {
        glDeleteBuffers(4, vbo_color);
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
