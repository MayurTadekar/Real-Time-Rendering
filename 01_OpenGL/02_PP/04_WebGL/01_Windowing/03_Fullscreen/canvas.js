

var canvas = null;
var context = null;

function main()
{
	canvas = document.getElementById("MST");
	if(!canvas)
		console.log("Obtaining Canvas Failed")
	else
		console.log("Obtaining Canvas Succeded")

	console.log("Canvas Width = " + canvas.width + " Canvas Height = " + canvas.height)

	context = canvas.getContext("2d");
	if(!context)
		console.log("Obtaining Context Failed");
	else
		console.log("Obtaining Context Succeded");

	context.fillStyle = "black";
	context.fillRect(0, 0, canvas.width, canvas.height);

	drawText("Hello, World!!!");

	window.addEventListener("keydown", keyDown, false);
	window.addEventListener("click", mouseDown, false);
}

function drawText(text)
{

	context.textAlign = "center";
	context.textBaseline = "middle";
	context.font = "48px sans-serif";
	context.fillStyle = "white";
	context.fillText(text, canvas.width/2, canvas.height/2);

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
	}
}

function keyDown(event)
{
	switch(event.keyCode)
	{
	case 70:
		toggleFullscreen();
		drawText("Hello, World!!!");
	}
}

function mouseDown(event)
{
}
