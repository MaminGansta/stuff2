
#define CANVAS_MAX_SIZE
#include "guiAlexandrov/include.h"


enum Window_stage
{
	Stage_Main_Menu,
	Stage_Host_Room,
	Stage_Client_Room,
	Stage_Game
};

/*
	Main window.
	In this class discribed menu gui stuff.

	Provide to init server and connect to it,
	then call game loop function that render 
	game in this context. 

	Also game loop deal with game transactions 
	(send(recive) info to the server).
*/

struct Battle_city_window : Window
{
	Window_stage stage = Stage_Main_Menu;

	// Menu
	Image background;

	// Menu: main menu
	Button bHost;
	Button bConnect;
	Button bClose;

	// Menu: host room
	Label lIp_port;
	Text tIp;
	Text tPort;
	Button bCreate_server;
	Button bHost_back;
	Button bStart;
	Text tHostLogInfo;

	// Menu: client room
	Label lClient_Ip_port;
	Text tClient_Ip;
	Text tClient_Port;
	Button bConnect2Server;
	Button bClient_back;
	Text tClientLogInfo;

	// game
	HANDLE game_loop_thread;

	Battle_city_window()
	{
		background.open(L"sprites/battle_city_bg.png");

		init(L"battle_city", 800, 600, [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args arg)
			{
				Battle_city_window* window = (Battle_city_window*)arg[0];

				switch (msg)
				{
					case WM_COMMAND:
					{
						// game loop declaration
						void game_loop(Battle_city_window * window);

						// Main menu
						if (LOWORD(wParam) == window->bClose.id)
						{
							safe_release(window);
							break;
						}

						else if (LOWORD(wParam) == window->bHost.id)
							window->change_stage(Stage_Host_Room);

						else if (LOWORD(wParam) == window->bConnect.id)
							window->change_stage(Stage_Client_Room);

						
						// Host room
						if (LOWORD(wParam) == window->bHost_back.id)
							window->change_stage(Stage_Main_Menu);

						if (LOWORD(wParam) == window->bStart.id)
						{
							window->change_stage(Stage_Game);
							window->game_loop_thread =
								CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)game_loop, LPVOID(window), NULL, NULL);
						}

						// Client room
						if (LOWORD(wParam) == window->bClient_back.id)
							window->change_stage(Stage_Main_Menu);

					}break;

					case WM_PAINT:
					{
						// render game in another place
						//if (window->stage == Stage_Game) break;

						PAINTSTRUCT ps;
						BeginPaint(hwnd, &ps);
						
						if (window->stage != Stage_Game)
						{
							draw_image_async(window->canvas, window->background, 0.0f, 0.0f, 1.0f, 1.0f);
							window->render_canvas(ps);
						}

						EndPaint(hwnd, &ps);
					}break;

					case WM_CLOSE:
					{
						TerminateThread(window->game_loop_thread, 0);
						CloseHandle(window->game_loop_thread);
					}

					default:
						return DefWindowProc(hwnd, msg, wParam, lParam);
				}

				return (LRESULT)0;
			});

		// set up gui elements:
		set_min_max_size(800, 600);

		// Main menu
		bHost.init(hwnd, L"Host", 0.3f, 0.7f, 0.4f, 0.2f, RESIZABLE);
		set_font_size(bHost.hwnd, 35);

		bConnect.init(hwnd, L"Connect", 0.3f, 0.45f, 0.4f, 0.2f, RESIZABLE);
		set_font_size(bConnect.hwnd, 35);

		bClose.init(hwnd, L"Close", 0.3f, 0.2f, 0.4f, 0.2f, RESIZABLE);
		set_font_size(bClose.hwnd, 35);

		// Host room
		lIp_port.init(hwnd, L" IP / PORT", 0.15f, 0.9f, 0.2f, 0.08f, RESIZABLE);
		set_font_size(lIp_port.hwnd, 32);

		tIp.init(hwnd, 0.15f, 0.8f, 0.3f, 0.1f, RESIZABLE);
		tIp.set_text(L"192.168.0.104");
		set_font_size(tIp.hwnd, 32);

		tPort.init(hwnd, 0.45f, 0.8f, 0.15f, 0.1f, RESIZABLE);
		tPort.set_text(L"5678");
		set_font_size(tPort.hwnd, 32);
		
		bCreate_server.init(hwnd, L"create server", 0.6f, 0.8f, 0.25f, 0.1f, RESIZABLE);
		set_font_size(bCreate_server.hwnd, 35);
		
		bHost_back.init(hwnd, L"Back", 0.05f, 0.05f, 0.2f, 0.1f);
		set_font_size(bHost_back.hwnd, 35);

		bStart.init(hwnd, L"Start", 0.75f, 0.05f, 0.2f, 0.1f);
		set_font_size(bStart.hwnd, 35);

		tHostLogInfo.init(hwnd, 0.15f, 0.2f, 0.7f, 0.55f, RESIZABLE);

		// Client room
		lClient_Ip_port.init(hwnd, L" IP / PORT", 0.15f, 0.9f, 0.2f, 0.08f, RESIZABLE);
		set_font_size(lClient_Ip_port.hwnd, 32);

		tClient_Ip.init(hwnd, 0.15f, 0.8f, 0.3f, 0.1f, RESIZABLE);
		tClient_Ip.set_text(L"192.168.0.104");
		set_font_size(tClient_Ip.hwnd, 32);

		tClient_Port.init(hwnd, 0.45f, 0.8f, 0.15f, 0.1f, RESIZABLE);
		tClient_Port.set_text(L"5678");
		set_font_size(tClient_Port.hwnd, 32);

		bConnect2Server.init(hwnd, L"Connect", 0.6f, 0.8f, 0.25f, 0.1f, RESIZABLE);
		set_font_size(bConnect2Server.hwnd, 35);

		bClient_back.init(hwnd, L"Back", 0.05f, 0.05f, 0.2f, 0.1f);
		set_font_size(bClient_back.hwnd, 35);

		tClientLogInfo.init(hwnd, 0.15f, 0.2f, 0.7f, 0.55f, RESIZABLE);


		// hide all components and load Main menu stage
		for (auto& component : components[hwnd])
			component.hide();

		change_stage(Stage_Main_Menu);
	}

	/*
		Hide and show gui elements when then used or not.
	*/
	void change_stage(Window_stage new_stage)
	{
		// clean up old components
		switch (stage)
		{
			case Stage_Main_Menu:
			{
				bClose.hide();
				bConnect.hide();
				bHost.hide();
			}break;

			case Stage_Host_Room:
			{
				lIp_port.hide();
				tIp.hide();
				tPort.hide();
				bCreate_server.hide();
				bHost_back.hide();
				bStart.hide();
				tHostLogInfo.hide();
			}break;

			case Stage_Client_Room:
			{
				lClient_Ip_port.hide();
				tClient_Ip.hide();
				tClient_Port.hide();
				bConnect2Server.hide();
				bClient_back.hide();
				tClientLogInfo.hide();
				break;
			}

		}

		// set new components
		stage = new_stage;
		switch (new_stage)
		{
			case Stage_Main_Menu:
			{
				bClose.show();
				bConnect.show();
				bHost.show();
			}break;

			case Stage_Host_Room:
			{
				lIp_port.show();
				tIp.show();
				tPort.show();
				bCreate_server.show();
				bHost_back.show();
				bStart.show();
				tHostLogInfo.show();
			}break;

			case Stage_Client_Room:
			{
				lClient_Ip_port.show();
				tClient_Ip.show();
				tClient_Port.show();
				bConnect2Server.show();
				bClient_back.show();
				tClientLogInfo.show();
				break;
			}
		}
	}
};


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE not_used, LPSTR cmd_args, int show_mode)
{
	al_init(hInstance);

	Battle_city_window* window = new Battle_city_window;

	Window::wait_msg_proc();
	return 0;
}










/*
	Game loop.
	Render game to the context has been set.
	And deal with game data exchange between
	server and client (on this proccess).

	Executed on another thread.
*/


void game_loop(Battle_city_window* window)
{
	// context to render
	Canvas& surface = window->canvas;

	while (true)
	{
		// exit from the game
		if (Input::was_pressed(VK_ESCAPE))
		{
			window->change_stage(Stage_Main_Menu);
			window->redraw();
			break;
		}

		// Game logic



		// Draw
		// clr screan
		draw_filled_rect_async(surface, 0.0f, 0.0f, 1.0f, 1.0f, Color(255));


		draw_filled_rect(surface, 0.0f, 0.0f, 0.5f, 0.5f, Color(255, 0, 0));


		
		// Render canvas to the window
		window->render_canvas();
	}
}