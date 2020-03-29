template <typename T>
void output(wchar_t buffer[], T a, T b);

template <>
void output<float>(wchar_t buffer[], float a, float b)
{
	swprintf_s(buffer, 32, L"X = (%.1f, %.1f)", a, b);
}

template <>
void output<Fraction>(wchar_t buffer[], Fraction a, Fraction b)
{
	swprintf_s(buffer, 32, L"X = (%d/%d, %d/%d)", a.top, a.bottom, b.top, b.bottom);
}


template <typename T>
struct Graph_window : Window
{
	fImage graph;
	
	Label lSolution;
	Label lTarget;
	Label lPoint;

	Graph_window(std::vector<T>& target, Mat<T>& limits, int type)
	{
		init(L"graphic method", 800, 600, [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)->LRESULT
			{
				Graph_window* window = (Graph_window*)args[0];

				switch (msg)
				{
					case WM_PAINT:
					{
						PAINTSTRUCT ps;
						BeginPaint(hwnd, &ps);

						draw_image(window->canvas, window->graph, 0, 0, 1.0f, 0.8f);
						draw_filled_rect(window->canvas, 0, 0.8f, 1.0f, 0.2f, fColor(1.0f));
						window->render_canvas();

						EndPaint(hwnd, &ps);
					}return 0;
					case WM_CLOSE:
					{
						safe_release(window);
					}return 0;
				}

				return DefWindowProc(hwnd, msg, wParam, lParam);
			});


		lSolution.init(getHWND(), L"Solution", 0, 0.1f, 0.05f, 0.2f, 0.05f, RESIZABLE);
		set_font_size(lSolution.handle, 25);
		set_min_max_size(800, 600);

		auto [F, point] = solve(target, limits, type);
		if (point.size() == 0) return;

		wchar_t buffer[32];
		wchar_t F_buffer[32];

		to_str(buffer, F);
		swprintf_s(F_buffer, 32, L"F = %s", buffer);
		lTarget.init(getHWND(), F_buffer, 0, 0.1f, 0.1f, 0.2f, 0.05f, RESIZABLE);

		output(buffer, point[0], point[1]);
		lPoint.init(getHWND(), buffer, 0, 0.1f, 0.15f, 0.2f, 0.05f, RESIZABLE);

		redraw();
	}

	std::tuple<T, std::vector<T>> solve(std::vector<T>& target, Mat<T>& limits, int type)
	{
		std::vector<std::vector<T>> points;

		// extend by adding axises
		Mat<T> ex_limits(limits.row + 2, limits.column);
		for (int i = 0; i < limits.row; i++)
			for (int j = 0; j < limits.column; j++)
				ex_limits[i][j] = limits[i][j];

		// add ixises
		ex_limits[limits.row][0] = 1;
		ex_limits[limits.row][1] = 0;
		ex_limits[limits.row][2] = 0;

		ex_limits[limits.row + 1][0] = 0;
		ex_limits[limits.row + 1][1] = 1;
		ex_limits[limits.row + 1][2] = 0;


		// find points that lay on intersection
		Mat<T> temp(2, ex_limits.column);

		for (int i = 0; i < ex_limits.row; i++)
		{
			for (int j = i; j < ex_limits.row; j++)
			{
				if (i == j) continue;

				for (int a = 0; a < temp.column; a++)
				{
					temp[0][a] = ex_limits[i][a];
					temp[1][a] = ex_limits[j][a];
				}

				auto [flag, intersection] = gausian_method(temp);

				if (flag > 0)
					points.push_back(intersection);
			}
		}

		// remove points that lay outside the limits
		std::vector<std::vector<T>> new_points;
		for (auto& point : points)
		{
			int i = 0;
			for (; i < limits.row; i++)
			{
				T x0 = 0;
				T y0 = limits[i][1] ? limits[i][2] / limits[i][1] : limits[i][2];
				T x1 = 4;
				T y1 = limits[i][1] ? (limits[i][2] - limits[i][0] * 4) / limits[i][1] * 4 : (limits[i][2] - limits[i][0] * 4);

				// cross product
				T dot = (x1 - x0) * (point[1] - y0) - (y1 - y0) * (point[0] - x0);
				if (dot > 0.001f)
					break;
			}
			if (i == limits.column)
				new_points.push_back(point);
		}
		points = new_points;

		if (points.size() == 0)
			return {0, std::vector<T>()};

		// find max value 
		int idx = 0;
		T F = target[0] * points[0][0] + target[1] * points[0][1] + target[2] + 0.0f;

		for (int i = 1; i < points.size(); i++)
		{
			int temp = target[0] * points[i][0] + target[1] * points[i][1] + target[2] + 0.0f;
			if (type == 0)
			{
				if (F > temp)
				{
					F = temp;
					idx = i;
				}
			}
			else
			{
				if (F < temp)
				{
					F = temp;
					idx = i;
				}
			}
		}

		make_graph(points[idx], limits, target);
		return { F, points[idx] };
	}


	void make_graph(std::vector<T>& point, Mat<T>& limits, std::vector<T>& target)
	{
		graph.resize(400, 400);
		int zero = 100;
		int x0 = -20, x1 = 20;
		int coef = 10;

		// draw axises
		drawLine(graph, 0, zero, graph.width, zero, fColor(1.0f));
		drawLine(graph, zero, 0, zero, graph.height, fColor(1.0f));

		for (int i = 0; i < graph.width; i++)
			drawPixel(graph, i * coef, zero + 1, fColor(1.0f));

		for (int i = 0; i < graph.height; i++)
			drawPixel(graph, zero + 1, i * coef, fColor(1.0f));


		for (int i = 0; i < limits.row; i++)
		{
			float y0 = 0;
			float y1 = 0;
			x0 = -20;
			x1 = 20;


			if (limits[i][1] + 0.0f != 0)
			{
				y0 = (limits[i][2] - limits[i][0] * x0) / limits[i][1] + 0.0f;
				y1 = (limits[i][2] - limits[i][0] * x1) / limits[i][1] + 0.0f;
			}
			else
			{
				x0 = x1 = limits[i][2] / limits[i][0] + 0.0f;
				y0 = -40;
				y1 = 100;
			}
			y0 *= coef;
			y1 *= coef;
			x0 *= coef;
			x1 *= coef;
			
			drawLine(graph, x0 + zero, y0 + zero, x1 + zero, y1 + zero, fColor(1.0f));
		}

		// draw target
		float y0 = 0;
		float y1 = 0;
		x0 = -20;
		x1 = 20;

		if (target[1] + 0.0f != 0)
		{
			y0 = (-target[2] - target[0] * x0) / target[1] + 0.0f;
			y1 = (-target[2] - target[0] * x1) / target[1] + 0.0f;
		}
		else
		{
			x0 = x1 = -target[2] / target[0] + 0.0f;
			y0 = -40;
			y1 = 100;
		}
		x0 += point[0] + 0.0f;
		x1 += point[0] + 0.0f;
		y0 += point[1] + 0.0f;
		y1 += point[1] + 0.0f;

		y0 *= coef;
		y1 *= coef;

		drawLine(graph, x0 * coef + zero, y0 + zero, x1 * coef + zero, y1 + zero, fColor(1.0f, 0 ,0));

	}


};