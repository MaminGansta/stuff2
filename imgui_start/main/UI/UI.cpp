
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
		ImGui::End();

		mImGuiControll.End();
	}


	void UI::DrawMenuBar()
	{

	}

}
