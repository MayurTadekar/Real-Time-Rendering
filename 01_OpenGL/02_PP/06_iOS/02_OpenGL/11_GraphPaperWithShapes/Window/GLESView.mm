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

#define     FIND_DISTANCE(x1, y1, x2, y2)   {  (float) sqrt( ( (x2-x1) * (x2-x1) ) + ( (y2-y1) * (y2-y1) )  ) }

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
    
    GLuint      vao_shape[4];
    GLuint      vbo_shape_position[4];
    GLuint      vbo_shape_color[4];

    GLuint      vao[4];
    GLuint      vbo_position[4];
    GLuint      vbo_color[4];
    
    GLuint u_mMatrix;
    GLuint u_vMatrix;
    GLuint u_pMatrix;
    
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
}

-(void)onDoubleTap:(UITapGestureRecognizer *)gestureRecognizer
{
    // Code
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
    "in vec4 vColor;\n" \
    "uniform mat4 u_mMatrix;\n" \
    "uniform mat4 u_vMatrix;\n" \
    "uniform mat4 u_pMatrix;\n" \
    "out vec4 ovColor;\n" \
    "void main(void)\n" \
    "{\n" \
    "gl_Position = u_pMatrix * u_vMatrix * u_mMatrix * vPosition;\n" \
    "ovColor = vColor;\n" \
    "}\n";
    
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
    "out vec4 FragColor;\n" \
    "void main(void)\n" \
    "{\n" \
    "FragColor = ovColor;\n" \
    "}\n";
    
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
    glBindAttribLocation(shaderProgramObject, MST_ATTRIBUTE_COLOR, "vColor");
    
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
                glVertexAttribPointer(MST_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
                glEnableVertexAttribArray(MST_ATTRIBUTE_POSITION);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                // COLOR
                glGenBuffers(1, &vbo_color[RED_HOR_LINE]);
                glBindBuffer(GL_ARRAY_BUFFER, vbo_color[RED_HOR_LINE]);
                glBufferData(GL_ARRAY_BUFFER, sizeof(red_line_color), red_line_color, GL_STATIC_DRAW);
                glVertexAttribPointer(MST_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
                glEnableVertexAttribArray(MST_ATTRIBUTE_COLOR);
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
                glVertexAttribPointer(MST_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
                glEnableVertexAttribArray(MST_ATTRIBUTE_POSITION);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                // Color
                glGenBuffers(1, &vbo_color[BLUE_HOR_LINE]);
                glBindBuffer(GL_ARRAY_BUFFER, vbo_color[BLUE_HOR_LINE]);
                glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 40 * 3, blue_hor_line_color, GL_STATIC_DRAW);
                glVertexAttribPointer(MST_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
                glEnableVertexAttribArray(MST_ATTRIBUTE_COLOR);
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
                glVertexAttribPointer(MST_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
                glEnableVertexAttribArray(MST_ATTRIBUTE_POSITION);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                // COLOR
                glGenBuffers(1, &vbo_color[GREEN_VERT_LINE]);
                glBindBuffer(GL_ARRAY_BUFFER, vbo_color[GREEN_VERT_LINE]);
                glBufferData(GL_ARRAY_BUFFER, sizeof(green_line_color), green_line_color, GL_STATIC_DRAW);
                glVertexAttribPointer(MST_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
                glEnableVertexAttribArray(MST_ATTRIBUTE_COLOR);
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
                glVertexAttribPointer(MST_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
                glEnableVertexAttribArray(MST_ATTRIBUTE_POSITION);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                // Color
                glGenBuffers(1, &vbo_color[BLUE_VERT_LINE]);
                glBindBuffer(GL_ARRAY_BUFFER, vbo_color[BLUE_VERT_LINE]);
                glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 60 * 3, blue_vert_line_color, GL_STATIC_DRAW);
                glVertexAttribPointer(MST_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
                glEnableVertexAttribArray(MST_ATTRIBUTE_COLOR);
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
        glVertexAttribPointer(MST_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(MST_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // COLOR
        glGenBuffers(1, &vbo_shape_color[TRIANGLE]);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_shape_color[TRIANGLE]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_color), triangle_color, GL_STATIC_DRAW);
        glVertexAttribPointer(MST_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(MST_ATTRIBUTE_COLOR);
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
            glVertexAttribPointer(MST_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(MST_ATTRIBUTE_POSITION);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // COLOR
            glGenBuffers(1, &vbo_shape_color[SQUARE]);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_shape_color[SQUARE]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(square_color), square_color, GL_STATIC_DRAW);
            glVertexAttribPointer(MST_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(MST_ATTRIBUTE_COLOR);
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
            glVertexAttribPointer(MST_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(MST_ATTRIBUTE_POSITION);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // COLOR
            glGenBuffers(1, &vbo_shape_color[CIRCLE]);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_shape_color[CIRCLE]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(circle_color), circle_color, GL_STATIC_DRAW);
            glVertexAttribPointer(MST_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(MST_ATTRIBUTE_COLOR);
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
            glVertexAttribPointer(MST_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(MST_ATTRIBUTE_POSITION);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // COLOR
            glGenBuffers(1, &vbo_shape_color[CIRCLE_2]);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_shape_color[CIRCLE_2]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(circle_color), circle_color, GL_STATIC_DRAW);
            glVertexAttribPointer(MST_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(MST_ATTRIBUTE_COLOR);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        }
        glBindVertexArray(0);       // FINISH RECORDING CASET

    }

    
    // depth
    glClearDepthf(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
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
    
    viewMatrix = vmath::lookat(vmath::vec3(0.0f, 0.0f, 4.0f),
                               vmath::vec3(0.0f, 0.0f, 0.0f),
                               vmath::vec3(0.0f, 1.0f, 0.0f));
    
    glUniformMatrix4fv(u_mMatrix, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(u_vMatrix, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(u_pMatrix, 1, GL_FALSE, perspectiveProjectionMatrix);
    
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
}

-(void)myupdate
{
    // Code
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
