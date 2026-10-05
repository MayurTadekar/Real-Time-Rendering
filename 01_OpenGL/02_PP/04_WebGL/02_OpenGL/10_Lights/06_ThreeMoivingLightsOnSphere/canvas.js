

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

var laUniform = [];
var ldUniform = [];
var lsUniform = [];
var lpUniform = [];

var perspectiveProjectionMatrix;

var angle = 0.0;
var bLight = false;
var bFullscreen = false;

var vfToggle = 1;

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
									"uniform vec3 u_la[3];\n"				+
									"uniform vec3 u_ld[3];\n"				+
									"uniform vec3 u_ls[3];\n"				+
									"uniform vec3 u_ka;\n" 					+
									"uniform vec3 u_kd;\n" 					+
									"uniform vec3 u_ks;\n" 					+
									"uniform mediump float u_kshine;\n" 	+
									"uniform mat4 u_mMatrix;\n" 			+
									"uniform mat4 u_vMatrix;\n" 			+
									"uniform mat4 u_pMatrix;\n" 			+
									"uniform mediump int u_lkeypressed;\n" 	+
									"uniform vec4 u_lp[3];\n"	+
									"out vec3 transformed_normal[3];\n" 		+
									"out vec3 light_direction[3];\n" 		+
									"out vec3 view_vector[3];\n" 				+
									"out vec3 phong_ads_light_i;\n" 		+
									"void main(void)\n" 					+
									"{\n" 									+
									"	if(u_lkeypressed == 1)\n"			+
									"	{\n" 								+
									"		for(int i = 0 ;i < 3; ++i)\n"	+
									"		{\n" 							+
									"			vec4 eye_coordinates = u_vMatrix * u_mMatrix * vPosition;\n" 						+
									"			vec3 transformed_normal = normalize(mat3(u_vMatrix * u_mMatrix)*vNormal);\n" 		+
									"			vec3 light_direction = normalize(vec3(u_lp[i] - eye_coordinates));\n" 		+
									"			vec3 reflection_vector = reflect(-light_direction,transformed_normal);\n" 			+
									"			vec3 view_vector = normalize(-(eye_coordinates.xyz));\n" 							+
									"			vec3 ambient = u_la[i] * u_ka;\n" 														+
									"			vec3 diffuse = u_ld[i] * u_kd * max(dot(light_direction,transformed_normal),0.0);\n" 	+
									"			vec3 specular = u_ls[i] * u_ks * pow(max(dot(reflection_vector,view_vector),0.0),u_kshine);\n" +
									"			phong_ads_light_i = phong_ads_light_i + ambient + diffuse + specular;\n" 									+	
									"		}\n" 								+
									"	}\n" 								+
									"	else if(u_lkeypressed == 2)\n"		+
									"	{\n" 								+
									"		for(int i = 0 ;i < 3; ++i)\n"	+
									"		{\n" 							+
									"			vec4 eye_coordinates = u_vMatrix * u_mMatrix * vPosition;\n" 	+
									"			transformed_normal[i] = mat3(u_vMatrix * u_mMatrix)*vNormal;\n" 	+
									"			light_direction[i] = vec3(u_lp[i] - eye_coordinates);\n" 	+
									"			view_vector[i] = -(eye_coordinates.xyz);\n" 						+
									"		}\n" 								+
									"	}\n" 								+
									"	else\n"								+
									"	{\n" 								+
									"		phong_ads_light_i = vec3(1.0,1.0,1.0);\n"		+
									"	}\n" 								+
									"	gl_Position = u_pMatrix * u_vMatrix * u_mMatrix * vPosition;\n" 	+
									"}";

	var fragmentShaderSourceCode = 	"#version 300 es"						+
									"\n"									+
									"precision highp float;\n"				+
									"uniform vec3 u_la[3];\n"				+
									"uniform vec3 u_ld[3];\n"				+
									"uniform vec3 u_ls[3];\n"				+
									"uniform vec3 u_ka;\n" 					+
									"uniform vec3 u_kd;\n" 					+
									"uniform vec3 u_ks;\n" 					+
									"uniform mediump float u_kshine;\n" 	+
									"uniform mediump int u_lkeypressed;\n" 	+
									"in vec3 transformed_normal[3];\n"			+
									"in vec3 light_direction[3];\n" 			+
									"in vec3 view_vector[3];\n" 				+
									"in vec3 phong_ads_light_i;\n" 			+
									"vec3 phong_ads_light;\n"	 			+
									"out vec4 FragColor;\n" 				+
									"void main(void)\n" 					+
									"{\n"									+
									"	if(u_lkeypressed == 1)\n" 			+
									"	{\n" 								+
									"		FragColor = vec4(phong_ads_light_i,1.0);\n" 	+
									"	}\n" 								+
									"	else if(u_lkeypressed == 2)\n" 			+
									"	{\n" 								+
									"		for(int i = 0 ;i < 3; ++i)\n"	+
									"		{\n" 							+
									"			vec3 normalized_transformed_normal = normalize(transformed_normal[i]);\n" 			+
									"			vec3 normalized_light_direction = normalize(light_direction[i]);\n" 				+
									"			vec3 reflection_vector = reflect(-normalized_light_direction,normalized_transformed_normal);\n"	+
									"			vec3 normalized_view_vector = normalize(view_vector[i]);\n" 						+
									"			vec3 ambient = u_la[i] * u_ka;\n" 													+
									"			vec3 diffuse = u_ld[i] * u_kd * max(dot(normalized_light_direction,normalized_transformed_normal),0.0);\n"	+
									"			vec3 specular = u_ls[i] * u_ks * pow(max(dot(reflection_vector,normalized_view_vector),0.0),u_kshine);\n" 	+
									"			phong_ads_light = phong_ads_light + ambient + diffuse + specular;\n" 								+
									"		}\n" 										+
									"		FragColor = vec4(phong_ads_light,1.0);\n" 	+
									"	}\n" 										+
									"	else\n" 									+
									"	{\n" 										+
									"		FragColor = vec4(1.0,1.0,1.0,1.0);\n" +
									"	}\n" 										+
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

	laUniform[0] = gl.getUniformLocation(shaderProgramObject, "u_la[0]");
	laUniform[1] = gl.getUniformLocation(shaderProgramObject, "u_la[1]");
	laUniform[2] = gl.getUniformLocation(shaderProgramObject, "u_la[2]");
	ldUniform[0] = gl.getUniformLocation(shaderProgramObject, "u_ld[0]");
	ldUniform[1] = gl.getUniformLocation(shaderProgramObject, "u_ld[1]");
	ldUniform[2] = gl.getUniformLocation(shaderProgramObject, "u_ld[2]");
	lsUniform[0] = gl.getUniformLocation(shaderProgramObject, "u_ls[0]");
	lsUniform[1] = gl.getUniformLocation(shaderProgramObject, "u_ls[1]");
	lsUniform[2] = gl.getUniformLocation(shaderProgramObject, "u_ls[2]");
	lpUniform[0] = gl.getUniformLocation(shaderProgramObject, "u_lp[0]");
	lpUniform[1] = gl.getUniformLocation(shaderProgramObject, "u_lp[1]");
	lpUniform[2] = gl.getUniformLocation(shaderProgramObject, "u_lp[2]");

	lightKeyUniform = gl.getUniformLocation(shaderProgramObject, "u_lkeypressed");

	sphere = new Mesh();

	makeSphere(sphere, 1.0, 40, 40);

	gl.enable(gl.DEPTH_TEST);
	gl.depthFunc(gl.LEQUAL);
	gl.clearDepth(1.0);

	gl.clearColor(0.0, 0.0, 0.0, 1.0);

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
						45.0,
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

	var lightAmbient = [0.0,0.0,0.0];

	var lightDiffuse1 = [1.0,0.0,0.0];
	var lightDiffuse2 = [0.0,1.0,0.0];
	var lightDiffuse3 = [0.0,0.0,1.0];
	
	var lightSpecular1 = [1.0,0.0,0.0];
	var lightSpecular2 = [0.0,1.0,0.0];
	var lightSpecular3 = [0.0,0.0,1.0];

	var lightPosition1 = [0.0, 10.0 * Math.sin(angle), 10.0 * Math.cos(angle), 1.0];
	var lightPosition2 = [10.0 * Math.sin(angle), 0.0, 10.0 * Math.cos(angle), 1.0];
	var lightPosition3 = [10.0 * Math.sin(angle), 10.0 * Math.cos(angle), 0.0, 1.0];
	
	var materialAmbient = [0.0,0.0,0.0];
	var materialDiffuse = [1.0,1.0,1.0];
	var materialSpecular = [1.0,1.0,1.0];
	var materialShininess = 128.0;

	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	gl.useProgram(shaderProgramObject);

	var translateMatrix = mat4.create();
	var rotateMatrix = mat4.create();
	var modelMatrix = mat4.create();
	var viewMatrix = mat4.create();

	var modelViewProjectionMatrix = mat4.create();

	mat4.translate(modelMatrix, translateMatrix, [0.0, 0.0, -5.0]);
	mat4.rotateX(rotateMatrix, rotateMatrix, degToRad(angle));
	mat4.rotateY(rotateMatrix, rotateMatrix, degToRad(angle));
	mat4.rotateZ(rotateMatrix, rotateMatrix, degToRad(angle));

	mat4.multiply(	modelMatrix,
					modelMatrix,
					rotateMatrix);

	// mat4.multiply(	modelViewProjectionMatrix,
	// 				perspectiveProjectionMatrix,
	// 				modelMatrix);

	gl.uniformMatrix4fv(mUniform, false, modelMatrix);
	gl.uniformMatrix4fv(vUniform, false, viewMatrix);
	gl.uniformMatrix4fv(pUniform, false, perspectiveProjectionMatrix);

	if( true === bLight)
	{
		gl.uniform1i(lightKeyUniform,vfToggle);

		gl.uniform3fv(laUniform[0],lightAmbient);
		gl.uniform3fv(laUniform[1],lightAmbient);
		gl.uniform3fv(laUniform[2],lightAmbient);

		gl.uniform3fv(ldUniform[0],lightDiffuse1);
		gl.uniform3fv(ldUniform[1],lightDiffuse2);
		gl.uniform3fv(ldUniform[2],lightDiffuse3);
		
		gl.uniform3fv(lsUniform[0],lightSpecular1);
		gl.uniform3fv(lsUniform[1],lightSpecular2);
		gl.uniform3fv(lsUniform[2],lightSpecular3);

		gl.uniform4fv(lpUniform[0],lightPosition1);
		gl.uniform4fv(lpUniform[1],lightPosition2);
		gl.uniform4fv(lpUniform[2],lightPosition3);

		gl.uniform3fv(kaUniform,materialAmbient);
		gl.uniform3fv(kdUniform,materialDiffuse);
		gl.uniform3fv(ksUniform,materialSpecular);
		gl.uniform1f(kshineUniform,materialShininess);
	}
	else
	{
		gl.uniform1i(lightKeyUniform, 0);
	}

	sphere.draw();

	update();

	requestAnimationFrame(draw, canvas);
}

function update()
{
	angle += 0.02;

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

	case 'v':
	case 'V':
		vfToggle++;

		if( 3 === vfToggle)
			vfToggle = 1;
	}

}

function mouseDown()
{
}
