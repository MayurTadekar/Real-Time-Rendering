

var canvas = null;
var context = null;

var gl = null;

var canvas_width;
var canvas_height;

var requestAnimationFrame = window.requestAnimationFrame ||
	window.webkitRequestAnimationFrame ||
	window.mozRequestAnimationFrame ||
	window.oRequestAnimationFrame ||
	window.msRequestAnimationFrame;

const WebGLMacros = {
	MST_ATTRIBUTE_VERTEX: 0,
	MST_ATTRIBUTE_NORMAL: 1,
	MST_ATTRIBUTE_COLOR: 2,
	MST_ATTRIBUTE_TEXTURE0: 3,
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
var bAnimation = false;
var bFullscreen = false;
var toggle_vf = 1;

var vertexShaderObjectCube;
var fragmentShaderObjectCube;
var shaderProgramObjectCube;

var vaoCube;
var vboCubeVertices;
var vboCubeTexcoords;
var mvpUniformCube;
var textureSamplerCube

var fbo;
var colorTexture = 0;
var depthTexture = 0;

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

function initializeAlbedoSphere() {
	var vertexShaderSourceCode = "#version 300 es" +
		"\n" +
		"in vec3 vNormal;\n" +
		"in vec4 vPosition;\n" +
		"uniform vec3 u_la;\n" +
		"uniform vec3 u_ld;\n" +
		"uniform vec3 u_ls;\n" +
		"uniform vec3 u_ka;\n" +
		"uniform vec3 u_kd;\n" +
		"uniform vec3 u_ks;\n" +
		"uniform mediump float u_kshine;\n" +
		"uniform mat4 u_mMatrix;\n" +
		"uniform mat4 u_vMatrix;\n" +
		"uniform mat4 u_pMatrix;\n" +
		"uniform mediump int u_lkeypressed;\n" +
		"uniform vec4 u_lp;\n" +
		"out vec3 transformed_normal;\n" +
		"out vec3 light_direction;\n" +
		"out vec3 view_vector;\n" +
		"out vec3 phong_ads_light_i;\n" +
		"void main(void)\n" +
		"{\n" +
		"	if(u_lkeypressed == 1)\n" +
		"	{\n" +
		"		vec4 eye_coordinates = u_vMatrix * u_mMatrix * vPosition;\n" +
		"		vec3 transformed_normal = normalize(mat3(u_vMatrix * u_mMatrix)*vNormal);\n" +
		"		vec3 light_direction = normalize(vec3(u_lp - eye_coordinates));\n" +
		"		vec3 reflection_vector = reflect(-light_direction,transformed_normal);\n" +
		"		vec3 view_vector = normalize(-(eye_coordinates.xyz));\n" +
		"		vec3 ambient = u_la * u_ka;\n" +
		"		vec3 diffuse = u_ld * u_kd * max(dot(light_direction,transformed_normal),0.0);\n" +
		"		vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector,view_vector),0.0),u_kshine);\n" +
		"		phong_ads_light_i = ambient + diffuse + specular;\n" +
		"	}\n" +
		"	else if(u_lkeypressed == 2)\n" +
		"	{\n" +
		"		vec4 eye_coordinates = u_vMatrix * u_mMatrix * vPosition;\n" +
		"		transformed_normal = mat3(u_vMatrix * u_mMatrix)*vNormal;\n" +
		"		light_direction = vec3(u_lp - eye_coordinates);\n" +
		"		view_vector = -(eye_coordinates.xyz);\n" +
		"	}\n" +
		"	else\n" +
		"	{\n" +
		"		phong_ads_light_i = vec3(1.0,1.0,1.0);\n" +
		"	}\n" +
		"	gl_Position = u_pMatrix * u_vMatrix * u_mMatrix * vPosition;\n" +
		"}";

	var fragmentShaderSourceCode = "#version 300 es" +
		"\n" +
		"precision highp float;\n" +
		"uniform vec3 u_la;\n" +
		"uniform vec3 u_ld;\n" +
		"uniform vec3 u_ls;\n" +
		"uniform vec3 u_ka;\n" +
		"uniform vec3 u_kd;\n" +
		"uniform vec3 u_ks;\n" +
		"uniform mediump float u_kshine;\n" +
		"uniform mediump int u_lkeypressed;\n" +
		"in vec3 transformed_normal;\n" +
		"in vec3 light_direction;\n" +
		"in vec3 view_vector;\n" +
		"in vec3 phong_ads_light_i;\n" +
		"vec3 phong_ads_light;\n" +
		"out vec4 FragColor;\n" +
		"void main(void)\n" +
		"{\n" +
		"	if(u_lkeypressed == 1)\n" +
		"	{\n" +
		"		FragColor = vec4(phong_ads_light_i,1.0);\n" +
		"	}\n" +
		"	else if(u_lkeypressed == 2)\n" +
		"	{\n" +
		"		vec3 normalized_transformed_normal = normalize(transformed_normal);\n" +
		"		vec3 normalized_light_direction = normalize(light_direction);\n" +
		"		vec3 reflection_vector = reflect(-normalized_light_direction,normalized_transformed_normal);\n" +
		"		vec3 normalized_view_vector = normalize(view_vector);\n" +
		"		vec3 ambient = u_la * u_ka;\n" +
		"		vec3 diffuse = u_ld * u_kd * max(dot(normalized_light_direction,normalized_transformed_normal),0.0);\n" +
		"		vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector,normalized_view_vector),0.0),u_kshine);\n" +
		"		phong_ads_light = ambient + diffuse + specular;\n" +
		"		FragColor = vec4(phong_ads_light,1.0);\n" +
		"	}\n" +
		"	else\n" +
		"	{\n" +
		"		FragColor = vec4(1.0,1.0,1.0,1.0);\n" +
		"	}\n" +
		"}";


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

	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.MST_ATTRIBUTE_VERTEX, "vPosition");
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.MST_ATTRIBUTE_NORMAL, "vNormal");

	gl.linkProgram(shaderProgramObject);

	if (!gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS)) {
		var error = gl.getProgramInfoLog(shaderProgramObject);
		if (0 < error.length) {
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

}

function initializeCube() {

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

	vertexShaderObjectCube = gl.createShader(gl.VERTEX_SHADER);
	gl.shaderSource(vertexShaderObjectCube, vertexShaderSourceCode);
	gl.compileShader(vertexShaderObjectCube);
	if (gl.getShaderParameter(vertexShaderObjectCube, gl.COMPILE_STATUS) == false) {
		var error = gl.getShaderInfoLog(vertexShaderObjectCube);

		if (0 < error.length) {
			alert(error);
			uinitialize();
		}
	}

	fragmentShaderObjectCube = gl.createShader(gl.FRAGMENT_SHADER);
	gl.shaderSource(fragmentShaderObjectCube, fragmentShaderSourceCode);
	gl.compileShader(fragmentShaderObjectCube);
	if (gl.getShaderParameter(fragmentShaderObjectCube, gl.COMPILE_STATUS) == false) {
		var error = gl.getShaderInfoLog(fragmentShaderObjectCube);

		if (0 < error.length) {
			alert(error);
			uninitialize();
		}
	}

	shaderProgramObjectCube = gl.createProgram();

	gl.attachShader(shaderProgramObjectCube, vertexShaderObjectCube);
	gl.attachShader(shaderProgramObjectCube, fragmentShaderObjectCube);

	gl.bindAttribLocation(shaderProgramObjectCube, WebGLMacros.MST_ATTRIBUTE_VERTEX, "vPosition");
	gl.bindAttribLocation(shaderProgramObjectCube, WebGLMacros.MST_ATTRIBUTE_TEXTURE0, "vTexCoord");

	gl.linkProgram(shaderProgramObjectCube);

	if (!gl.getProgramParameter(shaderProgramObjectCube, gl.LINK_STATUS)) {
		var error = gl.getProgramInfoLog(shaderProgramObjectCube);
		if (0 < error.length) {
			alert(error);
			uninitialize();
		}
	}

	mvpUniformCube = gl.getUniformLocation(shaderProgramObjectCube, "u_mvpMatrix");
	textureSamplerCube = gl.getUniformLocation(shaderProgramObjectCube, "u_Texture_Sampler");

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

	vaoCube = gl.createVertexArray();
	gl.bindVertexArray(vaoCube);
	{
		vboCubeVertices = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, vboCubeVertices);

		gl.bufferData(gl.ARRAY_BUFFER, cubeVertices, gl.STATIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.MST_ATTRIBUTE_VERTEX, 3, gl.FLOAT, false, 0, 0);

		gl.enableVertexAttribArray(WebGLMacros.MST_ATTRIBUTE_VERTEX);

		gl.bindBuffer(gl.ARRAY_BUFFER, null);

		vboCubeTexcoords = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, vboCubeTexcoords);

		gl.bufferData(gl.ARRAY_BUFFER, cubeTexCoords, gl.STATIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.MST_ATTRIBUTE_TEXTURE0, 2, gl.FLOAT, false, 0, 0);

		gl.enableVertexAttribArray(WebGLMacros.MST_ATTRIBUTE_TEXTURE0);

		gl.bindBuffer(gl.ARRAY_BUFFER, null);
	}
	gl.bindVertexArray(null);
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

	initializeAlbedoSphere();

	initializeCube();

	fbo = gl.createFramebuffer();
	gl.bindFramebuffer(gl.FRAMEBUFFER, fbo);

	colorTexture = gl.createTexture();
	gl.bindTexture(gl.TEXTURE_2D, colorTexture);

	gl.texStorage2D(gl.TEXTURE_2D, 9, gl.RGBA8, 512, 512);

	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);

	gl.bindTexture(gl.TEXTURE_2D, null);

	depthTexture = gl.createTexture();
	gl.bindTexture(gl.TEXTURE_2D, depthTexture);

	gl.texStorage2D(gl.TEXTURE_2D, 9, gl.DEPTH_COMPONENT32F, 512, 512);

	gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, colorTexture, 0);
	gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.DEPTH_ATTACHMENT, gl.TEXTURE_2D, depthTexture, 0);

	gl.bindTexture(gl.TEXTURE_2D, null);

	gl.drawBuffers([gl.COLOR_ATTACHMENT0]);

	gl.bindFramebuffer(gl.FRAMEBUFFER, null);


	gl.enable(gl.DEPTH_TEST);
	gl.depthFunc(gl.LEQUAL);
	gl.clearDepth(1.0);

	gl.clearColor(0.0, 0.0, 0.0, 1.0);

	perspectiveProjectionMatrix = mat4.create();
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

	var lightAmbient = [0.0, 0.0, 0.0];
	var lightDiffuse = [1.0, 1.0, 1.0];
	var lightPosition = [100.0, 100.0, 100.0, 1.0];
	var lightSpecular = [1.0, 1.0, 1.0];

	var materialAmbient = [0.1, 0.1, 0.1];
	var materialDiffuse = [0.5, 0.2, 0.7];
	var materialSpecular = [0.7, 0.7, 0.7];
	var materialShininess = 128.0;

	var blue = [0.0, 0.0, 0.3, 1.0];
	var one = 1.0;

	var translateMatrix = mat4.create();
	var rotateMatrix = mat4.create();
	var modelMatrix = mat4.create();
	var viewMatrix = mat4.create();

	gl.bindFramebuffer(gl.FRAMEBUFFER, fbo);

	gl.viewport(0, 0, 512, 512);
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	mat4.translate(modelMatrix, translateMatrix, [0.0, 0.0, -5.0]);
	mat4.rotateX(rotateMatrix, rotateMatrix, degToRad(angle));
	mat4.rotateY(rotateMatrix, rotateMatrix, degToRad(angle));
	mat4.rotateZ(rotateMatrix, rotateMatrix, degToRad(angle));

	mat4.multiply(modelMatrix,
		modelMatrix,
		rotateMatrix);

	gl.uniformMatrix4fv(mUniform, false, modelMatrix);
	gl.uniformMatrix4fv(vUniform, false, viewMatrix);
	gl.uniformMatrix4fv(pUniform, false, perspectiveProjectionMatrix);

	if (true === bLight) {
		gl.uniform1i(lightKeyUniform, toggle_vf);

		gl.uniform3fv(laUniform, lightAmbient);
		gl.uniform3fv(ldUniform, lightDiffuse);
		gl.uniform3fv(lsUniform, lightSpecular);
		gl.uniform4fv(lpUniform, lightPosition);

		gl.uniform3fv(kaUniform, materialAmbient);
		gl.uniform3fv(kdUniform, materialDiffuse);
		gl.uniform3fv(ksUniform, materialSpecular);
		gl.uniform1f(kshineUniform, materialShininess);
	}
	else {
		gl.uniform1i(lightKeyUniform, 0);
	}

	sphere.draw();

	gl.bindFramebuffer(gl.FRAMEBUFFER, null);

	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	rotateMatrix = mat4.create();

	mat4.lookAt(viewMatrix,
		[0.0, 0.0, 5.0],   // eye
		[0.0, 0.0, 0.0],   // center
		[0.0, 1.0, 0.0]    // up
	);


	gl.activeTexture(gl.TEXTURE0);
	gl.bindTexture(gl.TEXTURE_2D, colorTexture);

	gl.viewport(0, 0, canvas.width, canvas.height);

	gl.useProgram(shaderProgramObjectCube);

	gl.clearBufferfv(gl.COLOR, 0, blue);


	var mvpMatrix = mat4.create();

	mat4.rotateX(rotateMatrix, rotateMatrix, degToRad(angle));
	mat4.rotateY(rotateMatrix, rotateMatrix, degToRad(angle));
	mat4.rotateZ(rotateMatrix, rotateMatrix, degToRad(angle));

	mat4.multiply(modelMatrix,
		modelMatrix,
		rotateMatrix);

	mat4.multiply(mvpMatrix, viewMatrix, modelMatrix);
	mat4.multiply(mvpMatrix, perspectiveProjectionMatrix, mvpMatrix);

	gl.uniformMatrix4fv(mvpUniformCube, false, mvpMatrix);
	gl.uniform1i(textureSamplerCube, 0);

	gl.bindVertexArray(vaoCube);

	gl.drawArrays(gl.TRIANGLES, 0, 36);

	gl.bindVertexArray(null);

	gl.useProgram(null);

	update();

	requestAnimationFrame(draw, canvas);
}

function update() {

	if(bAnimation)
		angle += 1.0;

	if (360.0 <= angle) {
		angle -= 360.0;
	}
}

function uninitialize() {

	if (vao) {
		gl.deleteVertexArray(vao);
		vao = null;
	}

	if (vboVertex) {
		gl.deleteBuffer(vboVertex);
		vboVertex = null;
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

	switch (event.key) {
		case 'l':
		case 'L':
			if (false === bLight) {
				bLight = true;
			}
			else {
				bLight = false;
			}
			break;

		case 'a':
		case 'A':
			if(bAnimation)
				bAnimation = false;
			else
				bAnimation = true;
			break;

		case 'v':
		case 'V':
			toggle_vf++;

			if (3 === toggle_vf)
				toggle_vf = 1;
	}

}

function mouseDown() {
}
