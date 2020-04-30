

struct Bullet
{
	float pos_x = 0, pos_y = 0;
	float angle = 0;
	float speed_x = 0;
	float speed_y = 0;
	float speed = 0.5f;
};


struct Tank
{
	float pos_x = 0, pos_y = 0;
	int sprite = 0;

	Bullet bullet;
	float angle = 0;
	float speed = 0.3f;
};


/*
	Game loop.
	Render game to the context has been set.
	And deal with game data exchange between
	server and client (on this proccess).

	Executed on another thread.
*/


void game_loop(Battle_city_window* window)
{
	float tank_size = 0.07f;
	float tank_hsize = tank_size * 0.5f;
	float bullet_size = 0.02f;

	Image bullet(L"sprites/bullet.png");

	Image tank[2];
	tank[0].open(L"sprites/tank1.png");
	tank[1].open(L"sprites/tank2.png");


	runnig = true;

	// just buffer for all
	wchar_t buffer[128] = L"0";

	// every 2s change fps info
	float fps_info_timeout = 2.0f;

	// context to render
	Canvas& surface = window->canvas;

	// clear input data
	Input::keys_buffer_clear();


	// Player on this client connection
	Tank p{ 0.5f, 0.5f };


	Timer timer;

	while (true)
	{

		if (!runnig) break;

		// Handle input
		// exit from the game
		if (Input::was_pressed(VK_ESCAPE))
		{
			window->change_stage(Stage_Main_Menu);
			window->redraw();
			break;
		}


		float new_x = p.pos_x, new_y = p.pos_y;

		if (Input::pressed(VK_UP))
		{
			new_y = p.pos_y + p.speed * timer.elapsed;
			p.sprite = (p.sprite + 1) & 1;
			p.angle = PI * 1.5f;
		}
		else if (Input::pressed(VK_DOWN))
		{
			new_y = p.pos_y - p.speed * timer.elapsed;
			p.sprite = (p.sprite + 1) & 1;
			p.angle = PI * 0.5f;
		}
		else if (Input::pressed(VK_RIGHT))
		{
			new_x = p.pos_x + p.speed * timer.elapsed;
			p.sprite = (p.sprite + 1) & 1;
			p.angle = 0;
		}
		else if (Input::pressed(VK_LEFT))
		{
			new_x = p.pos_x - p.speed * timer.elapsed;
			p.sprite = (p.sprite + 1) & 1;
			p.angle = PI;
		}
		else if (Input::pressed(VK_SPACE))
		{
			if (p.bullet.pos_x == -1.0f && p.bullet.pos_y == -1.0f)
			{
				p.bullet.angle = p.angle;
				p.bullet.speed_x = fastcos(p.angle) * p.bullet.speed;
				p.bullet.speed_y = -fastsin(p.angle) * p.bullet.speed;

				p.bullet.pos_x = p.pos_x;
				p.bullet.pos_y = p.pos_y;
			}
		}


		// Game logic

		// proccess player
		if (new_x - tank_hsize > 0.0f && new_y - tank_hsize > 0.0f && new_x + tank_hsize < 1.0f && new_y + tank_hsize < 1.0f)
			p.pos_x = new_x,
			p.pos_y = new_y;

		// proccess bullet
		if (p.bullet.pos_x != -1.0f)
		{
			if (p.bullet.pos_x > 0.0f && p.bullet.pos_y > 0.0f && p.bullet.pos_y < 1.0f && p.bullet.pos_x < 1.0f)
			{
				p.bullet.pos_x += p.bullet.speed_x * timer.elapsed;
				p.bullet.pos_y += p.bullet.speed_y * timer.elapsed;
			}
			else
			{
				p.bullet.pos_x = -1.0f;
				p.bullet.pos_y = -1.0f;
			}

		}


		// data exchange between server



		// Draw
		// clr screan
		draw_filled_rect_async(surface, 0.0f, 0.0f, 1.0f, 1.0f, Color(0));

		// draw player
		draw_image_async_a_rotate(surface, tank[p.sprite], p.pos_x, p.pos_y, tank_size, tank_size, p.angle);

		// daraw bullet
		draw_image_a_rotate(surface, bullet, p.bullet.pos_x, p.bullet.pos_y, bullet_size, bullet_size, p.bullet.angle);

		// get ellapsed time
		timer.update();

		// set window title
		if ((fps_info_timeout -= timer.elapsed) < 0.0f)
		{
			swprintf_s(buffer, L"%d", timer.FPS);
			fps_info_timeout = 2.0f;
		}
		render_text(surface, 0.9f, 0.94f, buffer, Color(240, 150, 0), get_def_font(25));


		// Render canvas to the window
		window->render_canvas();
	}
}