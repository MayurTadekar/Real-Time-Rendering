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

#define FBO_WIDTH   512
#define FBO_HEIGHT  512

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
    
    GLfloat lightDiffuse[4];
    GLfloat lightAmbient[4];
    GLfloat lightSpecular[4];
    GLfloat lightPosition[4];
    
    GLfloat materialDiffuse[4];
    GLfloat materialAmbient[4];
    GLfloat materialSpecular[4];
    GLfloat materialShininess;
    
    GLuint    gVertexShaderObjectCube;
    GLuint    gFragmentShaderObjectCube;
    GLuint    shaderProgramObjectCube;
    
    GLuint vaoCube;
    GLuint vboPositionCube;
    GLuint vboTexcoordsCube;
    GLuint mMatrixUniformCube;
    GLuint vMatrixUniformCube;
    GLuint pMatrixUniformCube;
    GLuint u_texture;
    
    int gWidth;
    int gHeight;
    
    GLuint fbo;
    GLuint rbo;
    GLuint textureFbo;
    int fboResult;
    
    vmath::mat4 perspectiveProjectionMatrix;
    vmath::mat4    perspectiveProjectionMatrixSphere;
    
}

-(void)awakeFromNib
{
    // Code
    [super awakeFromNib];
    
    [self setBackgroundColor:[UIColor blackColor]];
    
    CAEAGLLayer *eaglLayer = (CAEAGLLayer *)[super layer];
    
    [eaglLayer setOpaque:YES];
//    NSDictionary *dictionary = [NSDictionary dictionaryWithObjectsAndKeys:
//                                kEAGLDrawablePropertyRetainedBacking, [NSNumber numberWithBool:NO],
//                                 kEAGLDrawablePropertyColorFormat, kEAGLColorFormatRGBA8, nil];
    NSDictionary *dictionary = [NSDictionary dictionaryWithObjectsAndKeys:
                                [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking,
                                kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat,
                                nil];
                            
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
    
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    
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
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    [self resize:width :height];
    
    [self drawView:self];
    
    
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
}

-(void)initializeSphere
{
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
            glBufferData(GL_ARRAY_BUFFER, [sphere getNumberOfSphereVertices] * sizeof(GLfloat), [sphere getSphereVertex], GL_STATIC_DRAW);
            
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
    
    lightDiffuse[0] = 0.5f;
    lightDiffuse[1] = 0.5f;
    lightDiffuse[2] = 0.5f;
    lightDiffuse[3] = 1.0f;
    
    lightAmbient[0] = 0.1f;
    lightAmbient[1] = 0.1f;
    lightAmbient[2] = 0.1f;
    lightAmbient[3] = 1.0f;
    
    lightSpecular[0] = 1.0f;
    lightSpecular[1] = 1.0f;
    lightSpecular[2] = 1.0f;
    lightSpecular[3] = 1.0f;
    
    lightPosition[0] = 10.0f;
    lightPosition[1] = 10.0f;
    lightPosition[2] = 10.0f;
    lightPosition[3] = 1.0f;

    materialDiffuse[0] = 0.5f;
    materialDiffuse[1] = 0.2f;
    materialDiffuse[2] = 0.7f;
    materialDiffuse[3] = 1.0f;
    
    materialAmbient[0] = 0.1f;
    materialAmbient[1] = 0.1f;
    materialAmbient[2] = 0.1f;
    materialAmbient[3] = 1.0f;

    materialSpecular[0] = 1.0f;
    materialSpecular[1] = 1.0f;
    materialSpecular[2] = 1.0f;
    materialSpecular[3] = 1.0f;
    
    materialShininess = 20.0f;
    
    bLight = FALSE;
}

-(void)initializeCube
{
    gVertexShaderObjectCube = glCreateShader(GL_VERTEX_SHADER);

    const GLchar* vertexShaderSourceCode =
    "#version 300 es\n" \
    "precision highp float;\n" \
    "\n"                                        \
    "in vec4 vPosition;"                        \
    "in vec2 vTexCoord;"                        \
    "out vec2 ovTexCoord;"                        \
    "uniform mat4 u_mMatrix;"                    \
    "uniform mat4 u_vMatrix;"                    \
    "uniform mat4 u_pMatrix;"                    \
    "void main(void)"                            \
    "{    "                                        \
    "   gl_Position = u_pMatrix * u_vMatrix * u_mMatrix * vPosition;"    \
    "   ovTexCoord = vTexCoord;"                \
    "}    ";

    glShaderSource(    gVertexShaderObjectCube,
                    1,
                    (const GLchar**) &vertexShaderSourceCode,
                    NULL);

    glCompileShader(gVertexShaderObjectCube);

    GLint infoLogLength = 0;
    GLint shaderCompileStatus = 0;
    char *szBuffer = NULL;

    glGetShaderiv(    gVertexShaderObjectCube,
                    GL_COMPILE_STATUS,
                    &shaderCompileStatus);

    if(shaderCompileStatus == GL_FALSE){
        
        glGetShaderiv(    gVertexShaderObjectCube,
                        GL_INFO_LOG_LENGTH,
                        &infoLogLength);

        if(infoLogLength > 0){

            szBuffer = (char*)malloc(sizeof(char)*infoLogLength);

            if(szBuffer != NULL){

                GLsizei written;

                glGetShaderInfoLog(    gVertexShaderObjectCube,
                                    infoLogLength,
                                    &written,
                                    szBuffer);

                printf("Vertex Shader Compilation Log : %s\n",szBuffer);

                free(szBuffer);

            }
        }
        [self uninitialize];
    }

    gFragmentShaderObjectCube = glCreateShader(GL_FRAGMENT_SHADER);

    const GLchar* fragmentShaderSourceCode =
    "#version 300 es\n" \
    "precision highp float;\n" \
    "\n"                                            \
    "out vec4 FragColor;"                            \
    "in vec2 ovTexCoord;"                            \
    "uniform sampler2D textureSampler;"                            \
    "void main(void)"                                \
    "{    "                                            \
    "    FragColor = texture(textureSampler, ovTexCoord);"    \
    "}    ";

    glShaderSource(    gFragmentShaderObjectCube,
                    1,
                    (const GLchar**) &fragmentShaderSourceCode,
                    NULL);

    glCompileShader(gFragmentShaderObjectCube);

    infoLogLength = 0;
    shaderCompileStatus = 0;
    szBuffer = NULL;

    glGetShaderiv(    gFragmentShaderObjectCube,
                    GL_COMPILE_STATUS,
                    &shaderCompileStatus);

    if(shaderCompileStatus == GL_FALSE){
        
        glGetShaderiv(    gFragmentShaderObjectCube,
                        GL_INFO_LOG_LENGTH,
                        &infoLogLength);

        if(infoLogLength > 0){

            szBuffer = (char*)malloc(sizeof(char)*infoLogLength);

            if(szBuffer != NULL){

                GLsizei written;

                glGetShaderInfoLog(    gFragmentShaderObjectCube,
                                    infoLogLength,
                                    &written,
                                    szBuffer);

                printf("Fragment Shader Compilation Log : %s\n",szBuffer);

                free(szBuffer);

            }
        }
        [self uninitialize];
    }

    shaderProgramObjectCube = glCreateProgram();

    glAttachShader(    shaderProgramObjectCube,
                    gVertexShaderObjectCube);

    glAttachShader(    shaderProgramObjectCube,
                    gFragmentShaderObjectCube);

    glBindAttribLocation(    shaderProgramObjectCube,
                            MST_ATTRIBUTE_POSITION,
                            "vPosition");

    glBindAttribLocation(    shaderProgramObjectCube,
                            MST_ATTRIBUTE_TEXCOORDS0,
                            "vTexCoord");

    glLinkProgram(shaderProgramObjectCube);

    GLint shaderProgramLinkStatus = 0;

    glGetProgramiv(    shaderProgramObjectCube,
                    GL_LINK_STATUS,
                    &shaderProgramLinkStatus);

    if(shaderProgramLinkStatus == GL_FALSE){
        
        glGetShaderiv(    shaderProgramObjectCube,
                        GL_INFO_LOG_LENGTH,
                        &infoLogLength);

        if(infoLogLength > 0){

            szBuffer = (char*)malloc(sizeof(char)*infoLogLength);

            if(szBuffer != NULL){

                GLsizei written;

                glGetProgramInfoLog(shaderProgramObjectCube,
                                    infoLogLength,
                                    &written,
                                    szBuffer);

                printf("Shader Program Link log : %s\n",szBuffer);

                free(szBuffer);

                [self release];
            }
        }
    }

    mMatrixUniformCube = glGetUniformLocation(shaderProgramObjectCube,"u_mMatrix");
    vMatrixUniformCube = glGetUniformLocation(shaderProgramObjectCube,"u_vMatrix");
    pMatrixUniformCube = glGetUniformLocation(shaderProgramObjectCube,"u_pMatrix");
    u_texture = glGetUniformLocation(shaderProgramObjectCube,"textureSampler");
    
    const GLfloat rectVertices[] = {

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

    const GLfloat cubeTexCoord[] = {
        // FRONT
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,

        // BACK
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,

        // RIGHT
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,

        // LEFT
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,

        // TOP
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,

        // BOTTOM
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
    };


    glGenVertexArrays(1,&vaoCube);
    glBindVertexArray(vaoCube);
    {
        glGenBuffers(1,&vboPositionCube);
        glBindBuffer(GL_ARRAY_BUFFER,vboPositionCube);
        glBufferData(GL_ARRAY_BUFFER,sizeof(rectVertices),rectVertices,GL_STATIC_DRAW);

        glVertexAttribPointer(    MST_ATTRIBUTE_POSITION,
                                3,
                                GL_FLOAT,
                                GL_FALSE,
                                0,
                                NULL);


        glEnableVertexAttribArray(MST_ATTRIBUTE_POSITION);

        glGenBuffers(1,&vboTexcoordsCube);
        glBindBuffer(GL_ARRAY_BUFFER,vboTexcoordsCube);
        glBufferData(GL_ARRAY_BUFFER,sizeof(cubeTexCoord),cubeTexCoord,GL_STATIC_DRAW);

        glVertexAttribPointer(    MST_ATTRIBUTE_TEXCOORDS0,
                                2,
                                GL_FLOAT,
                                GL_FALSE,
                                0,
                                NULL);


        glEnableVertexAttribArray(MST_ATTRIBUTE_TEXCOORDS0);

        glBindBuffer(GL_ARRAY_BUFFER,0);
    }
    glBindVertexArray(0);
}

-(BOOL)createAndPrepareFBOForDrawing:(GLint)textureWidth :(GLint) textureHeight
{
    // variable declarations
    GLint maxRenderBufferSize;

    // Code
    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderBufferSize);
    
    if(maxRenderBufferSize <= textureWidth || maxRenderBufferSize <= textureHeight)
    {
        printf("FBO WIDTH or FBO HEIGHT are excedding MAX RENDERBUFFER SIZE!\n");
        return(NO);
    }

    glGenFramebuffers(1, &fbo);
    
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    {
        glGenRenderbuffers(1, &rbo);
        
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        {
            glRenderbufferStorage(    GL_RENDERBUFFER,
                                  GL_DEPTH_COMPONENT16,
                                  textureWidth,
                                  textureHeight);
            
            glGenTextures(1, &textureFbo);
            
            glBindTexture(GL_TEXTURE_2D, textureFbo);
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RGBA,
                textureWidth,
                textureHeight,
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                NULL);
//            glGenerateMipmap(GL_TEXTURE_2D);
            
            glFramebufferTexture2D(    GL_FRAMEBUFFER,
                                   GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_2D,
                                   textureFbo,
                                   0);
            glFramebufferRenderbuffer(    GL_FRAMEBUFFER,
                                      GL_DEPTH_ATTACHMENT,
                                      GL_RENDERBUFFER,
                                      rbo);
            
            if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                printf("FBO Creation Incomplete!!\n");
                return(FALSE);
            }
        }
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return(YES);
}
-(int)initialize
{
    // Code
    [self printGLESInfo];
    
    printf("Initializing Cube\n");
    [self initializeCube];
    printf("Initialized Cube\n");
    
    if([self createAndPrepareFBOForDrawing:FBO_WIDTH :FBO_HEIGHT] == YES)
    {
        printf("FBO Creation successful!!\n");
    }
    else
    {
        printf("FBO Creation failed!!\n");
        return(-8);
    }
    
    printf("Initializing Sphere\n");
    [self initializeSphere];
    printf("Initialized Sphere\n");
    
    // depth
    glClearDepthf(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    rotate_angle = 0.0f;
    bRotate = FALSE;
    
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
    
    gWidth = width;
    gHeight = height;
    
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);

    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.01f, 10000.0f);
}

