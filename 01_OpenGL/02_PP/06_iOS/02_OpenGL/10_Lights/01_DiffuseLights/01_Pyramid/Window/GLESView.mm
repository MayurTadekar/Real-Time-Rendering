//
//  MyView.m
//  Window
//
//  Created by Mayur Tadekar on 25/04/26.
//

#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#include <QuartzCore/CADisplayLink.h>

#import "GLESView.h"
#import "vmath.h"

@implementation GLESView
{
    EAGLContext *eaglContext;
    
    GLuint framebuffer;
    GLuint colorRenderbuffer;
    GLuint depthRenderbuffer;
    
    CADisplayLink *displayLink;
    CAFrameRateRange frameRateRange;
    BOOL isDisplayLink;
    
    FILE *gpFile;
    
    GLuint shaderProgramObject;
    
    enum
    {
        MST_ATTRIBUTE_POSITION = 0,
        MST_ATTRIBUTE_COLOR,
        MST_ATTRIBUTE_TEXCOORDS0,
        MST_ATTRIBUTE_NORMAL
    };
    
    GLuint uLightDiffuse;
    GLuint uLightPosition;
    
    GLuint uMaterialDiffuse;
    GLuint ukey;
    
    GLuint vao[2];
    GLuint vbo_Position[2];
    GLuint vbo_Color[2];
    GLuint vbo_Normal[2];
    
    GLuint u_mMatrix;
    GLuint u_vMatrix;
    GLuint u_pMatrix;
    
    float rotate_angle;
    
    BOOL bAnimate;
    
    BOOL bLight;
    GLfloat lightDiffuse[4];
    GLfloat lightPosition[4];
    
    GLfloat materialDiffuse[4];
    
    vmath::mat4 perspectiveProjectionMatrix;
}

-(void)awakeFromNib
{
    // Code
    [super awakeFromNib];
    
    [self setBackgroundColor:[UIColor blackColor]];
    
    CAEAGLLayer *eaglLayer = (CAEAGLLayer *)[super layer];
    
    [eaglLayer setOpaque:YES];
    NSDictionary *dictionary = [NSDictionary dictionaryWithObjectsAndKeys:
                                kEAGLDrawablePropertyRetainedBacking, [NSNumber numberWithBool:NO],
                                 kEAGLDrawablePropertyColorFormat, kEAGLColorFormatRGBA8, nil];
    [eaglLayer setDrawableProperties:dictionary];
    
    eaglContext = [[EAGLContext alloc]initWithAPI:kEAGLRenderingAPIOpenGLES3];
    if(eaglContext == nil)
    {
        printf("OpenGL ES Context creation failed\n");
        return;
    }
    
    [EAGLContext setCurrentContext:eaglContext];
    
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    
    glGenRenderbuffers(1, &colorRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    [eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer);

    GLint width;
    GLint height;
    
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("initWithFrame() Framebuffer is not complete\n");
        [self uninitialize];
        return;
    }
    
    displayLink = nil;
    frameRateRange.minimum = 30.0f;
    frameRateRange.maximum = 60.0f;
    frameRateRange.preferred = 60.0f;
    isDisplayLink = NO;
    
    int result = [self initialize];
    if(result != 0)
    {
        printf("Initialize() is failed\n");
    }
    
    // Event handling
    
    // Single tap
    UITapGestureRecognizer *singleTapGestureRecognizer = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onSingleTap:)];
    [singleTapGestureRecognizer setNumberOfTapsRequired:1];
    [singleTapGestureRecognizer setNumberOfTouchesRequired:1];
    [singleTapGestureRecognizer setDelegate:self];
    [self addGestureRecognizer:singleTapGestureRecognizer];
    
    // Double tap
    UITapGestureRecognizer *doubleTapGestureRecognizer = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onDoubleTap:)];
    [doubleTapGestureRecognizer setNumberOfTapsRequired:2];
    [doubleTapGestureRecognizer setNumberOfTouchesRequired:1];
    [doubleTapGestureRecognizer setDelegate:self];
    [self addGestureRecognizer:doubleTapGestureRecognizer];
    
    // tell single tap recognizer to fail when there is double tap
    [singleTapGestureRecognizer requireGestureRecognizerToFail:doubleTapGestureRecognizer];
    
    // Swipe tap
    UISwipeGestureRecognizer *swipeGestureRecognizer = [[UISwipeGestureRecognizer alloc]initWithTarget:self action:@selector(onSwipe:)];
    [swipeGestureRecognizer setDelegate:self];
    [self addGestureRecognizer:swipeGestureRecognizer];
    
    // Long press
    UILongPressGestureRecognizer *longPressGestureRecognizer = [[UILongPressGestureRecognizer alloc]initWithTarget:self action:@selector(onLongPress:)];
    [longPressGestureRecognizer setDelegate:self];
    [self addGestureRecognizer:longPressGestureRecognizer];
}

