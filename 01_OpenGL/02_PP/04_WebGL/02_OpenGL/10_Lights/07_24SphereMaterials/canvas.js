

var canvas = null;
var context = null;

var gl = null;

var canvas_width;
var canvas_height;

var requestAnimationFrame = window.requestAnimationFrame		||
							window.webkitRequestAnimationFrame	||
							window.mozRequestAnimationFrame		||
							window.oRequestAnimationFrame		||
							window.msRequestAnimationFrame;

const WebGLMacros = {
						MST_ATTRIBUTE_VERTEX:0,
						MST_ATTRIBUTE_NORMAL:1,
						MST_ATTRIBUTE_COLOR:2,
						MST_ATTRIBUTE_TEXTURE0:3,
};

var vertexShaderObject;
var fragmentShaderObject;
var shaderProgramObject;

var vao;
var vboVertex;
var vboNormal;

var mUniform;
var vUniform;
var pUniform;
var lightKeyUniform;

var kaUniform;
var kdUniform;
var ksUniform;
var kshineUniform;

var laUniform;
var ldUniform;
var lsUniform;
var lpUniform;

var perspectiveProjectionMatrix;

var angle = 0.0;
var bLight = false;
var bFullscreen = false;

var materialAmbient = [		0.0215,0.1745,0.0215,1.0,
                            0.135,0.2225,0.1575,1.0,
                            0.05375,0.05,0.06625,1.0,
                            0.25,0.20725,0.20725,1.0,
                            0.1745,0.01175,0.01175,1.0,
                            0.1,0.18725,0.1745,1.0,
                            0.329412,0.223529,0.027451,1.0,
                            0.2125,0.1275,0.054,1.0,
                            0.25,0.25,0.25,1.0,
                            0.19125,0.0735,0.0225,1.0,
                            0.24725,0.1995,0.0745,1.0,
                            0.19225,0.19225,0.19225,1.0,
                            0.0,0.0,0.0,1.0,
                            0.0,0.1,0.06,1.0,
                            0.0,0.0,0.0,1.0,
                            0.0,0.0,0.0,1.0,
                            0.0,0.0,0.0,1.0,
                            0.0,0.0,0.0,1.0,
                            0.02,0.02,0.02,1.0,
                            0.0,0.05,0.05,1.0,
                            0.0,0.05,0.0,1.0,
                            0.05,0.0,0.0,1.0,
                            0.05,0.05,0.05,1.0,
                            0.05,0.05,0.0,1.0
                        ];
var materialDiffuse = [ 	0.07568,0.61424,0.07568,1.0, 
                            0.54,0.89,0.63,1.0,
                            0.18275,0.17,0.22525,1.0,
                            1.0,0.829,0.829,1.0,
                            0.61424,0.04136,0.04136,1.0, 
                            0.396,0.74151,0.69102,1.0,
                            0.780392,0.568627,0.113725,1.0,
                            0.714,0.4284,0.18144,1.0,
                            0.4,0.4,0.4,1.0,
                            0.7038,0.27048,0.0828,1.0,
                            0.75164,0.60648,0.22648,1.0,
                            0.50754,0.50754,0.50754,1.0,
                            0.01,0.01,0.01,1.0,
                            0.0,0.50980392,0.50980392,1.0,
                            0.1,0.35,0.1,1.0,
                            0.5,0.0,0.0,1.0,
                            0.55,0.55,0.55,1.0,
                            0.5,0.5,0.0,1.0,
                            0.01,0.01,0.01,1.0,
                            0.4,0.5,0.5,1.0,
                            0.4,0.5,0.4,1.0,
                            0.5,0.4,0.4,1.0,
                            0.5,0.5,0.5,1.0,
                            0.5,0.5,0.4,1.0
                        ];
var materialSpecular = [ 	0.633,0.727811,0.633,1.0,
                            0.316228,0.316228,0.316228,1.0,
                            0.332741,0.328634,0.346435,1.0,
                            0.296648,0.296648,0.296648,1.0,
                            0.727811,0.626959,0.626959,1.0,
                            0.297254,0.30829,0.306678,1.0,
                            0.992157,0.941176,0.807843,1.0,
                            0.393548,0.271906,0.166721,1.0,
                            0.774597,0.774597,0.774597,1.0,
                            0.256777,0.137622,0.086014,1.0,
                            0.628281,0.555802,0.366065,1.0,
                            0.508273,0.508273,0.508273,1.0,
                            0.50,0.50,0.50,1.0,
                            0.50196078,0.50196078,0.50196078,1.0,
                            0.45,0.55,0.45,1.0,
                            0.7,0.6,0.6,1.0,
                            0.70,0.70,0.70,1.0,
                            0.60,0.60,0.50,1.0,
                            0.4,0.4,0.4,1.0,
                            0.04,0.7,0.7,1.0,
                            0.04,0.7,0.04,1.0,
                            0.7,0.04,0.04,1.0,
                            0.7,0.7,0.7,1.0,
                            0.7,0.7,0.04,1.0
                        ];
