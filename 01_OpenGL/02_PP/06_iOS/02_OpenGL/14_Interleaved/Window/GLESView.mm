#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>
#import <QuartzCore/CADisplayLink.h>

#import "GLESView.h"
#import "vmath.h"

@implementation GLESView
{
    EAGLContext *eaglContext;

    GLuint FrameBuffer;
    GLuint ColorRenderBuffer;
    GLuint DepthRenderBuffer;

    CADisplayLink * DisplayLink;
    CAFrameRateRange FrameRateRange;
    BOOL isDisplayLink;

    // OpenGL Related Variables
	GLuint shaderProgramObject;
	enum 
	{
        MST_ATTRIBUTE_POSITION = 0,
        MST_ATTRIBUTE_COLOR,
        MST_ATTRIBUTE_TEXCOORDS0,
        MST_ATTRIBUTE_NORMAL
	};

    GLuint uLightDiffuse;
    GLuint uLightAmbient;
    GLuint uLightSpecular;
    GLuint uLightPosition;

    GLuint uMaterialDiffuse;
    GLuint uMaterialAmbient;
    GLuint uMaterialSpecular;
    GLuint uMaterialShininess;

	GLuint ukey;

	BOOL bLight;
    BOOL bTexture;

	GLuint texture_Marble;
	GLuint textureSamplerUniform;

    GLuint vao;
    GLuint vbo;

    GLuint u_mMatrix;
    GLuint u_vMatrix;
    GLuint u_pMatrix;
    
    GLfloat lightAmbient[4];
    GLfloat lightDiffuse[4];
    GLfloat lightSpecular[4];
    GLfloat lightPosition[4];
    
    GLfloat materialAmbient[4];
    GLfloat materialDiffuse[4];
    GLfloat materialSpecular[4];
    GLfloat materialShininess;
    
    float rotate_angle;

    vmath::mat4 perspectiveProjectionMatrix;
}

-(void)awakeFromNib
{
    // Code
    [super awakeFromNib];

   [self setBackgroundColor:[UIColor blackColor]];

    // Get Drawable layer
    CAEAGLLayer *eaglLayer = (CAEAGLLayer *)[super layer];

    // Set this layer Property
    [eaglLayer setOpaque:YES];
    NSDictionary *dictionary = [NSDictionary dictionaryWithObjectsAndKeys:
                                kEAGLDrawablePropertyRetainedBacking, [NSNumber numberWithBool:NO],
                                kEAGLDrawablePropertyColorFormat, kEAGLColorFormatRGBA8, nil];

    [eaglLayer setDrawableProperties:dictionary];

    // create OpenGL-ES context
    eaglContext = [[EAGLContext alloc]initWithAPI:kEAGLRenderingAPIOpenGLES3];
    if(eaglContext == nil)
    {
        printf("OpenGL ES Context Creation Failed!!\n");
        return;
    }

    // Set This Context as Current Context
    [EAGLContext setCurrentContext:eaglContext];

    // create Framebuffer
    glGenFramebuffers(1, &FrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);

    glGenRenderbuffers(1, &ColorRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, ColorRenderBuffer);
    [eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, ColorRenderBuffer);

    GLint width, height;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);

    glGenRenderbuffers(1, &DepthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, DepthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DepthRenderBuffer);

    // Check wheather Framebuffer complete or not
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("initWithFrame() : Framebuffer is not complete");
        [self uninitialize];
        return;
    }


    // Initialize other class variables
    DisplayLink = nil;
    FrameRateRange.minimum = 30.0f;
    FrameRateRange.maximum = 60.0f;
    FrameRateRange.preferred = 60.0f;
    isDisplayLink = NO;

    // Call Stub initialize
    int result = [self initialize];
    if(result != 0)
    {
        printf("initialize failed!!\n");
    }

    // event handling
    // Single Tap
    UITapGestureRecognizer * singleTapGestureRecognizer = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onSingleTap:)];
    [singleTapGestureRecognizer setNumberOfTapsRequired:1];
    [singleTapGestureRecognizer setNumberOfTouchesRequired:1];
    [singleTapGestureRecognizer setDelegate:self];
    [self addGestureRecognizer:singleTapGestureRecognizer];

    // Double Tap
    UITapGestureRecognizer * doubleTapGestureRecognizer = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onDoubleTap:)];
    [doubleTapGestureRecognizer setNumberOfTapsRequired:2];
    [doubleTapGestureRecognizer setNumberOfTouchesRequired:1];
    [doubleTapGestureRecognizer setDelegate:self];
    [self addGestureRecognizer:doubleTapGestureRecognizer];

    // tell single tap recognoizer to fail when double tap
    [singleTapGestureRecognizer requireGestureRecognizerToFail:doubleTapGestureRecognizer];

    // swipe
    UISwipeGestureRecognizer * swipeGestureRecognizer = [[UISwipeGestureRecognizer alloc]initWithTarget:self action:@selector(onSwipe:)];
    [swipeGestureRecognizer setDelegate:self];
    [self addGestureRecognizer:swipeGestureRecognizer];

    // longPress
    UILongPressGestureRecognizer * longPressGestureRecognizer = [[UILongPressGestureRecognizer alloc]initWithTarget:self action:@selector(onLongPress:)];
    [longPressGestureRecognizer setDelegate:self];
    [self addGestureRecognizer:longPressGestureRecognizer];

}