+(Class)layerClass
{
    // Code
    return([CAEAGLLayer class]);
}

/*
- (void)drawRect:(CGRect)rect {
    // Code
}*/

-(void)layoutSubviews
{
    // Code
    [super layoutSubviews];
    
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    [eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)[self layer]];
    
    GLint width;
    GLint height;
    
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("layoutSubviews() - Framebuffer is not complete\n");
        return;
    }
    
    [self resize:width :height];
    
    [self drawView:self];
}

-(void)drawView:(id)sender
{
    // Code
    [EAGLContext setCurrentContext:eaglContext];
    
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    
    [self myupdate];
    
    [self display];
    
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    [eaglContext presentRenderbuffer:colorRenderbuffer];
    
}

-(void)startDisplayLink
{
    // Code
    if(isDisplayLink == NO)
    {
        displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(drawView:)];
        [displayLink setPreferredFrameRateRange:frameRateRange];
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
        isDisplayLink = YES;
    }
}

-(void)stopDisplayLink
{
    // Code
    if(isDisplayLink == YES)
    {
        [displayLink invalidate];
        isDisplayLink = NO;
    }
}

-(BOOL)becomeFirstResponder
{
    // Code
    return(YES);
}

-(void)touchesBegan:(UITouch *)touches withEvent:(UIEvent *)event
{
    // Code
}

-(void)onSingleTap:(UITapGestureRecognizer *)gestureRecognizer
{
    // Code
    bAnimate = !bAnimate;
}

-(void)onDoubleTap:(UITapGestureRecognizer *)gestureRecognizer
{
    // Code
    bLight = !bLight;
}

-(void)onSwipe:(UISwipeGestureRecognizer *)gestureRecognizer
{
    // Code
    [self uninitialize];
    [self release];
    exit(0);
}

-(void)onLongPress:(UILongPressGestureRecognizer *)gestureRecognizer
{
    // Code
}

