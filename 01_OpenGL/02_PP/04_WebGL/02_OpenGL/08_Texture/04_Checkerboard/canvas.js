

var canvas = null;
var context = null;

var gl = null;

var bFullscreen = false;

var canvas_width;
var canvas_height;

var requestAnimationFrame = window.requestAnimationFrame ||
	window.webkitRequestAnimationFrame ||
	window.mozRequestAnimationFrame ||
	window.oRequestAnimationFrame ||
	window.msRequestAnimationFrame;

const webglMacros = {
	MST_ATTRUIBUTE_VERTEX: 0,
	MST_ATTRUIBUTE_NORMAL: 1,
	MST_ATTRUIBUTE_COLOR: 2,
	MST_ATTRUIBUTE_TEXCOORD: 3,
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

var checkImage = new Uint8Array(64 * 64 * 4);

function main() {
	canvas = document.getElementById("MST");
	if (!canvas)
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

function toggleFullscreen() {
	var fullscreen_elements = document.fullscreenElement ||
		document.webkitFullscreenElement ||
		document.mozkitFullscreenElement ||
		document.msFullscreenElement ||
		null;

	if (null == fullscreen_elements) {
		if (canvas.requestFullscreen) {
			canvas.requestFullscreen();
		}
		else if (canvas.webkitRequestFullscreen) {
			canvas.webkitRequestFullscreen();
		}
		else if (canvas.mozkitRequestFullscreen) {
			canvas.mozkitRequestFullscreen();
		}
		else if (canvas.mskitRequestFullscreen) {
			canvas.mskitRequestFullscreen();
		}

		bFullscreen = true;
	}
	else {
		if (document.exitFullscreen) {
			document.exitFullscreen()
		}
		else if (document.webkitExitFullscreen) {
			document.webkitExitFullscreen()
		}
		else if (document.mozCancelFullscreen) {
			document.mozCancelFullscreen()
		}
		else if (document.msExitFullscreen) {
			document.msExitFullscreen()
		}

		bFullscreen = false;
	}
}

function keyDown(event) {
	switch (event.keyCode) {
		case 70:
			toggleFullscreen();
			break;
	}
}

function mouseDown(event) {
}

function init() {
	gl = canvas.getContext("webgl2");

	if (!gl) {
		console.log("WebGL2 Context Failed");
	}
	else {
		console.log("WebGL2 Context Succeded");
	}

	gl.viewportWidth = canvas.width;
	gl.viewportHeight = canvas.height;

	var vertexShaderSourceCode = "#version 300 es\n" +
		"\n" +
		"in vec4 vPosition;\n" +
		"uniform mat4 u_mvpMatrix;\n" +
		"in vec2 vTexCoord;\n" +
		"out vec2 out_TexCoord;\n" +
		"void main(void)\n" +
		"{\n" +
		"	gl_Position = u_mvpMatrix * vPosition;\n" +
		"	out_TexCoord = vTexCoord;\n" +
		"}\n";

	var fragmentShaderSourceCode = "#version 300 es\n" +
		"\n" +
		"precision highp float;\n" +
		"in vec2 out_TexCoord;\n" +
		"out vec4 FragColor;\n" +
		"uniform highp sampler2D u_Texture_Sampler;\n" +
		"void main(void)\n" +
		"{\n" +
		"		FragColor = texture(u_Texture_Sampler,out_TexCoord);\n" +
		"}\n";

	vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);
	gl.shaderSource(vertexShaderObject, vertexShaderSourceCode);
	gl.compileShader(vertexShaderObject);
	if (gl.getShaderParameter(vertexShaderObject, gl.COMPILE_STATUS) == false) {
		var error = gl.getShaderInfoLog(vertexShaderObject);

		if (0 < error.length) {
			alert(error);
			uinitialize();
		}
	}

	fragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);
	gl.shaderSource(fragmentShaderObject, fragmentShaderSourceCode);
	gl.compileShader(fragmentShaderObject);
	if (gl.getShaderParameter(fragmentShaderObject, gl.COMPILE_STATUS) == false) {
		var error = gl.getShaderInfoLog(fragmentShaderObject);

		if (0 < error.length) {
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

	if (!gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS)) {
		var error = gl.getProgramInfoLog(shaderProgramObject);
		if (0 < error.length) {
			alert(error);
			uninitialize();
		}
	}

	mvpUniform = gl.getUniformLocation(shaderProgramObject, "u_mvpMatrix");
	textureSampler = gl.getUniformLocation(shaderProgramObject, "u_Texture_Sampler");

	var cubeVertices = new Float32Array([
		// Front
		-1.0, 1.0, 0.0,
		-1.0, -1.0, 0.0,
		1.0, -1.0, 0.0,
		-1.0, 1.0, 0.0,
		1.0, -1.0, 0.0,
		1.0, 1.0, 0.0,

	]);

	var cubeTexCoords = new Float32Array([
		// Front
		0.0, 1.0,
		0.0, 0.0,
		1.0, 0.0,
		0.0, 1.0,
		1.0, 0.0,
		1.0, 1.0,
	]);

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

	loadBitmapTexture();

	gl.enable(gl.DEPTH_TEST);
	gl.depthFunc(gl.LEQUAL);
	gl.clearDepth(1.0);

	gl.clearColor(0.4, 0.4, 0.4, 1.0);

	perspectiveProjectionMatrix = mat4.create();
}

function loadBitmapTexture() {

	makeCheckImage();

	checkerImage = gl.createTexture();

	gl.bindTexture(gl.TEXTURE_2D, checkerImage);

	gl.pixelStorei(gl.UNPACK_ALIGNMENT, 4);

	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.REPEAT);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.REPEAT);

	gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, 64, 64, 0, gl.RGBA, gl.UNSIGNED_BYTE, checkImage);

}

