#pragma once

#include "sdl_window/sdl_window.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"


namespace Bubble
{
    class ImGuiControll
    {
		SDL_WINDOW* mWindow;

		void ImGuiMultiViewports();

    public:
        ImGuiControll(SDL_WINDOW* window);

        void OnAttach();
        void OnDetach();

        // Imgui scope
        void Begin();
        void End();

		void BeginMenuBar();
		void EndMenuBar();

        void OnEvent(SDL_Event& event);
    };
}