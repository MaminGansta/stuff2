
#include "sdl_window.h"


namespace Bubble
{
    SDL_WINDOW::SDL_WINDOW(const std::string& name)
    {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
        {
            printf("Error: %s", SDL_GetError());
			throw std::runtime_error("Window can't be created");
		}
        
        // GL 3.3 + GLSL 330
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

        // Create window with graphics context
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

        mWindow = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
        mGLContext = SDL_GL_CreateContext(mWindow);

        SDL_GL_MakeCurrent(mWindow, mGLContext);
        SDL_GL_SetSwapInterval(1); // Enable vsync

        if (int error = glewInit(); error != GLEW_OK)
        {
            printf("Error: Glew init \n%s", glewGetErrorString(error));
            throw std::runtime_error("Window can't be created");
        }
    }

	SDL_WINDOW::~SDL_WINDOW()
	{
		SDL_GL_DeleteContext(mGLContext);
		SDL_DestroyWindow(mWindow);
	}

	bool SDL_WINDOW::IsOpen()
	{
        return mIsOpen;
	}

	int SDL_WINDOW::GetWidth()
    {
        int width = 0, height = 0;
        SDL_GetWindowSize(mWindow, &width, &height);
        return width;
    }

    int SDL_WINDOW::GetHeight()
    {
		int width = 0, height = 0;
        SDL_GetWindowSize(mWindow, &width, &height);
        return height;
    }

    SDL_Window* SDL_WINDOW::GetWindow()
    {
        return mWindow;
    }

    SDL_GLContext SDL_WINDOW::GetGLContext()
    {
        return mGLContext;
    }

    const char* SDL_WINDOW::GetGLSLVersion()
    {
        return glsl_version;
    }

    bool SDL_WINDOW::PollEvent(SDL_Event& event)
    {
        return SDL_PollEvent(&event);
    }

    void SDL_WINDOW::OnEvent(SDL_Event& event)
    {
        if (event.type == SDL_QUIT)
        {
            mIsOpen = false;
        }
        if (event.type == SDL_WINDOWEVENT &&
            event.window.event == SDL_WINDOWEVENT_CLOSE &&
            event.window.windowID == SDL_GetWindowID(mWindow))
        {
            mIsOpen = false;
        }
    }

    void SDL_WINDOW::OnUpdate()
    {
        SDL_GL_SwapWindow(mWindow);
    }

    void SDL_WINDOW::Close()
    {
        mIsOpen = false;
    }

	void SDL_WINDOW::SetVSync(bool mode)
	{
        SDL_GL_SetSwapInterval(mode);
	}

}