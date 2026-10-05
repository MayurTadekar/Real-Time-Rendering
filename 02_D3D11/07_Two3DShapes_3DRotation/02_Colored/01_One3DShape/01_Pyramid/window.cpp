

#include<windows.h>
#include<stdio.h>

#include<d3d11.h>
#include<d3dcompiler.h>

#pragma warning(disable:4838)
#include "XNAMath_204\xnamath.h"

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"D3dcompiler.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

FILE *gpFile = NULL;
char gszLogFileName[] = "Log.txt";

HWND ghwnd = NULL;

DWORD dwStyle;

WINDOWPLACEMENT wpPrev = {sizeof(WINDOWPLACEMENT)};

bool gbActiveWindow = false;
bool gbEscapeKeyIsPressed = false;
bool gbFullscreen = false;

float angle_Pyramid = 0.0f;
float angle_Cube = 0.0f;

float gClearColor[4];
IDXGISwapChain *gpIDXGISwapChain = NULL;
ID3D11Device *gpID3D11Device = NULL;
ID3D11DeviceContext *gpID3D11DeviceContext = NULL;
ID3D11RenderTargetView *gpID3D11RenderTargetView = NULL;

ID3D11VertexShader *gpID3D11VertexShader = NULL;
ID3D11PixelShader *gpID3D11PixelShader = NULL;
ID3D11Buffer *gpID3D11Buffer_VertexBuffer_Pyramid_Position = NULL;
ID3D11Buffer *gpID3D11Buffer_VertexBuffer_Pyramid_Color = NULL;
ID3D11Buffer *gpID3D11Buffer_VertexBuffer_Cube_Position = NULL;
ID3D11Buffer *gpID3D11Buffer_VertexBuffer_Cube_Color = NULL;
ID3D11InputLayout *gpID3D11InputLayout = NULL;
ID3D11Buffer *gpID3D11Buffer_ConstantBuffer = NULL;

ID3D11RasterizerState *gpID3D11RasterizerState = NULL;

ID3D11DepthStencilView *gpID3D11DepthStencilView = NULL;

DWORD 	desktopWidth = 0;
DWORD 	desktopHeight = 0;

struct CBUFFER
{
	XMMATRIX WorldViewProjectionMatrix;
};

