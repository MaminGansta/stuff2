#define MAX_THREADS 8
#include "guiAlexandrov/include.h"

#define roundf(x) floor(x + 0.5f)

void line_DDA(Canvas& surface, float x1, float y1, float x2, float y2)
{
    // (1) ÷елочисленные значени€ координат начала и конца отрезка,
    // округленные до ближайшего целого
    int iX1 = roundf(x1);
    int iY1 = roundf(y1);
    int iX2 = roundf(x2);
    int iY2 = roundf(y2);

    // (2) ƒлина и высота линии
    int deltaX = abs(iX1 - iX2);
    int deltaY = abs(iY1 - iY2);

    // (3) —читаем минимальное количество итераций, необходимое
    // дл€ отрисовки отрезка. ¬ыбира€ максимум из длины и высоты
    // линии, обеспечиваем св€зность линии
    int length = max(deltaX, deltaY);
    // особый случай, на экране закрашиваетс€ ровно один пиксел
    if (length == 0)
    {
        drawPixel(surface, iX1, iY1, Color(255));
        return;
    }

    // (4) ¬ычисл€ем приращени€ на каждом шаге по ос€м абсцисс и ординат
    double dX = (x2 - x1) / length;
    double dY = (y2 - y1) / length;

    // (5) Ќачальные значени€
    double x = x1;
    double y = y1;

    // ќсновной цикл
    length++;
    while (length--)
    {
        x += dX;
        y += dY;
        drawPixel(surface, roundf(x), roundf(y), Color(255));
    }
}

void line_Bresenham(Canvas& surface, int x0, int y0, int x1, int y1) {
        bool steep = false;
        if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
            std::swap(x0, y0);
            std::swap(x1, y1);
            steep = true;
        }
        if (x0 > x1) {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }
        int dx = x1 - x0;
        int dy = y1 - y0;
        float derror = std::abs(dy / float(dx));
        float error = 0;
        int y = y0;
        for (int x = x0; x <= x1; x++) {
            if (steep) {
                drawPixel(surface, x, y, Color(255));

            }
            else {
                drawPixel(surface, y, x, Color(255));
            }
            error += derror;
            if (error > .5) {
                y += (y1 > y0 ? 1 : -1);
                error -= 1.;
            }
        }
    }


struct Line
{
	float x0 = 0, y0 = 0, x1 = 0, y1 = 0;
    int type = 0;
};


void draw_lesson_line(Canvas& surface, Line& line)
{

    if (line.type == 0)
        line_DDA(surface, surface.width * line.x0, surface.height * line.y0,
                          surface.width * line.x1, surface.height * line.y1);
    else
        line_Bresenham(surface,  surface.height * line.y0, surface.width * line.x0,
                                surface.height * line.y1, surface.width * line.x1);
}


template <typename Image_type>
struct My_window : Window
{
	Line show_line;

	std::vector<Line> lines;

    RadioButton DDA;
    RadioButton Bresenham;


	My_window()
	{

		init(L"window", 800, 600, [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)->LRESULT
			{
				My_window* window = (My_window*)args[0];

				switch (msg)
				{
                    case WM_LBUTTONDOWN:
                    {
                        if (window->show_line.x0 == -1)
                        {
                            window->show_line.type = window->Bresenham.chosed();
                            window->show_line.x0 = Mouse::pos_x;
                            window->show_line.y0 = Mouse::pos_y;
                            window->Bresenham.hide();
                            window->DDA.hide();
                        }
                        else
                        {
                            window->lines.push_back(window->show_line);
                            window->show_line.x0 = -1;
                            window->Bresenham.show();
                            window->DDA.show();
                        }
                    }break;
                    case WM_MOUSEMOVE:
                        if (window->show_line.x0 == -1) break;
                        window->show_line.x1 = Mouse::pos_x;
                        window->show_line.y1 = Mouse::pos_y;
					
                    case WM_PAINT:
                    {
                        PAINTSTRUCT ps;
                        BeginPaint(hwnd, &ps);
                        // clear screen
                        draw_filled_rect(window->canvas, 0.0f, 0.0f, 1.0f, 1.0f, Color(0));

                        for (int i = 0; i < window->lines.size(); i++)
                            draw_lesson_line(window->canvas, window->lines[i]);

                        if (window->show_line.x0 != -1)
                            draw_lesson_line(window->canvas, window->show_line);

                        window->render_canvas();
                        EndPaint(hwnd, &ps);
                    }break;
				}
				return DefWindowProc(hwnd, msg, wParam, lParam);
			});


        DDA.init(hwnd, L"DDA", 0.05f, 0.9f, 0.15f, 0.1f);
        set_font_size(DDA.hwnd, 25);

        Bresenham.init(hwnd, L"Bresenham", 0.2f, 0.9f, 0.15f, 0.1f);
        set_font_size(Bresenham.hwnd, 25);

        show_line.x0 = -1;
	}

};



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	al_init(hInstance);

	My_window<Image>* window = new My_window<Image>();

	Window::wait_msg_proc();
	return 0;
}
