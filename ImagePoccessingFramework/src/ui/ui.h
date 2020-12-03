#pragma once

#include "imgui.h"
#include "time/timer.h"
#include "module/module.h"
#include "imgui_controll/imgui_controll.h"

// Temp
#include "renderer.h"

namespace Bubble
{
	struct UI
	{
		static std::vector<Module*> sModules;
		ImGuiControll mImGuiControll;

		UI() = default;
		UI(Window* window);
		~UI();

		void OnUpdate(DeltaTime dt);
		void DrawMenuBar();

		static void AddModule(Module* module);

		// Temp
		Ref<Shader> mShader;
		Ref<Texture2D> mImage;
		Ref<VertexArray> FullScreenVAO;
	};

}