-(int)initialize
{
    // Code
    [self printGLESInfo];
    
    // Vertex Shader
    const GLchar* vertexShaderSourceCode =
    "#version 300 es\n" \
    "precision highp float;\n" \
    "in vec4 vPosition;\n" \
    "in vec3 vNormal;\n"                                                            \
    "uniform mat4 u_mMatrix;\n"                                             \
    "uniform mat4 u_vMatrix;\n"                                             \
    "uniform mat4 u_pMatrix;\n"                                             \
    "uniform int  u_lKeyPressed;\n"                                                 \
    "uniform vec3  u_ld;\n"                                                         \
    "uniform vec3  u_kd;\n"                                                         \
    "uniform vec4  u_LightPosition;\n"                                              \
    "out vec3 diffuse_Light;\n"                                                     \
    "void main(void)\n"                                                             \
    "{"                                                                             \
    "  if( u_lKeyPressed == 1 ) "                                                   \
    "  {   "                                                                        \
    "   vec4 eye_coord = u_vMatrix * u_mMatrix * vPosition;   "                        \
    "   mat3 normal_matrix = mat3( transpose( inverse( u_vMatrix * u_mMatrix ) ) );\n"  \
    "   vec3 t_norm = normalize(normal_matrix * vNormal);\n"                        \
    "   vec3 S = normalize( vec3(u_LightPosition - eye_coord) );\n"                 \
    "   diffuse_Light = u_ld * u_kd * max( dot(S, t_norm), 0.0f)  ;\n"              \
    "  }    "                                                                       \
    "  else    "                                                                       \
    "  {    "                                                                       \
    "   diffuse_Light = vec3(1.0f);    "                                                                       \
    "  }    "                                                                       \
    "gl_Position = u_pMatrix * u_vMatrix * u_mMatrix * vPosition;\n"          \
    "}";
    
    GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);
    
    glCompileShader(vertexShaderObject);
    
    GLint status = 0;
    GLint infoLogLength = 0;
    GLchar* szInfoLog = NULL;
    
    glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);
    
    if (status == GL_FALSE)
    {
        glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 0)
        {
            szInfoLog = (GLchar*)malloc(infoLogLength * sizeof(GLchar));
            if (szInfoLog != NULL)
            {
                glGetShaderInfoLog(vertexShaderObject, infoLogLength, NULL, szInfoLog);
                free(szInfoLog);
                szInfoLog = NULL;
            }
        }
        [self uninitialize];
    }
    
    // FRAGMENT SHADER
    const GLchar* fragmentShaderSourceCode =
    "#version 300 es\n" \
    "precision highp float;\n" \
    "in vec4 ovColor;\n" \
    "uniform int u_lKeyPressed;\n"      \
    "out vec4 Fragcolor;\n" \
    "in  vec3 diffuse_Light;\n"                         \
    "vec4 color;\n"                                     \
    "void main(void)"                                   \
    "{"                                                 \
    "   color = vec4( diffuse_Light, 1.0f );\n"     \
    "   Fragcolor = color;\n"                           \
    "}";
    
    GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);
    
    glCompileShader(fragmentShaderObject);
    
    status = 0;
    infoLogLength = 0;
    szInfoLog = NULL;
    
    glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 0)
        {
            szInfoLog = (GLchar*)malloc(infoLogLength * sizeof(GLchar));
            if (szInfoLog != NULL)
            {
                glGetShaderInfoLog(fragmentShaderObject, infoLogLength, NULL, szInfoLog);
                //fprintf(gpFile, "Fragment Shader Compilation Log = %s\n", szInfoLog);
                free(szInfoLog);
                szInfoLog = NULL;
            }
        }
        [self uninitialize];
    }
    
    // Create, Attach, Link Shader Program Object
    shaderProgramObject = glCreateProgram();
    glAttachShader(shaderProgramObject, vertexShaderObject);
    glAttachShader(shaderProgramObject, fragmentShaderObject);
    
    // Bind Shader Attribute at a certain index in shader to same index in host program.
    glBindAttribLocation(shaderProgramObject, MST_ATTRIBUTE_POSITION, "vPosition");
    glBindAttribLocation(shaderProgramObject, MST_ATTRIBUTE_NORMAL, "vNormal");
    
    glLinkProgram(shaderProgramObject);
    
    status = 0;
    infoLogLength = 0;
    szInfoLog = NULL;
    glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 0)
        {
            szInfoLog = (GLchar*)malloc(infoLogLength * sizeof(GLchar));
            if (szInfoLog != NULL)
            {
                glGetProgramInfoLog(shaderProgramObject, infoLogLength, NULL, szInfoLog);
                //fprintf(gpFile, "Shader Program Link Log = %s\n", szInfoLog);
                free(szInfoLog);
                szInfoLog = NULL;
            }
        }
        [self uninitialize];
    }
    
    // Get the required uniform location from the Shader
    u_mMatrix = glGetUniformLocation(shaderProgramObject, "u_mMatrix");
    u_vMatrix = glGetUniformLocation(shaderProgramObject, "u_vMatrix");
    u_pMatrix = glGetUniformLocation(shaderProgramObject, "u_pMatrix");
    
    uLightDiffuse = glGetUniformLocation(shaderProgramObject,"u_ld");
    uLightPosition = glGetUniformLocation(shaderProgramObject,"u_LightPosition");

    uMaterialDiffuse = glGetUniformLocation(shaderProgramObject,"u_kd");

    ukey = glGetUniformLocation(shaderProgramObject,"u_lKeyPressed");

    
    // provide vertex, position, color, normal, texcoord etc.
    //  Triangle
    const GLfloat shape_position0[] =
    {
        0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 1.0f,
         1.0f, -1.0f, 1.0f,

         0.0f, 1.0f, 0.0f,
         1.0f, -1.0f, 1.0f,
         1.0f, -1.0f, -1.0f,

         0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

         0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
    };
        
    const GLfloat shape_normal0[] =
    {
        // FRONT
        0.0f,0.447214f,0.894427f,
        0.0f,0.447214f,0.894427f,
        0.0f,0.447214f,0.894427f,

        // RIGHT
        0.894427f, 0.447214f, 0.0f,
        0.894427f, 0.447214f, 0.0f,
        0.894427f, 0.447214f, 0.0f,
         
        // BACK
        0.0f, 0.447214f, -0.894427f,
        0.0f, 0.447214f, -0.894427f,
        0.0f, 0.447214f, -0.894427f,
         
        // LEFT
        -0.894427f, 0.447214f, 0.0f,
        -0.894427f, 0.447214f, 0.0f,
        -0.894427f, 0.447214f, 0.0f
    };
    
    // Vertex array object for arrays of vertex attributes
    glGenVertexArrays(1, &vao[0]);
    glBindVertexArray(vao[0]);
    {
        // Position Buffer
        glGenBuffers(1, &vbo_Position[0]);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_Position[0]);
        {
            glBufferData(GL_ARRAY_BUFFER, sizeof(shape_position0), shape_position0, GL_STATIC_DRAW);
            
            glVertexAttribPointer(MST_ATTRIBUTE_POSITION,
                                  3,
                                  GL_FLOAT,
                                  GL_FALSE,
                                  0,
                                  NULL);
            glEnableVertexAttribArray(MST_ATTRIBUTE_POSITION);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        // Normal Buffer
        glGenBuffers(1, &vbo_Normal[0]);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_Normal[0]);
        {
            glBufferData(GL_ARRAY_BUFFER, sizeof(shape_normal0), shape_normal0, GL_STATIC_DRAW);
            
            glVertexAttribPointer(MST_ATTRIBUTE_NORMAL,
                                  3,
                                  GL_FLOAT,
                                  GL_FALSE,
                                  0,
                                  NULL
                                  );
            glEnableVertexAttribArray(MST_ATTRIBUTE_NORMAL);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
    }
    glBindVertexArray(0);
    
    //  Rectangle
    const GLfloat shape_position[] =
    {
        // Front
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,
         1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 1.0f,

        // Back
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        // Right
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,

        // Left
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

        // Top
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,

        // Bottom
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
    };
    
    const GLfloat shape_color[] =
    {
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,

        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
    };
    
    // Vertex array object for arrays of vertex attributes
    glGenVertexArrays(1, &vao[1]);
    glBindVertexArray(vao[1]);
    {
        // Position Buffer
        glGenBuffers(1, &vbo_Position[1]);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_Position[1]);
        {
            glBufferData(GL_ARRAY_BUFFER, sizeof(shape_position), shape_position, GL_STATIC_DRAW);
            
            glVertexAttribPointer(MST_ATTRIBUTE_POSITION,
                                  3,
                                  GL_FLOAT,
                                  GL_FALSE,
                                  0,
                                  NULL);
            glEnableVertexAttribArray(MST_ATTRIBUTE_POSITION);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    
        // Color Buffer
        glGenBuffers(1, &vbo_Color[1]);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_Color[1]);
        {
            glBufferData(GL_ARRAY_BUFFER, sizeof(shape_color), shape_color, GL_STATIC_DRAW);
            
            glVertexAttribPointer(MST_ATTRIBUTE_COLOR,
                                  3,
                                  GL_FLOAT,
                                  GL_FALSE,
                                  0,
                                  NULL
                                  );
            glEnableVertexAttribArray(MST_ATTRIBUTE_COLOR);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
    }
    glBindVertexArray(0);
    
    
    lightDiffuse[0] = 0.5f;
    lightDiffuse[1] = 0.5f;
    lightDiffuse[2] = 0.5f;
    lightDiffuse[3] = 1.0f;
    
    materialDiffuse[0] = 1.0f;
    materialDiffuse[1] = 1.0f;
    materialDiffuse[2] = 1.0f;
    materialDiffuse[3] = 1.0f;
    
    lightPosition[0] = 0.0f;
    lightPosition[1] = 0.0f;
    lightPosition[2] = 10.0f;
    lightPosition[3] = 1.0f;
    
    bLight = FALSE;
    bAnimate = FALSE;

    // depth
    glClearDepthf(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    rotate_angle = 0.0f;
    
    // clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    return(0);
}

