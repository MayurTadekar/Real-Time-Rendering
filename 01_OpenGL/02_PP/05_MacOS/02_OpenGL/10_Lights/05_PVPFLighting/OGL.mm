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
#import        "Sphere.h"

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

    [window setTitle:@"MST: PVPF Sphere"];

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


GLfloat lightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat lightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat lightAmbient[] = {0.1f, 0.1f, 0.1f, 1.0f};
GLfloat lightPosition[] = {100.0f, 100.0f, 100.0f, 1.0f};

GLfloat materialDiffuse[] = {0.5f, 0.2f, 0.7f, 1.0f};
GLfloat materialSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat materialAmbient[] = {0.1f, 0.1f, 0.1f, 1.0f};
GLfloat materialShininess = 20;

BOOL bLight = FALSE;
BOOL isFragment = FALSE;
BOOL bAnimate = FALSE;

@implementation MyOpenGLView{
    @private
        CVDisplayLinkRef displayLink;

        GLuint    gVertexShaderObject;
        GLuint    gFragmentShaderObject;
        GLuint    gShaderProgramObject;

        GLuint    vao;
        GLuint    vbo_Position;
        GLuint    vbo_Normal;
        GLuint    mMatrixUniform;
        GLuint    vMatrixUniform;
        GLuint    pMatrixUniform;

        GLuint  uLightDiffuse;
        GLuint  uLightSpecular;
        GLuint  uLightAmbient;
        GLuint  uLightPosition;

        GLuint  uMaterialDiffuse;
        GLuint  uMaterialSpecular;
        GLuint  uMaterialAmbient;
        GLuint  uMaterialShininess;

        GLuint ukey;

        Sphere* sphere;

        float   rotationAngle;

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
    "in vec4 vPosition;\n"                                                          \
    "in vec3 vNormal;\n"                                                            \
    "uniform mat4 u_mMatrix;\n"                                             \
    "uniform mat4 u_vMatrix;\n"                                             \
    "uniform mat4 u_pMatrix;\n"                                            \
    "uniform int  u_lKeyPressed;"                                                                                       \
    "uniform vec3  u_la;"                                                                                               \
    "uniform vec3  u_ld;"                                                                                               \
    "uniform vec3  u_ls;"                                                                                               \
    "uniform vec4  u_LightPosition;"                                                                                    \
    "uniform vec3  u_ka;"                                                                                               \
    "uniform vec3  u_kd;"                                                                                               \
    "uniform vec3  u_ks;"                                                                                               \
    "uniform float  u_kshiny;"                                                                                          \
    "out vec3 phong_ads_light;"                                                                                         \
    "out vec3 transformed_normal;"                                                                                      \
    "out vec3 light_direction;"                                                                                         \
    "out vec3 view_vector;"                                                                                             \
    "void main(void)"                                                                                                   \
    "{"                                                                                                                 \
    "  if( u_lKeyPressed == 1 ) "                                                                                       \
    "  {   "                                                                                                            \
    "   vec4 eye_coord =  u_vMatrix * u_mMatrix * vPosition;   "                                                 \
    "   vec3 transformed_normal = normalize( mat3(u_vMatrix * u_mMatrix) * vNormal);"                            \
    "   vec3 light_direction = normalize( vec3(u_LightPosition - eye_coord) );"                                         \
    "   vec3 reflection_vector = reflect( -light_direction, transformed_normal);"                                       \
    "   vec3 view_vector = normalize(-eye_coord.xyz);"                                                                  \
    "   vec3 intensity_ambient = u_la * u_ka;"                                                                          \
    "   vec3 intensity_diffuse = u_ld * u_kd * max( dot(light_direction, transformed_normal), 0.0f);"                   \
    "   vec3 intensity_specular = u_ls * u_ks * pow( max( dot( reflection_vector, view_vector ), 0.0f), u_kshiny);"     \
    "   phong_ads_light = intensity_ambient + intensity_diffuse + intensity_specular;"                                  \
    "   }"
    "   else if( u_lKeyPressed == 2 ) "                                                                                       \
    "   {   "                                                                                                            \
    "   vec4 eye_coord =  u_vMatrix * u_mMatrix * vPosition;   "                                                 \
    "   transformed_normal = mat3(u_vMatrix * u_mMatrix) * vNormal;"                                             \
    "   light_direction = vec3(u_LightPosition - eye_coord);"                                                           \
    "   view_vector = -eye_coord.xyz;"                                                                                  \
    "   } else {"                                                                                                       \
    "       phong_ads_light = vec3(1.0f, 1.0f, 1.0f);"                                                                  \
    "   }"                                                                                                              \
    "   gl_Position = u_pMatrix * u_vMatrix * u_mMatrix * vPosition;"                                   \
    "}";

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
    "out vec4 Fragcolor;"                           \
    "in vec3 transformed_normal;"                                                                                               \
    "in vec3 light_direction;"                                                                                                  \
    "in vec3 view_vector;"                                                                                                      \
    "in vec3 phong_ads_light;"                                                                                                     \
    "vec3 perFragLight;"                                                                                                     \
    "uniform vec3  u_la;"                                                                                                       \
    "uniform vec3  u_ld;"                                                                                                       \
    "uniform vec3  u_ls;"                                                                                                       \
    "uniform vec3  u_ka;"                                                                                                       \
    "uniform vec3  u_kd;"                                                                                                       \
    "uniform vec3  u_ks;"                                                                                                       \
    "uniform float  u_kshiny;"                                                                                                  \
    "uniform int  u_lKeyPressed;"                                                                                               \
    "void main(void)"                                                                                                           \
    "{"         
    "   if( u_lKeyPressed == 1 ) "                                                                                              \
    "   { "
    "       Fragcolor = vec4(phong_ads_light, 1.0f);"                                                                                                                   \
    "   } "
    "   else if( u_lKeyPressed == 2 ) "                                                                                              \
    "   {   "                                                                                                                   \
    "   vec3 normalized_transformed_normal = normalize(transformed_normal);"                                                    \
    "   vec3 normalized_light_direction = normalize(light_direction);"                                                          \
    "   vec3 normalized_view_vector = normalize(view_vector);"                                                                  \
    "   vec3 reflection_vector = reflect( -normalized_light_direction, normalized_transformed_normal);"                         \
    "   vec3 intensity_ambient = u_la * u_ka;"                                                                                  \
    "   vec3 intensity_diffuse = u_ld * u_kd * max( dot(normalized_light_direction, normalized_transformed_normal), 0.0f);"     \
    "   vec3 intensity_specular = u_ls * u_ks * pow( max( dot( reflection_vector, normalized_view_vector ), 0.0f), u_kshiny);"  \
    "   perFragLight = intensity_ambient + intensity_diffuse + intensity_specular;"                                         \
    "   Fragcolor = vec4(perFragLight, 1.0f);"   \
    "   } else {"                                                                                       \
    "       Fragcolor = vec4(1.0f);"                                                                        \
    "   }"                                                                                                                      \
    "}";

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
                            MST_ATTRIBUTES_NORMAL,
                            "vNormal");

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

    mMatrixUniform = glGetUniformLocation(gShaderProgramObject,"u_mMatrix");
    vMatrixUniform = glGetUniformLocation(gShaderProgramObject,"u_vMatrix");
    pMatrixUniform = glGetUniformLocation(gShaderProgramObject,"u_pMatrix");

    uLightDiffuse = glGetUniformLocation(gShaderProgramObject,"u_ld");
    uLightSpecular = glGetUniformLocation(gShaderProgramObject,"u_ls");
    uLightAmbient = glGetUniformLocation(gShaderProgramObject,"u_la");
    uLightPosition = glGetUniformLocation(gShaderProgramObject,"u_LightPosition");

    uMaterialDiffuse = glGetUniformLocation(gShaderProgramObject,"u_kd");
    uMaterialSpecular = glGetUniformLocation(gShaderProgramObject,"u_ks");
    uMaterialAmbient = glGetUniformLocation(gShaderProgramObject,"u_ka");
    uMaterialShininess = glGetUniformLocation(gShaderProgramObject,"u_kshiny");

    ukey = glGetUniformLocation(gShaderProgramObject, "u_lKeyPressed");

    sphere = [[Sphere alloc] init];
    [sphere initialize:0.5 :28 :28];

    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);
    {
        glGenBuffers(1,&vbo_Position);
        glBindBuffer(GL_ARRAY_BUFFER,vbo_Position);
        glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat) * [sphere getNumberOfSphereVertices], [sphere getSphereVertex], GL_STATIC_DRAW);

        glVertexAttribPointer(    MST_ATTRIBUTES_POSITION,
                                3,
                                GL_FLOAT,
                                GL_FALSE,
                                0,
                                NULL);


        glEnableVertexAttribArray(MST_ATTRIBUTES_POSITION);

        glGenBuffers(1,&vbo_Normal);
        glBindBuffer(GL_ARRAY_BUFFER,vbo_Normal);
        glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat) * [sphere getNumberOfSphereNormal], [sphere getSphereNormal], GL_STATIC_DRAW);

        glVertexAttribPointer(    MST_ATTRIBUTES_NORMAL,
                                3,
                                GL_FLOAT,
                                GL_FALSE,
                                0,
                                NULL);


        glEnableVertexAttribArray(MST_ATTRIBUTES_NORMAL);
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

    [self display];

    [self myUpdate];

    CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);

    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
}

