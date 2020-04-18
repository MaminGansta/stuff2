

// ============== from text ======================

#define wtoi(str) ( str ? _wtoi(str) : 0)
#define wtof(str) ( str ? _wtof(str) : 0.0f)


// ================== font =======================

// Defult font for all compontnts
HFONT DEF_FONT = CreateFont(16, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");


HFONT get_def_font(int size)
{
	return CreateFont(size, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");
}

void set_font_size(HWND hwnd, int size)
{
	HFONT hFont = CreateFont(size, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");
	//HFONT hFont  = (HFONT)SendMessage(hwnd, WM_GETFONT, 0, 0);
	SendMessage(hwnd, WM_SETFONT, WPARAM(hFont), 0);
}

void set_font(HWND hwnd, HFONT font)
{
	SendMessage(hwnd, WM_SETFONT, WPARAM(font), FALSE);
}


// =============== render Text ====================

HBITMAP CreateHBITMAPfromByteArray(HDC hdc, int nImageWidth, int nImageHeight, void* pImageData)
{
	BITMAPINFOHEADER   bmih;
	memset(&bmih, 0, sizeof(BITMAPINFOHEADER));

	bmih.biWidth = nImageWidth;
	bmih.biHeight = nImageHeight;
	bmih.biBitCount = 32;
	bmih.biCompression = BI_RGB;
	bmih.biSize = sizeof(BITMAPINFOHEADER);
	bmih.biPlanes = 1;

	BITMAPINFO* bmi = (BITMAPINFO*)&bmih;

	return CreateDIBitmap(hdc, &bmih, CBM_INIT, pImageData, bmi, DIB_RGB_COLORS);

}

void WriteTextOnByteArray(int nImageWidth, int nImageHeight, int x, int y, void* pImageData, HFONT hFont, const wchar_t* szText, COLORREF textColor)
{
	HDC hdc = GetDC(NULL);
	HDC memDC = CreateCompatibleDC(hdc);
	HBITMAP hBmp = CreateHBITMAPfromByteArray(hdc, nImageWidth, nImageHeight, pImageData);

	HGDIOBJ oldBMP = SelectObject(memDC, hBmp);
	HGDIOBJ oldFont = SelectObject(memDC, hFont);
	SetBkMode(memDC, TRANSPARENT);
	SetTextColor(memDC, textColor);
	
	int height;
	GetCharWidth32A(memDC,0,0, &height);
	y -= height;

	TextOutW(memDC, x, y, szText, wcslen(szText));
	SelectObject(memDC, oldBMP);
	SelectObject(memDC, oldFont);

	BITMAPINFO bmpInfo;
	memset(&bmpInfo, 0, sizeof(BITMAPINFOHEADER));
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

	GetDIBits(memDC, hBmp, 0, 0, NULL, &bmpInfo, DIB_RGB_COLORS);
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;
	GetDIBits(memDC, hBmp, 0, bmpInfo.bmiHeader.biHeight, pImageData, &bmpInfo, DIB_RGB_COLORS);


	DeleteObject(hBmp);
	DeleteDC(memDC);
	ReleaseDC(NULL, hdc);
}



template <typename Surface_type>
void render_text(Surface_type& surface, float fx, float fy, const std::wstring& text, const Color& color = Color(), HFONT hFont = DEF_FONT)
{
	int x = surface.width * fx;
	int y = surface.height * (1.0f - fy);
	WriteTextOnByteArray(surface.width, surface.height, x, y, surface.data, hFont, text.c_str(), RGB(color.r, color.g, color.b));
}