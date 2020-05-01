#include <filesystem>
namespace fs = std::filesystem;

struct Sprite : Image
{
	float size = 0.0f;
};


// necessary for map editor
struct Screan_area
{
	float pos_x = 0.0f, pos_y = 0.0f;
	float width = 0.0f, height = 0.0f;

	void set(float x, float y, float w, float h)
	{
		pos_x = x;
		pos_y = y;
		width = w;
		height = h;
	}


	bool clicked(float x, float y)
	{
		return (x > pos_x && y > pos_y &&
				y < pos_y + height &&
				x < pos_x + width);
	}

};



enum Window_stage
{
	Stage_Main_Menu,
	Stage_Host_Room,
	Stage_Client_Room,
	Stage_Map_editor,
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
	Button bMap_editor;
	Text tNickname;

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

	// Map editor
	Screan_area materials[5];
	Screan_area Edit_exit{ 0.9f, 0.9f, 0.1f, 0.05f };
	Screan_area Edit_save{ 0.9f, 0.95f, 0.1f, 0.05f };;

	// game
	HANDLE game_loop_thread;

	// sprites
	Sprite environment[5];
	Sprite tank[2];
	Sprite bullet;
	Sprite explosion[3];

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
						window->close();
						break;
					}
					else if (LOWORD(wParam) == window->bHost.id)
						window->change_stage(Stage_Host_Room);

					else if (LOWORD(wParam) == window->bConnect.id)
						window->change_stage(Stage_Client_Room);

					else if (LOWORD(wParam) == window->bMap_editor.id)
						window->change_stage(Stage_Map_editor);


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


				case WM_LBUTTONDOWN:
				{
					if (window->stage == Stage_Map_editor)
					{
						if (Mouse::pos_y > 0.9f)
						{
							for (int i = 0; i < 5; i++)
								if (window->materials[i].clicked(Mouse::pos_x, Mouse::pos_y))
									doutput("click %d\n", i);
							
							if (window->Edit_exit.clicked(Mouse::pos_x, Mouse::pos_y))
								doutput(L"Exit\n");


							if (window->Edit_save.clicked(Mouse::pos_x, Mouse::pos_y))
								doutput(L"Save\n");
						}
						else
						{

						}
					}
				}break;

				case WM_MOUSEMOVE:
				case WM_PAINT:
				{
					PAINTSTRUCT ps;
					BeginPaint(hwnd, &ps);

					switch (window->stage)
					{
					case Stage_Game:
						break;

					case Stage_Client_Room:
					case Stage_Host_Room:
					case Stage_Main_Menu:
						draw_image_async(window->canvas, window->background, 0.0f, 0.0f, 1.0f, 1.0f);
						window->render_canvas(ps);
						break;

					case Stage_Map_editor:
						draw_filled_rect_async(window->canvas, 0.0f, 0.0f, 1.0f, 1.0f, Color(0));

						for (int i = 0; i < 5; i++)
						{
							Sprite& material = window->environment[i];
							draw_image_a(window->canvas, material, 0.05f + 0.15 * i, 0.91f, material.size * 2.0f, material.size * 2.0f);
						}

						render_text(window->canvas, 0.9f, 0.96f, L"save", Color(255, 255, 0), get_def_font(25));
						render_text(window->canvas, 0.9f, 0.91f, L"exit", Color(255, 255, 0), get_def_font(25));

						draw_line(window->canvas, 0.0f, 0.9f, 1.0f, 0.9f, Color(255));
						
						window->render_canvas();
						break;
					}

					EndPaint(hwnd, &ps);
				}break;

				case WM_CLOSE:
				{
					runnig = false;
					WaitForSingleObject(window->game_loop_thread, INFINITE);
				}break;

				default:
					return DefWindowProc(hwnd, msg, wParam, lParam);
				}

				return (LRESULT)0;
			});

		// Load sprites
		void load_sprites(Battle_city_window * window);
		load_sprites(this);


		// set up gui elements:
		set_min_max_size(800, 600);

		// Main menu
		bHost.init(hwnd, L"Host", 0.3f, 0.7f, 0.4f, 0.2f, RESIZABLE);
		set_font_size(bHost.hwnd, 35);

		bConnect.init(hwnd, L"Connect", 0.3f, 0.45f, 0.4f, 0.2f, RESIZABLE);
		set_font_size(bConnect.hwnd, 35);

		bClose.init(hwnd, L"Close", 0.3f, 0.2f, 0.4f, 0.2f, RESIZABLE);
		set_font_size(bClose.hwnd, 35);

		bMap_editor.init(hwnd, L"Map editor", 0.05f, 0.05f, 0.2f, 0.1f, RESIZABLE);
		set_font_size(bMap_editor.hwnd, 25);

		tNickname.init(hwnd, 0.025f, 0.75f, 0.25f, 0.07f, RESIZABLE, WS_VISIBLE | WS_CHILD | WS_BORDER);
		set_font_size(tNickname.hwnd, 30);
		tNickname.set_text(L"Nickname");

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
			tNickname.hide();
			bClose.hide();
			bConnect.hide();
			bHost.hide();
			bMap_editor.hide();
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
			tNickname.show();
			bClose.show();
			bConnect.show();
			bHost.show();
			bMap_editor.show();
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


void load_sprites(Battle_city_window* window)
{
	// load enviroment
	int i = 0;
	for (const auto& entry : fs::directory_iterator(L"sprites/environment"))
	{
		if (i > 4) break;
		fs::path path = entry.path();
		window->environment[i].open(path.c_str());
		window->materials[i].set(0.05f + 0.15 * i, 0.91f, 0.08f, 0.08f);
		i++;
	}
	window->environment[0].size = 0.02f;
	window->environment[1].size = 0.04f;
	window->environment[2].size = 0.04f;
	window->environment[3].size = 0.02f;
	window->environment[4].size = 0.04f;


	window->tank[0].open(L"sprites/tank1.png");
	window->tank[1].open(L"sprites/tank2.png");
	window->tank[0].size = 0.07f;
	window->tank[1].size = 0.07f;

	window->bullet.open(L"sprites/bullet.png");
	window->bullet.size = 0.02f;
}