XMMATRIX gPerspectiveProjectionMatrix;

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszCmdLine,int iCmdShow)
{
	HRESULT initialize(void);
	void uninitialize(void);
	void display(void);

	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[] = TEXT("CWindow");

	bool bDone = false;

	if(fopen_s(&gpFile,"Log.txt","w") != 0)
	{
		MessageBox(NULL,TEXT("Log File Can Not be Created Exitting..."),TEXT("DONE"),MB_OK);
		exit(0);
	}
	else
	{
		fprintf_s(gpFile,"Log file is successfully opened\n");
		fclose(gpFile);
	}

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(NULL,IDI_APPLICATION);
	wndclass.hIconSm = LoadIcon(NULL,IDI_APPLICATION);
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = szClassName;
	wndclass.lpszMenuName = NULL;
	wndclass.hCursor = LoadCursor(NULL,IDC_ARROW);

	RegisterClassEx(&wndclass);

    desktopWidth = GetSystemMetrics(SM_CXSCREEN);
    desktopHeight = GetSystemMetrics(SM_CYSCREEN);

	hwnd = CreateWindowEx(	WS_EX_APPWINDOW,
							szClassName,
							TEXT("Mayur Tadekar - 02_D3DX11 - 3D Colored Pyramid Rotation"),
							WS_OVERLAPPEDWINDOW,
							(desktopWidth / 2 - WIN_WIDTH / 2),
							(desktopHeight / 2 - WIN_HEIGHT / 2),
							WIN_WIDTH,
							WIN_HEIGHT,
							NULL,
							NULL,
							hInstance,
							NULL);

	ghwnd = hwnd;

	ShowWindow(hwnd,iCmdShow);
	SetForegroundWindow(hwnd);

	SetFocus(hwnd);

	HRESULT hr;

	hr = initialize();

	if(FAILED(hr))
	{

		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"initialize() failed.Exitting Now....\n");
		fclose(gpFile);
		DestroyWindow(hwnd);
		hwnd = NULL;
	}
	else
	{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"initialize() succeeded....\n");
		fclose(gpFile);
	}

	while(bDone == false)
	{

		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{
				bDone = true;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			display();
			if(gbActiveWindow == true)
			{
				if(gbEscapeKeyIsPressed == true)
				{
					bDone = true;
				}
			}
		}

	}

	uninitialize();

	return((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd,UINT iMsg,WPARAM wParam,LPARAM lParam)
{

	HRESULT resize(int,int);
	void ToggleFullscreen(void);
	void uninitialize(void);

	HRESULT hr;

	switch(iMsg)
	{

		case WM_ACTIVATE:

			if(HIWORD(wParam) == 0)
			{
				gbActiveWindow = true;
			}
			else
			{
				gbActiveWindow = false;
			}

			break;

		case WM_ERASEBKGND:

			return(0);

			break;

		case WM_SIZE:

			if(gpID3D11DeviceContext){
				hr = resize(LOWORD(lParam),HIWORD(lParam));

				if(FAILED(hr))
				{

					fopen_s(&gpFile,gszLogFileName,"a+");
					fprintf_s(gpFile,"resize() failed\n");
					fclose(gpFile);
					return(hr);
				}
				else
				{
					fopen_s(&gpFile,gszLogFileName,"a+");
					fprintf_s(gpFile,"resize() succeeded\n");
					fclose(gpFile);
				}

			}

			break;

		case WM_KEYDOWN:

			switch(wParam)
			{

				case VK_ESCAPE:

					if(gbEscapeKeyIsPressed == false)
					{
						gbEscapeKeyIsPressed = true;
					}

					break;

				case 0x46:

					if(gbFullscreen == false)
					{
						ToggleFullscreen();
						gbFullscreen = true;
					}
					else
					{
						ToggleFullscreen();
						gbFullscreen = false;
					}
					break;

				default:

				break;
			}

			break;

		case WM_LBUTTONDOWN:
			break;

		case WM_CLOSE:
			uninitialize();
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:

		break;

	}

	return(DefWindowProc(hwnd,iMsg,wParam,lParam));

}

void ToggleFullscreen(void)
{
	
	MONITORINFO mi;

	if(gbFullscreen == false)
	{
		dwStyle = GetWindowLong(ghwnd,GWL_STYLE);

		if(dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi = {sizeof(MONITORINFO)};

			if(GetWindowPlacement(ghwnd,&wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd,MONITORINFOF_PRIMARY),&mi))
			{
				SetWindowLong(ghwnd,GWL_STYLE,dwStyle & ~WS_OVERLAPPEDWINDOW);

				SetWindowPos(ghwnd,HWND_TOP,mi.rcMonitor.left,mi.rcMonitor.top,(mi.rcMonitor.right - mi.rcMonitor.left),(mi.rcMonitor.bottom - mi.rcMonitor.top),SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}

		ShowCursor(FALSE);
	}
	else
	{

		SetWindowLong(ghwnd,GWL_STYLE,dwStyle | WS_OVERLAPPEDWINDOW);

		SetWindowPlacement(ghwnd,&wpPrev);

		SetWindowPos(ghwnd,HWND_TOP,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
			
	}

}

HRESULT PrintD3DInfo(void)
{
	IDXGIFactory *pIDXGIFactory = NULL;
	IDXGIAdapter *pIDXGIAdapter = NULL;
	DXGI_ADAPTER_DESC dxgiAdapterDesc;
	HRESULT hr;
	char str[255];

	hr = CreateDXGIFactory(__uuidof(IDXGIFactory),(void**)&pIDXGIFactory);

	if(FAILED(hr))
	{

		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf(gpFile,"CreateDXGIFactory failed");
		fclose(gpFile);
		return(S_FALSE);
	}

	if(pIDXGIFactory->EnumAdapters(0,&pIDXGIAdapter) == DXGI_ERROR_NOT_FOUND)
	{
		fprintf(gpFile,"EnumAdapters failed");
		return(S_FALSE);
	}

	ZeroMemory((void*)&dxgiAdapterDesc,sizeof(DXGI_ADAPTER_DESC));

	pIDXGIAdapter->GetDesc(&dxgiAdapterDesc);

	WideCharToMultiByte(CP_ACP,0,dxgiAdapterDesc.Description,255,str,255,NULL,NULL);

	fopen_s(&gpFile,gszLogFileName,"a+");

	fprintf(gpFile,"Graphics Card Name = %s\n",str);

	fprintf(gpFile,"Graphic Card VRAM = %I64d bytes\n",dxgiAdapterDesc.DedicatedVideoMemory);

	fprintf(gpFile,"VRAM in GB = %d\n",(int)(ceil(dxgiAdapterDesc.DedicatedVideoMemory/1024.0/1024.0/1024.0 )));

	fclose(gpFile);

	return(S_OK);

}

HRESULT initialize(void)
{

	void uninitialize(void);
	HRESULT resize(int,int);

	HRESULT hr;
	D3D_DRIVER_TYPE d3dDriverType;
	D3D_DRIVER_TYPE d3dDriverTypes[] = {
		D3D_DRIVER_TYPE_HARDWARE,D3D_DRIVER_TYPE_WARP,D3D_DRIVER_TYPE_REFERENCE,
	};

	D3D_FEATURE_LEVEL d3dFeatureLevel_required = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL d3dFeatureLevel_acquired = D3D_FEATURE_LEVEL_10_0;

	UINT createDeviceFlags = 0;
	UINT numDriverTypes = 0;
	UINT numFeatureLevels = 1;

	hr = PrintD3DInfo();

	if(FAILED(hr))
	{


		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"PrintD3DInfo() failed\n");
		fclose(gpFile);
		return(hr);

	}
	else
	{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"PrintD3DInfo() succeeded\n");
		fclose(gpFile);
	}

	numDriverTypes = sizeof(d3dDriverTypes)/sizeof(d3dDriverTypes[0]);

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	ZeroMemory((void*)&dxgiSwapChainDesc,sizeof(DXGI_SWAP_CHAIN_DESC));

	dxgiSwapChainDesc.BufferCount = 1;
	dxgiSwapChainDesc.BufferDesc.Width = WIN_WIDTH;
	dxgiSwapChainDesc.BufferDesc.Height = WIN_HEIGHT;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.OutputWindow = ghwnd;
	dxgiSwapChainDesc.SampleDesc.Count = 1;
	dxgiSwapChainDesc.SampleDesc.Quality = 0;
	dxgiSwapChainDesc.Windowed = TRUE;

	for(UINT driverTypeIndex = 0;driverTypeIndex < numDriverTypes;driverTypeIndex++)
	{

		d3dDriverType = d3dDriverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL,
			d3dDriverType,
			NULL,
			createDeviceFlags,
			&d3dFeatureLevel_required,
			numFeatureLevels,
			D3D11_SDK_VERSION,
			&dxgiSwapChainDesc,
			&gpIDXGISwapChain,
			&gpID3D11Device,
			&d3dFeatureLevel_acquired,
			&gpID3D11DeviceContext);

		if(SUCCEEDED(hr))
		{
			break;
		}

	}

	if(FAILED(hr))
	{


		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"D3D11CreateDeviceAndSwapChain() failed\n");
		fclose(gpFile);
		return(hr);

	}
	else
	{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"D3D11CreateDeviceAndSwapChain() succeeded\n");
		fprintf_s(gpFile,"The Chosen Driver Is Of ");
		if(d3dDriverType == D3D_DRIVER_TYPE_HARDWARE)
		{
			fprintf_s(gpFile,"Hardware Type\n");
		}
		else if(d3dDriverType == D3D_DRIVER_TYPE_WARP)
		{
			fprintf_s(gpFile,"Warp Type\n");
		}
		else if(d3dDriverType == D3D_DRIVER_TYPE_REFERENCE)
		{
			fprintf_s(gpFile,"Reference Type\n");
		}
		else
		{
			fprintf_s(gpFile,"Unknown Type\n");
		}

		fprintf_s(gpFile,"The Supported Highest Feature Level Is ");
		if(d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_11_0)
		{
			fprintf_s(gpFile,"11.0\n");
		}
		else if(d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_10_1)
		{
			fprintf_s(gpFile,"10.1\n");
		}
		else if(d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_10_0)
		{
			fprintf_s(gpFile,"10.0\n");
		}
		else
		{
			fprintf_s(gpFile,"Unknown\n");
		}

		fclose(gpFile);

	}

	
	
	const char *vertexShaderSourceCode = 
	"cbuffer ConstantBuffer" \
	"{" \
	"float4x4 worldViewProjectionMatrix;" \
	"}" \
	"struct vertex_Output{" \
	"float4 position:SV_POSITION;" \
	"float4 color:COLOR;" \
	"};" \
	"vertex_Output main(float4 pos : POSITION,float4 col : COLOR)" \
	"{" \
	"vertex_Output output;" \
	"output.position = mul(worldViewProjectionMatrix,pos);" \
	"output.color = col;" \
	"return(output);" \
	"}";

	

	ID3DBlob *pID3dBlob_VertexShaderCode = NULL;
	ID3DBlob *pID3DBlob_Error = NULL;

	
	hr = D3DCompile(vertexShaderSourceCode,
		lstrlenA(vertexShaderSourceCode) + 1,
		"VS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"vs_5_0",
		0,
		0,
		&pID3dBlob_VertexShaderCode,
		&pID3DBlob_Error
		);

	if(FAILED(hr))
	{

		if(pID3DBlob_Error != NULL)
		{

			fopen_s(&gpFile,gszLogFileName,"a+");
			fprintf_s(gpFile,"D3DCompile() Failed For Vertex Shader : %s.\n",(char*)pID3DBlob_Error->GetBufferPointer());
			fclose(gpFile);
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
			return hr;
		}
		else
		{
			fopen_s(&gpFile,gszLogFileName,"a+");
			fprintf_s(gpFile,"Com Error\n");
			fclose(gpFile);
			return hr;
		}
	}
	else
	{

		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"D3DCompile() Succeeded For Vertex Shader\n");
		fclose(gpFile);

	}

	
	hr = gpID3D11Device->CreateVertexShader(pID3dBlob_VertexShaderCode->GetBufferPointer(),
		pID3dBlob_VertexShaderCode->GetBufferSize(),
		NULL,
		&gpID3D11VertexShader
		);

	if(FAILED(hr))
	{

		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateVertexShader() failed.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateVertexShader() Succeeded\n");
		fclose(gpFile);
	}

	
	gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader,
		NULL,
		0 
		);

	

	const char *pixelShaderSourceCode =
	"struct vertex_Output{" \
	"float4 position:SV_POSITION;" \
	"float4 color:COLOR;" \
	"};" \
	"float4 main(vertex_Output input) : SV_TARGET" \
	"{" \
	"return(input.color);" \
	"}";

	ID3DBlob *pID3dBlob_PixelShaderCode = NULL;
	pID3DBlob_Error = NULL;

	hr = D3DCompile(pixelShaderSourceCode,
		lstrlenA(pixelShaderSourceCode) + 1,
		"PS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_5_0",
		0,
		0,
		&pID3dBlob_PixelShaderCode,
		&pID3DBlob_Error);

	if(FAILED(hr))
	{

		if(pID3DBlob_Error != NULL)
		{

			fopen_s(&gpFile,gszLogFileName,"a+");
			fprintf_s(gpFile,"D3DCompile() Failed For Pixel Shader : %s.\n",(char*)pID3DBlob_Error->GetBufferPointer());
			fclose(gpFile);
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
			return hr;
		}
		else
		{
			fopen_s(&gpFile,gszLogFileName,"a+");
			fprintf_s(gpFile,"Com Error\n");
			fclose(gpFile);
			return hr;
		}
	}
	else
	{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"D3DCompile() Succeeded For Pixel Shader\n");
		fclose(gpFile);
	}

	
	hr = gpID3D11Device->CreatePixelShader(pID3dBlob_PixelShaderCode->GetBufferPointer(),pID3dBlob_PixelShaderCode->GetBufferSize(),NULL,&gpID3D11PixelShader);

	if(FAILED(hr))
	{

		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreatePixelShader() failed.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreatePixelShader() Succeeded\n");
		fclose(gpFile);
	}

	
	gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader,NULL,0);

	

	if(pID3DBlob_Error){
		pID3DBlob_Error->Release();
		pID3DBlob_Error = NULL;
	}

	if(pID3dBlob_PixelShaderCode){
		pID3dBlob_PixelShaderCode->Release();
		pID3dBlob_PixelShaderCode = NULL;
	}

	

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[2];

	inputElementDesc[0].SemanticName = "POSITION"; 
	inputElementDesc[0].SemanticIndex = 0;
	inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[0].InputSlot = 0;
	inputElementDesc[0].AlignedByteOffset = 0;
	inputElementDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[0].InstanceDataStepRate = 0;

	inputElementDesc[1].SemanticName = "COLOR"; 
	inputElementDesc[1].SemanticIndex = 0;
	inputElementDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[1].InputSlot = 1;
	inputElementDesc[1].AlignedByteOffset = 0;
	inputElementDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[1].InstanceDataStepRate = 0;

	

	hr = gpID3D11Device->CreateInputLayout(inputElementDesc,
		_ARRAYSIZE(inputElementDesc),
		pID3dBlob_VertexShaderCode->GetBufferPointer(),
		pID3dBlob_VertexShaderCode->GetBufferSize(),
		&gpID3D11InputLayout);

	if(FAILED(hr))
	{

		if(pID3dBlob_VertexShaderCode){
			pID3dBlob_VertexShaderCode->Release();
			pID3dBlob_VertexShaderCode = NULL;
		}
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateInputLayout() Failed\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		if(pID3dBlob_VertexShaderCode){
			pID3dBlob_VertexShaderCode->Release();
			pID3dBlob_VertexShaderCode = NULL;
		}
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateInputLayout() Succeeded\n");
		fclose(gpFile);
	}

	

	gpID3D11DeviceContext->IASetInputLayout(gpID3D11InputLayout);

	

	float pyramid_vertices[] = {

		
		0.0f, 1.0f, 0.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		
		0.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		
		0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		
		0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,

	};

	float pyramid_color[] = {

		
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f,

		
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,

		
		1.0f, +0.0f, 0.0f,
		0.0f, +0.0f, 1.0f,
		0.0f, +1.0f, 0.0f,

		
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,

	};

	float Cube_vertices[] = {

		-1.0f, +1.0f, +1.0f,
		+1.0f, +1.0f, +1.0f,
		-1.0f, +1.0f, -1.0f,
		-1.0f, +1.0f, -1.0f,
		+1.0f, +1.0f, +1.0f,
		+1.0f, +1.0f, -1.0f,
		
		+1.0f, -1.0f, -1.0f,
		+1.0f, -1.0f, +1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		+1.0f, -1.0f, +1.0f,
		-1.0f, -1.0f, +1.0f,
		
		-1.0f, +1.0f, -1.0f,
		+1.0f, +1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		+1.0f, +1.0f, -1.0f,
		+1.0f, -1.0f, -1.0f,
		
		+1.0f, -1.0f, +1.0f,
		+1.0f, +1.0f, +1.0f,
		-1.0f, -1.0f, +1.0f,
		-1.0f, -1.0f, +1.0f,
		+1.0f, +1.0f, +1.0f,
		-1.0f, +1.0f, +1.0f,
		
		-1.0f, +1.0f, +1.0f,
		-1.0f, +1.0f, -1.0f,
		-1.0f, -1.0f, +1.0f,
		-1.0f, -1.0f, +1.0f,
		-1.0f, +1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		
		+1.0f, -1.0f, -1.0f,
		+1.0f, +1.0f, -1.0f,
		+1.0f, -1.0f, +1.0f,
		+1.0f, -1.0f, +1.0f,
		+1.0f, +1.0f, -1.0f,
		+1.0f, +1.0f, +1.0f,

	};

	float cube_color[] = {

		+1.0f, +0.0f, +0.0f,
		+1.0f, +0.0f, +0.0f,
		+1.0f, +0.0f, +0.0f,
		+1.0f, +0.0f, +0.0f,
		+1.0f, +0.0f, +0.0f,
		+1.0f, +0.0f, +0.0f,

		+0.0f, +1.0f, +0.0f,
		+0.0f, +1.0f, +0.0f,
		+0.0f, +1.0f, +0.0f,
		+0.0f, +1.0f, +0.0f,
		+0.0f, +1.0f, +0.0f,
		+0.0f, +1.0f, +0.0f,

		+0.0f, +0.0f, +1.0f,
		+0.0f, +0.0f, +1.0f,
		+0.0f, +0.0f, +1.0f,
		+0.0f, +0.0f, +1.0f,
		+0.0f, +0.0f, +1.0f,
		+0.0f, +0.0f, +1.0f,

		+0.0f, +1.0f, +1.0f,
		+0.0f, +1.0f, +1.0f,
		+0.0f, +1.0f, +1.0f,
		+0.0f, +1.0f, +1.0f,
		+0.0f, +1.0f, +1.0f,
		+0.0f, +1.0f, +1.0f,

		+1.0f, +0.0f, +1.0f,
		+1.0f, +0.0f, +1.0f,
		+1.0f, +0.0f, +1.0f,
		+1.0f, +0.0f, +1.0f,
		+1.0f, +0.0f, +1.0f,
		+1.0f, +0.0f, +1.0f,

		+1.0f, +1.0f, +0.0f,
		+1.0f, +1.0f, +0.0f,
		+1.0f, +1.0f, +0.0f,
		+1.0f, +1.0f, +0.0f,
		+1.0f, +1.0f, +0.0f,
		+1.0f, +1.0f, +0.0f,

	};

	

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc,sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(float) * _ARRAYSIZE(pyramid_vertices);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	

	hr = gpID3D11Device->CreateBuffer(&bufferDesc,
		NULL,
		&gpID3D11Buffer_VertexBuffer_Pyramid_Position);

	if(FAILED(hr))
	{

		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateBuffer() Failed For Vertex Buffer\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateBuffer() Succeeded For Vertex Buffer\n");
		fclose(gpFile);
	}

	

	ZeroMemory(&bufferDesc,sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(float) * _ARRAYSIZE(pyramid_color);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	

	hr = gpID3D11Device->CreateBuffer(&bufferDesc,
		NULL,
		&gpID3D11Buffer_VertexBuffer_Pyramid_Color);

	if(FAILED(hr))
	{

		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateBuffer() Failed For Color Buffer\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateBuffer() Succeeded For Color Buffer\n");
		fclose(gpFile);
	}

	

	ZeroMemory(&bufferDesc,sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(float) * _ARRAYSIZE(Cube_vertices);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	

	hr = gpID3D11Device->CreateBuffer(&bufferDesc,
		NULL,
		&gpID3D11Buffer_VertexBuffer_Cube_Position);

	if(FAILED(hr))
	{

		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateBuffer() Failed For Vertex Buffer\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateBuffer() Succeeded For Vertex Buffer\n");
		fclose(gpFile);
	}

	

	ZeroMemory(&bufferDesc,sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(float) * _ARRAYSIZE(cube_color);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	

	hr = gpID3D11Device->CreateBuffer(&bufferDesc,
		NULL,
		&gpID3D11Buffer_VertexBuffer_Cube_Color);

	if(FAILED(hr))
	{

		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateBuffer() Failed For Vertex Buffer\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateBuffer() Succeeded For Vertex Buffer\n");
		fclose(gpFile);
	}

	

	

	

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	ZeroMemory(&mappedSubresource,sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer_Pyramid_Position,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedSubresource);

	
	memcpy(mappedSubresource.pData,pyramid_vertices,sizeof(pyramid_vertices));
	
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_Pyramid_Position,0);

	

	ZeroMemory(&mappedSubresource,sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer_Pyramid_Color,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedSubresource);

	
	memcpy(mappedSubresource.pData,pyramid_color,sizeof(pyramid_color));
	
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_Pyramid_Color,0);


	

	

	ZeroMemory(&mappedSubresource,sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer_Cube_Position,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedSubresource);

	
	memcpy(mappedSubresource.pData,Cube_vertices,sizeof(Cube_vertices));
	
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_Cube_Position,0);

	

	ZeroMemory(&mappedSubresource,sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer_Cube_Color,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedSubresource);

	
	memcpy(mappedSubresource.pData,cube_color,sizeof(cube_color));
	
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_Cube_Color,0);

	

	D3D11_BUFFER_DESC bufferDesc_ConstantBuffer;
	ZeroMemory(&bufferDesc_ConstantBuffer,sizeof(D3D11_BUFFER_DESC));
	bufferDesc_ConstantBuffer.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc_ConstantBuffer.ByteWidth = sizeof(CBUFFER);
	bufferDesc_ConstantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	hr = gpID3D11Device->CreateBuffer(&bufferDesc_ConstantBuffer,nullptr,&gpID3D11Buffer_ConstantBuffer);

	if(FAILED(hr))
	{

		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateBuffer() Failed\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateBuffer() Succeeded\n");
		fclose(gpFile);
	}

	gpID3D11DeviceContext->VSSetConstantBuffers(0,
		1,
		&gpID3D11Buffer_ConstantBuffer);

	

	D3D11_RASTERIZER_DESC d3d11RasterizerDesc;

	ZeroMemory((void*)&d3d11RasterizerDesc,sizeof(D3D11_RASTERIZER_DESC));

	d3d11RasterizerDesc.AntialiasedLineEnable = FALSE;
	d3d11RasterizerDesc.CullMode = D3D11_CULL_NONE;
	d3d11RasterizerDesc.DepthBias = 0;
	d3d11RasterizerDesc.DepthBiasClamp = 0.0f;
	d3d11RasterizerDesc.DepthClipEnable = TRUE;
	d3d11RasterizerDesc.FillMode = D3D11_FILL_SOLID; 
	d3d11RasterizerDesc.FrontCounterClockwise = FALSE;
	d3d11RasterizerDesc.MultisampleEnable = FALSE;
	d3d11RasterizerDesc.ScissorEnable = FALSE;
	d3d11RasterizerDesc.SlopeScaledDepthBias = 0.0f;

	hr = gpID3D11Device->CreateRasterizerState(&d3d11RasterizerDesc,&gpID3D11RasterizerState);

	if(FAILED(hr))
	{

		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateRasterizerState() Failed\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateRasterizerState() Succeeded\n");
		fclose(gpFile);
	}

	gpID3D11DeviceContext->RSSetState(gpID3D11RasterizerState);

	

	gClearColor[0] = 0.0f;
	gClearColor[1] = 0.0f;
	gClearColor[2] = 0.0f;
	gClearColor[3] = 1.0f;

	gPerspectiveProjectionMatrix = XMMatrixIdentity();

	hr = resize(WIN_WIDTH,WIN_HEIGHT);
	if(FAILED(hr))
	{

		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"resize() failed\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"resize() succeeded\n");
		fclose(gpFile);
	}

	return(S_OK);

}

