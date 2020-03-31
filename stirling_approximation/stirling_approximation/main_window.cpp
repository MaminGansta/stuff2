#include <cassert>
#define CALCULATE_ID 100

// input struct
struct Params
{
	double A, B, C, D;
	double alpha, betta, epsilon, gamma;
	int nodes;
	double delata;
};

fImage make_graph(Params p)
{
	if (p.alpha == 0) p.alpha = 1;
	if (p.betta == 0) p.betta = 1;
	if (p.gamma == 0) p.gamma = 1;
	if (p.epsilon == 0) p.epsilon = 1;

	if (p.A == 0) p.A = -10;
	if (p.B == 0) p.B = 10;
	if (p.C == 0) p.C = -10;
	if (p.D == 0) p.D = 10;



	int width = 1600;
	int height = 800;

	int graph_w = p.B - p.A;
	int graph_h = p.D - p.C;

	double x_coef = (double)graph_w / width;
	double y_coef = (double)graph_h / (height / 2);

	fImage graph(width, height);


	// step
	double h = double(graph_w) / (p.nodes * 2 + 1);

	// clear graph area
	draw_filled_rect(graph, 0, 0, 1.0f, 1.0f, fColor(0));

	// draw y axis
	if (p.A < 0 && p.B > 0)
	{
		for (int y = 0; y < height; y++)
		{
			drawPixel(graph, width * (double(abs(p.A) / graph_w)), y, fColor(1.0f));
			drawPixel(graph, width * (double(abs(p.A) / graph_w)) + 1, y, fColor(1.0f));
		}
	}

	// draw x axis
	if (p.C < 0 && p.D > 0)
	{
		for (int x = 0; x < width; x++)
		{
			drawPixel(graph, x, height * (double(abs(p.C) / graph_h)), fColor(1.0f));
			drawPixel(graph, x, height * (double(abs(p.C) / graph_h)) + 1, fColor(1.0f));
		}
	}


	for (int x = 0; x < width; x++)
	{
		double xx = x * x_coef + p.A;
		double y = p.alpha * sinf(tanf(p.betta * xx)) + p.epsilon * cosf(p.gamma * xx);

		y = y / y_coef + height / 2;

		for (int i = -2; i < 2; i++)
			for (int j = -3; j < 3; j++)
				addPixel(graph, x + i, y + j, fColor(1.0 / (1 + i * i + j * j)));
	}

	if (p.nodes == 0)
		return graph;

	// delata y
	std::vector<std::vector<double>> dy(p.nodes * 2 + 2);

	for (int i = 0; i < p.nodes * 2 + 2; i++)
		dy[0].push_back(p.alpha * sinf(tanf(p.betta * (p.A + h*i))) + p.epsilon * cosf(p.gamma * (p.A + h * i)));


	for (int i = 1; i < 2 * p.nodes + 2; i++)
		for (int j = 0; j < 2 * p.nodes + 2 - i; j++)
			dy[i].push_back(dy[i - 1][j + 1] - dy[i - 1][j]);




	for (int x = 0; x < width; x++)
	{
		double xx = x * x_coef + p.A;

		double c = 1;
		double x0 = p.A + p.nodes * (p.B - p.A) / (2 * p.nodes + 1);
		double q = (xx - x0) * (2 * p.nodes + 1) / (p.B - p.A);
		double y = (dy[0][p.nodes] + dy[0][p.nodes + 1]) / 2;
		y = y + (q - 0.5) * dy[1][p.nodes];

		for (int i = 1; i < p.nodes + 1; i++)
		{
			c = c * (q + i - 1) * (q - i) / (2 * i);
			y = y + c * (dy[2 * i][p.nodes - i] + dy[2 * i][p.nodes - i + 1]) / 2;
			c = c / (2 * i + 1);
			y = y + (q - 0.5) * c * dy[2 * i + 1][p.nodes - i];
		}

		y = y / y_coef + height / 2;

		for (int i = -2; i < 2; i++)
			for (int j = -3; j < 3; j++)
				addPixel(graph, x + i, y + j, fColor(1.0f / (1 + i * i + j * j), 0.0f, 0.0f));

	}


	return graph;
}


struct MainWindow : Window
{
	Label lExpression;
	Text tAlpha;
	Text tBetta;
	Text tEpsilon;
	Text tGamma;

	Label lA;
	Text  tA;
	Label lB;
	Text  tB;
	Label lC;
	Text  tC;
	Label lD;
	Text  tD;
	Label lNodes;
	Text  tNodes;
	Label lDelta;
	Text  tDelta;
	Button bCalculate;

	fImage graph;
	Params params;

