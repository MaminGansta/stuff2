#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>

#define FULL_SCREAN_CANVAS
#include "guiAlexandrov/include.h"

bool runnig = false;

struct vec2 { float x, y; };

std::tuple<wchar_t*, int> map_to_data(const std::vector<std::pair<int, vec2>>& map);
std::vector<std::pair<int, vec2>> parse_map(wchar_t* data);

#include "client.cpp"
#include "server.cpp"
#include "main_window.cpp"
#include "game_loop.cpp"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE not_used, LPSTR cmd_args, int show_mode)
{
	al_init(hInstance);

	Battle_city_window* window = new Battle_city_window;

	Window::wait_msg_proc();
	return 0;
}