

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

float angle_Triangle = 0.0f;

float gClearColor[4];
IDXGISwapChain *gpIDXGISwapChain = NULL;
ID3D11Device *gpID3D11Device = NULL;
ID3D11DeviceContext *gpID3D11DeviceContext = NULL;
ID3D11RenderTargetView *gpID3D11RenderTargetView = NULL;

ID3D11VertexShader *gpID3D11VertexShader = NULL;
ID3D11HullShader *gpID3D11HullShader = NULL; 
ID3D11DomainShader *gpID3D11DomainShader = NULL; 
ID3D11PixelShader *gpID3D11PixelShader = NULL;
ID3D11Buffer *gpID3D11Buffer_VertexBuffer_Triangle_Position = NULL;
ID3D11InputLayout *gpID3D11InputLayout = NULL;
ID3D11Buffer *gpID3D11Buffer_ConstantBuffer_HullShader = NULL;
ID3D11Buffer *gpID3D11Buffer_ConstantBuffer_DomainShader = NULL;
ID3D11Buffer *gpID3D11Buffer_ConstantBuffer_PixelShader = NULL;

ID3D11RasterizerState *gpID3D11RasterizerState = NULL;

ID3D11DepthStencilView *gpID3D11DepthStencilView = NULL;


struct CBUFFER_HullShader{
	XMVECTOR Hull_Constant_Function_Params;
};

struct CBUFFER_DomainShader{
	XMMATRIX WorldViewProjectionMatrix;
};

struct CBUFFER_PixelShader{
	XMVECTOR LineColor;
};

