#pragma once

#include "imgui.h"
#include "imgui_controll/imgui_controll.h"
#include "time/timer.h"

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