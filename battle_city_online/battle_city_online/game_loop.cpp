


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
	// context to render
	Canvas& surface = window->canvas;

	// Sprites and map
	Sprite* tank = window->tank;
	Sprite* explosion = window->explosion;
	Sprite& bullet = window->bullet;
	Sprite* envinment = window->environment;

	// array of explosions
	small::array<Explosion, 100> explosions;

	// load map
	auto map = window->game_map;

	// buffer for all stuff
	wchar_t buffer[32] = L"\0";


	runnig = true;
	while (runnig)
	{
		// if game is start agen
		Sleep(100);

		// clear input keys
		Input::keys_buffer_clear();

		// Player on this client
		Tank p{ init_pos[0], init_pos[1] };
		float bullet_delay = 0.5f;

		// load nickname
		float nickname_timeout = 5.0f;
		memmove(p.name, window->tNickname.get_text(), sizeof(wchar_t) * 16);


		// Game info
		int nAlives = 4;


		// send init data
		Packet packet = P_Player;
		send(window->client.Connection, (char*)&packet, sizeof(Packet), NULL);
		send(window->client.Connection, (char*)&p, sizeof(Tank), NULL);

		Timer timer;
		while (client_runnig && runnig)
		{
			if (!runnig) break;

			// exit from the game
			if (Input::was_pressed(VK_ESCAPE))
			{
				window->change_stage(Stage_Main_Menu);
				return;
			}

			// ================ Game logic ====================

			float new_x = p.pos_x, new_y = p.pos_y;

			if (Input::pressed(VK_UP))
			{
				new_y = p.pos_y + p.speed * timer.elapsed;
				p.sprite = (p.sprite + 1) & 1;
				p.angle = PI * 1.5f;
			}
			else if(Input::pressed(VK_DOWN))
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
			if (Input::pressed(VK_SPACE))
			{
				// if last bullet not active now
				if (p.alive && p.bullet.pos_x == -1.0f && p.bullet.pos_y == -1.0f && bullet_delay < 0.0f)
				{
					p.bullet.angle = p.angle;
					p.bullet.speed_x = cosf(p.angle) * p.bullet.speed;
					p.bullet.speed_y = -sinf(p.angle) * p.bullet.speed;

					p.bullet.pos_x = p.pos_x;
					p.bullet.pos_y = p.pos_y;
					bullet_delay = 0.5f;
				}
			}
			bullet_delay -= timer.elapsed;

			// colision detection tank and objects
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

			// colision detection between tanks
			if (!collided)
			{
				for (int i = 0; i < nEnemies; i++)
				{
					if (enemies[i].alive)
					{
						if (box_collison_detection({ new_x - tank->size * 0.5f , new_y - tank->size * 0.5f }, tank->size,
							{ enemies[i].pos_x - tank->size * 0.5f , enemies[i].pos_y - tank->size * 0.5f }, tank->size))
						{
							collided = true;
						}
					}
				}
			}

			// player set new position
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

					if (obj.first < 2)
					{
						Packet packet = P_Destroy;
						send(window->client.Connection, (char*)&packet, sizeof(Packet), NULL);
						send(window->client.Connection, (char*)&i, sizeof(int), NULL);
					}

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

			// Detect if bullet hit tank
			for (int i = 0; i < nEnemies; i++)
			{
				if (box_collison_detection(vec2{ p.pos_x - tank->size/2, p.pos_y - tank->size/2}, tank->size,
					vec2{ enemies[i].bullet.pos_x - bullet.size/2, enemies[i].bullet.pos_y - bullet.size/2}, bullet.size))
				{
					if (p.alive)
					{
						p.alive = false;
						explosions.push_back(Explosion{ p.pos_x, p.pos_y, 3 });
					}
				}
			}

			// Detect if bullet hit enemy tank
			for (int i = 0; i < nEnemies; i++)
			{
				if (box_collison_detection(vec2{ p.bullet.pos_x - bullet.size/2, p.bullet.pos_y - bullet.size/2}, bullet.size,
					vec2{ enemies[i].pos_x - tank->size/2, enemies[i].pos_y - tank->size/2 }, tank->size))
				{
					if (enemies[i].alive)
					{
						p.Score += 100;
						explosions.push_back(Explosion{ enemies[i].pos_x, enemies[i].pos_y, 3 });
					}
				}
			}

			// Add explosions if tank was distroyed
			for (int i = 0; i < nEnemies; i++)
			{
				for (int j = 0; j < nEnemies; j++)
				{
					if (i == j) continue;

					if (box_collison_detection(vec2{ enemies[i].pos_x - tank->size/2, enemies[i].pos_y - tank->size/2 }, tank->size,
						vec2{ enemies[j].bullet.pos_x - bullet.size/2, enemies[j].bullet.pos_y - -bullet.size/2 }, bullet.size))
					{
						if (enemies[i].alive)
							explosions.push_back(Explosion{ enemies[i].pos_x, enemies[i].pos_y, 3 });
					}
				}
			}

			// data exchange with server
			
			// send data
			Packet packet = P_Player;
			send(window->client.Connection, (char*)&packet, sizeof(Packet), NULL);
			send(window->client.Connection, (char*)&p, sizeof(Tank), NULL);


			// Proccess distroctions
			for (int i = 0; i < nDestroy; i++)
			{
				// add explossion effect
				explosions.push_back(Explosion{ map[destroy[i]].second.x + BIG_SPRITE * 0.5f,
												map[destroy[i]].second.y + BIG_SPRITE * 0.5f , 2 });

				// remove destroyed element
				map.erase(map.begin() + destroy[i]);
			}
			nDestroy = 0;


			// if alive anly one then game will be restarted
			nAlives = p.alive;
			for (int i = 0; i < nEnemies; i++)
				nAlives += enemies[i].alive;

			if (nEnemies > 0 && nAlives < 2 )
				break;


			// =================== Draw ====================
			// clr screan
			draw_filled_rect_async(surface, 0.0f, 0.0f, 1.0f, 1.0f, Color(0));

			// draw player
			if (p.alive)
				draw_image_async_a_rotate(surface, tank[p.sprite], p.pos_x, p.pos_y, tank[0].size, tank[0].size, p.angle);

			// draw enemies
			for (int i = 0; i < nEnemies; i++)
			{
				if (enemies[i].alive)
					draw_image_async_a_rotate(surface, tank[enemies[i].sprite], enemies[i].pos_x, enemies[i].pos_y,
						tank[0].size, tank[0].size, enemies[i].angle);
			}

			// daraw bullet
			draw_image_a_rotate(surface, bullet, p.bullet.pos_x, p.bullet.pos_y, bullet.size, bullet.size, p.bullet.angle);

			// draw enemies bullets
			for (int i = 0; i < nEnemies; i++)
			{
				draw_image_a_rotate(surface, bullet, enemies[i].bullet.pos_x, enemies[i].bullet.pos_y,
					bullet.size, bullet.size, enemies[i].bullet.angle);
			}

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
					explosions[i].pos_y - size * 0.5f, size, size);
			}

			// ================== Ohter stuff =====================

			// get ellapsed time
			timer.update();

			// Render nickname
			nickname_timeout -= timer.elapsed;

			if (nickname_timeout > 0.0f)
			{
				if (p.alive)
				render_text(surface, p.pos_x - 0.05f, p.pos_y + 0.04f, p.name, Color(240, 150, 0), get_def_font(25));

				for (int i = 0; i < nEnemies; i++)
				{
					if (enemies[i].alive)
					render_text(surface, enemies[i].pos_x - 0.05f, enemies[i].pos_y + 0.04f, enemies[i].name, Color(240, 150, 0), get_def_font(25));
				}
			}

			// Render FPS on right top of window
			swprintf_s(buffer, L"FPS %d", timer.FPS);
			render_text(surface, 0.9f, 0.96f, buffer, Color(240, 150, 0), get_def_font(20));

			// Render score
			swprintf_s(buffer, L"%d %s", p.Score, p.name);
			render_text(surface, 0.9f, 0.93f, buffer, Color(240, 200, 0), get_def_font(16));

			for (int i = 0; i < nEnemies; i++)
			{
				swprintf_s(buffer, L"%d %s", enemies[i].Score, enemies[i].name);
				render_text(surface, 0.9f, 0.9f - i * 0.03, buffer, Color(240, 200, 0), get_def_font(16));
			}

			// Render canvas on the screan
			window->render_canvas();
		}
	}

	if (!wm_close)
		window->change_stage(Stage_Main_Menu);
}




bool box_collison_detection(vec2 pos1, float size1, vec2 pos2, float size2)
{
	return (pos1.x + size1 > pos2.x && pos1.x < pos2.x + size2) &&
		   (pos1.y + size1 > pos2.y && pos1.y < pos2.y + size2);
}