-(void)resizeSphere:(int)width :(int)height
{

    if( height < 0 )
    {
        height = 1;
    }

    glViewport(0, 0, (GLsizei)width, (GLsizei)height);

    perspectiveProjectionMatrixSphere = vmath::perspective(  45.0f,
                                                        (GLfloat)width/(GLfloat)height,
                                                        0.1f,
                                                        100.0f);
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

//-(void)display
//{
//    [self displaySphere];
//    
//    [self resize:gWidth :gHeight];
//    
////    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
////
////    glViewport(0, 0, gWidth, gHeight);
//    
//    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//    glUseProgram(shaderProgramObjectCube);
//
//    vmath::mat4 modelMatrix = vmath::mat4::identity();
//    vmath::mat4 viewMatrix = vmath::lookat(
//        vmath::vec3(0.0f, 0.0f, 20.0f),
//        vmath::vec3(0.0f, 0.0f, 0.0f),
//        vmath::vec3(0.0f, 1.0f, 0.0f)
//    );
//
//    modelMatrix *= vmath::rotate(rotate_angle, 1.0f, 0.0f, 0.0f);
//    modelMatrix *= vmath::rotate(rotate_angle, 0.0f, 1.0f, 0.0f);
//    modelMatrix *= vmath::rotate(rotate_angle, 0.0f, 0.0f, 1.0f);
//
//    glUniformMatrix4fv(mMatrixUniformCube, 1, GL_FALSE, modelMatrix);
//    glUniformMatrix4fv(vMatrixUniformCube, 1, GL_FALSE, viewMatrix);
//    glUniformMatrix4fv(pMatrixUniformCube, 1, GL_FALSE, perspectiveProjectionMatrix);
//
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, textureFbo);
//    glUniform1i(u_texture, 0);
//
//    glBindVertexArray(vaoCube);
//    glDrawArrays(GL_TRIANGLES, 0, 36);
//    glBindVertexArray(0);
//
//    glUseProgram(0);
//}