+(Class)layerClass
{
    return([CAEAGLLayer class]);
}

/*

-(void)drawRect:(CGRect)rect
{
    // Code
}

*/

-(void)layoutSubviews
{
    // Code
    [super layoutSubviews];
    
    glBindRenderbuffer(GL_RENDERBUFFER, ColorRenderBuffer);
    [eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)[self layer]];

    GLint width, height;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);

    glGenRenderbuffers(1, &DepthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, DepthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DepthRenderBuffer);

    // Check wheather Framebuffer complete or not
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("layoutSubviews() :Framebuffer is not complete");
        return;
    }

    // call stub resize
    [self resize:width :height];

    // call drawing function here
    [self drawView:self];

}

-(void)drawView:(id)sender
{
    [EAGLContext setCurrentContext:eaglContext];
    glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);

    [self myUpdate];

    [self display];

    glBindRenderbuffer(GL_RENDERBUFFER, ColorRenderBuffer);
    [eaglContext presentRenderbuffer:ColorRenderBuffer];
}

-(void)startDisplayLink
{
    if(isDisplayLink == NO)
    {
        DisplayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(drawView:)];
        [DisplayLink setPreferredFrameRateRange:FrameRateRange];
        [DisplayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
        isDisplayLink = YES;
    }
}

-(void)stopDisplayLink
{
    if(isDisplayLink == YES)
    {
        [DisplayLink invalidate];
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
    bTexture = !bTexture;
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
    [self printGLESInfo];

	bLight = FALSE;
    bTexture = FALSE;

	lightAmbient[0] = 0.0f;
	lightAmbient[1] = 0.0f;
	lightAmbient[2] = 0.0f;
	lightAmbient[3] = 1.0f;

	lightDiffuse[0] = 1.0f;
	lightDiffuse[1] = 1.0f;
	lightDiffuse[2] = 1.0f;
	lightDiffuse[3] = 1.0f;

	lightSpecular[0] = 1.0f;
	lightSpecular[1] = 1.0f;
	lightSpecular[2] = 1.0f;
	lightSpecular[3] = 1.0f;

	lightPosition[0] = 100.0f;
	lightPosition[1] = 100.0f;
	lightPosition[2] = 100.0f;
	lightPosition[3] = 1.0f;

	materialAmbient[0] = 0.25f;
	materialAmbient[1] = 0.25f;
	materialAmbient[2] = 0.25f;
	materialAmbient[3] = 1.0f;

	materialDiffuse[0] = 1.0f;
	materialDiffuse[1] = 1.0f;
	materialDiffuse[2] = 1.0f;
	materialDiffuse[3] = 1.0f;

	materialSpecular[0] = 1.0f;
	materialSpecular[1] = 1.0f;
	materialSpecular[2] = 1.0f;
	materialSpecular[3] = 1.0f;

	materialShininess = 128.0f;

    // VERTEX SHADER
	const GLchar * vertexShaderSourceCode = 
	"#version 300 es\n" \
	"in vec4 vPosition;\n" \
	"in vec4 vColor;\n" \
	"in vec2 vTexCoord;\n" \
	"in vec3 vNormal;\n" \

	"uniform mat4 uModelMatrix;\n" \
	"uniform mat4 uViewMatrix;\n" \
	"uniform mat4 uProjectionMatrix;\n" \
	"uniform vec4 uLightPosition;\n" \
	"uniform int u_lkeypressed;\n" \

	"out vec4 out_color;\n" \
	"out vec2 out_TexCoord;\n" \
	"out vec3 out_TransformedNormals;\n" \
	"out vec3 out_LightDirection;\n" \
	"out vec3 out_ViewerVector;\n" \
	
	"void main(void)\n" \
	"{\n" \
		"gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vPosition;\n" \

		"if(u_lkeypressed == 1)\n" \
		"{\n" \
			"vec4 eyeCoordinates = uViewMatrix * uModelMatrix * vPosition;\n" \
			"mat3 normalMatrix = mat3(uViewMatrix * uModelMatrix);\n" \
			"out_TransformedNormals = normalMatrix * vNormal;\n" \
			"out_LightDirection = vec3(uLightPosition - eyeCoordinates);\n" \
			"out_ViewerVector = -eyeCoordinates.xyz;\n" \
		"}\n" \

		"out_color = vColor;\n" \
		"out_TexCoord = vTexCoord;\n" \
	"}\n";

	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);

	glCompileShader(vertexShaderObject);

	GLint status = 0;
	GLint infoLogLength = 0;
	GLchar * szInfoLog = NULL;

	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);

        if (infoLogLength > 0) // as their are some error which don't any string
		{
			szInfoLog = (GLchar *)malloc(infoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, NULL, szInfoLog);
				printf( "Vertex Shader compilation log : %s\n", szInfoLog);

				free(szInfoLog);
				szInfoLog = NULL;
			}
		}

		[self uninitialize];
		[self release];
		exit(0);
	}
	

	// FRAGMENT SHADER
	const GLchar * fragmetShaderSourceCode = 
	"#version 300 es\n" \
	"precision highp float;\n" \
	"in vec4 out_color;\n" \
	"in vec2 out_TexCoord;\n" \
	"in vec3 out_TransformedNormals;\n" \
	"in vec3 out_LightDirection;\n" \
	"in vec3 out_ViewerVector;\n" \
	
	"uniform sampler2D uTextureSampler;\n" \
	"uniform vec3 uLA;\n" \
	"uniform vec3 uLD;\n" \
	"uniform vec3 uLS;\n" \
	"uniform vec3 uKA;\n" \
	"uniform vec3 uKD;\n" \
	"uniform vec3 uKS;\n" \
	"uniform float uMaterialShininess;\n" \
	"uniform highp int u_lkeypressed;\n" \
	
	"out vec4 FragColor;\n" \
	
	"void main(void)\n" \
	"{\n" \
		"vec4 texture = texture(uTextureSampler, out_TexCoord);\n" \
		"vec3 fong_ADS_light;\n" \
		"if(u_lkeypressed == 1)\n" \
		"{\n" \
			"vec3 normalized_TransformedNormals = normalize(out_TransformedNormals);\n" \
			"vec3 normalized_LightDirection = normalize(out_LightDirection);\n" \
			"vec3 normalized_ViewerVector = normalize(out_ViewerVector);\n" \
			"vec3 ambientLight = uLA * uKA;\n" \
			"vec3 diffuseLight = uLD * uKD * max(dot(normalized_LightDirection, normalized_TransformedNormals), 0.0);\n" \
			"vec3 reflectionVector = reflect(-normalized_LightDirection, normalized_TransformedNormals);\n" \
			"vec3 specularLight = uLS * uKS * pow(max(dot(reflectionVector, normalized_ViewerVector), 0.0), uMaterialShininess);\n" \
			"fong_ADS_light = ambientLight + diffuseLight + specularLight;\n" \

			"FragColor = out_color * vec4(fong_ADS_light, 1.0f) * texture;\n" \
		"}\n" \
		"else\n" \
		"{\n" \
			"FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n" \
		"}\n" \
		
	"}\n";

	GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fragmentShaderObject, 1, (GLchar **)&fragmetShaderSourceCode, NULL);

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
			szInfoLog = (GLchar *)malloc(infoLogLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, NULL, szInfoLog);


				printf( "fragment Shader compilation log : %s\n", szInfoLog);

				free(szInfoLog);
				szInfoLog = NULL;
			}
		}

		[self uninitialize];
		[self release];
		exit(0);
	}

	shaderProgramObject = glCreateProgram();
	
	glAttachShader(shaderProgramObject, vertexShaderObject);
	glAttachShader(shaderProgramObject, fragmentShaderObject);

	glBindAttribLocation(shaderProgramObject, MST_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(shaderProgramObject, MST_ATTRIBUTE_COLOR, "vColor");
	glBindAttribLocation(shaderProgramObject, MST_ATTRIBUTE_TEXCOORDS0, "vTexCoord");
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
			szInfoLog = (GLchar *)malloc(infoLogLength * sizeof(GLchar));

			if (szInfoLog != NULL)
			{
				glGetProgramInfoLog(shaderProgramObject, infoLogLength, NULL, szInfoLog);

				printf( "Shader Program Link Log : %s\n", szInfoLog);

				free(szInfoLog);
				szInfoLog = NULL;
			}
		}
		
		[self uninitialize];
		[self release];
		exit(0);
	}

	textureSamplerUniform = glGetUniformLocation(shaderProgramObject, "uTextureSampler");
	
    u_mMatrix = glGetUniformLocation(shaderProgramObject, "uModelMatrix");
	u_vMatrix = glGetUniformLocation(shaderProgramObject, "uViewMatrix");
	u_pMatrix = glGetUniformLocation(shaderProgramObject, "uProjectionMatrix");
	
    uLightAmbient = glGetUniformLocation(shaderProgramObject, "uLA");
	uLightDiffuse = glGetUniformLocation(shaderProgramObject, "uLD");
	uLightSpecular = glGetUniformLocation(shaderProgramObject, "uLS");
	uLightPosition = glGetUniformLocation(shaderProgramObject, "uLightPosition");

    uMaterialAmbient = glGetUniformLocation(shaderProgramObject, "uKA");
	uMaterialDiffuse = glGetUniformLocation(shaderProgramObject, "uKD");
	uMaterialSpecular = glGetUniformLocation(shaderProgramObject, "uKS");
	uMaterialShininess = glGetUniformLocation(shaderProgramObject, "uMaterialShininess");
	
	ukey = glGetUniformLocation(shaderProgramObject, "u_lkeypressed");

    
	GLfloat cube_PCNT[] =
	{
		// Front
		// position				// color			 // normals				// texcoords
		1.0f,  1.0f,  1.0f,		1.0f, 0.0f, 0.0f,	 0.0f,  0.0f,  1.0f,	1.0f, 1.0f,
		-1.0f,  1.0f,  1.0f,	1.0f, 0.0f, 0.0f,	 0.0f,  0.0f,  1.0f,	0.0f, 1.0f,
		-1.0f, -1.0f,  1.0f,	1.0f, 0.0f, 0.0f,	 0.0f,  0.0f,  1.0f,	0.0f, 0.0f,
		1.0f, -1.0f,  1.0f,		1.0f, 0.0f, 0.0f,	 0.0f,  0.0f,  1.0f,	1.0f, 0.0f,
							
		// Right
		// position				// color			 // normals				// texcoords
		1.0f,  1.0f, -1.0f,		0.0f, 0.0f, 1.0f,	 1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
		1.0f,  1.0f,  1.0f,		0.0f, 0.0f, 1.0f,	 1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
		1.0f, -1.0f,  1.0f,		0.0f, 0.0f, 1.0f,	 1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
		1.0f, -1.0f, -1.0f,		0.0f, 0.0f, 1.0f,	 1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
							
		// Back
		// position				// color			 // normals				// texcoords
		1.0f,  1.0f, -1.0f,		1.0f, 1.0f, 0.0f,	 0.0f,  0.0f, -1.0f,	1.0f, 1.0f,
		-1.0f,  1.0f, -1.0f,	1.0f, 1.0f, 0.0f,	 0.0f,  0.0f, -1.0f,	0.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,	1.0f, 1.0f, 0.0f,	 0.0f,  0.0f, -1.0f,	0.0f, 0.0f,
		1.0f, -1.0f, -1.0f,		1.0f, 1.0f, 0.0f,	 0.0f,  0.0f, -1.0f,	1.0f, 0.0f,
							
		// Left
		// position				// color			 // normals				// texcoords
		-1.0f,  1.0f,  1.0f,	1.0f, 0.0f, 1.0f,	-1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
		-1.0f,  1.0f, -1.0f,	1.0f, 0.0f, 1.0f,	-1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,	1.0f, 0.0f, 1.0f,	-1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
		-1.0f, -1.0f,  1.0f,	1.0f, 0.0f, 1.0f,	-1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
							
		// Top
		// position				// color			 // normals				// texcoords
		1.0f,  1.0f, -1.0f,		0.0f, 1.0f, 0.0f,	 0.0f,  1.0f,  0.0f,	1.0f, 1.0f,
		-1.0f,  1.0f, -1.0f,	0.0f, 1.0f, 0.0f,	 0.0f,  1.0f,  0.0f,	0.0f, 1.0f,
		-1.0f,  1.0f,  1.0f,	0.0f, 1.0f, 0.0f,	 0.0f,  1.0f,  0.0f,	0.0f, 0.0f,
		1.0f,  1.0f,  1.0f,		0.0f, 1.0f, 0.0f,	 0.0f,  1.0f,  0.0f,	1.0f, 0.0f,
							
		// Bottom
		// position				// color			 // normals				// texcoords
		1.0f, -1.0f,  1.0f,		1.0f, 0.5f, 0.0f,	 0.0f, -1.0f,  0.0f,	1.0f, 1.0f,
		-1.0f, -1.0f,  1.0f,	1.0f, 0.5f, 0.0f,	 0.0f, -1.0f,  0.0f,	0.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,	1.0f, 0.5f, 0.0f,	 0.0f, -1.0f,  0.0f,	0.0f, 0.0f,
		1.0f, -1.0f, -1.0f,		1.0f, 0.5f, 0.0f,	 0.0f, -1.0f,  0.0f,	1.0f, 0.0f,
	};

	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);
    
	glGenBuffers(1, &vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_PCNT), cube_PCNT, GL_STATIC_DRAW);

	glVertexAttribPointer(MST_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *)(0 * sizeof(float)));

	glEnableVertexAttribArray(MST_ATTRIBUTE_POSITION);

	glVertexAttribPointer(MST_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *)(3 * sizeof(float)));

	glEnableVertexAttribArray(MST_ATTRIBUTE_COLOR);

	glVertexAttribPointer(MST_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *)(6 * sizeof(float)));

	glEnableVertexAttribArray(MST_ATTRIBUTE_NORMAL);

	glVertexAttribPointer(MST_ATTRIBUTE_TEXCOORDS0, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *)(9 * sizeof(float)));

	glEnableVertexAttribArray(MST_ATTRIBUTE_TEXCOORDS0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0); 

    glClearDepthf(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

	texture_Marble = [self LoadGLTexture:@"marble" :@"png"];
	if(texture_Marble == 0)
	{
		printf( "Failed to load Marble Texture\n");
		return(-1);
	}

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    return(0);
}

