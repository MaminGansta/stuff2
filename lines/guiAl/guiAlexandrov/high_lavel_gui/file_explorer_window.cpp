
bool open_file_window(wchar_t* filename, int max_size, HWND parent)
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = parent;
	ofn.lpstrFile = (LPWSTR)filename;
	filename[0] = L'\0';
	ofn.nMaxFile = max_size;
	ofn.nFilterIndex = 1;

	GetOpenFileName(&ofn);
	return wcsnlen_s(filename, 128) != 0;
}