var materialShininess = [	0.6 * 128,
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
                        ];

var axis = 1;

function main()
{
	canvas = document.getElementById("MST");
	if(!canvas)
		console.log("Obtaining Canvas Failed")
	else
		console.log("Obtaining Canvas Succeded")

	console.log("Canvas Width = " + canvas.width + " Canvas Height = " + canvas.height)

	canvas_width = canvas.width;
	canvas_height = canvas.height;

	window.addEventListener("keydown", keyDown, false);
	window.addEventListener("click", mouseDown, false);
	window.addEventListener("resize", resize, false);

	init();

	resize();

	draw();
}

function toggleFullscreen()
{
	var fullscreen_elements = 	document.fullscreenElement 			||
								document.webkitFullscreenElement	||
								document.mozkitFullscreenElement	||
								document.msFullscreenElement		||
								null;

	if(null == fullscreen_elements)
	{
		if( canvas.requestFullscreen)
		{
			canvas.requestFullscreen();
		}
		else if( canvas.webkitRequestFullscreen )
		{
			canvas.webkitRequestFullscreen();
		}
		else if( canvas.mozkitRequestFullscreen )
		{
			canvas.mozkitRequestFullscreen();
		}
		else if( canvas.mskitRequestFullscreen )
		{
			canvas.mskitRequestFullscreen();
		}

		bFullscreen = true;
	}
	else
	{
		if( document.exitFullscreen )
		{
			document.exitFullscreen()
		}
		else if( document.webkitExitFullscreen )
		{
			document.webkitExitFullscreen()
		}
		else if( document.mozCancelFullscreen )
		{
			document.mozCancelFullscreen()
		}
		else if( document.msExitFullscreen )
		{
			document.msExitFullscreen()
		}

		bFullscreen = false;
	}
}

function keyDown(event)
{
	switch(event.keyCode)
	{
	case 70:
		toggleFullscreen();
		break;
	}
}

function mouseDown(event)
{
}