HRESULT resize(int width,int height){

	

	if(height < 0){
		height = 1;
	}

	HRESULT hr = S_OK;

	if(gpID3D11RenderTargetView){
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView = NULL;
	}

	if(gpID3D11DepthStencilView){
		gpID3D11DepthStencilView->Release();
		gpID3D11DepthStencilView = NULL;
	}

	gpIDXGISwapChain->ResizeBuffers(1,width,height,DXGI_FORMAT_R8G8B8A8_UNORM,0);

	ID3D11Texture2D *pID3D11Texture2D_BackBuffer;
	gpIDXGISwapChain->GetBuffer(0,__uuidof(ID3D11Texture2D),(LPVOID*)&pID3D11Texture2D_BackBuffer);

	hr = gpID3D11Device->CreateRenderTargetView(pID3D11Texture2D_BackBuffer,NULL,&gpID3D11RenderTargetView);


	if(FAILED(hr))
	{

		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateRenderTargetView() failed\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateRenderTargetView() succeeded\n");
		fclose(gpFile);
	}

	pID3D11Texture2D_BackBuffer->Release();
	pID3D11Texture2D_BackBuffer = NULL;

	

	

	
	

	D3D11_TEXTURE2D_DESC d3d11Texture2DDesc;
	ZeroMemory((void*)&d3d11Texture2DDesc,sizeof(D3D11_TEXTURE2D_DESC));

	d3d11Texture2DDesc.Width = (UINT)width; 
	d3d11Texture2DDesc.Height = (UINT)height; 
	d3d11Texture2DDesc.Format = DXGI_FORMAT_D32_FLOAT; 
	d3d11Texture2DDesc.Usage = D3D11_USAGE_DEFAULT;
	d3d11Texture2DDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	d3d11Texture2DDesc.SampleDesc.Count = 1; 
	d3d11Texture2DDesc.SampleDesc.Quality = 0; 
	d3d11Texture2DDesc.ArraySize = 1;
	d3d11Texture2DDesc.MipLevels = 1; 
	d3d11Texture2DDesc.CPUAccessFlags = 0; 
	d3d11Texture2DDesc.MiscFlags = 0;

	ID3D11Texture2D *pID3D11Texture2D_DepthBuffer = NULL;
	hr = gpID3D11Device->CreateTexture2D(&d3d11Texture2DDesc,
		NULL,
		&pID3D11Texture2D_DepthBuffer);

	if(FAILED(hr))
	{

		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateTexture2D() failed\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateTexture2D() succeeded\n");
		fclose(gpFile);
	}

	

	D3D11_DEPTH_STENCIL_VIEW_DESC d3d11DepthStencilViewDesc;
	ZeroMemory((void*)&d3d11DepthStencilViewDesc,sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	
	d3d11DepthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	d3d11DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS; 

	hr = gpID3D11Device->CreateDepthStencilView(pID3D11Texture2D_DepthBuffer,&d3d11DepthStencilViewDesc,&gpID3D11DepthStencilView);

	if(FAILED(hr))
	{

		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateTexture2D() failed\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateTexture2D() succeeded\n");
		fclose(gpFile);
	}

	

	gpID3D11DeviceContext->OMSetRenderTargets(1,&gpID3D11RenderTargetView,gpID3D11DepthStencilView);
	D3D11_VIEWPORT d3dViewPort;
	d3dViewPort.TopLeftX = 0;
	d3dViewPort.TopLeftY = 0;
	d3dViewPort.Width = (float)width;
	d3dViewPort.Height = (float)height;
	d3dViewPort.MinDepth = 0.0f;
	d3dViewPort.MaxDepth = 1.0f;
	gpID3D11DeviceContext->RSSetViewports(1,&d3dViewPort);

	gPerspectiveProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f),(float)width/(float)height,0.1f,100.0f);



	return(hr);
}