-(void)display{
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    glUseProgram(gShaderProgramObject);

    mat4 modelMatrix = mat4::identity();
    
    mat4 viewMatrix = vmath::lookat(vmath::vec3(0.0f, 0.0f, 2.0f),
                                    vmath::vec3(0.0f, 0.0f, 0.0f),
                                    vmath::vec3(0.0f, 1.0f, 0.0f));


    modelMatrix = mat4::identity();
    modelMatrix *= vmath::translate(0.0f, 0.0f, 0.0f);
    modelMatrix *= vmath::rotate(rotationAngle, 1.0f, 0.0f, 0.0f);
    modelMatrix *= vmath::rotate(rotationAngle, 0.0f, 1.0f, 0.0f);
    modelMatrix *= vmath::rotate(rotationAngle, 0.0f, 0.0f, 1.0f);

    glUniformMatrix4fv(mMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(vMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(pMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

    if(YES == bLight)
    {
        glUniform3fv(uLightDiffuse, 1, lightDiffuse);
        glUniform3fv(uLightSpecular, 1, lightSpecular);
        glUniform3fv(uLightAmbient, 1, lightAmbient);
        glUniform3fv(uLightPosition, 1, lightPosition);

        glUniform3fv(uMaterialDiffuse, 1, materialDiffuse);
        glUniform3fv(uMaterialSpecular, 1, materialSpecular);
        glUniform3fv(uMaterialAmbient, 1, materialAmbient);
        glUniform1f(uMaterialShininess, materialShininess);

        glUniform1i(ukey, isFragment ? 2 : 1);
    }
    else
    {
        glUniform1i(ukey, 0);
    }

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, [sphere getNumberOfSphereVertices]);
    glBindVertexArray(0);

    glUseProgram(0);
}

-(void)myUpdate{
    
    if(bAnimate)
        rotationAngle += 0.5f;

    if( 360.0f <= rotationAngle )
    {
        rotationAngle -= 360.0f;
    }
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

        case 'l':
        case 'L':
            bLight = !bLight;
            break;

        case 'a':
        case 'A':
            bAnimate = !bAnimate;
            break;

        case 'v':
        case 'V':
            isFragment = !isFragment;
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
        glDeleteVertexArrays(1, &vao);
    }

    if(vbo_Position){
        glDeleteBuffers(1,&vbo_Position);
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
    }

    if (vao)
    {
        glDeleteVertexArrays(1, &vao);
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