function init()
{
	gl = canvas.getContext("webgl2");

	if(!gl)
	{
		console.log("WebGL2 Context Failed");
	}
	else
	{
		console.log("WebGL2 Context Succeded");
	}

	gl.viewportWidth = canvas.width;
	gl.viewportHeight = canvas.height;

	var vertexShaderSourceCode =	"#version 300 es" 						+
									"\n" 									+
									"in vec3 vNormal;\n" 					+
									"in vec4 vPosition;\n" 					+
									"uniform mat4 u_mMatrix;\n" 			+
									"uniform mat4 u_vMatrix;\n" 			+
									"uniform mat4 u_pMatrix;\n" 			+
									"uniform mediump int u_lkeypressed;\n" 	+
									"uniform vec4 u_lp;\n" 		+
									"out vec3 transformed_normal;\n" 		+
									"out vec3 light_direction;\n" 			+
									"out vec3 view_vector;\n" 				+
									"void main(void)\n" 					+
									"{\n" 									+
									"	if(u_lkeypressed == 1)\n"			+
									"	{\n" 								+
									"		vec4 eye_coordinates = u_vMatrix * u_mMatrix * vPosition;\n" 	+
									"		transformed_normal = mat3(u_vMatrix * u_mMatrix)*vNormal;\n" 	+
									"		light_direction = vec3(u_lp - eye_coordinates);\n" 	+
									"		view_vector = -(eye_coordinates.xyz);\n" 						+
									"	}\n" 								+
									"	gl_Position = u_pMatrix * u_vMatrix * u_mMatrix * vPosition;\n" 	+
									"}";

	var fragmentShaderSourceCode = 	"#version 300 es"						+
									"\n"									+
									"precision highp float;\n"				+
									"uniform vec3 u_la;\n" 					+
									"uniform vec3 u_ld;\n" 					+
									"uniform vec3 u_ls;\n" 					+
									"uniform vec3 u_ka;\n" 					+
									"uniform vec3 u_kd;\n" 					+
									"uniform vec3 u_ks;\n" 					+
									"uniform mediump float u_kshine;\n" 	+
									"uniform mediump int u_lkeypressed;\n" 	+
									"in vec3 transformed_normal;\n"			+
									"in vec3 light_direction;\n" 			+
									"in vec3 view_vector;\n" 				+
									"out vec4 FragColor;\n" 				+
									"void main(void)\n" 					+
									"{\n"									+
									"	vec3 phong_ads_light;\n" 			+
									"	if(u_lkeypressed == 1)\n" 			+
									"	{\n" 								+
									"		vec3 normalized_transformed_normal = normalize(transformed_normal);\n" 			+
									"		vec3 normalized_light_direction = normalize(light_direction);\n" 				+
									"		vec3 reflection_vector = reflect(-normalized_light_direction,normalized_transformed_normal);\n"	+
									"		vec3 normalized_view_vector = normalize(view_vector);\n" 						+
									"		vec3 ambient = u_la * u_ka;\n" 													+
									"		vec3 diffuse = u_ld * u_kd * max(dot(normalized_light_direction,normalized_transformed_normal),0.0);\n"	+
									"		vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector,normalized_view_vector),0.0),u_kshine);\n" 	+
									"		phong_ads_light = ambient + diffuse + specular;\n" 								+
									"	}\n" 										+
									"	else\n" 									+
									"	{\n" 										+
									"		phong_ads_light = vec3(1.0,1.0,1.0);\n" +
									"	}\n" 										+
									"	FragColor = vec4(phong_ads_light,1.0);\n" 	+
									"}";

	vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);
	gl.shaderSource(vertexShaderObject, vertexShaderSourceCode);
	gl.compileShader(vertexShaderObject);
	if(gl.getShaderParameter(vertexShaderObject, gl.COMPILE_STATUS) == false)
	{
		var error = gl.getShaderInfoLog(vertexShaderObject);

		if(0 < error.length)
		{
			alert(error);
			uinitialize();
		}
	}

	fragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);
	gl.shaderSource(fragmentShaderObject, fragmentShaderSourceCode);
	gl.compileShader(fragmentShaderObject);
	if(gl.getShaderParameter(fragmentShaderObject, gl.COMPILE_STATUS) == false)
	{
		var error = gl.getShaderInfoLog(fragmentShaderObject);

		if(0 < error.length)
		{
			alert(error);
			uninitialize();
		}
	}

	shaderProgramObject = gl.createProgram();

	gl.attachShader(shaderProgramObject, vertexShaderObject);
	gl.attachShader(shaderProgramObject, fragmentShaderObject);

	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.MST_ATTRIBUTE_VERTEX, "vPosition");
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.MST_ATTRIBUTE_NORMAL, "vNormal");

	gl.linkProgram(shaderProgramObject);

	if(!gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS))
	{
		var error = gl.getProgramInfoLog(shaderProgramObject);
		if(0 < error.length)
		{
			alert(error);
			uninitialize();
		}
	}

	mUniform = gl.getUniformLocation(shaderProgramObject, "u_mMatrix");
	vUniform = gl.getUniformLocation(shaderProgramObject, "u_vMatrix");
	pUniform = gl.getUniformLocation(shaderProgramObject, "u_pMatrix");
	
	kaUniform = gl.getUniformLocation(shaderProgramObject, "u_ka");
	kdUniform = gl.getUniformLocation(shaderProgramObject, "u_kd");
	ksUniform = gl.getUniformLocation(shaderProgramObject, "u_ks");
	kshineUniform = gl.getUniformLocation(shaderProgramObject, "u_kshine");

	laUniform = gl.getUniformLocation(shaderProgramObject, "u_la");
	ldUniform = gl.getUniformLocation(shaderProgramObject, "u_ld");
	lsUniform = gl.getUniformLocation(shaderProgramObject, "u_ls");
	lpUniform = gl.getUniformLocation(shaderProgramObject, "u_lp");
	lightKeyUniform = gl.getUniformLocation(shaderProgramObject, "u_lkeypressed");

	sphere = new Mesh();

	makeSphere(sphere, 1.0, 40, 40);

	gl.enable(gl.DEPTH_TEST);
	gl.depthFunc(gl.LEQUAL);
	gl.clearDepth(1.0);

	gl.clearColor(0.4, 0.4, 0.4, 1.0);

	perspectiveProjectionMatrix = mat4.create();
}

function resize()
{
	if( true == bFullscreen)
	{
		canvas.width = window.innerWidth;
		canvas.height = window.innerHeight;
	}
	else
	{
		canvas.width = canvas_width;
		canvas.height = canvas_height;
	}

	gl.viewport(0, 0, canvas.width, canvas.height);

	mat4.perspective(	perspectiveProjectionMatrix,
						50.0,
						parseFloat(canvas.width) / parseFloat(canvas.height),
						0.1,
						100.0
					);
}

function degToRad(deg)
{
	return(deg * Math.PI / 180.0);
}

