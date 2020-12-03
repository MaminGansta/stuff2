#pragma once

#include "imgui.h"
#include "imgui_controll/imgui_controll.h"
#include "time/timer.h"

// Temp
#include "renderer.h"

namespace Bubble
{
	struct UI
	{
		ImGuiControll mImGuiControll;

		UI(Window* window);
		~UI();

		void Draw(DeltaTime dt);
		void DrawMenuBar();

		// Temp
		Ref<Shader> mShader;
		Ref<Texture2D> mImage;
		Ref<VertexArray> FullScreenVAO;
	};

}