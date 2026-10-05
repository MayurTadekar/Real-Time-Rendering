

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

	var vertexShaderSourceCode =	"#version 300 es\n" 						+
									"in vec3 vNormal;\n" 						+
									"in vec4 vPosition;\n" 						+
									"uniform mat4 u_mMatrix;\n" 			+
									"uniform mat4 u_vMatrix;\n" 			+
									"uniform mat4 u_pMatrix;\n" 		+
									"uniform int u_lkeypressed;\n" 				+
									"uniform vec3 u_la[2];\n" 					+
									"uniform vec3 u_ld[2];\n" 					+
									"uniform vec4 u_lp[2];\n" 		+
									"uniform vec3 u_ls[2];\n" 					+
									"uniform vec3 u_ka;\n" 						+
									"uniform vec3 u_kd;\n" 						+
									"uniform vec3 u_ks;\n" 						+
									"uniform float u_kshine;\n" 				+
									"out vec3 phong_ads_light;\n" 				+
									"void main(void)\n" 						+
									"{\n" 										+
									"	vec3 ambient[2];\n" 					+
									"	vec3 diffuse[2];\n" 					+
									"	vec3 specular[2];\n" 					+
									"	int i;\n" 								+
									"	if(u_lkeypressed == 1)\n" 				+
									"	{\n" 									+
									"		vec4 eye_coordinates = u_vMatrix * u_mMatrix * vPosition;\n" 				+
									"		vec3 transformed_normal = normalize(mat3(u_vMatrix * u_mMatrix)*vNormal);\n" +
									"		vec3 light_direction[2];\n" 		+
									"		vec3 reflection_vector[2];\n" 		+
									"		vec3 view_vector = normalize(-(eye_coordinates.xyz));\n" +
									"		for(i = 0;i < 2;i++)\n" 			+
									"		{\n" 								+
									"			ambient[i] = u_la[i] * u_ka;\n"	+
									"			light_direction[i] = normalize(vec3(u_lp[i] - eye_coordinates));\n" 	+
									"			diffuse[i] = u_ld[i] * u_kd * max(dot(light_direction[i],transformed_normal),0.0);\n"	+
									"			reflection_vector[i] = reflect(-light_direction[i],transformed_normal);\n" 		+
									"			specular[i] = u_ls[i] * u_ks * pow(max(dot(reflection_vector[i],view_vector),0.0),u_kshine);\n" +
									"			phong_ads_light = phong_ads_light + ambient[i] + diffuse[i] + specular[i];\n" 	+
									"		}\n" 									+	
									"	}\n" 										+	
									"	else\n" 									+
									"	{\n" 										+
									"		phong_ads_light = vec3(1.0,1.0,1.0);\n"	+
									"	}\n" 										+
									"	gl_Position = u_pMatrix * u_vMatrix * u_mMatrix * vPosition;\n" 	+
									"}\n";

	var fragmentShaderSourceCode = 	"#version 300 es"					+
									"\n"								+
									"precision highp float;\n" 			+
									"in vec3 phong_ads_light;\n" 		+
									"out vec4 FragColor;\n" 			+
									"void main(void)" 					+
									"{" 								+
									"	FragColor = vec4(phong_ads_light,1.0);\n"	+
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
	gl.bindAttribLocation(shaderProgramObject, webglMacros.MST_ATTRUIBUTE_NORMAL, "vNormal");

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
	
	ldUniform[0] = gl.getUniformLocation(shaderProgramObject, "u_ld[0]");
	ldUniform[1] = gl.getUniformLocation(shaderProgramObject, "u_ld[1]");
	
	lsUniform[0] = gl.getUniformLocation(shaderProgramObject, "u_ls[0]");
	lsUniform[1] = gl.getUniformLocation(shaderProgramObject, "u_ls[1]");

	lpUniform[0] = gl.getUniformLocation(shaderProgramObject, "u_lp[0]");
	lpUniform[1] = gl.getUniformLocation(shaderProgramObject, "u_lp[1]");

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
	var light_am1 =  [0.0,0.0,0.0];
    var light_di1 =  [1.0,0.0,0.0];
    var light_sp1 =  [1.0,0.0,0.0];
    var light_pos1 = [-2.0,0.0,1.0,1.0];

    var light_am2 =  [0.0,0.0,0.0,];
    var light_di2 =  [0.0,0.0,1.0,];
    var light_sp2 =  [0.0,0.0,1.0,];
    var light_pos2 = [2.0,0.0,1.0,1.0];

    var materialAmbient = [0.0,0.0,0.0];
    var materialDiffuse = [1.0,1.0,1.0];
    var materialSpecular = [1.0,1.0,1.0];
    var materialShininess = 128.0;

	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	gl.useProgram(shaderProgramObject);

	var light_position = new Float32Array([0.0,0.0,2.0,1.0]);

	var translateMatrix = mat4.create();
	var rotateMatrix = mat4.create();
	var modelMatrix = mat4.create();
	var viewMatrix = mat4.create();

	var modelViewProjectionMatrix = mat4.create();

	mat4.translate(modelMatrix, translateMatrix, [0.0, 0.0, -5.0]);
	mat4.rotateY(rotateMatrix, rotateMatrix, degToRad(angle));

	mat4.multiply(	modelMatrix,
					modelMatrix,
					rotateMatrix);

	// mat4.multiply(	modelViewProjectionMatrix,
	// 				perspectiveProjectionMatrix,
	// 				modelViewMatrix);

	gl.uniformMatrix4fv(mUniform, false, modelMatrix);
	gl.uniformMatrix4fv(vUniform, false, viewMatrix);
	gl.uniformMatrix4fv(pUniform, false, perspectiveProjectionMatrix);

	if( true === bLight)
	{

        gl.uniform1i(lightKeyUniform,1);

        gl.uniform3fv(laUniform[0],light_am1);
        gl.uniform3fv(ldUniform[0],light_di1);
        gl.uniform3fv(lsUniform[0],light_sp1);
        gl.uniform4fv(lpUniform[0],light_pos1);

        gl.uniform3fv(laUniform[1],light_am2);
        gl.uniform3fv(ldUniform[1],light_di2);
        gl.uniform3fv(lsUniform[1],light_sp2);
        gl.uniform4fv(lpUniform[1],light_pos2);

        gl.uniform3fv(kaUniform,materialAmbient);
        gl.uniform3fv(kdUniform,materialDiffuse);
        gl.uniform3fv(ksUniform,materialSpecular);
        gl.uniform1f(kshineUniform,materialShininess);
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
