#pragma once

#include "imgui.h"
#include "time/timer.h"
#include "imgui_controll/imgui_controll.h"

#include <vector>


namespace Bubble
{
	struct UI
	{
		ImGuiControll mImGuiControll;

		UI(SDL_WINDOW* window);
		~UI();

		void Draw(DeltaTime dt);
		void DrawMenuBar();
	};

}