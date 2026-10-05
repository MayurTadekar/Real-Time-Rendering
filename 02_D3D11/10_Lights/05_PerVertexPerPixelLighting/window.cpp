

#include<windows.h>
#include<stdio.h>

#include<d3d11.h>
#include<d3dcompiler.h>

#pragma warning(disable:4838)
#include "XNAMath_204\xnamath.h"
#include"Sphere.h"

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"D3dcompiler.lib")
#pragma comment(lib,"Sphere.lib")

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

float gClearColor[4];
IDXGISwapChain *gpIDXGISwapChain = NULL;
ID3D11Device *gpID3D11Device = NULL;
ID3D11DeviceContext *gpID3D11DeviceContext = NULL;
ID3D11RenderTargetView *gpID3D11RenderTargetView = NULL;

ID3D11VertexShader *gpID3D11VertexShader_Per_Vertex = NULL;
ID3D11PixelShader *gpID3D11PixelShader_Per_Vertex = NULL;

ID3D11VertexShader *gpID3D11VertexShader_Per_Fragment = NULL;
ID3D11PixelShader *gpID3D11PixelShader_Per_Fragment = NULL;


ID3D11Buffer *gpID3D11Buffer_VertexBuffer_Sphere_Position = NULL;
ID3D11Buffer *gpID3D11Buffer_VertexBuffer_Sphere_Normal = NULL;
ID3D11Buffer *gpID3D11Buffer_VertexBuffer_Index = NULL;
ID3D11InputLayout *gpID3D11InputLayout_Per_Vertex = NULL;
ID3D11InputLayout *gpID3D11InputLayout_Per_Fragment = NULL;

ID3D11Buffer *gpID3D11Buffer_ConstantBuffer_Per_Vertex = NULL;
ID3D11Buffer *gpID3D11Buffer_ConstantBuffer_Per_Fragment = NULL;

ID3D11RasterizerState *gpID3D11RasterizerState = NULL;

ID3D11DepthStencilView *gpID3D11DepthStencilView = NULL;


UINT gNumVertices;
UINT gNumElements;

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];

float lightAmbient[] = {0.0f,0.0f,0.0f,1.0f};
float lightDiffuse[] = {1.0f,1.0f,1.0f,1.0f};
float lightSpecular[] = {1.0f,1.0f,1.0f,1.0f};
float lightPosition[] = {100.0f,100.0f,-100.0f,1.0f};

float materialAmbient[] = {0.1f, 0.1f, 0.1f,1.0f};
float materialDiffuse[] = {0.5f, 0.2f, 0.7f,1.0f};
float materialSpecular[] = {0.7f,0.7f,0.7f,1.0f};
float materialShininess = 50.0f;

struct CBUFFER
{
	XMMATRIX WorldMatrix;
	XMMATRIX ViewMatrix;
	XMMATRIX ProjectionMatrix;

	XMVECTOR La;
	XMVECTOR Ld;
	XMVECTOR Ls;
	XMVECTOR LightPosition;

	XMVECTOR Ka;
	XMVECTOR Kd;
	XMVECTOR Ks;

	float MaterialShininess;

};

char light = 'v';

XMMATRIX gPerspectiveProjectionMatrix;

