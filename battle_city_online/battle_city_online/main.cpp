
#include "guiAlexandrov/include.h"

enum Window_stage
{
	Stage_Main_Menu,
	Stage_Host_Room,
	Stage_Client_Room,
	Stage_Game
};


struct Battle_city : Window
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
	Text tLogInfo;

	// Menu: client room

	Battle_city()
	{
		background.open(L"sprites/battle_city_bg.png");

		init(L"battle_city", 800, 600, [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args arg)
			{
				Battle_city* window = (Battle_city*)arg[0];

				switch (msg)
				{
					case WM_COMMAND:
					{
						// Main menu
						if (LOWORD(wParam) == window->bClose.id)
						{
							safe_release(window);
							break;
						}

						else if (LOWORD(wParam) == window->bHost.id)
						{
							window->change_stage(Stage_Host_Room);
							break;
						}

						else if (LOWORD(wParam) == window->bConnect.id)
						{
							window->change_stage(Stage_Client_Room);
							break;
						}

						// Host room
						if (LOWORD(wParam) == window->bHost_back.id)
						{
							window->change_stage(Stage_Main_Menu);
							break;
						}

						// Client room

					}break;

					case WM_PAINT:
					{
						// render game in another place
						if (window->stage == Stage_Game) break;

						PAINTSTRUCT ps;
						BeginPaint(hwnd, &ps);
						
						draw_image_async(window->canvas, window->background, 0.0f, 0.0f, 1.0f, 1.0f);
						window->render_canvas(ps);

						EndPaint(hwnd, &ps);
					}break;

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
		set_font_size(tIp.hwnd, 32);

		tPort.init(hwnd, 0.45f, 0.8f, 0.15f, 0.1f, RESIZABLE);
		set_font_size(tPort.hwnd, 32);
		
		bCreate_server.init(hwnd, L"create server", 0.6f, 0.8f, 0.25f, 0.1f, RESIZABLE);
		set_font_size(bCreate_server.hwnd, 35);
		
		bHost_back.init(hwnd, L"Back", 0.05f, 0.05f, 0.2f, 0.1f);
		set_font_size(bHost_back.hwnd, 35);

		tLogInfo.init(hwnd, 0.15f, 0.2f, 0.7f, 0.55f, RESIZABLE);

		// Client room



		// hide all components and load Main menu stage
		for (auto& component : components[hwnd])
			component.hide();

		change_stage(Stage_Main_Menu);
	}

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
				tLogInfo.hide();
			}break;

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
			tLogInfo.show();
		}break;


		}
	}
};


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE not_used, LPSTR cmd_args, int show_mode)
{
	al_init(hInstance);

	Battle_city* window = new Battle_city;



	Window::wait_msg_proc();
	return 0;
}