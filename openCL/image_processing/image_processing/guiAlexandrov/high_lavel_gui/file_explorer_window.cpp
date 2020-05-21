
bool open_file_window(wchar_t* filename, int max_size, HWND parent, wchar_t* filter = NULL)
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = (LPWSTR)filename;
	filename[0] = L'\0';
	ofn.nMaxFile = max_size;
	ofn.nFilterIndex = 0;
	ofn.lpstrFilter = filter;
	ofn.nMaxCustFilter = 3;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;


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
	ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
	ofn.nFilterIndex = 0;
	ofn.lpstrFilter = filter;

	GetSaveFileName(&ofn);
	return wcsnlen_s(dirname, max_size) != 0;
}