#include "GUI.h"
#include "RenderMain.h"
#include "ImGui\ImGui.h"
#include "imgui\imgui_impl_dx11.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT
#include "stb\stb_image_write.h"


bool show_Details_Window = true;
bool show_test_window = true;

tGUI gGUI;



void SaveNoiseToImageFile(const char *fileOut)
{
	extern RENDERTARGET					g_FinalNoiseForSaveRT;
	D3D11_MAPPED_SUBRESOURCE			MappedData;
	HRESULT hRes=	g_Device.pD3DContext->Map(g_FinalNoiseForSaveRT.pD3DTexture2D, 0, D3D11_MAP_READ, 0, &MappedData);

	if (hRes == S_OK)
	{
		int bSuccess = stbi_write_tga(fileOut, 256, 256, 4, MappedData.pData);
	}
	g_Device.pD3DContext->Unmap(g_FinalNoiseForSaveRT.pD3DTexture2D, 0);
}


void ShowMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			// Save noise to file
			if (ImGui::MenuItem("Save"  , "CTRL+S")) 
			{
				OPENFILENAMEA file;
				ZeroMemory( &file , sizeof( file));
				char fileOut[512] ={0};
				file.lStructSize = sizeof ( OPENFILENAMEA );
				file.hwndOwner = 0;//g_hWnd  ;
				file.lpstrFile = fileOut ;
				file.lpstrFile[0] = '\0';
				file.nMaxFile = sizeof( fileOut );
				file.lpstrFilter = "TGA File\0*.TGA\0\0";
				file.nFilterIndex =1;
				file.lpstrFileTitle = NULL ;
				file.nMaxFileTitle = 0 ;
				file.lpstrInitialDir=NULL ;
				GetSaveFileNameA(&file);

				if(strlen(fileOut))
				{
					SaveNoiseToImageFile(fileOut);
				}
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}



void ShowDetailsView()
{
	ImGui::Begin("Details",				&show_Details_Window);

	if (ImGui::CollapsingHeader("Render Options",ImGuiTreeNodeFlags_DefaultOpen))
	{
       
		ImGui::SliderFloat("LERP", &gGUI.LerpVal, -1.0f, 2.0f, "%1.3f", 1.0f);

		ImGui::SliderInt("Iterations", &gGUI.Iterations, 0, 100);
	}
	ImGui::End();
}



void ProcessGUI()
{
	ImGui_ImplDX11_NewFrame();

	// Global Style
	ImGuiStyle& style = ImGui::GetStyle();
	style.FrameRounding		= 0.0f;
    style.WindowRounding	= 0.0f;


	//ImGui::ShowTestWindow(&show_test_window);

	ShowMenuBar();

	ShowDetailsView();

}