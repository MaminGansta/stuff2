
bool open_file_window(wchar_t* filename, int max_size, HWND parent, wchar_t* filter = NULL)
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = parent;
	ofn.lpstrFile = (LPWSTR)filename;
	filename[0] = L'\0';
	ofn.nMaxFile = max_size;
	ofn.nFilterIndex = 0;
	ofn.lpstrFilter = filter;

	GetOpenFileName(&ofn);
	return wcsnlen_s(filename, max_size) != 0;
}


bool save_file_window(wchar_t* dirname, int max_size, HWND parent, wchar_t* filter = NULL)
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = parent;
	ofn.lpstrFile = (LPWSTR)dirname;
	dirname[0] = L'\0';
	ofn.nMaxFile = max_size;
	ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT;
	ofn.nFilterIndex = 0;
	ofn.lpstrFilter = filter;

	GetSaveFileName(&ofn);
	return wcsnlen_s(dirname, max_size) != 0;
}