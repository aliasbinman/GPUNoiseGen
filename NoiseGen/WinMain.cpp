// NoiseGen.cpp : Defines the entry point for the application.
//

#include <Windows.h>
#include <d3d11.h>
#include "Winmain.h"

#include "Core\Core.h"
#include "Platform/RHI.h"
#include "Platform/Win/PCInputEnum.h"

#include "imgui/imgui_impl_dx11.h"
extern LRESULT ImGui_ImplDX11_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE	hInst;                                // current instance
HWND		gHWnd;
IDXGISwapChain*                     g_pSwapChain	= NULL;


WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


extern void InitApp();
extern void CloseApp();

extern bool CreateResources();
extern bool DestroyResources();

extern void Update();
extern void Render();




void InitD3DDevice()
{
	ID3D11Device*                       d3dDevice = NULL;
	ID3D11DeviceContext*                pImmediateContext = NULL;
	HRESULT hr;

	RECT rc;
	GetClientRect(g_Device.hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = g_Device.hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	D3D_DRIVER_TYPE                     g_driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL                   g_featureLevel = D3D_FEATURE_LEVEL_11_0;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		g_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, g_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &g_pSwapChain, &d3dDevice, &g_featureLevel, &pImmediateContext);

		if(hr==S_OK)
			break;
	}

	g_Device.pD3DDevice		= d3dDevice;
	g_Device.pD3DContext	= pImmediateContext;
}



//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void CleanupDevice()
{
    ImGui_ImplDX11_Shutdown();
	g_pSwapChain->Release();
	g_Device.Release();

}



//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
{
#if 0
	g_Window_W = 2000;
	g_Window_H = 1200;
#else
	g_Window_W = 1600;
	g_Window_H = 900;
#endif
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_NOISEGEN);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"TutorialWindowClass";
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	// Create window
	g_Device.hInstance = hInstance;
	RECT rc = { 0, 0, (LONG) g_Window_W, (LONG) g_Window_H };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	g_Device.hWnd = CreateWindow(L"TutorialWindowClass", L"MeshProc", WS_OVERLAPPEDWINDOW | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,//WS_OVERLAPPED | WS_CAPTION  | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
		NULL);
	if (!g_Device.hWnd)
		return E_FAIL;

	ShowWindow(g_Device.hWnd, nCmdShow);

	return S_OK;
}



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	if (FAILED(InitWindow(hInstance, nCmdShow)))
		return 0;

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_NOISEGEN, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);


	// Create the D3D Device
	InitD3DDevice();
	InitApp();	   

	
	if(!CreateResources())
	{
		CleanupDevice();
		return 0;
	}
	ImGui_ImplDX11_Init(g_Device.hWnd, g_Device.pD3DDevice, g_Device.pD3DContext);



    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_NOISEGEN));

	MSG msg = { 0 };


	// Main message loop
	while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }
		Update();
		Render();
	}
	//MessageBox(gHWnd, L"sdsds", L"sdsdsd", 0);

	CloseApp();
	DestroyResources();

	CleanupDevice();
	CloseWindow(g_Device.hWnd);
//	_CrtDumpMemoryLeaks(); 

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{

#if 0
	g_Window_W = 2000;
	g_Window_H = 1200;
#else
	g_Window_W = 1600;
	g_Window_H = 900;
#endif


    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_NOISEGEN));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_NOISEGEN);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplDX11_WndProcHandler(hWnd, message, wParam, lParam))
	{
	//	return true;	//JDW do we need the return?
	}

    switch (message)
    {
	//	case WM_SIZE:
    //    if (g_Device.pD3DDevice != NULL && wParam != SIZE_MINIMIZED)
    //    {
    //        ImGui_ImplDX11_InvalidateDeviceObjects();
	//		//CleanupRenderTarget();
    //        g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
	//		//CreateRenderTarget();
    //        ImGui_ImplDX11_CreateDeviceObjects();
    //    }
    //    return 0;

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


