
function main()
{
	var canvas = document.getElementById("MST");
	if(!canvas)
		console.log("Obtaining Canvas Failed")
	else
		console.log("Obtaining Canvas Succeded")

	console.log("Canvas Width = " + canvas.width + " Canvas Height = " + canvas.height)

	var context = canvas.getContext("2d");
	if(!context)
		console.log("Obtaining Context Failed");
	else
		console.log("Obtaining Context Succeded");

	context.fillStyle = "black";
	context.fillRect(0, 0, canvas.width, canvas.height);

	context.textAlign = "center";
	context.textBaseline = "middle";
	context.font = "48px sans-serif";

	var str = "Hello, World!!!";

	context.fillStyle = "white";
	context.fillText(str, canvas.width/2, canvas.height/2);

	window.addEventListener("keydown", keyDown, false);
	window.addEventListener("click", mouseDown, false);
}

function keyDown(event)
{
	alert("Key is Pressed");
}

function mouseDown(event)
{
	alert("Mouse Button Down");
}
