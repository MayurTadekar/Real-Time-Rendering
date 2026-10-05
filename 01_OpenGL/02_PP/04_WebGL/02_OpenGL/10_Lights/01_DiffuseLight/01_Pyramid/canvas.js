

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

const webglMacros = {
						MST_ATTRUIBUTE_VERTEX:0,
						MST_ATTRUIBUTE_NORMAL:1,
						MST_ATTRUIBUTE_COLOR:2,
						MST_ATTRUIBUTE_TEXCOORD:3,
};

var vertexShaderObject;
var fragmentShaderObject;
var shaderProgramObject;

var vao;
var vboVertex;
var vboNormal;

var mvUniform;
var pUniform;
var lightKeyUniform;
var kdUniform;
var ldUniform;
var lpUniform;

var perspectiveProjectionMatrix;

var angle = 0.0;
var bLight = false;
var bFullscreen = false;

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

	var vertexShaderSourceCode =	"#version 300 es" 									+
									"\n" 												+
									"in vec3 vNormal;\n" 								+
									"in vec4 vPosition;\n" 								+
									"uniform mat4 u_mvMatrix;\n" 				+
									"uniform mat4 u_pMatrix;\n" 				+
									"uniform mediump int u_lkeypressed;\n" 				+
									"uniform vec3 u_ld;\n" 								+
									"uniform vec3 u_kd;\n" 								+
									"uniform vec4 u_lp;\n" 					+
									"out vec3 diffuse_light;\n" 						+
									"void main(void)\n" 								+
									"{\n" 												+
									"	if(u_lkeypressed == 1)\n"						+
									"	{\n" 											+
									"		vec4 eye_coordinates = u_mvMatrix*vPosition;\n" 			+
									"		mat3 normal_matrix = mat3(transpose(inverse(u_mvMatrix)));\n" +
									"		vec3 tnorm = normalize(normal_matrix*vNormal);\n" 					+
									"		vec3 s = normalize(vec3(u_lp - eye_coordinates));\n" 	+
									"		diffuse_light = u_ld * u_kd * max(dot(s,tnorm),0.0);\n" 			+
									"	}\n" 											+
									"	gl_Position = u_pMatrix*u_mvMatrix*vPosition;\n" 	+
									"}";

	var fragmentShaderSourceCode = 	"#version 300 es"						+
									"\n"									+
									"precision highp float;\n"				+
									"in vec3 diffuse_light;\n" 				+
									"uniform mediump int u_lkeypressed;\n" 	+
									"out vec4 FragColor;\n" 				+
									"void main(void)\n" 					+
									"{\n"									+
									"	vec4 color;\n" 						+
									"	if(u_lkeypressed == 1)\n"			+
									"	{\n" 								+
									"		color = vec4(diffuse_light,1.0);\n" 	+
									"	}\n" 								+
									"	else\n"								+
									"	{\n" 								+
									"		color = vec4(1.0,1.0,1.0,1.0);\n" 		+
									"	}\n" 								+
									"	FragColor = color;\n"				+
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

	gl.bindAttribLocation(shaderProgramObject, webglMacros.MST_ATTRUIBUTE_VERTEX, "vPosition");

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

	mvUniform = gl.getUniformLocation(shaderProgramObject, "u_mvMatrix");
	pUniform = gl.getUniformLocation(shaderProgramObject, "u_pMatrix");
	kdUniform = gl.getUniformLocation(shaderProgramObject, "u_kd");
	ldUniform = gl.getUniformLocation(shaderProgramObject, "u_ld");
	lpUniform = gl.getUniformLocation(shaderProgramObject, "u_lp");
	lightKeyUniform = gl.getUniformLocation(shaderProgramObject, "u_lkeypressed");

	var pyramidVertices = new Float32Array([	0.0, 1.0, 0.0,
												-1.0, -1.0, 1.0,
												1.0, -1.0, 1.0,

												0.0, 1.0, 0.0,
												1.0, -1.0, 1.0,
												1.0, -1.0, -1.0,

												0.0, 1.0, 0.0,
												-1.0, -1.0, -1.0,
												1.0, -1.0, -1.0,

												0.0, 1.0, 0.0,
												-1.0, -1.0, 1.0,
												-1.0, -1.0, -1.0,
											]);

	var pyramidNormals = new Float32Array([	
											// FRONT
										    0.0,0.447214,0.894427,
										    0.0,0.447214,0.894427,
										    0.0,0.447214,0.894427,

										    // RIGHT
										    0.894427, 0.447214, 0.0,
										    0.894427, 0.447214, 0.0,
										    0.894427, 0.447214, 0.0,
										     
										    // BACK
										    0.0, 0.447214, -0.894427,
										    0.0, 0.447214, -0.894427,
										    0.0, 0.447214, -0.894427,
										     
										    // LEFT
										    -0.894427, 0.447214, 0.0,
										    -0.894427, 0.447214, 0.0,
										    -0.894427, 0.447214, 0.0
										]);

	vao = gl.createVertexArray();

	gl.bindVertexArray(vao);
	{
		vboVertex = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, vboVertex);

		gl.bufferData(gl.ARRAY_BUFFER, pyramidVertices, gl.STATIC_DRAW);
		gl.vertexAttribPointer(webglMacros.MST_ATTRUIBUTE_VERTEX, 3, gl.FLOAT, false, 0, 0);

		gl.enableVertexAttribArray(webglMacros.MST_ATTRUIBUTE_VERTEX);

		gl.bindBuffer(gl.ARRAY_BUFFER, null);


		vboNormal = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, vboNormal);

		gl.bufferData(gl.ARRAY_BUFFER, pyramidNormals, gl.STATIC_DRAW);
		gl.vertexAttribPointer(webglMacros.MST_ATTRUIBUTE_NORMAL, 3, gl.FLOAT, false, 0, 0);

		gl.enableVertexAttribArray(webglMacros.MST_ATTRUIBUTE_NORMAL);

		gl.bindBuffer(gl.ARRAY_BUFFER, null);
	}
	gl.bindVertexArray(null);

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
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	gl.useProgram(shaderProgramObject);

	var light_position = new Float32Array([0.0,0.0,2.0,1.0]);

	var translateMatrix = mat4.create();
	var rotateMatrix = mat4.create();
	var modelViewMatrix = mat4.create();

	var modelViewProjectionMatrix = mat4.create();

	mat4.translate(modelViewMatrix, translateMatrix, [0.0, 0.0, -5.0]);
	mat4.rotateY(rotateMatrix, rotateMatrix, degToRad(angle));

	mat4.multiply(	modelViewMatrix,
					modelViewMatrix,
					rotateMatrix);

	mat4.multiply(	modelViewProjectionMatrix,
					perspectiveProjectionMatrix,
					modelViewMatrix);

	gl.uniformMatrix4fv(mvUniform, false, modelViewMatrix);
	gl.uniformMatrix4fv(pUniform, false, perspectiveProjectionMatrix);

	if( true === bLight)
	{
		gl.uniform1i(lightKeyUniform, 1);
		gl.uniform3f(ldUniform, 1.0, 1.0, 1.0);
		gl.uniform3f(kdUniform, 0.5, 0.5, 0.5);
		gl.uniform4fv(lpUniform, light_position);	
	}
	else
	{
		gl.uniform1i(lightKeyUniform, 0);
	}

	gl.bindVertexArray(vao);

	gl.drawArrays(gl.TRIANGLES, 0, 12);

	gl.bindVertexArray(null);

	gl.useProgram(null);

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
	}

}

function mouseDown()
{
}