	MainWindow()
	{
		Params get_input(MainWindow * window);


		init(L"Stirling approximation", 800, 600, [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)->LRESULT
			{
				MainWindow* window = (MainWindow*)args[0];

				switch (msg)
				{
					case WM_COMMAND:
					{
						if (LOWORD(wParam) == CALCULATE_ID)
						{
							window->params = get_input(window);
							window->graph = make_graph(window->params);
							window->redraw();
						}
					}break;
					case WM_PAINT:
					{
						PAINTSTRUCT plug;
						BeginPaint(hwnd, &plug);

						draw_filled_rect(window->canvas, 0.0f, 0.7f, 1.0f, 1.0f, fColor(0.8f));
						draw_image(window->canvas, window->graph, 0.0f, 0.0f, 1.0f, 0.7f);
						window->render_canvas();

						EndPaint(hwnd, &plug);
					}break;
					case WM_CLOSE:
					{
						quick_exit(0);
					}return 0;
				}

				return DefWindowProc(hwnd, msg, wParam, lParam);
			});

		UINT myStyle = WS_VISIBLE | WS_CHILD | WS_BORDER;

		lExpression.init(this->getHWND(), L"sin(tg(          x)) +           cos(          x)", 0, 0.1f, 0.02f, 0.53f, 0.06f, STATIC);
		tAlpha.init(this->getHWND(), 1, 0.03f, 0.02f, 0.07f, 0.06f, STATIC, myStyle);
		tBetta.init(this->getHWND(), 2, 0.2f, 0.02f, 0.07f, 0.06f, STATIC, myStyle);
		tEpsilon.init(this->getHWND(), 3, 0.35f, 0.02f, 0.07f, 0.06f, STATIC, myStyle);
		tGamma.init(this->getHWND(), 5, 0.51f, 0.02f, 0.07f, 0.06f, STATIC, myStyle);
		//
		//
		lA.init(this->getHWND(), L"A=", 0, 0.01f, 0.1f, 0.05f, 0.05f, STATIC);
		tA.init(this->getHWND(), 1, 0.05f, 0.1f, 0.07f, 0.05f, STATIC, myStyle);
		//
		lB.init(this->getHWND(), L"B=", 0, 0.01f, 0.15f, 0.05f, 0.05f, STATIC);
		tB.init(this->getHWND(), 1, 0.05f, 0.15f, 0.07f, 0.05f, STATIC, myStyle);
		//
		lC.init(this->getHWND(), L"C=", 0, 0.01f, 0.2f, 0.05f, 0.05f, STATIC);
		tC.init(this->getHWND(), 1, 0.05f, 0.2f, 0.07f, 0.05f, STATIC, myStyle);
		//
		lD.init(this->getHWND(),  L"D=", 0, 0.01f, 0.25f, 0.05f, 0.05f, STATIC);
		tD.init(this->getHWND(),  1, 0.05f, 0.25f, 0.07f, 0.05f, STATIC, myStyle);
		//
		lNodes.init(this->getHWND(), L"Nodes=", 0, 0.7f, 0.02f, 0.1f, 0.05f, STATIC);
		tNodes.init(this->getHWND(), 1, 0.8f, 0.02f, 0.07f, 0.05f, STATIC, myStyle);

		lDelta.init(this->getHWND(), L"Delta=", 0, 0.7f, 0.07f, 0.1f, 0.05f, STATIC);
		tDelta.init(this->getHWND(), 1, 0.8f, 0.07f, 0.11f, 0.05f, STATIC, myStyle);


		bCalculate.init(this->getHWND(), L"Calculate", CALCULATE_ID, 0.25f, 0.12f, 0.2f, 0.15f, STATIC);

		// set font size
		for (auto component : components[this->getHWND()])
			set_font_size(component.handle, 25);
		
		set_font_size(lExpression.handle, 30);
		set_min_max_size(800, 600);
	}
};


#define wtoi(str) ( str ? _wtoi(str) : 0)
#define wtof(str) ( str ? _wtof(str) : 0.0f)

Params get_input(MainWindow* window)
{
	Params p;

	p.A = wtof(window->tA.getText());
	p.B = wtof(window->tB.getText());
	p.C = wtof(window->tC.getText());
	p.D = wtof(window->tD.getText());

	p.alpha = wtof(window->tAlpha.getText());
	p.betta = wtof(window->tBetta.getText());
	p.epsilon= wtof(window->tEpsilon.getText());
	p.gamma= wtof(window->tGamma.getText());

	p.nodes = wtoi(window->tNodes.getText());
	p.delata= wtof(window->tDelta.getText());
	
	return p;
}