-(void)printGLESInfo
{
    printf("**************************************************************\n");
    printf("OpenGL information\n");
    printf("**************************************************************\n");
    printf("OpenGL-ES Vendor : %s \n", glGetString(GL_VENDOR));
    printf("OpenGL-ES Renderer : %s \n", glGetString(GL_RENDERER));
    printf("OpenGL-ES Version : %s \n", glGetString(GL_VERSION));
    printf("GLSL version : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    printf("**************************************************************\n");
}

-(GLuint)LoadGLTexture:(NSString *)textureFileName :(NSString *)extention
{
	// Code
	NSBundle *appBundle=[NSBundle mainBundle];

    NSString *textureFileNameWithPath = [appBundle pathForResource:textureFileName ofType:extention];

	UIImage * uiImage = [[UIImage alloc]initWithContentsOfFile:textureFileNameWithPath];

	CGImageRef cgImage = [uiImage CGImage];

	int imageWidth = (int)CGImageGetWidth(cgImage);

	int imageHeight = (int)CGImageGetHeight(cgImage);

	CGDataProviderRef cgDataProvider = CGImageGetDataProvider(cgImage);

	CFDataRef cfData = CGDataProviderCopyData(cgDataProvider);

	void * imageData = (void *)CFDataGetBytePtr(cfData);

	GLuint texture = 0;

    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexImage2D(	GL_TEXTURE_2D,
                    0,
                    GL_RGBA,
                    imageWidth,
                    imageHeight,
                    0,
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    imageData);

    glGenerateMipmap(GL_TEXTURE_2D);
        
    glBindTexture(GL_TEXTURE_2D, 0);

    CFRelease(cfData);

    return(texture);
}

-(void)resize:(int)width :(int)height
{
	if (height <= 0)
	{
		height = 1;
	}

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix = vmath::perspective(	45.0f,
															(GLfloat)width/(GLfloat)height,
															0.1f,
															100.0f);
}

-(void)display
{
	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramObject);

	vmath::mat4 modelMatrix = vmath::mat4::identity();
	vmath::mat4 viewMatrix = vmath::mat4::identity();
	vmath::mat4 translationMatrix = vmath::mat4::identity();

    viewMatrix = vmath::lookat(vmath::vec3(0.0f, 0.0f, 6.0f),
                               vmath::vec3(0.0f, 0.0f, 0.0f),
                               vmath::vec3(0.0f, 1.0f, 0.0f));

	vmath::mat4 scaleMatrix = vmath::mat4::identity();
	vmath::mat4 rotationMatrix = vmath::mat4::identity();
	vmath::mat4 rotationMatrix1 = vmath::mat4::identity();
	vmath::mat4 rotationMatrix2 = vmath::mat4::identity();
	vmath::mat4 rotationMatrix3 = vmath::mat4::identity();

	scaleMatrix = vmath::scale(0.75f, 0.75f, 0.75f);
	rotationMatrix1 = vmath::rotate(rotate_angle, 1.0f, 0.0f, 0.0f);
	rotationMatrix2 = vmath::rotate(rotate_angle, 0.0f, 1.0f, 0.0f);
	rotationMatrix3 = vmath::rotate(rotate_angle, 0.0f, 0.0f, 1.0f);

	rotationMatrix = rotationMatrix1 * rotationMatrix2 * rotationMatrix3;

	modelMatrix = translationMatrix * scaleMatrix * rotationMatrix;

	glUniformMatrix4fv(u_mMatrix, 1, GL_FALSE, modelMatrix);

	glUniformMatrix4fv(u_vMatrix, 1, GL_FALSE, viewMatrix);

	glUniformMatrix4fv(u_pMatrix, 1, GL_FALSE, perspectiveProjectionMatrix);

	if(bLight == TRUE)
	{
		glUniform3fv(uLightAmbient, 1, lightAmbient);
		glUniform3fv(uLightDiffuse, 1, lightDiffuse);
		glUniform3fv(uLightSpecular, 1, lightSpecular);
		glUniform4fv(uLightPosition, 1, lightPosition);

		glUniform3fv(uMaterialAmbient, 1, materialAmbient);
		glUniform3fv(uMaterialDiffuse, 1, materialDiffuse);
		glUniform3fv(uMaterialSpecular, 1, materialSpecular);
		glUniform1f(uMaterialShininess, materialShininess);

		glUniform1i(ukey, 1);
	}
	else
	{
		glUniform1i(ukey, 0);
	}

    if(bTexture)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_Marble);
        glUniform1i(textureSamplerUniform, 0);
    }
	glBindVertexArray(vao);

	
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);


	glBindVertexArray(0);

	glUseProgram(0);
}

-(void)myUpdate
{
    rotate_angle += 0.1f;
    
        
    if(rotate_angle >= 360.0f)
    {
        rotate_angle -= 360.0f;
    }
}

-(void)uninitialize
{
    if (vbo)
	{
		glDeleteBuffers(1, &vbo);
		vbo = 0;
	}

	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}

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

    if(DepthRenderBuffer)
    {
        glDeleteRenderbuffers(1, &DepthRenderBuffer);
        DepthRenderBuffer = 0;
    }

    if(ColorRenderBuffer)
    {
        glDeleteRenderbuffers(1, &ColorRenderBuffer);
        ColorRenderBuffer = 0;
    }

    if(FrameBuffer)
    {
        glDeleteFramebuffers(1, &FrameBuffer);
        FrameBuffer = 0;
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