function makeCheckImage() {

	var i, j, c = 0;

	for (i = 0; i < 64; i++) {

		for (j = 0; j < 64; j++) {

			c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;

			checkImage[(i * 64 + j) * 4 + 0] = c;
			checkImage[(i * 64 + j) * 4 + 1] = c;
			checkImage[(i * 64 + j) * 4 + 2] = c;
			checkImage[(i * 64 + j) * 4 + 3] = 255;
		}
	}

}

function resize() {
	if (true == bFullscreen) {
		canvas.width = window.innerWidth;
		canvas.height = window.innerHeight;
	}
	else {
		canvas.width = canvas_width;
		canvas.height = canvas_height;
	}

	gl.viewport(0, 0, canvas.width, canvas.height);

	mat4.perspective(perspectiveProjectionMatrix,
		45.0,
		parseFloat(canvas.width) / parseFloat(canvas.height),
		0.1,
		100.0
	);
}

function degToRad(deg) {
	return (deg * Math.PI / 180.0);
}

function draw() {
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	gl.useProgram(shaderProgramObject);

	var translateMatrix = mat4.create();
	var rotateMatrix = mat4.create();
	var modelViewMatrix = mat4.create();

	var modelViewProjectionMatrix = mat4.create();

	mat4.translate(modelViewMatrix, translateMatrix, [-1.5, 0.0, -5.0]);

	mat4.multiply(modelViewMatrix,
		modelViewMatrix,
		rotateMatrix);

	mat4.multiply(modelViewProjectionMatrix,
		perspectiveProjectionMatrix,
		modelViewMatrix);

	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

	gl.bindVertexArray(vaoCube);

	gl.activeTexture(gl.TEXTURE0);
	gl.bindTexture(gl.TEXTURE_2D, checkerImage);
	gl.uniform1i(textureSampler, 0);

	gl.drawArrays(gl.TRIANGLES, 0, 6 * 6);

	gl.bindVertexArray(null);

	var translateMatrix = mat4.create();
	var rotateMatrix = mat4.create();
	var modelViewMatrix = mat4.create();

	var modelViewProjectionMatrix = mat4.create();

	mat4.translate(modelViewMatrix, translateMatrix, [1.5, 0.0, -5.0]);
	mat4.rotateY(modelViewMatrix, modelViewMatrix, -40.0);

	mat4.multiply(modelViewMatrix,
		modelViewMatrix,
		rotateMatrix);

	mat4.multiply(modelViewProjectionMatrix,
		perspectiveProjectionMatrix,
		modelViewMatrix);

	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

	gl.bindVertexArray(vaoCube);

	gl.activeTexture(gl.TEXTURE0);
	gl.bindTexture(gl.TEXTURE_2D, checkerImage);
	gl.uniform1i(textureSampler, 0);

	gl.drawArrays(gl.TRIANGLES, 0, 6 * 6);

	gl.bindVertexArray(null);

	gl.useProgram(null);

	update();

	requestAnimationFrame(draw, canvas);
}

function update() {
	angle += 1.0;

	if (360.0 <= angle) {
		angle -= 360.0;
	}
}

function uninitialize() {

	if (vaoPyramid) {
		gl.deleteVertexArray(vaoPyramid);
		vaoPyramid = null;
	}

	if (vboPyramidVertices) {
		gl.deleteBuffer(vboPyramidVertices);
		vboPyramidVertices = null;
	}

	if (shaderProgramObject) {
		if (fragmentShaderObject) {
			gl.detachShader(shaderProgramObject, fragmentShaderObject);
			gl.deleteShader(fragmentShaderObject);
			fragmentShaderObject = null;
		}

		if (vertexShaderObject) {
			gl.detachShader(shaderProgramObject, vertexShaderObject);
			gl.deleteShader(vertexShaderObject);
			vertexShaderObject = null;
		}

		gl.deleteProgram(shaderProgramObject);
		shaderProgramObject = null;
	}

}

function keyDown(event) {

	switch (event.keyCode) {

		case 27:
			uninitialize();
			window.close();
			break;

		case 70:
			toggleFullscreen();
			break;
	}

}

function mouseDown() {
}