function draw()
{
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	gl.useProgram(shaderProgramObject);

	var lightAmbient = [0.0,0.0,0.0];
	var lightDiffuse = [1.0,1.0,1.0];
	var lightPosition = [];
	var lightSpecular = [1.0,1.0,1.0];

	var translateMatrix = mat4.create();
	var rotateMatrix = mat4.create();
	var modelMatrix = mat4.create();
	var viewMatrix = mat4.create();

	var modelViewProjectionMatrix = mat4.create();

	gl.uniformMatrix4fv(vUniform, false, viewMatrix);
	gl.uniformMatrix4fv(pUniform, false, perspectiveProjectionMatrix);

	switch(axis)
	{

	case 1:
		lightPosition[0] = 0.0;
		lightPosition[1] = 100.0 * Math.cos(degToRad(angle));
		lightPosition[2] = 100.0 * Math.sin(degToRad(angle));
		lightPosition[3] = 1.0;
		break;

	case 2:
		lightPosition[1] = 0.0;
		lightPosition[0] = 100.0 * Math.cos(degToRad(angle));
		lightPosition[2] = 100.0 * Math.sin(degToRad(angle));
		lightPosition[3] = 1.0;
		break;

	case 3:
		lightPosition[2] = 0.0;
		lightPosition[1] = 100.0 * Math.cos(degToRad(angle));
		lightPosition[0] = 100.0 * Math.sin(degToRad(angle));
		lightPosition[3] = 1.0;
		break;
	}

	if( true === bLight)
	{
		gl.uniform1i(lightKeyUniform,1);

		gl.uniform3fv(laUniform,lightAmbient);
		gl.uniform3fv(ldUniform,lightDiffuse);
		gl.uniform3fv(lsUniform,lightSpecular);
		gl.uniform4fv(lpUniform,lightPosition);

	}
	else
	{
		gl.uniform1i(lightKeyUniform, 0);
	}

	for(let le = 0; le < 24; ++le)
	{

		mat4.identity(modelMatrix);
		mat4.identity(translateMatrix);
		mat4.identity(rotateMatrix);

		mat4.translate(modelMatrix, translateMatrix, [ 2.5 * parseInt(le % 6) - (2 *3), 2.5 * parseInt(le / 6) - (2 * 2), -50.0]);
		mat4.rotateX(rotateMatrix, rotateMatrix, degToRad(angle));
		mat4.rotateY(rotateMatrix, rotateMatrix, degToRad(angle));
		mat4.rotateZ(rotateMatrix, rotateMatrix, degToRad(angle));


		gl.uniform3f(kaUniform,materialAmbient[le * 4 + 0],materialAmbient[le * 4 + 1],materialAmbient[le * 4 + 2]);
		gl.uniform3f(kdUniform,materialDiffuse[le * 4 + 0],materialDiffuse[le * 4 + 1],materialDiffuse[le * 4 + 2]);
		gl.uniform3f(ksUniform,materialSpecular[le * 4 + 0],materialSpecular[le * 4 + 1],materialSpecular[le * 4 + 2]);
		gl.uniform1f(kshineUniform,materialShininess[le]);

		mat4.multiply(	modelMatrix,
						modelMatrix,
						rotateMatrix);

		gl.uniformMatrix4fv(mUniform, false, modelMatrix);


		sphere.draw();
	}

	update();

	requestAnimationFrame(draw, canvas);
}

function update()
{
	angle += 1.0;

	if( 360.0 <= angle)
	{
		angle -= 360.0;
	}
}

function uninitialize()
{

	if(vao)
	{
		gl.deleteVertexArray(vao);
		vao = null;
	}

	if(vboVertex)
	{
		gl.deleteBuffer(vboVertex);
		vboVertex = null;
	}

	if(shaderProgramObject)
	{
		if(fragmentShaderObject)
		{
			gl.detachShader(shaderProgramObject,fragmentShaderObject);
			gl.deleteShader(fragmentShaderObject);
			fragmentShaderObject = null;
		}

		if(vertexShaderObject)
		{
			gl.detachShader(shaderProgramObject,vertexShaderObject);
			gl.deleteShader(vertexShaderObject);
			vertexShaderObject = null;
		}

		gl.deleteProgram(shaderProgramObject);
		shaderProgramObject = null;
	}

}

function keyDown(event)
{

	switch(event.keyCode)
	{

		case 27:
			uninitialize();
			window.close();
			break;

		case 70:
			toggleFullscreen();
			break;
	}

	switch(event.key)
	{
	case 'l':
	case 'L':
		if(false === bLight)
		{
			bLight = true;
		}
		else
		{
			bLight = false;
		}
		break;

	case 'x':
	case 'X':
		axis = 1;
		break;

	case 'y':
	case 'Y':
		axis = 2;
		break;

	case 'z':
	case 'Z':
		axis = 3;
		break;
	}

}

function mouseDown()
{
}