DWORD 	desktopWidth = 0;
DWORD 	desktopHeight = 0;

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
							TEXT("Mayur Tadekar - 02_D3DX11 - PVPF Lighting"),
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

		case WM_CHAR:

			switch(LOWORD(wParam)){
				case 'v':
				case 'V':

					light = 'v';

				break;

				case 'F':
				case 'f':

					light = 'f';

				break;

				case 'q':
				case 'Q':

					if(gbEscapeKeyIsPressed == false)
					{
						gbEscapeKeyIsPressed = true;
					}

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

	fprintf(gpFile,"Graphic Card VRAM = %ld bytes\n",dxgiAdapterDesc.DedicatedVideoMemory);

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

	

	
	
	const char *vertexShaderSourceCode_Per_Vertex = 
	"cbuffer ConstantBuffer" \
	"{" \
	"float4x4 worldMatrix;" \
	"float4x4 viewMatrix;" \
	"float4x4 projectionMatrix;" \
	"float4 la;" \
	"float4 ld;" \
	"float4 ls;" \
	"float4 lightPosition;" \
	"float4 ka;" \
	"float4 kd;" \
	"float4 ks;" \
	"float materialShininess;" \
	"}" \
	"struct vertex_Output{" \
	"float4 position:SV_POSITION;" \
	"float4 phong_ADS_Light:COLOR;" \
	"};" \
	"vertex_Output main(float4 pos : POSITION,float4 normal : NORMAL)" \
	"{" \
	"vertex_Output output;" \
	"float4 eyePosition = mul(worldMatrix,pos);" \
	"eyePosition = mul(viewMatrix,eyePosition);" \
	"float3 transformed_normals = (float3)normalize(mul((float3x3)worldMatrix,(float3)normal));" \
	"float3 light_Direction = (float3)normalize(lightPosition - eyePosition);" \
	"float4 ambient = la * ka;" \
	"float4 diffuse = ld * kd * max(dot(light_Direction,transformed_normals),0.0);" \
	"float3 reflection_vector = reflect(-light_Direction,transformed_normals);" \
	"float3 viewer_vector = normalize(-eyePosition.xyz);" \
	"float4 specular = ls * ks * pow(max(dot(reflection_vector,viewer_vector),0.0),materialShininess);" \
	"output.phong_ADS_Light = ambient + diffuse + specular;" \
	"float4 position = mul(worldMatrix,pos);" \
	"position = mul(viewMatrix,position);" \
	"position = mul(projectionMatrix,position);" \
	"output.position = position;" \
	"return(output);" \
	"}";

	

	ID3DBlob *pID3dBlob_VertexShaderCode_Per_Vertex = NULL;
	ID3DBlob *pID3DBlob_Error = NULL;

	
	hr = D3DCompile(vertexShaderSourceCode_Per_Vertex,
		lstrlenA(vertexShaderSourceCode_Per_Vertex) + 1,
		"VS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"vs_5_0",
		0,
		0,
		&pID3dBlob_VertexShaderCode_Per_Vertex,
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

	
	hr = gpID3D11Device->CreateVertexShader(pID3dBlob_VertexShaderCode_Per_Vertex->GetBufferPointer(),
		pID3dBlob_VertexShaderCode_Per_Vertex->GetBufferSize(),
		NULL,
		&gpID3D11VertexShader_Per_Vertex 
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

	

	const char *pixelShaderSourceCode_Per_Vertex =
	"struct vertex_Output{" \
	"float4 position:SV_POSITION;" \
	"float4 phong_ADS_Light:COLOR;" \
	"};" \
	"float4 main(vertex_Output input) : SV_TARGET" \
	"{" \
	"float4 color = input.phong_ADS_Light;" \
	"return(color);" \
	"}";

	ID3DBlob *pID3dBlob_PixelShaderCode_Per_Vertex = NULL;
	pID3DBlob_Error = NULL;

	hr = D3DCompile(pixelShaderSourceCode_Per_Vertex,
		lstrlenA(pixelShaderSourceCode_Per_Vertex) + 1,
		"PS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_5_0",
		0,
		0,
		&pID3dBlob_PixelShaderCode_Per_Vertex,
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

	
	hr = gpID3D11Device->CreatePixelShader(pID3dBlob_PixelShaderCode_Per_Vertex->GetBufferPointer(),pID3dBlob_PixelShaderCode_Per_Vertex->GetBufferSize(),NULL,&gpID3D11PixelShader_Per_Vertex);

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
	

	if(pID3DBlob_Error){
		pID3DBlob_Error->Release();
		pID3DBlob_Error = NULL;
	}

	

	

	
	
	const char *vertexShaderSourceCode_Per_Fragment = 
	"cbuffer ConstantBuffer" \
	"{" \
	"float4x4 worldMatrix;" \
	"float4x4 viewMatrix;" \
	"float4x4 projectionMatrix;" \
	"float4 la;" \
	"float4 ld;" \
	"float4 ls;" \
	"float4 lightPosition;" \
	"float4 ka;" \
	"float4 kd;" \
	"float4 ks;" \
	"float materialShininess;" \
	"}" \
	"struct vertex_Output{" \
	"float4 position            : SV_POSITION;" \
	"float3 transformed_normals : NORMAL0;" \
	"float3 light_Direction     : NORMAL1;" \
	"float3 viewer_vector       : NORMAL2;" \
	"};" \
	"vertex_Output main(float4 pos : POSITION,float4 normal : NORMAL)" \
	"{" \
	"vertex_Output output;" \
	"float4 eyePosition = mul(worldMatrix,pos);" \
	"eyePosition = mul(viewMatrix,eyePosition);" \
	"output.transformed_normals = (float3)mul((float3x3)worldMatrix,(float3)normal);" \
	"output.light_Direction = (float3)lightPosition - eyePosition.xyz;" \
	"output.viewer_vector = -eyePosition.xyz;" \
	"float4 position = mul(worldMatrix,pos);" \
	"position = mul(viewMatrix,position);" \
	"position = mul(projectionMatrix,position);" \
	"output.position = position;" \
	"return(output);" \
	"}";

	

	ID3DBlob *pID3dBlob_VertexShaderCode_Per_Fragment = NULL;
	pID3DBlob_Error = NULL;

	
	hr = D3DCompile(vertexShaderSourceCode_Per_Fragment,
		lstrlenA(vertexShaderSourceCode_Per_Fragment) + 1,
		"VS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"vs_5_0",
		0,
		0,
		&pID3dBlob_VertexShaderCode_Per_Fragment,
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

	
	hr = gpID3D11Device->CreateVertexShader(pID3dBlob_VertexShaderCode_Per_Fragment->GetBufferPointer(),
		pID3dBlob_VertexShaderCode_Per_Fragment->GetBufferSize(),
		NULL,
		&gpID3D11VertexShader_Per_Fragment
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

	

	const char *pixelShaderSourceCode_Per_Fragment =
	"cbuffer ConstantBuffer" \
	"{" \
	"float4x4 worldMatrix;" \
	"float4x4 viewMatrix;" \
	"float4x4 projectionMatrix;" \
	"float4 la;" \
	"float4 ld;" \
	"float4 ls;" \
	"float4 lightPosition;" \
	"float4 ka;" \
	"float4 kd;" \
	"float4 ks;" \
	"float materialShininess;" \
	"}" \
	"struct vertex_Output{" \
	"float4 position            : SV_POSITION;" \
	"float3 transformed_normals : NORMAL0;" \
	"float3 light_Direction     : NORMAL1;" \
	"float3 viewer_vector       : NORMAL2;" \
	"};" \
	"float4 main(vertex_Output input) : SV_TARGET" \
	"{" \
	"float3 phong_ADS_Light;" \
	"float3 normalized_transformed_normals = normalize(input.transformed_normals);" \
	"float3 normalized_light_direction = normalize(input.light_Direction);" \
	"float3 normalized_viewer_vector = normalize(input.viewer_vector);" \
	"float3 ambient = (float3)(la * ka);" \
	"float3 diffuse = (float3)(ld * kd) * max(dot(normalized_light_direction,normalized_transformed_normals),0.0);" \
	"float3 reflection_vector = reflect(-normalized_light_direction,normalized_transformed_normals);" \
	"float3 specular = (float3)(ls * ks) * pow(max(dot(reflection_vector,normalized_viewer_vector),0.0),materialShininess);" \
	"phong_ADS_Light = ambient + diffuse + specular;" \
	"float4 color = float4(phong_ADS_Light,1.0);" \
	"return(color);" \
	"}";

	ID3DBlob *pID3dBlob_PixelShaderCode_Per_Fragment = NULL;
	pID3DBlob_Error = NULL;

	hr = D3DCompile(pixelShaderSourceCode_Per_Fragment,
		lstrlenA(pixelShaderSourceCode_Per_Fragment) + 1,
		"PS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_5_0",
		0,
		0,
		&pID3dBlob_PixelShaderCode_Per_Fragment,
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

	
	hr = gpID3D11Device->CreatePixelShader(pID3dBlob_PixelShaderCode_Per_Fragment->GetBufferPointer(),pID3dBlob_PixelShaderCode_Per_Fragment->GetBufferSize(),NULL,&gpID3D11PixelShader_Per_Fragment);

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

	

	if(pID3DBlob_Error){
		pID3DBlob_Error->Release();
		pID3DBlob_Error = NULL;
	}

	

	

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[2];

	inputElementDesc[0].SemanticName = "POSITION"; 
	inputElementDesc[0].SemanticIndex = 0;
	inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[0].InputSlot = 0;
	inputElementDesc[0].AlignedByteOffset = 0;
	inputElementDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[0].InstanceDataStepRate = 0;

	inputElementDesc[1].SemanticName = "NORMAL"; 
	inputElementDesc[1].SemanticIndex = 0;
	inputElementDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[1].InputSlot = 1;
	inputElementDesc[1].AlignedByteOffset = 0;
	inputElementDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[1].InstanceDataStepRate = 0;

	

	hr = gpID3D11Device->CreateInputLayout(inputElementDesc,
		_ARRAYSIZE(inputElementDesc),
		pID3dBlob_VertexShaderCode_Per_Vertex->GetBufferPointer(),
		pID3dBlob_VertexShaderCode_Per_Vertex->GetBufferSize(),
		&gpID3D11InputLayout_Per_Vertex);

	if(FAILED(hr))
	{

		if(pID3dBlob_VertexShaderCode_Per_Vertex){
			pID3dBlob_VertexShaderCode_Per_Vertex->Release();
			pID3dBlob_VertexShaderCode_Per_Vertex = NULL;
		}
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateInputLayout() Failed\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		if(pID3dBlob_VertexShaderCode_Per_Vertex){
			pID3dBlob_VertexShaderCode_Per_Vertex->Release();
			pID3dBlob_VertexShaderCode_Per_Vertex = NULL;
		}
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateInputLayout() Succeeded\n");
		fclose(gpFile);
	}


	

	

	hr = gpID3D11Device->CreateInputLayout(inputElementDesc,
		_ARRAYSIZE(inputElementDesc),
		pID3dBlob_VertexShaderCode_Per_Fragment->GetBufferPointer(),
		pID3dBlob_VertexShaderCode_Per_Fragment->GetBufferSize(),
		&gpID3D11InputLayout_Per_Fragment);

	if(FAILED(hr))
	{

		if(pID3dBlob_VertexShaderCode_Per_Fragment){
			pID3dBlob_VertexShaderCode_Per_Fragment->Release();
			pID3dBlob_VertexShaderCode_Per_Fragment = NULL;
		}
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateInputLayout() Failed\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		if(pID3dBlob_VertexShaderCode_Per_Fragment){
			pID3dBlob_VertexShaderCode_Per_Fragment->Release();
			pID3dBlob_VertexShaderCode_Per_Fragment = NULL;
		}
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateInputLayout() Succeeded\n");
		fclose(gpFile);
	}

	


	

	

	getSphereVertexData(sphere_vertices,sphere_normals,sphere_textures,sphere_elements);

	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();

	
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc,sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = gNumVertices * 3 * sizeof(float);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	

	hr = gpID3D11Device->CreateBuffer(&bufferDesc,
		NULL,
		&gpID3D11Buffer_VertexBuffer_Sphere_Position);

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
	bufferDesc.ByteWidth = gNumVertices * 3 * sizeof(float);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	

	hr = gpID3D11Device->CreateBuffer(&bufferDesc,
		NULL,
		&gpID3D11Buffer_VertexBuffer_Sphere_Normal);

	if(FAILED(hr))
	{

		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateBuffer() Failed For Normal Buffer\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateBuffer() Succeeded For Normal Buffer\n");
		fclose(gpFile);
	}

	

	ZeroMemory(&bufferDesc,sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = gNumElements * sizeof(short);
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;


	hr = gpID3D11Device->CreateBuffer(&bufferDesc,
		NULL,
		&gpID3D11Buffer_VertexBuffer_Index);

	if(FAILED(hr))
	{

		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateBuffer() Failed For Index Buffer\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateBuffer() Succeeded For Index Buffer\n");
		fclose(gpFile);
	}

	

	
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	ZeroMemory(&mappedSubresource,sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer_Sphere_Position,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedSubresource);

	
	memcpy(mappedSubresource.pData,sphere_vertices,gNumVertices * 3 * sizeof(float));
	
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_Sphere_Position,0);

	

	ZeroMemory(&mappedSubresource,sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer_Sphere_Normal,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedSubresource);

	
	memcpy(mappedSubresource.pData,sphere_normals,gNumVertices * 3 * sizeof(float));
	
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_Sphere_Normal,0);

	

	ZeroMemory(&mappedSubresource,sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer_Index,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedSubresource);

	
	memcpy(mappedSubresource.pData,sphere_elements,gNumElements * sizeof(short));
	
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_Index,0);

	

	

	D3D11_BUFFER_DESC bufferDesc_ConstantBuffer;
	ZeroMemory(&bufferDesc_ConstantBuffer,sizeof(D3D11_BUFFER_DESC));
	bufferDesc_ConstantBuffer.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc_ConstantBuffer.ByteWidth = sizeof(CBUFFER);
	bufferDesc_ConstantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	hr = gpID3D11Device->CreateBuffer(&bufferDesc_ConstantBuffer,nullptr,&gpID3D11Buffer_ConstantBuffer_Per_Vertex);

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


	hr = gpID3D11Device->CreateBuffer(&bufferDesc_ConstantBuffer,nullptr,&gpID3D11Buffer_ConstantBuffer_Per_Fragment);

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

	if(light == 'v'){
		gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader_Per_Vertex,
		NULL,
		0 
		);


		gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader_Per_Vertex,NULL,0);



		gpID3D11DeviceContext->IASetInputLayout(gpID3D11InputLayout_Per_Vertex);




		gpID3D11DeviceContext->VSSetConstantBuffers(0,
				1,
				&gpID3D11Buffer_ConstantBuffer_Per_Vertex);

	}
	else if(light == 'f'){
		gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader_Per_Fragment,
		NULL,
		0 
		);

		gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader_Per_Fragment,NULL,0);


		gpID3D11DeviceContext->IASetInputLayout(gpID3D11InputLayout_Per_Fragment);



		gpID3D11DeviceContext->VSSetConstantBuffers(0,
				1,
				&gpID3D11Buffer_ConstantBuffer_Per_Fragment);

		gpID3D11DeviceContext->PSSetConstantBuffers(0,
				1,
				&gpID3D11Buffer_ConstantBuffer_Per_Fragment);
	}

	

	UINT stride = sizeof(float)*3;
	UINT offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(0,
		1,
		&gpID3D11Buffer_VertexBuffer_Sphere_Position,
		&stride,
		&offset);

	gpID3D11DeviceContext->IASetVertexBuffers(1,
		1,
		&gpID3D11Buffer_VertexBuffer_Sphere_Normal,
		&stride,
		&offset);

	gpID3D11DeviceContext->IASetIndexBuffer(gpID3D11Buffer_VertexBuffer_Index,
		DXGI_FORMAT_R16_UINT,
		0
		);

	

	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX worldMatrix = XMMatrixTranslation(0.0f,0.0f,1.5f);

	XMMATRIX viewMatrix = XMMatrixIdentity();

	CBUFFER constantBuffer;
	ZeroMemory((void*)&constantBuffer,sizeof(CBUFFER));

	constantBuffer.WorldMatrix = worldMatrix;
	constantBuffer.ViewMatrix = viewMatrix;
	constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

	constantBuffer.La = XMVectorSet(lightAmbient[0],lightAmbient[1],lightAmbient[2],lightAmbient[3]);
	constantBuffer.Ld = XMVectorSet(lightDiffuse[0],lightDiffuse[1],lightDiffuse[2],lightDiffuse[3]);
	constantBuffer.Ls = XMVectorSet(lightSpecular[0],lightSpecular[1],lightSpecular[2],lightSpecular[3]);
	constantBuffer.LightPosition = XMVectorSet(lightPosition[0],lightPosition[1],lightPosition[2],lightPosition[3]);
	constantBuffer.Ka = XMVectorSet(materialAmbient[0],materialAmbient[1],materialAmbient[2],materialAmbient[3]);
	constantBuffer.Kd = XMVectorSet(materialDiffuse[0],materialDiffuse[1],materialDiffuse[2],materialDiffuse[3]);
	constantBuffer.Ks = XMVectorSet(materialSpecular[0],materialSpecular[1],materialSpecular[2],materialSpecular[3]);
	constantBuffer.MaterialShininess = materialShininess;

	if(light == 'v'){
		
		gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer_Per_Vertex,
			0,
			NULL,
			&constantBuffer,
			0,
			0);
	}
	else if(light == 'f'){
		
		gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer_Per_Fragment,
			0,
			NULL,
			&constantBuffer,
			0,
			0);
	}

		

	gpID3D11DeviceContext->DrawIndexed(gNumElements,
		0,
		0
		);


	gpIDXGISwapChain->Present(0,0);

}