void display(void){

	

	void update(void);

	

	gpID3D11DeviceContext->ClearRenderTargetView(gpID3D11RenderTargetView,gClearColor);

	gpID3D11DeviceContext->ClearDepthStencilView(gpID3D11DepthStencilView,
		D3D11_CLEAR_DEPTH,
		1.0f, 
		0);

	

	UINT stride = sizeof(float)*3;
	UINT offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(0,
		1,
		&gpID3D11Buffer_VertexBuffer_Pyramid_Position,
		&stride,
		&offset);

	stride = sizeof(float)*3;
	offset = 0;

	gpID3D11DeviceContext->IASetVertexBuffers(1,
		1,
		&gpID3D11Buffer_VertexBuffer_Pyramid_Color,
		&stride,
		&offset);

	

	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX translationMatrix = XMMatrixTranslation(0.0f,0.0f,6.0f);
	XMMATRIX rotationMatrix = XMMatrixRotationY(angle_Pyramid);
	XMMATRIX worldMatrix = rotationMatrix * translationMatrix;
	XMMATRIX viewMatrix = XMMatrixIdentity();

	XMMATRIX wvpMatrix = worldMatrix * viewMatrix * gPerspectiveProjectionMatrix;

	CBUFFER constantBuffer;

	constantBuffer.WorldViewProjectionMatrix = wvpMatrix;

	
	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer,
		0,
		NULL,
		&constantBuffer,
		0,
		0);

	gpID3D11DeviceContext->Draw(12,
		0);

	// stride = sizeof(float)*3;
	// offset = 0;

	// gpID3D11DeviceContext->IASetVertexBuffers(0,
	// 	1,
	// 	&gpID3D11Buffer_VertexBuffer_Cube_Position,
	// 	&stride,
	// 	&offset);

	// gpID3D11DeviceContext->IASetVertexBuffers(1,
	// 	1,
	// 	&gpID3D11Buffer_VertexBuffer_Cube_Color,
	// 	&stride,
	// 	&offset);

	// gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// translationMatrix = XMMatrixTranslation(1.5f,0.0f,6.0f);
	// XMMATRIX r1 = XMMatrixRotationX(angle_Cube);
	// XMMATRIX r2 = XMMatrixRotationY(angle_Cube);
	// XMMATRIX r3 = XMMatrixRotationZ(angle_Cube);
	// rotationMatrix = r1 * r2 * r3;
	// XMMATRIX scaleMatrix = XMMatrixScaling(0.75f,0.75f,0.75f);
	// worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;
	// viewMatrix = XMMatrixIdentity();

	// wvpMatrix = worldMatrix * viewMatrix * gPerspectiveProjectionMatrix;

	// constantBuffer;

	// constantBuffer.WorldViewProjectionMatrix = wvpMatrix;

	
	// gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer,
	// 	0,
	// 	NULL,
	// 	&constantBuffer,
	// 	0,
	// 	0);

	// gpID3D11DeviceContext->Draw(6,
	// 	0);

	// gpID3D11DeviceContext->Draw(6,
	// 	6);

	// gpID3D11DeviceContext->Draw(6,
	// 	12);

	// gpID3D11DeviceContext->Draw(6,
	// 	18);

	// gpID3D11DeviceContext->Draw(6,
	// 	24);

	// gpID3D11DeviceContext->Draw(6,
	// 	30);

	gpIDXGISwapChain->Present(0,0);

	update();

}

