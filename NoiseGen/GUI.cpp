#include "GUI.h"
#include "RenderMain.h"
#include "ImGui\ImGui.h"
#include "imgui\imgui_impl_dx11.h"


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