-(void)printGLESInfo
{
    // Code
    printf("OpenGL-ES Vendor = %s\n", glGetString(GL_VENDOR));
    printf("OpenGL-ES Renderer = %s\n", glGetString(GL_RENDERER));
    printf("OpenGL-ES Version = %s\n", glGetString(GL_VERSION));
    printf("GLSL Version = %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

-(void)resize:(int)width :(int)height
{
    // Code
    if(height <= 0)
    {
        height = 1;
    }
    
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.01f, 100.0f);
}

-(void)display
{
    // Code
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(shaderProgramObject);
    
    // Transformations
    vmath::mat4 modelMatrix = vmath::mat4::identity();
    vmath::mat4 viewMatrix = vmath::mat4::identity();
    
    viewMatrix = vmath::lookat(vmath::vec3(0.0f, 0.0f, 6.0f),
                               vmath::vec3(0.0f, 0.0f, 0.0f),
                               vmath::vec3(0.0f, 1.0f, 0.0f));
    
    modelMatrix = vmath::translate(0.0f, 0.0f, 0.0f);
    modelMatrix *= vmath::rotate(rotate_angle, 0.0f, 1.0f, 0.0f);
    
    glUniformMatrix4fv(u_mMatrix, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(u_vMatrix, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(u_pMatrix, 1, GL_FALSE, perspectiveProjectionMatrix);
    
    if(bLight)
    {
        glUniform3fv(uLightDiffuse, 1, lightDiffuse);
        glUniform4fv(uLightPosition, 1, lightPosition);
        
        glUniform3fv(uMaterialDiffuse, 1, materialDiffuse);

        glUniform1i(ukey, 1);
    }
    else
    {
        glUniform1i(ukey, 0);
    }
    
    glBindVertexArray(vao[0]);
    {
        glDrawArrays(GL_TRIANGLES, 0, 3 * 4);
    }
    glBindVertexArray(0);
    
//    modelMatrix = vmath::translate(1.5f, 0.0f, -1.0f);
//    modelMatrix *= vmath::rotate(rotate_angle, 1.0f, 0.0f, 0.0f);
//    modelMatrix *= vmath::rotate(rotate_angle, 0.0f, 1.0f, 0.0f);
//    modelMatrix *= vmath::rotate(rotate_angle, 0.0f, 0.0f, 1.0f);
//    glUniformMatrix4fv(u_mMatrix, 1, GL_FALSE, modelMatrix);
//    glUniformMatrix4fv(u_vMatrix, 1, GL_FALSE, viewMatrix);
//    glUniformMatrix4fv(u_pMatrix, 1, GL_FALSE, perspectiveProjectionMatrix);
//    
//    glBindVertexArray(vao[1]);
//    {
//        glDrawArrays(GL_TRIANGLES, 0, 6 * 6);
//    }
//    glBindVertexArray(0);

    glUseProgram(0);
}

-(void)myupdate
{
    // Code
    
    if(bAnimate)
        rotate_angle += 0.5f;
    
    if(rotate_angle >= 360.0f)
    {
        rotate_angle -= 360.0f;
    }
}

-(void)uninitialize
{
    // Code
    if(depthRenderbuffer)
    {
        glDeleteRenderbuffers(1, &depthRenderbuffer);
        depthRenderbuffer = 0;
    }
    if(colorRenderbuffer)
    {
        glDeleteRenderbuffers(1, &colorRenderbuffer);
        colorRenderbuffer = 0;
    }
    if(framebuffer)
    {
        glDeleteFramebuffers(1, &framebuffer);
        framebuffer = 0;
    }
    
    if([EAGLContext currentContext] == eaglContext)
    {
        [EAGLContext setCurrentContext:nil];
        eaglContext = nil;
    }
}

-(void)dealloc
{
    // Code
    [self uninitialize];
    [self release];
    
    [super dealloc];
}

@end
