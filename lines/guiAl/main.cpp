#include "guiAlexandrov/include.h"

#include "figure.cpp"
#include "draw.cpp"



void draw_figure(Canvas& surface, const Figure& figure)
{
	// dda
	if (figure.type == FigureType::line_dda)
		line_DDA(surface, surface.width * figure.x0, surface.height * figure.y0,
			surface.width * figure.x1, surface.height * figure.y1);

	// brasenham
	if (figure.type == FigureType::line_bresenham)
		line_Bresenham(surface, surface.height * figure.y0, surface.width * figure.x0,
			surface.height * figure.y1, surface.width * figure.x1);

	// circle
	if (figure.type == FigureType::circle)
		circleBres(surface, figure.x0 * surface.width, figure.y0 * surface.height,
			sqrtf(pow((figure.x1 - figure.x0),2) + pow((figure.y1 - figure.y0), 2)) * surface.height);

	// bezier
	if (figure.type == FigureType::bezier)
		drawBezier(surface, figure.bezier);

}




struct My_window : Window
{
	Figure active_figure;

	std::vector<Figure> figures;

    RadioButton DDA;
    RadioButton Bresenham;
	RadioButton Circle;
	RadioButton Bezier;

	My_window()
	{

		init(L"window", 800, 600, [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)->LRESULT
			{
				My_window* window = (My_window*)args[0];

				switch (msg)
				{
					case WM_RBUTTONDOWN:
					{
						if (window->active_figure.type == FigureType::bezier)
						{
							window->figures.push_back(window->active_figure);
							window->active_figure.type = FigureType::none;
							window->active_figure.bezier.clear();

							for (auto& component : components[window->hwnd])
								component.show();
						}
					}break;

                    case WM_LBUTTONDOWN:
                    {
						if (window->active_figure.type != FigureType::none &&
							window->active_figure.type != FigureType::bezier)
						{
							window->figures.push_back(window->active_figure);
							window->active_figure.type = FigureType::none;

							for (auto& component : components[window->hwnd])
								component.show();
							
							break;
						}

						// Define figure type
						if (window->Bresenham.chosed())
								window->active_figure.type = FigureType::line_bresenham;

						if (window->DDA.chosed())
							window->active_figure.type = FigureType::line_dda;

						if (window->Circle.chosed())
							window->active_figure.type = FigureType::circle;

						if (window->Bezier.chosed())
							window->active_figure.type = FigureType::bezier;


						// Bezier handler
						if (window->active_figure.type == FigureType::bezier)
								window->active_figure.bezier.push_back(vec2(Mouse::pos_x, Mouse::pos_y));

						// default handler
						if (window->active_figure.type != FigureType::none)
						{
							window->active_figure.x0 = Mouse::pos_x;
							window->active_figure.y0 = Mouse::pos_y;

							window->active_figure.x1 = Mouse::pos_x;
							window->active_figure.y1 = Mouse::pos_y;

							for (auto& component : components[window->hwnd])
								component.hide();
						}
                    }break;

                    case WM_MOUSEMOVE:
                        if (window->active_figure.type == FigureType::none)
							break;
                        
						if (window->active_figure.type == FigureType::bezier)
							window->active_figure.bezier.back() = vec2(Mouse::pos_x, Mouse::pos_y);

						window->active_figure.x1 = Mouse::pos_x;
                        window->active_figure.y1 = Mouse::pos_y;
					
                    case WM_PAINT:
                    {
                        PAINTSTRUCT ps;
                        BeginPaint(hwnd, &ps);

                        // clear screen
                        draw_filled_rect_async(window->canvas, 0.0f, 0.0f, 1.0f, 1.0f, Color(0));

                        for (int i = 0; i < window->figures.size(); i++)
                            draw_figure(window->canvas, window->figures[i]);

						
						if (window->active_figure.type != FigureType::none)
							draw_figure(window->canvas, window->active_figure);


                        window->render_canvas();
                        EndPaint(hwnd, &ps);
                    }break;
				}
				return DefWindowProc(hwnd, msg, wParam, lParam);
			});


        DDA.init(hwnd, L"DDA", 0.05f, 0.9f, 0.15f, 0.1f);
        set_font_size(DDA.hwnd, 25);

        Bresenham.init(hwnd, L"Bresenham", 0.2f, 0.9f, 0.2f, 0.1f);
        set_font_size(Bresenham.hwnd, 25);

		Circle.init(hwnd, L"Circle", 0.399f, 0.9f, 0.15f, 0.1f);
		set_font_size(Circle.hwnd, 25);
        
		Bezier.init(hwnd, L"Bezier", 0.548f, 0.9f, 0.15f, 0.1f);
		set_font_size(Bezier.hwnd, 25);

		active_figure.type = FigureType::none;
	}

};




int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	al_init(hInstance);

	new My_window();

	Window::wait_msg_proc();
	return 0;
}
