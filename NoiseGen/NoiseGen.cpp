
#include <d3d11.h>
#include <d3d9.h>

#include "Core\Core.h"
#include "GUI.h"
#include "Platform\RHI.h"

void InitApp()
{
	RHIInit();

#if 0
	// Input System
	g_pInput = new CPCInputEnum;
	g_pInput->Initialise(g_Device.hWnd, g_Device.hInstance);

	// Create the cameras
	g_OrbitCamera.Init();
	g_UVCamera.Init();


#endif


}



void CloseApp()
{
	
}



void Update()
{
	ProcessGUI();
	
}


