
#include "UI.h"


namespace Bubble
{
	UI::UI(SDL_WINDOW* window)
		: mImGuiControll(window)
	{
		mImGuiControll.OnAttach();
	}

	UI::~UI()
	{
		mImGuiControll.OnDetach();
	}

	void UI::Draw(DeltaTime dt)
	{
		mImGuiControll.Begin();

		mImGuiControll.BeginMenuBar();
		DrawMenuBar();
		mImGuiControll.EndMenuBar();

		ImGui::Begin("Test");
		ImGui::Button("Button", { 100, 50 });

		static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
		ImGui::PlotLines("Frame Times", arr, IM_ARRAYSIZE(arr), 0, NULL, 0.0f, 2.0f, ImVec2(0, 200.0f));

		ImGui::End();


		// DemoWindow
		ImGui::ShowDemoWindow();

		mImGuiControll.End();
	}


	void UI::DrawMenuBar()
	{

	}

}
