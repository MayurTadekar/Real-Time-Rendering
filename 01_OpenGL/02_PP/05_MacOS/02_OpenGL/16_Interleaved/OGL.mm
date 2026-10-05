

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

    [window setTitle:@"MST: Interleaved"];

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
    GLuint gFragmentShaderObject;
    GLuint gShaderProgramObject;

    GLuint vaoCube;    
    GLuint vbo;
    GLfloat angle;

    GLuint modelMatrixUniform;
    GLuint viewMatrixUniform;
    GLuint projectionMatrixUniform;

    GLuint laUniform;
    GLuint ldUniform;
    GLuint lightPositionUniform;
    GLuint lsUniform;

    GLuint kaUniform;
    GLuint kdUniform;
    GLuint ksUniform;
    GLuint kshineUniform;

    GLuint lKeyPressedUniform;
    mat4 perspectiveProjectionMatrix;

    GLuint vboCubeNormal;

    bool bLights;

    bool bAnimate;

    bool bTexture;

    GLuint texture_marble;



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
    
    angle = 0.0f;
    bLights = false;

    bAnimate = false;

    bTexture = false;

    [super prepareOpenGL];
    
    [[self openGLContext]makeCurrentContext];
    
    GLint swapint = 1;
    
    [[self openGLContext]setValues:&swapint forParameter:NSOpenGLCPSwapInterval];
    
    
    gVertexShaderObject  = glCreateShader(GL_VERTEX_SHADER);

    const GLchar* vertexShaderSourceCode =
        "#version 410 core" \
        "\n" \
        "in vec2 vTexCoord;" \
        "in vec3 vNormal;" \
        "in vec4 vColor;" \
        "in vec4 vPosition;" \
        "uniform mat4 u_model_matrix;" \
        "uniform mat4 u_view_matrix;" \
        "uniform mat4 u_projection_matrix;" \
        "uniform int u_lkeypressed;" \
        "uniform vec4 u_light_position;" \
        "out vec4 out_Color;" \
        "out vec2 out_TexCoord;" \
        "out vec3 transformed_normal;" \
        "out vec3 light_direction;" \
        "out vec3 view_vector;" \
        "void main(void)" \
        "{" \
        "   if(u_lkeypressed == 1)" \
        "   {" \
        "       vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;" \
        "       transformed_normal = mat3(u_view_matrix * u_model_matrix)*vNormal;" \
        "       light_direction = vec3(u_light_position - eye_coordinates);" \
        "       view_vector = -(eye_coordinates.xyz);" \
        "   }" \
        "   gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
        "   out_Color = vColor;" \
        "   out_TexCoord = vTexCoord;" \
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

    gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

    const GLchar* fragmentShaderSourceCode = 
        "#version 410 core" \
        "\n" \
        "in vec3 transformed_normal;" \
        "in vec3 light_direction;" \
        "in vec3 view_vector;" \
        "in vec2 out_TexCoord;" \
        "uniform vec3 u_la;" \
        "uniform vec3 u_ld;" \
        "uniform vec3 u_ls;" \
        "uniform vec3 u_ka;" \
        "uniform vec3 u_kd;" \
        "uniform vec3 u_ks;" \
        "uniform float u_kshine;" \
        "in vec3 diffuse_light;" \
        "uniform int u_lkeypressed;" \
        "uniform sampler2D u_Texture_Sampler;" \
        "in vec4 out_Color;" \
        "out vec4 FragColor;" \
        "vec4 tex;" \
        "void main(void)" \
        "{" \
        "   vec3 phong_ads_light;" \
        "   vec4 color;" \
        "   if(u_lkeypressed == 1)" \
        "   {" \
        "       vec3 normalized_transformed_normal = normalize(transformed_normal);" \
        "       vec3 normalized_light_direction = normalize(light_direction);" \
        "       vec3 reflection_vector = reflect(-normalized_light_direction,normalized_transformed_normal);" \
        "       vec3 normalized_view_vector = normalize(view_vector);" \
        "       vec3 ambient = u_la * u_ka;" \
        "       vec3 diffuse = u_ld * u_kd * max(dot(normalized_light_direction,normalized_transformed_normal),0.0);" \
        "       vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector,normalized_view_vector),0.0),u_kshine);" \
        "       phong_ads_light = ambient + diffuse + specular;" \
        "       tex = texture(u_Texture_Sampler,out_TexCoord) * out_Color;" \
        "       color = vec4(vec3(tex)*phong_ads_light,1.0);" \
        "   }" \
        "   else" \
        "   {" \
        "       color = out_Color * texture(u_Texture_Sampler,out_TexCoord);" \
        "   }" \
        "   FragColor = color;" \
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
                                        gFragmentShaderObject);

    glBindAttribLocation(gShaderProgramObject,MST_ATTRIBUTE_POSITION,"vPosition");
    glBindAttribLocation(gShaderProgramObject,MST_ATTRIBUTE_COLOR,"vColor");
    glBindAttribLocation(gShaderProgramObject,MST_ATTRIBUTE_NORMAL,"vNormal");
    glBindAttribLocation(gShaderProgramObject,MST_ATTRIBUTE_TEXTURE0,"vTexCoord");

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

    modelMatrixUniform = glGetUniformLocation(gShaderProgramObject,"u_model_matrix");

    viewMatrixUniform = glGetUniformLocation(gShaderProgramObject,"u_view_matrix");

    projectionMatrixUniform = glGetUniformLocation(gShaderProgramObject,"u_projection_matrix");

    laUniform = glGetUniformLocation(gShaderProgramObject,"u_la");

    ldUniform = glGetUniformLocation(gShaderProgramObject,"u_ld");

    lightPositionUniform = glGetUniformLocation(gShaderProgramObject,"u_light_position");

    lsUniform = glGetUniformLocation(gShaderProgramObject,"u_ls");

    kaUniform = glGetUniformLocation(gShaderProgramObject,"u_ka");

    kdUniform = glGetUniformLocation(gShaderProgramObject,"u_kd");

    ksUniform = glGetUniformLocation(gShaderProgramObject,"u_ks");

    kshineUniform = glGetUniformLocation(gShaderProgramObject,"u_kshine");

    lKeyPressedUniform = glGetUniformLocation(gShaderProgramObject,"u_lkeypressed");

    const GLfloat cubevcnt[] = {

        0.5f,0.5f,0.5f,1.0f, 0.0f, 0.0f,0.0f,0.0f,1.0f,0.0f, 0.0f,
        -0.5f,0.5f,0.5f,1.0f, 0.0f, 0.0f,0.0f,0.0f,1.0f,1.0f, 0.0f,
        -0.5f,-0.5f,0.5f,1.0f, 0.0f, 0.0f,0.0f,0.0f,1.0f,1.0f,1.0f,
        0.5f,-0.5f,0.5f,1.0f, 0.0f, 0.0f,0.0f,0.0f,1.0f,0.0f,1.0f,
        0.5f,0.5f,-0.5f,0.0f, 1.0f, 0.0f,1.0f,0.0f,0.0f,1.0f, 0.0f,
        0.5f,0.5f,0.5f,0.0f, 1.0f, 0.0f,1.0f,0.0f,0.0f,1.0f,1.0f,
        0.5f,-0.5f,0.5f,0.0f, 1.0f, 0.0f,1.0f,0.0f,0.0f,0.0f,1.0f,
        0.5f,-0.5f,-0.5f,0.0f, 1.0f, 0.0f,1.0f,0.0f,0.0f,0.0f, 0.0f,
        0.5f,0.5f,-0.5f,0.0f, 0.0f, 1.0f,0.0f,0.0f,-1.0f,1.0f, 0.0f,
        -0.5f,0.5f,-0.5f,0.0f, 0.0f, 1.0f,0.0f,0.0f,-1.0f,1.0f, 1.0f,
        -0.5f,-0.5f,-0.5f,0.0f, 0.0f, 1.0f,0.0f,0.0f,-1.0f,0.0f, 1.0f,
        0.5f,-0.5f,-0.5f,0.0f, 0.0f, 1.0f,0.0f,0.0f,-1.0f,0.0f, 0.0f,
        -0.5f,0.5f,0.5f,0.0f, 1.0f, 1.0f,-1.0f,0.0f,0.0f,0.0f, 0.0f,
        -0.5f,0.5f,-0.5f,0.0f, 1.0f, 1.0f,-1.0f,0.0f,0.0f,1.0f, 0.0f,
        -0.5f,-0.5f,-0.5f,0.0f, 1.0f, 1.0f,-1.0f,0.0f,0.0f,1.0f, 1.0f,
        -0.5f,-0.5f,0.5f,0.0f, 1.0f, 1.0f,-1.0f,0.0f,0.0f,0.0f, 1.0f,
        0.5f,0.5f,0.5f,1.0f, 0.0f, 1.0f,0.0f,1.0f,0.0f,0.0f,1.0f,
        -0.5f,0.5f,0.5f,1.0f, 0.0f, 1.0f,0.0f,1.0f,0.0f,0.0f, 0.0f,
        -0.5f,0.5f,-0.5f,1.0f, 0.0f, 1.0f,0.0f,1.0f,0.0f,1.0f, 0.0f,
        0.5f,0.5f,-0.5f,1.0f, 0.0f, 1.0f,0.0f,1.0f,0.0f,1.0f, 1.0f,
        0.5f,-0.5f,0.5f,1.0f, 1.0f, 0.0f,0.0f,-1.0f,0.0f,1.0f, 1.0f,
        -0.5f,-0.5f,0.5f,1.0f, 1.0f, 0.0f,0.0f,-1.0f,0.0f,0.0f, 1.0f,
        -0.5f,-0.5f,-0.5f,1.0f, 1.0f, 0.0f,0.0f,-1.0f,0.0f,0.0f, 0.0f,
        0.5f,-0.5f,-0.5f,1.0f, 1.0f, 0.0f,0.0f,-1.0f,0.0f,1.0f, 0.0f,
        
    };

    
    glGenVertexArrays(1,&vaoCube);
    glBindVertexArray(vaoCube);

    glGenBuffers(1,&vbo);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER,24*11*sizeof(float),cubevcnt,GL_STATIC_DRAW);

    glVertexAttribPointer(MST_ATTRIBUTE_POSITION,3,GL_FLOAT,GL_FALSE,11*sizeof(float),(void*)0);

    glEnableVertexAttribArray(MST_ATTRIBUTE_POSITION);

    
    glVertexAttribPointer(MST_ATTRIBUTE_COLOR,3,GL_FLOAT,GL_FALSE,11*sizeof(float),(void*)(3*sizeof(float)));

    glEnableVertexAttribArray(MST_ATTRIBUTE_COLOR);

    
    glVertexAttribPointer(MST_ATTRIBUTE_NORMAL,3,GL_FLOAT,GL_FALSE,11*sizeof(float),(void*)(6*sizeof(float)));

    glEnableVertexAttribArray(MST_ATTRIBUTE_NORMAL);

    
    glVertexAttribPointer(MST_ATTRIBUTE_TEXTURE0,2,GL_FLOAT,GL_FALSE,11*sizeof(float),(void*)(9*sizeof(float)));

    glEnableVertexAttribArray(MST_ATTRIBUTE_TEXTURE0);

    glBindBuffer(GL_ARRAY_BUFFER,0);

    glBindVertexArray(0);

    glClearDepth(1.0f);

    glEnable(GL_DEPTH_TEST);

    glDepthFunc(GL_LEQUAL);

    glClearColor(0.0f,0.0f,0.0f,0.0f);

    perspectiveProjectionMatrix = mat4::identity();

    texture_marble = [self loadTextureFromBmpFile:"Marble.bmp"];
    
    fprintf(gpFile, "OpenGL Version                           : %s\n", glGetString(GL_VERSION));
    fprintf(gpFile, "Graphics Library Shading Language(GLSL)  : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    
    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    
    CVDisplayLinkSetOutputCallback(displayLink, &MyDisplayLinkCallback, self);
    
    CGLContextObj cglContext = (CGLContextObj)[[self openGLContext]CGLContextObj];
    
    CGLPixelFormatObj cglPixelFormat =(CGLPixelFormatObj)[[self pixelFormat]CGLPixelFormatObj];
    
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
    
    CVDisplayLinkStart(displayLink);
    
    
}

