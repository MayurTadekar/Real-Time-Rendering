

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

const WebGLMacros = {
						MST_ATTRIBUTE_VERTEX:0,
						MST_ATTRIBUTE_NORMAL:1,
						MST_ATTRIBUTE_COLOR:2,
						MST_ATTRIBUTE_TEXTURE0:3,
};

var vertexShaderObject;
var fragmentShaderObject;
var shaderProgramObject;

var mvpUniform;

var perspectiveProjectionMatrix;

var sphere = null;
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

	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.MST_ATTRIBUTE_VERTEX, "vPosition");

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
	return( deg * Math.PI / 180.0 );
}

function draw()
{
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	gl.useProgram(shaderProgramObject);

	var translateMatrix = mat4.create();
	var rotateMatrix = mat4.create();
	var modelViewMatrix = mat4.create();

	var modelViewProjectionMatrix = mat4.create();

	mat4.translate(modelViewMatrix, translateMatrix, [0.0, 0.0, -5.0]);
	mat4.rotateX(rotateMatrix, rotateMatrix, degToRad(angle));

	mat4.multiply(	modelViewMatrix,
					modelViewMatrix,
					rotateMatrix);

	mat4.multiply(	modelViewProjectionMatrix,
					perspectiveProjectionMatrix,
					modelViewMatrix);

	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

	sphere.draw();

	gl.useProgram(null);

	update();

	requestAnimationFrame(draw, canvas);
}

function update()
{
	angle += 1.0;

	if( angle <= 360.0 )
	{
		angle -= 360.0;
	}
}

function uninitialize()
{

	if(sphere)
	{
		sphere.deallocate();
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
