#include "GUI.h"
#include "RenderMain.h"
#include "ImGui\ImGui.h"
#include "imgui\imgui_impl_dx11.h"


int   g_GUI_TriFillMode = TriFillMode_SOLIDWIRE;
bool show_Details_Window = true;
bool show_test_window = true;

tGUI gGUI;


void ShowMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::EndMenu();
		}


		if (ImGui::BeginMenu("View"))
		{
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
		static float f = 0.0f;
		ImGui::Text("Render Options");
		ImGui::RadioButton("Solid",      &g_GUI_TriFillMode, TriFillMode_SOLID);        ImGui::SameLine();
        ImGui::RadioButton("WireFrame",  &g_GUI_TriFillMode, TriFillMode_WIRE);         ImGui::SameLine();
        ImGui::RadioButton("Solid+Wire", &g_GUI_TriFillMode, TriFillMode_SOLIDWIRE);
        
		ImGui::SliderFloat("LERP", &gGUI.LerpVal, -1.0f, 2.0f, "%1.3f", 1.0f);

		ImGui::InputInt("NoiseOffset", &gGUI.NoiseOffset);
		ImGui::SliderInt("Iterations", &gGUI.Iterations, 0, 100);


//		ImGui::RadioButton("Color",      &g_GUI_TriViewMode, TriViewMode_COLOR);        ImGui::SameLine();
 //       ImGui::RadioButton("Normal",     &g_GUI_TriViewMode, TriViewMode_NORMAL);      
        


//	ImGui::Checkbox("Show Floor", &g_GUI_Show_Floor);
//	ImGui::Checkbox("Show AABB",  &g_GUI_Show_AABB);
//	ImGui::Checkbox("Show Model", &g_GUI_Show_Model);
//	ImGui::Checkbox("Depth Only Rendering", &g_GUI_DepthOnly);
//
//	ImGui::SliderFloat("Model Scale", &g_GUI_ModelScale, 0.1f, 10.0f, "", 1.0f);
//	ImGui::SliderInt("Model Grid",		&g_GUI_ModelGridSz, 1, 5, "");
	}
	ImGui::End();
}



void ProcessGUI()
{
	ImGui_ImplDX11_NewFrame();

//	HandleHotKeys();


	// Global Style
	ImGuiStyle& style = ImGui::GetStyle();
	style.FrameRounding		= 0.0f;
    style.WindowRounding	= 0.0f;


	//ImGui::ShowTestWindow(&show_test_window);

	ShowMenuBar();

	ShowDetailsView();

}