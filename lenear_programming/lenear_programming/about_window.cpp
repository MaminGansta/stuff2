
struct About_window : Window
{
	Label lAbout;

	About_window()
	{
		init(L"��������", 800, 600, [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)
			{
				About_window* window = (About_window*)args[0];

				return DefWindowProc(hwnd, msg, wParam, lParam);
			}, DEF_WINDOW);

		lAbout.init(getHWND(), L""
			"��������\n" 
			""
			"1)��� ���� ����� ������ ������� ���������� ��������� ���� '����������' � '�����������'"
			"������������ ������� �� 16 ������������\n"
			"����� � ��������� ������ ������� � ������(0.4), � ����� ����� / (1/5) ��� ����� ����� (5)\n\n"
			""
			"2) ����� �������� ��������� �������:\n"
			"     1) ����� ������� (�������� ��� ������������ �����)\n"
			"     2) ��� ������� ������� (���, ����)\n"
			"     3) ��� ������� �����\n\n"
			""
			"3) ������� ������ ������\n\n"
			""
			"4) ����� ����� �������� ���� ������� ������ ������� ��� ������.\n"
			" ��� ���� ����� ��������� ��������� ��� ������� ������ ���������.\n"
			" ��� ���� ����� ��������� �� ���������� ��� ������� ������ ����������.\n"
			" ��� ���� ����� ������� ����������� ������������� ������� ������ ����.\n"
			" ��� ���� ����� ������� ������� ������� ������� �� ����� ����, � �������� ������,"
			" ��� ������������ ��� ��������� ��������.\n\n"
			""
			"5) �������� ����������� ���� ������ ����� ������ ��� ������� ���������\n\n"
			""
			"6) � �������� ������ ��� ����� ������������ ������, �� ����� �������� �������������\n\n"
			""
			"7) ����/�����\n"
			"  ��� ���� ����� ������ ��������� � ���� ������� �� ���� ���� � ���������, "
			"  � ���������� � ���������� ��������� ���� � ������� ����� � ��������\n"
			"  ����� ������� ���� ����� ������ �� ���� ���� ������� � ������� ������ ����\n"
			"", 0, 0.0f, 0.0f, 1.0f, 1.0f, RESIZABLE);

		set_font_size(lAbout.handle, 20);
	}
};