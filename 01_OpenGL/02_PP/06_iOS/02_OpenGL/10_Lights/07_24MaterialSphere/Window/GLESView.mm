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
#import "Sphere.h"

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
    
    GLuint  uLightDiffuse;
    GLuint  uLightSpecular;
    GLuint  uLightAmbient;
    GLuint  uLightPosition;

    GLuint  uMaterialDiffuse;
    GLuint  uMaterialSpecular;
    GLuint  uMaterialAmbient;
    GLuint  uMaterialShininess;
    
    GLuint ukey;
    
    GLuint vao[2];
    GLuint vbo_Position[2];
    GLuint vbo_Color[2];
    GLuint vbo_Normal[2];
    
    Sphere *sphere;
    
    GLuint u_mMatrix;
    GLuint u_vMatrix;
    GLuint u_pMatrix;
    
    float rotate_angle;
    BOOL bRotate;
    
    BOOL bLight;
    
    int rotation_axis;
    
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
    bRotate = !bRotate;
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
    rotation_axis++;
    
    if(rotation_axis > 2)
        rotation_axis = 0;
}

-(int)initialize
{
    // Code
    [self printGLESInfo];
    
    // Vertex Shader
    const GLchar* vertexShaderSourceCode =
    "#version 300 es\n" \
    "precision highp float;\n" \
    "in vec4 vPosition;\n"  \
    "in vec3 vNormal;\n"  \
    "uniform mat4 u_mMatrix;"                                                                                       \
    "uniform mat4 u_vMatrix;"                                                                                        \
    "uniform mat4 u_pMatrix;"                                                                                  \
    "uniform int  u_lKeyPressed;"                                                                                       \
    "uniform vec4  u_LightPosition;"                                                                                    \
    "out vec3 phong_ads_light;"                                                                                         \
    "out vec3 transformed_normal;"                                                                                      \
    "out vec3 light_direction;"                                                                                         \
    "out vec3 view_vector;"                                                                                             \
    "void main(void)"                                                                                                   \
    "{"                                                                                                                 \
    "  if( u_lKeyPressed == 1 ) "                                                                                       \
    "  {   "                                                                                                            \
    "   vec4 eye_coord =  u_vMatrix * u_mMatrix * vPosition;   "                                                 \
    "   transformed_normal = mat3(u_vMatrix * u_mMatrix) * vNormal;"                                             \
    "   light_direction = vec3(u_LightPosition - eye_coord);"                                                           \
    "   view_vector = -eye_coord.xyz;"                                                                                  \
    "   }"                                                                                                              \
    "   gl_Position = u_pMatrix * u_vMatrix * u_mMatrix * vPosition;"                                   \
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
                printf("Vertex Shader: %s", szInfoLog);
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
    "in vec3 transformed_normal;"                                                                                               \
    "in vec3 light_direction;"                                                                                                  \
    "in vec3 view_vector;"                                                                                                      \
    "out vec4 Fragcolor;"                                                                                                      \
    "vec3 phong_ads_light;"                                                                                                     \
    "uniform vec3  u_la;"                                                                                                       \
    "uniform vec3  u_ld;"                                                                                                       \
    "uniform vec3  u_ls;"                                                                                                       \
    "uniform vec3  u_ka;"                                                                                                       \
    "uniform vec3  u_kd;"                                                                                                       \
    "uniform vec3  u_ks;"                                                                                                       \
    "uniform float  u_kshiny;"                                                                                                  \
    "uniform highp int  u_lKeyPressed;"                                                                                               \
    "void main(void)"                                                                                                           \
    "{"                                                                                                                         \
    "  if( u_lKeyPressed == 1 ) "                                                                                               \
    "   {   "                                                                                                                   \
    "   vec3 normalized_transformed_normal = normalize(transformed_normal);"                                                    \
    "   vec3 normalized_light_direction = normalize(light_direction);"                                                          \
    "   vec3 normalized_view_vector = normalize(view_vector);"                                                                  \
    "   vec3 reflection_vector = reflect( -normalized_light_direction, normalized_transformed_normal);"                         \
    "   vec3 intensity_ambient = u_la * u_ka;"                                                                                  \
    "   vec3 intensity_diffuse = u_ld * u_kd * max( dot(normalized_light_direction, normalized_transformed_normal), 0.0f);"     \
    "   vec3 intensity_specular = u_ls * u_ks * pow( max( dot( reflection_vector, normalized_view_vector ), 0.0f), u_kshiny);"  \
    "   phong_ads_light = intensity_ambient + intensity_diffuse + intensity_specular;"                                  \
    "   } else {"                                                                                                       \
    "       phong_ads_light = vec3(1.0f, 1.0f, 1.0f);"                                                                  \
    "   }"                                                                                                              \
    "   Fragcolor = vec4(phong_ads_light, 1.0f);"   \
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
                printf("Fragment Shader: %s", szInfoLog);
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
                printf("Shader Program: %s", szInfoLog);
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
    uLightSpecular = glGetUniformLocation(shaderProgramObject,"u_ls");
    uLightAmbient = glGetUniformLocation(shaderProgramObject,"u_la");
    uLightPosition = glGetUniformLocation(shaderProgramObject,"u_LightPosition");

    uMaterialDiffuse = glGetUniformLocation(shaderProgramObject,"u_kd");
    uMaterialSpecular = glGetUniformLocation(shaderProgramObject,"u_ks");
    uMaterialAmbient = glGetUniformLocation(shaderProgramObject,"u_ka");
    uMaterialShininess = glGetUniformLocation(shaderProgramObject,"u_kshiny");

    ukey = glGetUniformLocation(shaderProgramObject, "u_lKeyPressed");
    
    bRotate = FALSE;
    
    sphere = [[Sphere alloc] init];
    [sphere initialize:0.5 :18 :18];
    
    // Vertex array object for arrays of vertex attributes
    glGenVertexArrays(1, &vao[0]);
    glBindVertexArray(vao[0]);
    {
        // Position Buffer
        glGenBuffers(1, &vbo_Position[0]);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_Position[0]);
        {
            glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * [sphere getNumberOfSphereVertices], [sphere getSphereVertex], GL_STATIC_DRAW);
            
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
            glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * [sphere getNumberOfSphereNormal], [sphere getSphereNormal], GL_STATIC_DRAW);
            
            glVertexAttribPointer(MST_ATTRIBUTE_NORMAL,
                                  3,
                                  GL_FLOAT,
                                  GL_FALSE,
                                  0,
                                  NULL);
            glEnableVertexAttribArray(MST_ATTRIBUTE_NORMAL);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(0);
    
    
    bLight = FALSE;
    rotation_axis = 0;
    
    // depth
    glClearDepthf(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    rotate_angle = 0.0f;
    bRotate = FALSE;
    
    // clear color
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
    
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
    
    perspectiveProjectionMatrix = vmath::perspective(35.0f, (GLfloat)width / (GLfloat)height, 0.01f, 10000.0f);
}

