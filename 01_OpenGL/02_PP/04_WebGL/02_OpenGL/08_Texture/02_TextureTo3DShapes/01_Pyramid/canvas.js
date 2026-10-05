

var canvas = null;
var context = null;

var gl = null;

var bFullscreen = false;

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

var mvpUniform;
var textureSampler;

var vaoPyramid;
var vboPyramidVertices;
var vboPyramidTexcoords;

var vaoCube;
var vboCubeVertices;
var vboCubeTexcoords;

var perspectiveProjectionMatrix;

var angle = 0.0;

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

	var vertexShaderSourceCode =	"#version 300 es\n"								+
									"\n"											+
									"in vec4 vPosition;\n"							+
									"uniform mat4 u_mvpMatrix;\n"					+
									"in vec2 vTexCoord;\n" 							+
									"out vec2 out_TexCoord;\n" 						+
									"void main(void)\n"								+
									"{\n"											+
									"	gl_Position = u_mvpMatrix * vPosition;\n"	+
									"	out_TexCoord = vTexCoord;\n"				+
									"}\n";

	var fragmentShaderSourceCode = 	"#version 300 es\n"				+
									"\n"							+
									"precision highp float;\n"		+
									"in vec2 out_TexCoord;\n" 		+
									"out vec4 FragColor;\n"			+
									"uniform highp sampler2D u_Texture_Sampler;\n" +
									"void main(void)\n"				+
									"{\n"							+
									"		FragColor = texture(u_Texture_Sampler,out_TexCoord);\n"+
									"}\n";

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
	gl.bindAttribLocation(shaderProgramObject, webglMacros.MST_ATTRUIBUTE_TEXCOORD, "vTexCoord");

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

	mvpUniform = gl.getUniformLocation(shaderProgramObject, "u_mvpMatrix");
	textureSampler = gl.getUniformLocation(shaderProgramObject, "u_Texture_Sampler");

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

	var pyramidTexCoords = new Float32Array([	0.5, 1.0,
												0.0, 0.0,
												1.0, 0.0,

												0.5, 1.0,
												0.0, 0.0,
												1.0, 0.0,

												0.5, 1.0,
												0.0, 0.0,
												1.0, 0.0,

												0.5, 1.0,
												0.0, 0.0,
												1.0, 0.0
											]);

	var cubeVertices = new Float32Array([
											// Front
											-1.0, 1.0, 1.0,
											-1.0, -1.0, 1.0,
											1.0, -1.0, 1.0,
											-1.0, 1.0, 1.0,
											1.0, -1.0, 1.0,
											1.0, 1.0, 1.0,

											// Back
											-1.0, 1.0, -1.0,
											-1.0, -1.0, -1.0,
											1.0, -1.0, -1.0,
											-1.0, 1.0, -1.0,
											1.0, -1.0, -1.0,
											1.0, 1.0, -1.0,

											// Right
											1.0, 1.0, 1.0,
											1.0, -1.0, 1.0,
											1.0, -1.0, -1.0,
											1.0, 1.0, 1.0,
											1.0, -1.0, -1.0,
											1.0, 1.0, -1.0,

											// Left
											-1.0, 1.0, 1.0,
											-1.0, -1.0, 1.0,
											-1.0, -1.0, -1.0,
											-1.0, 1.0, 1.0,
											-1.0, -1.0, -1.0,
											-1.0, 1.0, -1.0,

											// Top
											1.0, 1.0, 1.0,
											1.0, 1.0, -1.0,
											-1.0, 1.0, -1.0,
											1.0, 1.0, 1.0,
											-1.0, 1.0, -1.0,
											-1.0, 1.0, 1.0,

											// Bottom
											1.0, -1.0, 1.0,
											1.0, -1.0, -1.0,
											-1.0, -1.0, -1.0,
											1.0, -1.0, 1.0,
											-1.0, -1.0, -1.0,
											-1.0, -1.0, 1.0

	]);

	var cubeTexCoords = new Float32Array([
										    // Front
										    0.0, 1.0,
										    0.0, 0.0,
										    1.0, 0.0,
										    0.0, 1.0,
										    1.0, 0.0,
										    1.0, 1.0,

										    // Back
										    0.0, 1.0,
										    0.0, 0.0,
										    1.0, 0.0,
										    0.0, 1.0,
										    1.0, 0.0,
										    1.0, 1.0,

										    // Right
										    0.0, 1.0,
										    0.0, 0.0,
										    1.0, 0.0,
										    0.0, 1.0,
										    1.0, 0.0,
										    1.0, 1.0,

										    // Left
										    0.0, 1.0,
										    0.0, 0.0,
										    1.0, 0.0,
										    0.0, 1.0,
										    1.0, 0.0,
										    1.0, 1.0,

										    // Top
										    0.0, 1.0,
										    0.0, 0.0,
										    1.0, 0.0,
										    0.0, 1.0,
										    1.0, 0.0,
										    1.0, 1.0,

										    // Bottom
										    0.0, 1.0,
										    0.0, 0.0,
										    1.0, 0.0,
										    0.0, 1.0,
										    1.0, 0.0,
										    1.0, 1.0
										]);


	vaoPyramid = gl.createVertexArray();

	gl.bindVertexArray(vaoPyramid);
	{
		vboPyramidVertices = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, vboPyramidVertices);

		gl.bufferData(gl.ARRAY_BUFFER, pyramidVertices, gl.STATIC_DRAW);
		gl.vertexAttribPointer(webglMacros.MST_ATTRUIBUTE_VERTEX, 3, gl.FLOAT, false, 0, 0);

		gl.enableVertexAttribArray(webglMacros.MST_ATTRUIBUTE_VERTEX);

		gl.bindBuffer(gl.ARRAY_BUFFER, null);

		vboPyramidTexcoords = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, vboPyramidTexcoords);

		gl.bufferData(gl.ARRAY_BUFFER, pyramidTexCoords, gl.STATIC_DRAW);
		gl.vertexAttribPointer(webglMacros.MST_ATTRUIBUTE_TEXCOORD, 2, gl.FLOAT, false, 0, 0);

		gl.enableVertexAttribArray(webglMacros.MST_ATTRUIBUTE_TEXCOORD);

		gl.bindBuffer(gl.ARRAY_BUFFER, null);
	}
	gl.bindVertexArray(null);

	vaoCube = gl.createVertexArray();
	gl.bindVertexArray(vaoCube);
	{
		vboCubeVertices = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, vboCubeVertices);

		gl.bufferData(gl.ARRAY_BUFFER, cubeVertices, gl.STATIC_DRAW);
		gl.vertexAttribPointer(webglMacros.MST_ATTRUIBUTE_VERTEX, 3, gl.FLOAT, false, 0, 0);

		gl.enableVertexAttribArray(webglMacros.MST_ATTRUIBUTE_VERTEX);

		gl.bindBuffer(gl.ARRAY_BUFFER, null);

		vboCubeTexcoords = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, vboCubeTexcoords);

		gl.bufferData(gl.ARRAY_BUFFER, cubeTexCoords, gl.STATIC_DRAW);
		gl.vertexAttribPointer(webglMacros.MST_ATTRUIBUTE_TEXCOORD, 2, gl.FLOAT, false, 0, 0);

		gl.enableVertexAttribArray(webglMacros.MST_ATTRUIBUTE_TEXCOORD);

		gl.bindBuffer(gl.ARRAY_BUFFER, null);
	}
	gl.bindVertexArray(null);


	stoneTexture = gl.createTexture();
	stoneTexture.image = new Image();
	stoneTexture.image.onload = function(){
		
		gl.bindTexture(gl.TEXTURE_2D,stoneTexture);

		gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL,true);

		gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_MAG_FILTER,gl.NEAREST);
		gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_MIN_FILTER,gl.NEAREST);

		gl.texImage2D(gl.TEXTURE_2D,0,gl.RGBA,gl.RGBA,gl.UNSIGNED_BYTE,stoneTexture.image);

		gl.bindTexture(gl.TEXTURE_2D,null);
	}
	stoneTexture.image.src = "stone.png";

	// kundaliTexture = gl.createTexture();
	// kundaliTexture.image = new Image();
	// kundaliTexture.image.onload = function(){
		
	// 	gl.bindTexture(gl.TEXTURE_2D,kundaliTexture);

	// 	gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL,true);

	// 	gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_MAG_FILTER,gl.NEAREST);
	// 	gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_MIN_FILTER,gl.NEAREST);

	// 	gl.texImage2D(gl.TEXTURE_2D,0,gl.RGBA,gl.RGBA,gl.UNSIGNED_BYTE,kundaliTexture.image);

	// 	gl.bindTexture(gl.TEXTURE_2D,null);

	// }
	// kundaliTexture.image.src = "./kundali.png";

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

	var translateMatrix = mat4.create();
	var rotateMatrix = mat4.create();
	var modelViewMatrix = mat4.create();

	var modelViewProjectionMatrix = mat4.create();

	mat4.translate(modelViewMatrix, translateMatrix, [0.0, 0.0, -7.0]);
	mat4.rotateY(rotateMatrix, rotateMatrix, degToRad(angle));

	mat4.multiply(	modelViewMatrix,
					modelViewMatrix,
					rotateMatrix);

	mat4.multiply(	modelViewProjectionMatrix,
					perspectiveProjectionMatrix,
					modelViewMatrix);

	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

	gl.activeTexture(gl.TEXTURE0);
	gl.bindTexture(gl.TEXTURE_2D, stoneTexture);
	gl.uniform1i(textureSampler, 0);

	gl.bindVertexArray(vaoPyramid);

	gl.drawArrays(gl.TRIANGLES, 0, 4 * 3);

	gl.bindVertexArray(null);

	// mat4.identity(translateMatrix);
	// mat4.identity(rotateMatrix);
	// mat4.identity(modelViewMatrix);
	// mat4.identity(modelViewProjectionMatrix);

	// mat4.translate(modelViewMatrix, translateMatrix, [0.0, 0.0, -8.0]);
	// mat4.rotateX(rotateMatrix, rotateMatrix, degToRad(angle));
	// mat4.rotateY(rotateMatrix, rotateMatrix, degToRad(angle));
	// mat4.rotateZ(rotateMatrix, rotateMatrix, degToRad(angle));

	// mat4.multiply(	modelViewMatrix,
	// 				modelViewMatrix,
	// 				rotateMatrix);

	// mat4.multiply(	modelViewProjectionMatrix,
	// 				perspectiveProjectionMatrix,
	// 				modelViewMatrix);

	// gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

	// gl.bindVertexArray(vaoCube);

	// gl.activeTexture(gl.TEXTURE0);
	// gl.bindTexture(gl.TEXTURE_2D, kundaliTexture);
	// gl.uniform1i(textureSampler, 0);

	// gl.drawArrays(gl.TRIANGLES, 0, 6 * 6);

	// gl.bindVertexArray(null);

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

	if(vaoPyramid)
	{
		gl.deleteVertexArray(vaoPyramid);
		vaoPyramid = null;
	}

	if(vboPyramidVertices)
	{
		gl.deleteBuffer(vboPyramidVertices);
		vboPyramidVertices = null;
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

}

function mouseDown()
{
}