unsigned int guiNumberOfLineSegments = 1;

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
							TEXT("Mayur Tadekar - 02_D3DX11 - Hull and Domain Shader"),
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

				case VK_UP:
		            guiNumberOfLineSegments++;
		            if(guiNumberOfLineSegments >= 30){
		                guiNumberOfLineSegments = 30;
		            }
		        break;

		        case VK_DOWN:
		            guiNumberOfLineSegments--;
		            if(guiNumberOfLineSegments <= 0){
		                guiNumberOfLineSegments = 1;
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

	
	
	const char *vertexShaderSourceCode = 
	"struct vertex_Output{" \
	"float4 position : POSITION;" \
	"};" \
	"vertex_Output main(float2 pos : POSITION)" \
	"{" \
	"vertex_Output output;" \
	"output.position = float4(pos,0.0,1.0);" \
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

	

	const char *hullShaderSourceCode =
	"\n" \
	"cbuffer ConstantBuffer{" \
	"\n" \
	"float4 hull_constant_function_params;" \
	"\n" \
	"}" \
	"\n" \
	"struct vertex_Output{" \
	"\n" \
	"float4 position : POSITION;" \
	"\n" \
	"};" \
	"\n" \
	"struct hull_constant_output{" \
	"\n" \
	"float edges[2] : SV_TESSFACTOR;" \
	"\n" \
	"};" \
	"\n" \
	"hull_constant_output hull_constant_function(void){" \
	"\n" \
	"hull_constant_output output;" \
	"\n" \
	"float numberOfLineStrips = hull_constant_function_params[0];" \
	"\n" \
	"float numberOfLineSegments = hull_constant_function_params[1];" \
	"\n" \
	"output.edges[0] = numberOfLineStrips;" \
	"\n" \
	"output.edges[1] = numberOfLineSegments;" \
	"\n" \
	"return(output);" \
	"\n" \
	"}" \
	"\n" \
	"struct hull_out{" \
	"\n" \
	"float4 position : POSITION;" \
	"\n" \
	"};" \
	"\n" \
	"[domain(\"isoline\")]" \
	"\n" \
	"[partitioning(\"integer\")]" \
	"\n" \
	"[outputtopology(\"line\")]" \
	"\n" \
	"[outputcontrolpoints(4)]" \
	"\n" \
	"[patchconstantfunc(\"hull_constant_function\")]" \
	"\n" \
	"hull_out main(InputPatch<vertex_Output,4> input_patch,uint i : SV_OUTPUTCONTROLPOINTID){" \
	"\n" \
	"hull_out output;" \
	"\n" \
	"output.position = input_patch[i].position;" \
	"\n" \
	"return(output);" \
	"\n" \
	"}";

	ID3DBlob *pID3dBlob_HullShaderCode = NULL;
	pID3DBlob_Error = NULL;

	hr = D3DCompile(hullShaderSourceCode,
		lstrlenA(hullShaderSourceCode) + 1,
		"HS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"hs_5_0",
		0,
		0,
		&pID3dBlob_HullShaderCode,
		&pID3DBlob_Error);

	if(FAILED(hr))
	{

		if(pID3DBlob_Error != NULL)
		{

			fopen_s(&gpFile,gszLogFileName,"a+");
			fprintf_s(gpFile,"D3DCompile() Failed For Hull Shader : %s.\n",(char*)pID3DBlob_Error->GetBufferPointer());
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
		fprintf_s(gpFile,"D3DCompile() Succeeded For Hull Shader\n");
		fclose(gpFile);
	}

	
	hr = gpID3D11Device->CreateHullShader(pID3dBlob_HullShaderCode->GetBufferPointer(),pID3dBlob_HullShaderCode->GetBufferSize(),NULL,&gpID3D11HullShader);

	if(FAILED(hr))
	{

		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateHullShader() failed.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateHullShader() Succeeded\n");
		fclose(gpFile);
	}

	
	gpID3D11DeviceContext->HSSetShader(gpID3D11HullShader,NULL,0);

	

	if(pID3DBlob_Error){
		pID3DBlob_Error->Release();
		pID3DBlob_Error = NULL;
	}

	if(pID3dBlob_HullShaderCode){
		pID3dBlob_HullShaderCode->Release();
		pID3dBlob_HullShaderCode = NULL;
	}

	


	const char *domainShaderSourceCode =
	"\n" \
	"cbuffer ConstantBuffer{" \
	"\n" \
	"float4x4 worldViewProjectionMatrix;" \
	"\n" \
	"}" \
	"struct hull_out{" \
	"\n" \
	"float4 position : POSITION;" \
	"\n" \
	"};" \
	"\n" \
	"struct hull_constant_output{" \
	"\n" \
	"float edges[2] : SV_TESSFACTOR;" \
	"};" \
	"\n" \
	"struct domain_output{" \
	"\n" \
	"float4 position : SV_POSITION;" \
	"\n" \
	"};" \
	"\n" \
	"[domain(\"isoline\")]" \
	"\n" \
	"domain_output main(hull_constant_output patch,OutputPatch<hull_out,4> output_patch,float2 tessCoord : SV_DOMAINLOCATION){" \
	"\n" \
	"domain_output output;" \
	"\n" \
	"float3 p0 = output_patch[0].position.xyz;" \
	"\n" \
	"float3 p1 = output_patch[1].position.xyz;" \
	"\n" \
	"float3 p2 = output_patch[2].position.xyz;" \
	"\n" \
	"float3 p3 = output_patch[3].position.xyz;" \
	"\n" \
	"float3 p = p0 * (1.0 - tessCoord.x) * (1.0 - tessCoord.x) * (1.0 - tessCoord.x) + p1 * 3.0 * tessCoord.x * (1.0 - tessCoord.x) * (1.0 - tessCoord.x) + p2 * 3.0 * tessCoord.x * tessCoord.x * (1.0 - tessCoord.x) + p3 * tessCoord.x * tessCoord.x * tessCoord.x;" \
	"\n" \
	"float4 position = float4(p,1.0);" \
	"\n" \
	"output.position = mul(worldViewProjectionMatrix,position);" \
	"\n" \
	"return(output);" \
	"\n" \
	"}";

	ID3DBlob *pID3dBlob_DomainShaderCode = NULL;
	pID3DBlob_Error = NULL;

	hr = D3DCompile(domainShaderSourceCode,
		lstrlenA(domainShaderSourceCode) + 1,
		"DS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ds_5_0",
		0,
		0,
		&pID3dBlob_DomainShaderCode,
		&pID3DBlob_Error);

	if(FAILED(hr))
	{

		if(pID3DBlob_Error != NULL)
		{

			fopen_s(&gpFile,gszLogFileName,"a+");
			fprintf_s(gpFile,"D3DCompile() Failed For Domain Shader : %s.\n",(char*)pID3DBlob_Error->GetBufferPointer());
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
		fprintf_s(gpFile,"D3DCompile() Succeeded For Domain Shader\n");
		fclose(gpFile);
	}

	
	hr = gpID3D11Device->CreateDomainShader(pID3dBlob_DomainShaderCode->GetBufferPointer(),pID3dBlob_DomainShaderCode->GetBufferSize(),NULL,&gpID3D11DomainShader);

	if(FAILED(hr))
	{

		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateDomainShader() failed.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateDomainShader() Succeeded\n");
		fclose(gpFile);
	}

	
	gpID3D11DeviceContext->DSSetShader(gpID3D11DomainShader,NULL,0);

	

	if(pID3DBlob_Error){
		pID3DBlob_Error->Release();
		pID3DBlob_Error = NULL;
	}

	if(pID3dBlob_DomainShaderCode){
		pID3dBlob_DomainShaderCode->Release();
		pID3dBlob_DomainShaderCode = NULL;
	}

	

	

	const char *pixelShaderSourceCode =
	"cbuffer ConstantBuffer{" \
	"float4 lineColor;" \
	"}" \
	"float4 main(void) : SV_TARGET" \
	"{" \
	"float4 color;" \
	"color = lineColor;" \
	"return(color);" \
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

	

	D3D11_INPUT_ELEMENT_DESC inputElementDesc;

	inputElementDesc.SemanticName = "POSITION"; 
	inputElementDesc.SemanticIndex = 0;
	inputElementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDesc.InputSlot = 0;
	inputElementDesc.AlignedByteOffset = 0;
	inputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc.InstanceDataStepRate = 0;

	

	hr = gpID3D11Device->CreateInputLayout(&inputElementDesc,
		1,
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

	

	float vertices[] = {

		-1.0f,-1.0f,
        -0.5f,1.0f,
        0.5f,-1.0f,
        1.0f,1.0f,

	};

	

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc,sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(float) * _ARRAYSIZE(vertices);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	

	hr = gpID3D11Device->CreateBuffer(&bufferDesc,
		NULL,
		&gpID3D11Buffer_VertexBuffer_Triangle_Position);

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
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer_Triangle_Position,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedSubresource);

	
	memcpy(mappedSubresource.pData,vertices,sizeof(vertices));
	
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_Triangle_Position,0);


	

	D3D11_BUFFER_DESC bufferDesc_ConstantBuffer;
	ZeroMemory(&bufferDesc_ConstantBuffer,sizeof(D3D11_BUFFER_DESC));
	bufferDesc_ConstantBuffer.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc_ConstantBuffer.ByteWidth = sizeof(CBUFFER_HullShader);
	bufferDesc_ConstantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	hr = gpID3D11Device->CreateBuffer(&bufferDesc_ConstantBuffer,nullptr,&gpID3D11Buffer_ConstantBuffer_HullShader);

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

	ZeroMemory(&bufferDesc_ConstantBuffer,sizeof(D3D11_BUFFER_DESC));
	bufferDesc_ConstantBuffer.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc_ConstantBuffer.ByteWidth = sizeof(CBUFFER_DomainShader);
	bufferDesc_ConstantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	hr = gpID3D11Device->CreateBuffer(&bufferDesc_ConstantBuffer,nullptr,&gpID3D11Buffer_ConstantBuffer_DomainShader);

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

	ZeroMemory(&bufferDesc_ConstantBuffer,sizeof(D3D11_BUFFER_DESC));
	bufferDesc_ConstantBuffer.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc_ConstantBuffer.ByteWidth = sizeof(CBUFFER_PixelShader);
	bufferDesc_ConstantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	hr = gpID3D11Device->CreateBuffer(&bufferDesc_ConstantBuffer,nullptr,&gpID3D11Buffer_ConstantBuffer_PixelShader);

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

	gpID3D11DeviceContext->HSSetConstantBuffers(0,
		1,
		&gpID3D11Buffer_ConstantBuffer_HullShader);

	gpID3D11DeviceContext->DSSetConstantBuffers(0,
		1,
		&gpID3D11Buffer_ConstantBuffer_DomainShader);

	gpID3D11DeviceContext->PSSetConstantBuffers(0,
		1,
		&gpID3D11Buffer_ConstantBuffer_PixelShader);

	

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

	

	UINT stride = sizeof(float)*2;
	UINT offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(0,
		1,
		&gpID3D11Buffer_VertexBuffer_Triangle_Position,
		&stride,
		&offset);

	

	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	XMMATRIX translationMatrix = XMMatrixTranslation(0.0f,0.0f,4.0f);
	XMMATRIX rotationMatrix = XMMatrixRotationY(angle_Triangle);
	XMMATRIX worldMatrix = rotationMatrix * translationMatrix;
	XMMATRIX viewMatrix = XMMatrixIdentity();

	XMMATRIX wvpMatrix = worldMatrix * viewMatrix * gPerspectiveProjectionMatrix;

	CBUFFER_HullShader constantBuffer_HullShader;
	CBUFFER_DomainShader constantBuffer_DomainShader;
	CBUFFER_PixelShader constantBuffer_PixelShader;

	ZeroMemory((void*)&constantBuffer_HullShader,sizeof(CBUFFER_HullShader));
	ZeroMemory((void*)&constantBuffer_DomainShader,sizeof(CBUFFER_DomainShader));
	ZeroMemory((void*)&constantBuffer_PixelShader,sizeof(CBUFFER_PixelShader));

	constantBuffer_HullShader.Hull_Constant_Function_Params = XMVectorSet(1.0f,(FLOAT)guiNumberOfLineSegments,0.0f,1.0f);

	constantBuffer_DomainShader.WorldViewProjectionMatrix = wvpMatrix;

	constantBuffer_PixelShader.LineColor = XMVectorSet(1.0f,1.0f,0.0f,1.0f);

	
	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer_HullShader,
		0,
		NULL,
		&constantBuffer_HullShader,
		0,
		0);

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer_DomainShader,
		0,
		NULL,
		&constantBuffer_DomainShader,
		0,
		0);

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer_PixelShader,
		0,
		NULL,
		&constantBuffer_PixelShader,
		0,
		0);

	gpID3D11DeviceContext->Draw(4,
		0);

	gpIDXGISwapChain->Present(0,0);

}


