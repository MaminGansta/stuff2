

struct Bullet
{
	float pos_x = -1.0f, pos_y = -1.0f;
	float angle = 0;
	float speed_x = 0;
	float speed_y = 0;
	float speed = 0.6f;
};


struct Tank
{
	float pos_x = 0, pos_y = 0;
	int sprite = 0;

	Bullet bullet;
	float angle = 0;
	float speed = 0.2f;
};


struct Explosion
{
	float pos_x = 0.0f, pos_y = 0.0f;
	int sprites = 3;

	float sprite_time = 0.1f;

	int sprite = 0;
	float time = sprite_time;

	int get_sprite_id(float elapsed)
	{
		time -= elapsed;
		if (time < 0)
		{
			sprite++;
			time = sprite_time;
		}
		
		if (sprite == sprites) 
			sprite = -1;

		return sprite;
	}
};

/*
	Game loop.
	Render game to the context has been set.
	And deal with game data exchange with server.

	Executed on another thread.
*/


void game_loop(Battle_city_window* window)
{
	Sprite* tank = window->tank;
	Sprite* explosion = window->explosion;
	Sprite& bullet = window->bullet;

	runnig = true;
	Input::keys_buffer_clear();


	// contain fps to render it on the screan
	wchar_t fps_buffer[32] = L"0";
	float fps_info_timeout = 2.0f;


	// context to render
	Canvas& surface = window->canvas;
	Sprite* envinment = window->environment;
	auto& map = window->game_map;


	// Player on this client connection
	Tank p{ 0.9f, 0.9f};
	float bullet_delay = 0.5f;

	small::array<Explosion, 10> explosions;


	Timer timer;
	while (true)
	{
		if (!runnig) break;

		// exit from the game
		if (Input::was_pressed(VK_ESCAPE))
		{
			window->change_stage(Stage_Main_Menu);
			break;
		}

		// ================ Game logic ====================

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
			// if last bullet not active now
			if (p.bullet.pos_x == -1.0f && p.bullet.pos_y == -1.0f && bullet_delay < 0.0f)
			{
				p.bullet.angle = p.angle;
				p.bullet.speed_x = fastcos(p.angle) * p.bullet.speed;
				p.bullet.speed_y = -fastsin(p.angle) * p.bullet.speed;

				p.bullet.pos_x = p.pos_x;
				p.bullet.pos_y = p.pos_y;
				bullet_delay = 0.5f;
			}
		}
		bullet_delay -= timer.elapsed;

		// colision detection for tank
		bool collided = false;

		for (auto& obj : map)
		{
			if (obj.first == 2) continue;
			float size = envinment[obj.first].size;
			if (box_collison_detection({ new_x - tank->size * 0.5f , new_y - tank->size * 0.5f }, tank->size, { obj.second.x, obj.second.y }, size))
			{
				collided = true;
				break;
			}
		}

		// player
		if (!collided &&
			new_x - tank[0].size * 0.5f > 0.0f && new_y - tank[0].size * 0.5f > 0.0f &&
			new_x + tank[0].size * 0.5f < 1.0f && new_y + tank[0].size * 0.5f < 1.0f)
		{
			
			p.pos_x = new_x;
			p.pos_y = new_y;
		}

		// proccess bullet
		collided = false;
		int i = -1;

		for (auto& obj : map)
		{
			i++;
			if (obj.first == 2) continue;
			float size = envinment[obj.first].size;
			if (box_collison_detection({ p.bullet.pos_x, p.bullet.pos_y }, bullet.size, { obj.second.x, obj.second.y }, size))
			{
				collided = true;

				// add explossion effect
				explosions.push_back(Explosion{ p.bullet.pos_x, p.bullet.pos_y , 2});

				// remove destroyed element
				if (obj.first < 2)
					map.erase(map.begin() + i);

				break;
			}
		}

		if (p.bullet.pos_x != -1.0f)
		{
			if (!collided && p.bullet.pos_x > 0.0f && p.bullet.pos_y > 0.0f &&
							 p.bullet.pos_y < 1.0f && p.bullet.pos_x < 1.0f)
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


		// data exchange with server



		// =================== Draw ====================
		// clr screan
		draw_filled_rect_async(surface, 0.0f, 0.0f, 1.0f, 1.0f, Color(0));

		// draw player
		draw_image_async_a_rotate(surface, tank[p.sprite], p.pos_x, p.pos_y, tank[0].size, tank[0].size, p.angle);

		// daraw bullet
		draw_image_a_rotate(surface, bullet, p.bullet.pos_x, p.bullet.pos_y, bullet.size, bullet.size, p.bullet.angle);

		// draw_map
		for (auto& obj : map)
		{
			Sprite& sprite = envinment[obj.first];
			draw_image_async_a(surface, sprite, obj.second.x, obj.second.y, sprite.size, sprite.size);
		}

		// draw explosion
		for (int i = 0; i < explosions.size; i++)
		{
			int num = explosions[i].get_sprite_id(timer.elapsed);

			if (num == -1)
			{
				explosions.remove(i--);
				continue;
			}

			float size = explosion[num].size;
			draw_image_async_a(surface, explosion[num], explosions[i].pos_x - size * 0.5f, 
								explosions[i].pos_y - size * 0.5f, size , size);
		}

		// ================== Ohter stuff =====================

		// get ellapsed time
		timer.update();

		// Render FPS on right top of window
		if ((fps_info_timeout -= timer.elapsed) < 0.0f)
		{
			swprintf_s(fps_buffer, L"%d", timer.FPS);
			fps_info_timeout = 2.0f;
		}
		render_text(surface, 0.9f, 0.94f, fps_buffer, Color(240, 150, 0), get_def_font(25));

		// Render canvas on the screan
		window->render_canvas();
	}
}




bool box_collison_detection(vec2 pos1, float size1, vec2 pos2, float size2)
{
	return (pos1.x + size1 > pos2.x && pos1.x < pos2.x + size2) &&
		   (pos1.y + size1 > pos2.y && pos1.y < pos2.y + size2);
}