void uninitialize(void){

	if(gpID3D11RasterizerState){
		gpID3D11RasterizerState->Release();
		gpID3D11RasterizerState = NULL;
	}

	if(gpID3D11Buffer_ConstantBuffer_Per_Vertex){
		gpID3D11Buffer_ConstantBuffer_Per_Vertex->Release();
		gpID3D11Buffer_ConstantBuffer_Per_Vertex = NULL;
	}

	if(gpID3D11Buffer_ConstantBuffer_Per_Fragment){
		gpID3D11Buffer_ConstantBuffer_Per_Fragment->Release();
		gpID3D11Buffer_ConstantBuffer_Per_Fragment = NULL;
	}

	if(gpID3D11InputLayout_Per_Vertex){
		gpID3D11InputLayout_Per_Vertex->Release();
		gpID3D11InputLayout_Per_Vertex = NULL;
	}

	if(gpID3D11InputLayout_Per_Fragment){
		gpID3D11InputLayout_Per_Fragment->Release();
		gpID3D11InputLayout_Per_Fragment = NULL;
	}

	if(gpID3D11Buffer_VertexBuffer_Sphere_Position){
		gpID3D11Buffer_VertexBuffer_Sphere_Position->Release();
		gpID3D11Buffer_VertexBuffer_Sphere_Position = NULL;
	}

	if(gpID3D11Buffer_VertexBuffer_Index){
		gpID3D11Buffer_VertexBuffer_Index->Release();
		gpID3D11Buffer_VertexBuffer_Index = NULL;
	}

	if(gpID3D11Buffer_VertexBuffer_Sphere_Normal){
		gpID3D11Buffer_VertexBuffer_Sphere_Normal->Release();
		gpID3D11Buffer_VertexBuffer_Sphere_Normal = NULL;
	}

	if(gpID3D11PixelShader_Per_Vertex){
		gpID3D11PixelShader_Per_Vertex->Release();
		gpID3D11PixelShader_Per_Vertex = NULL;
	}

	if(gpID3D11PixelShader_Per_Fragment){
		gpID3D11PixelShader_Per_Fragment->Release();
		gpID3D11PixelShader_Per_Fragment = NULL;
	}

	if(gpID3D11VertexShader_Per_Vertex){
		gpID3D11VertexShader_Per_Vertex->Release();
		gpID3D11VertexShader_Per_Vertex = NULL;
	}

	if(gpID3D11VertexShader_Per_Fragment){
		gpID3D11VertexShader_Per_Fragment->Release();
		gpID3D11VertexShader_Per_Fragment = NULL;
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