-(void)display
{
    // Render sphere into texture
    [self displaySphere];

    // Bind default framebuffer again
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Force viewport reset using actual renderbuffer dimensions
    GLint w, h;
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &w);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &h);
    glViewport(0, 0, w, h);

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    
    // Clear entire screen
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgramObjectCube);

    vmath::mat4 modelMatrix = vmath::mat4::identity();

    vmath::mat4 viewMatrix = vmath::lookat(
        vmath::vec3(0.0f, 0.0f, 5.0f),
        vmath::vec3(0.0f, 0.0f, 0.0f),
        vmath::vec3(0.0f, 1.0f, 0.0f)
    );

    modelMatrix *= vmath::rotate(rotate_angle, 1.0f, 0.0f, 0.0f);
    modelMatrix *= vmath::rotate(rotate_angle, 0.0f, 1.0f, 0.0f);
    modelMatrix *= vmath::rotate(rotate_angle, 0.0f, 0.0f, 1.0f);

    glUniformMatrix4fv(mMatrixUniformCube, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(vMatrixUniformCube, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(pMatrixUniformCube, 1, GL_FALSE, perspectiveProjectionMatrix);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureFbo);
    glUniform1i(u_texture, 0);

    glBindVertexArray(vaoCube);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glUseProgram(0);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

-(void)displaySphere{
    
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
    [self resizeSphere:FBO_WIDTH :FBO_HEIGHT];
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    glUseProgram(shaderProgramObject);
    
    vmath::mat4 modelMatrix = vmath::mat4::identity();
    
    vmath::mat4 viewMatrix = vmath::lookat(vmath::vec3(0.0f, 0.0f, 2.0f),
                                           vmath::vec3(0.0f, 0.0f, 0.0f),
                                           vmath::vec3(0.0f, 1.0f, 0.0f));
    
    
    modelMatrix = vmath::mat4::identity();
    modelMatrix *= vmath::translate(0.0f, 0.0f, 0.0f);
    modelMatrix *= vmath::rotate(rotate_angle, 1.0f, 0.0f, 0.0f);
    modelMatrix *= vmath::rotate(rotate_angle, 0.0f, 1.0f, 0.0f);
    modelMatrix *= vmath::rotate(rotate_angle, 0.0f, 0.0f, 1.0f);
    
    glUniformMatrix4fv(u_mMatrix, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(u_vMatrix, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(u_pMatrix, 1, GL_FALSE, perspectiveProjectionMatrixSphere);
    
    if(YES == bLight)
    {
        glUniform3fv(uLightDiffuse, 1, lightDiffuse);
        glUniform3fv(uLightSpecular, 1, lightSpecular);
        glUniform3fv(uLightAmbient, 1, lightAmbient);
        glUniform4fv(uLightPosition, 1, lightPosition);
        
        glUniform3fv(uMaterialDiffuse, 1, materialDiffuse);
        glUniform3fv(uMaterialSpecular, 1, materialSpecular);
        glUniform3fv(uMaterialAmbient, 1, materialAmbient);
        glUniform1f(uMaterialShininess, materialShininess);
        
        glUniform1i(ukey, 1);
    }
    else
    {
        glUniform1i(ukey, 0);
    }
    
    glBindVertexArray(vao[0]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, [sphere getNumberOfSphereVertices] / 3);
    glBindVertexArray(0);
    
    glUseProgram(0);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

-(void)myupdate
{
    // Code
    
    if(bRotate)
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
    [super dealloc];
}

@end
