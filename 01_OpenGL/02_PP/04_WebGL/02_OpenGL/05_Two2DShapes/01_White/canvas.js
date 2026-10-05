

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

var vaoTriangle;
var vboTriangle;

var vaoRectangle;
var vboRectangle;

var perspectiveProjectionMatrix;

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
									"void main(void)\n"								+
									"{\n"											+
									"	gl_Position = u_mvpMatrix * vPosition;\n"	+
									"}\n";

	var fragmentShaderSourceCode = 	"#version 300 es\n"				+
									"\n"							+
									"precision highp float;\n"		+
									"out vec4 FragColor;\n"			+
									"void main(void)\n"				+
									"{\n"							+
									"	FragColor = vec4(1.0);\n"	+
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

	var triangleVertices = new Float32Array([	0.0, 1.0, 0.0,
												-1.0, -1.0, 0.0,
												1.0, -1.0, 0.0
											]);

	var rectangleVertices = new Float32Array([	-1.0, 1.0, 0.0,
												-1.0, -1.0, 0.0,
												1.0, -1.0, 0.0,

												-1.0, 1.0, 0.0,
												1.0, -1.0, 0.0,
												1.0, 1.0, 0.0
											]);

	vaoTriangle = gl.createVertexArray();

	gl.bindVertexArray(vaoTriangle);
	{
		vboTriangle = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, vboTriangle);
		gl.bufferData(gl.ARRAY_BUFFER, triangleVertices, gl.STATIC_DRAW);
		gl.vertexAttribPointer(webglMacros.MST_ATTRUIBUTE_VERTEX, 3, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(webglMacros.MST_ATTRUIBUTE_VERTEX);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);
	}
	gl.bindVertexArray(null);

	vaoRectangle = gl.createVertexArray();
	gl.bindVertexArray(vaoRectangle);
	{
		vboRectangle = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, vboRectangle);
		gl.bufferData(gl.ARRAY_BUFFER, rectangleVertices, gl.STATIC_DRAW);
		gl.vertexAttribPointer(webglMacros.MST_ATTRUIBUTE_VERTEX, 3, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(webglMacros.MST_ATTRUIBUTE_VERTEX);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);
	}
	gl.bindVertexArray(null);

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

function draw()
{
	gl.clear(gl.COLOR_BUFFER_BIT);

	gl.useProgram(shaderProgramObject);

	var translateMatrix = mat4.create();
	var modelViewMatrix = mat4.create();

	var modelViewProjectionMatrix = mat4.create();

	mat4.translate(modelViewMatrix, translateMatrix, [-1.5, 0.0, -5.0]);

	mat4.multiply(	modelViewProjectionMatrix,
					perspectiveProjectionMatrix,
					modelViewMatrix);

	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

	gl.bindVertexArray(vaoTriangle);

	gl.drawArrays(gl.TRIANGLES, 0, 6);

	gl.bindVertexArray(null);

	mat4.translate(modelViewMatrix, translateMatrix, [1.5, 0.0, -5.0]);

	mat4.multiply(	modelViewProjectionMatrix,
					perspectiveProjectionMatrix,
					modelViewMatrix);

	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

	gl.bindVertexArray(vaoRectangle);

	gl.drawArrays(gl.TRIANGLES, 0, 6);

	gl.bindVertexArray(null);

	gl.useProgram(null);

	requestAnimationFrame(draw, canvas);
}

function uninitialize()
{

	if(vaoTriangle)
	{
		gl.deleteVertexArray(vaoTriangle);
		vaoTriangle = null;
	}

	if(vboTriangle)
	{
		gl.deleteBuffer(vboTriangle);
		vboTriangle = null;
	}

	if(vaoRectangle)
	{
		gl.deleteVertexArray(vaoRectangle);
		vaoRectangle = null;
	}

	if(vboRectangle)
	{
		gl.deleteBuffer(vboRectangle);
		vboRectangle = null;
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