void uninitialize(void){

	if(gpID3D11RasterizerState){
		gpID3D11RasterizerState->Release();
		gpID3D11RasterizerState = NULL;
	}

	if(gpID3D11Buffer_ConstantBuffer_PixelShader){
		gpID3D11Buffer_ConstantBuffer_PixelShader->Release();
		gpID3D11Buffer_ConstantBuffer_PixelShader = NULL;
	}

	if(gpID3D11Buffer_ConstantBuffer_HullShader){
		gpID3D11Buffer_ConstantBuffer_HullShader->Release();
		gpID3D11Buffer_ConstantBuffer_HullShader = NULL;
	}

	if(gpID3D11Buffer_ConstantBuffer_DomainShader){
		gpID3D11Buffer_ConstantBuffer_DomainShader->Release();
		gpID3D11Buffer_ConstantBuffer_DomainShader = NULL;
	}

	if(gpID3D11InputLayout){
		gpID3D11InputLayout->Release();
		gpID3D11InputLayout = NULL;
	}

	if(gpID3D11Buffer_VertexBuffer_Triangle_Position){
		gpID3D11Buffer_VertexBuffer_Triangle_Position->Release();
		gpID3D11Buffer_VertexBuffer_Triangle_Position = NULL;
	}

	if(gpID3D11PixelShader){
		gpID3D11PixelShader->Release();
		gpID3D11PixelShader = NULL;
	}

	if(gpID3D11DomainShader){
		gpID3D11DomainShader->Release();
		gpID3D11DomainShader = NULL;
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