-(void)display
{
    // Variables
    GLfloat lightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat lightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat lightAmbient[] = {0.1f, 0.1f, 0.1f, 1.0f};
    GLfloat lightPosition[] = {100.0f, 100.0f, 100.0f, 1.0f};

    GLfloat materialAmbient[] = {   0.0215,0.1745,0.0215,1.0f,
                                    0.135,0.2225,0.1575,1.0f,
                                    0.05375,0.05,0.06625,1.0f,
                                    0.25,0.20725,0.20725,1.0f,
                                    0.1745,0.01175,0.01175,1.0f,
                                    0.1,0.18725,0.1745,1.0f,
                                    0.329412,0.223529,0.027451,1.0f,
                                    0.2125,0.1275,0.054,1.0f,
                                    0.25,0.25,0.25,1.0f,
                                    0.19125,0.0735,0.0225,1.0f,
                                    0.24725,0.1995,0.0745,1.0f,
                                    0.19225,0.19225,0.19225,1.0f,
                                    0.0,0.0,0.0,1.0f,
                                    0.0,0.1,0.06,1.0f,
                                    0.0,0.0,0.0,1.0f,
                                    0.0,0.0,0.0,1.0f,
                                    0.0,0.0,0.0,1.0f,
                                    0.0,0.0,0.0,1.0f,
                                    0.02,0.02,0.02,1.0f,
                                    0.0,0.05,0.05,1.0f,
                                    0.0,0.05,0.0,1.0f,
                                    0.05,0.0,0.0,1.0f,
                                    0.05,0.05,0.05,1.0f,
                                    0.05,0.05,0.0,1.0f
                                };
    GLfloat materialDiffuse[] = {   0.07568,0.61424,0.07568,1.0f,
                                        0.54,0.89,0.63,1.0f,
                                        0.18275,0.17,0.22525,1.0f,
                                        1.0,0.829,0.829,1.0f,
                                        0.61424,0.04136,0.04136,1.0f,
                                        0.396,0.74151,0.69102,1.0f,
                                        0.780392,0.568627,0.113725,1.0f,
                                        0.714,0.4284,0.18144,1.0f,
                                        0.4,0.4,0.4,1.0f,
                                        0.7038,0.27048,0.0828,1.0f,
                                        0.75164,0.60648,0.22648,1.0f,
                                        0.50754,0.50754,0.50754,1.0f,
                                        0.01,0.01,0.01,1.0f,
                                        0.0,0.50980392,0.50980392,1.0f,
                                        0.1,0.35,0.1,1.0f,
                                        0.5,0.0,0.0,1.0f,
                                        0.55,0.55,0.55,1.0f,
                                        0.5,0.5,0.0,1.0f,
                                        0.01,0.01,0.01,1.0f,
                                        0.4,0.5,0.5,1.0f,
                                        0.4,0.5,0.4,1.0f,
                                        0.5,0.4,0.4,1.0f,
                                        0.5,0.5,0.5,1.0f,
                                        0.5,0.5,0.4,1.0f
                                    };
    GLfloat materialSpecular[] = {  0.633,0.727811,0.633,1.0f,
                                        0.316228,0.316228,0.316228,1.0f,
                                        0.332741,0.328634,0.346435,1.0f,
                                        0.296648,0.296648,0.296648,1.0f,
                                        0.727811,0.626959,0.626959,1.0f,
                                        0.297254,0.30829,0.306678,1.0f,
                                        0.992157,0.941176,0.807843,1.0f,
                                        0.393548,0.271906,0.166721,1.0f,
                                        0.774597,0.774597,0.774597,1.0f,
                                        0.256777,0.137622,0.086014,1.0f,
                                        0.628281,0.555802,0.366065,1.0f,
                                        0.508273,0.508273,0.508273,1.0f,
                                        0.50,0.50,0.50,1.0f,
                                        0.50196078,0.50196078,0.50196078,1.0f,
                                        0.45,0.55,0.45,1.0f,
                                        0.7,0.6,0.6,1.0f,
                                        0.70,0.70,0.70,1.0f,
                                        0.60,0.60,0.50,1.0f,
                                        0.4,0.4,0.4,1.0f,
                                        0.04,0.7,0.7,1.0f,
                                        0.04,0.7,0.04,1.0f,
                                        0.7,0.04,0.04,1.0f,
                                        0.7,0.7,0.7,1.0f,
                                        0.7,0.7,0.04,1.0f
                                    };
    GLfloat materialShininess[] = { 0.6 * 128,
                                            0.1 * 128,
                                            0.3 * 128,
                                            0.088 * 128,
                                            0.6 * 128,
                                            0.1 * 128,
                                            0.21794872 * 128,
                                            0.2 * 128,
                                            0.6 * 128,
                                            0.1 * 128,
                                            0.4 * 128,
                                            0.4 * 128,
                                            0.25 * 128,
                                            0.25 * 128,
                                            0.25 * 128,
                                            0.25 * 128,
                                            0.25 * 128,
                                            0.25 * 128,
                                            0.078125 * 128,
                                            0.078125 * 128,
                                            0.078125 * 128,
                                            0.078125 * 128,
                                            0.078125 * 128,
                                            0.078125 * 128
                                        };

    
    // Code
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(shaderProgramObject);
    
    // Transformations
    vmath::mat4 modelMatrix = vmath::mat4::identity();
    vmath::mat4 viewMatrix = vmath::mat4::identity();
    
    viewMatrix = vmath::lookat(vmath::vec3(3.5f, 1.40f * 1.5, 10.0f),
                               vmath::vec3(3.5f, 1.40f * 1.5, 0.0f),
                               vmath::vec3(0.0f, 1.0f, 0.0f));
    
    modelMatrix *= vmath::rotate(rotate_angle, 1.0f, 0.0f, 0.0f);
    modelMatrix *= vmath::rotate(rotate_angle, 0.0f, 1.0f, 0.0f);
    modelMatrix *= vmath::rotate(rotate_angle, 0.0f, 0.0f, 1.0f);
    
    if(bLight)
    {
        glUniform3fv(uLightDiffuse, 1, lightDiffuse);
        glUniform3fv(uLightSpecular, 1, lightSpecular);
        glUniform3fv(uLightAmbient, 1, lightAmbient);

        switch(rotation_axis)
        {
        
        case 0:
            lightPosition[0] = 2.25f;
            lightPosition[1] = 20.0f * sinf(rotate_angle);
            lightPosition[2] = 20.0f * cosf(rotate_angle);
            lightPosition[3] = 1.0f;
            break;
        
        case 1:
            lightPosition[0] = 2.25f + 20.0f * sinf(rotate_angle);
            lightPosition[1] = 0.0f;
            lightPosition[2] = 20.0f * cosf(rotate_angle);
            lightPosition[3] = 1.0f;
            break;
        
        case 2:
            lightPosition[0] = 2.25f + 20.0f * sinf(rotate_angle);
            lightPosition[1] = 20.0f * cosf(rotate_angle);
            lightPosition[2] = 0.0f;
            lightPosition[3] = 1.0f;
            break;
        }

        glUniform4fv(uLightPosition, 1, lightPosition);
        glUniform1i(ukey, 1);
    }
    else
    {
        glUniform1i(ukey, 0);
    }

    
    if(sphere)
    {
        for(int le = 0; le < 6 * 4; ++le)
        {
            
            modelMatrix = vmath::mat4::identity();
            modelMatrix *= vmath::translate(1.5f * (le % 6), 1.5f * (le / 6), 0.0f);
            glUniformMatrix4fv(u_mMatrix, 1, GL_FALSE, modelMatrix);
            glUniformMatrix4fv(u_vMatrix, 1, GL_FALSE, viewMatrix);
            glUniformMatrix4fv(u_pMatrix, 1, GL_FALSE, perspectiveProjectionMatrix);

            glUniform3fv(uMaterialDiffuse, 1, &materialDiffuse[le * 4]);
            glUniform3fv(uMaterialSpecular, 1, &materialSpecular[le * 4]);
            glUniform3fv(uMaterialAmbient, 1, &materialAmbient[le * 4]);
            glUniform1f(uMaterialShininess, materialShininess[le]);

            glBindVertexArray(vao[0]);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, [sphere getNumberOfSphereVertices] / 3);
            glBindVertexArray(0);
        }
    }
    
    glUseProgram(0);
}

-(void)myupdate
{
    // Code
    
    if(bRotate)
        rotate_angle += 0.1f;
    
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