void update(void){

	angle_Pyramid += 0.0001f;
	if(angle_Pyramid >= 360.0f){
		angle_Pyramid = 0.0f;
	}

	angle_Cube += 0.0001f;
	if(angle_Cube >= 360.0f){
		angle_Cube = 0.0f;
	}

}


void uninitialize(void){

	if(gpID3D11RasterizerState){
		gpID3D11RasterizerState->Release();
		gpID3D11RasterizerState = NULL;
	}

	if(gpID3D11Buffer_ConstantBuffer){
		gpID3D11Buffer_ConstantBuffer->Release();
		gpID3D11Buffer_ConstantBuffer = NULL;
	}

	if(gpID3D11InputLayout){
		gpID3D11InputLayout->Release();
		gpID3D11InputLayout = NULL;
	}

	if(gpID3D11Buffer_VertexBuffer_Pyramid_Position){
		gpID3D11Buffer_VertexBuffer_Pyramid_Position->Release();
		gpID3D11Buffer_VertexBuffer_Pyramid_Position = NULL;
	}

	if(gpID3D11Buffer_VertexBuffer_Pyramid_Color){
		gpID3D11Buffer_VertexBuffer_Pyramid_Color->Release();
		gpID3D11Buffer_VertexBuffer_Pyramid_Color = NULL;
	}

	if(gpID3D11Buffer_VertexBuffer_Cube_Position){
		gpID3D11Buffer_VertexBuffer_Cube_Position->Release();
		gpID3D11Buffer_VertexBuffer_Cube_Position = NULL;
	}

	if(gpID3D11Buffer_VertexBuffer_Cube_Color){
		gpID3D11Buffer_VertexBuffer_Cube_Color->Release();
		gpID3D11Buffer_VertexBuffer_Cube_Color = NULL;
	}

	if(gpID3D11PixelShader){
		gpID3D11PixelShader->Release();
		gpID3D11PixelShader = NULL;
	}

	if(gpID3D11VertexShader){
		gpID3D11VertexShader->Release();
		gpID3D11VertexShader = NULL;
	}

	if(gpID3D11DepthStencilView){
		gpID3D11DepthStencilView->Release();
		gpID3D11DepthStencilView = NULL;
	}

	if(gpID3D11RenderTargetView){
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView = NULL;
	}

	if(gpIDXGISwapChain){
		gpIDXGISwapChain->Release();
		gpIDXGISwapChain = NULL;
	}

	if(gpID3D11DeviceContext){
		gpID3D11DeviceContext->Release();
		gpID3D11DeviceContext = NULL;
	}

	if(gpID3D11Device){
		gpID3D11Device->Release();
		gpID3D11Device = NULL;
	}

	if(gpFile){
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"uninitialize() succeeded\n");
		fprintf_s(gpFile,"Log File is successfully closed\n");
		fclose(gpFile);
	}
}