-(GLuint)loadTextureFromBmpFile:(const char*)imageFileName{
    
    
    NSBundle *appBundle = [NSBundle mainBundle];
    
    NSString *appDirPath = [appBundle bundlePath];
    
    NSString *parentDirPath = [appDirPath stringByDeletingLastPathComponent];

    NSString *imageFileNameWithPath = [NSString stringWithFormat:@"%@/%s",parentDirPath,imageFileName];

    
    NSImage *bmpImage = [[NSImage alloc] initWithContentsOfFile:imageFileNameWithPath]; 

    if(!bmpImage){
        fprintf(gpFile,"NSImage conversion of imagefile failed\n");
        return(0);
    }

    
    CGImageRef cgImage = [bmpImage CGImageForProposedRect:nil context:nil hints:nil];

    
    int imageWidth = (int)CGImageGetWidth(cgImage);
    int imageHeight = (int)CGImageGetHeight(cgImage);

    
    CFDataRef imageData = CGDataProviderCopyData(CGImageGetDataProvider(cgImage));

    
    void* pixels = (void*)CFDataGetBytePtr(imageData); 
    GLuint texture;

    glPixelStorei(GL_UNPACK_ALIGNMENT,4);

    glGenTextures(1,&texture);

    glBindTexture(GL_TEXTURE_2D,texture);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);

    
    glTexImage2D(GL_TEXTURE_2D,
                0,
                GL_RGBA,
                imageWidth,
                imageHeight,
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                pixels
                );
                            glGenerateMipmap(GL_TEXTURE_2D);

    return (texture);

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
    GLfloat lightAmbient[] = {0.2f,0.2f,0.2f};
    GLfloat lightDiffuse[] = {1.0f,1.0f,1.0f};
    GLfloat lightPosition[] = {0.0f,0.0f,6.0f,0.0f};
    GLfloat lightSpecular[] = {1.0f,1.0f,1.0f};

    GLfloat materialAmbient[] = {0.0f,0.0f,0.0f};
    GLfloat materialDiffuse[] = {1.0f,1.0f,1.0f};
    GLfloat materialSpecular[] = {1.0f,1.0f,1.0f};
    GLfloat materialShininess = 128.0f;

    [[self openGLContext]makeCurrentContext];
    
    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(gShaderProgramObject);

    if(bLights == true){
        glUniform1i(lKeyPressedUniform,1);

        glUniform3fv(laUniform,1,(GLfloat*)lightAmbient);
        glUniform3fv(ldUniform,1,(GLfloat*)lightDiffuse);
        glUniform4fv(lightPositionUniform,1,(GLfloat*)lightPosition);
        glUniform3fv(lsUniform,1,(GLfloat*)lightSpecular);

        glUniform3fv(kaUniform,1,(GLfloat*)materialAmbient);
        glUniform3fv(kdUniform,1,(GLfloat*)materialDiffuse);
        glUniform3fv(ksUniform,1,(GLfloat*)materialSpecular);
        glUniform1f(kshineUniform,materialShininess);
    }
    else{
        glUniform1i(lKeyPressedUniform,0);
    }

    if(bTexture)
        glBindTexture(GL_TEXTURE_2D,texture_marble);
    else
        glBindTexture(GL_TEXTURE_2D,0);


    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();

    mat4 translateMatrix = mat4::identity(); 

    mat4 rotationMatrix = mat4::identity();

    mat4 scaleMatrix = mat4::identity();

    translateMatrix = vmath::translate(0.0f,0.0f,-4.0f);

    rotationMatrix = vmath::rotate(angle,angle,angle);


    modelMatrix = translateMatrix * rotationMatrix;

    glUniformMatrix4fv(viewMatrixUniform,
                        1,
                        GL_FALSE,
                        viewMatrix);

    glUniformMatrix4fv(modelMatrixUniform, 
                        1,
                        GL_FALSE,
                        modelMatrix);

    glUniformMatrix4fv(projectionMatrixUniform,  
                        1,
                        GL_FALSE,
                        perspectiveProjectionMatrix);

    glBindVertexArray(vaoCube);
                
    glDrawArrays(GL_TRIANGLE_FAN,0,4);
    glDrawArrays(GL_TRIANGLE_FAN,4,4);
    glDrawArrays(GL_TRIANGLE_FAN,8,4);
    glDrawArrays(GL_TRIANGLE_FAN,12,4);
    glDrawArrays(GL_TRIANGLE_FAN,16,4);
    glDrawArrays(GL_TRIANGLE_FAN,20,4);

    glBindVertexArray(0);

    glUseProgram(0);

    
    [self Update];
    
    CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);                 
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);

    
}

-(void)Update{

    if(bAnimate)
        angle += 0.4f;

    if(angle >= 360.0f){
        angle = 0.0f;
    }
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

        case 'L':
        case 'l':

            if(bLights == true){
                bLights = false;
            }
            else{
                bLights = true;
            }

        case 'a':
        case 'A':
            bAnimate = !bAnimate;
            break;

        case 't':
        case 'T':
            bTexture = !bTexture;
            break;

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

    
    if(vaoCube){
        glDeleteVertexArrays(1,&vaoCube);
        vaoCube = 0;
    }


    if(vbo){
        glDeleteBuffers(1,&vbo);
        vbo = 0;
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