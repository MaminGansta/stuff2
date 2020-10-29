#pragma once

#define GLEW_STATIC
#include "GL/glew.h"
#include "SDL2/SDL.h"

#include <cstdio>
#include <cassert>
#include <stdexcept>


namespace Bubble
{
	class SDL_WINDOW
	{
		SDL_Window* mWindow;
		SDL_GLContext mGLContext;
		const char* glsl_version = "#version 330";
		bool mIsOpen = true;

	public:
		SDL_WINDOW(const std::string& name = "Window");
		~SDL_WINDOW();

		bool IsOpen();

		int GetWidth();
		int GetHeight();

		SDL_Window* GetWindow();
		SDL_GLContext GetGLContext();
		const char* GetGLSLVersion();

		bool PollEvent(SDL_Event& event);
		void OnEvent(SDL_Event& event);

		void OnUpdate();
		void Close();

		void SetVSync(bool mode);
	};
}