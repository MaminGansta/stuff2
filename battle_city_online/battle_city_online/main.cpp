
#define FULL_SCREAN_CANVAS
#include "guiAlexandrov/include.h"

bool runnig = false;

#include "main_window.cpp"
#include "game_loop.cpp"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE not_used, LPSTR cmd_args, int show_mode)
{
	al_init(hInstance);

	Battle_city_window* window = new Battle_city_window;

	Window::wait_msg_proc();
	return 0;
}