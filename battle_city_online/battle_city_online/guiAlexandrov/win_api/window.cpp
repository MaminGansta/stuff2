
// =========================================== CALLBACK ARGUMENTS =============================================================

struct Args
{
	void* vals[2];
	std::function<LRESULT(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)> callback;

	void* operator [] (int i) { return vals[i]; }
};

struct Arguments
{
	std::vector<std::pair<HWND, Args>> buffer;

	void add(HWND hwnd, void* pwindow, void* parg, std::function<LRESULT(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)> callback)
	{
		buffer.push_back(std::make_pair(hwnd, Args{ pwindow, parg, callback}));
	}

	Args get(HWND hwnd)
	{
		auto it = std::find_if(buffer.begin(), buffer.end(), [hwnd](std::pair<HWND, Args> in) { return hwnd == in.first; });
		return it == buffer.end() ? Args{NULL,NULL} : it->second;
	}

	void remove(HWND hwnd)
	{
		auto it = std::find_if(buffer.begin(), buffer.end(), [hwnd](std::pair<HWND, Args> in) { return hwnd == in.first; });
		if (it != buffer.end())
			buffer.erase(it);
	}
};
Arguments arguments;


// ================================== WINDOW COMPONENTS ====================================================
#define STATIC 0
#define DYNAMIC 1
#define RESIZABLE 2

struct Component
{
	int  id;
	float x, y, width, height;
	UINT type;
	UINT style;
	HWND hwnd;
	HWND parent;

	Component(int id, float x, float y, float width, float height,
		      UINT type, UINT style, HWND hwnd, HWND parent)
		: 
		id(id), x(x), y(y), width(width), height(height),
		type(type), style(style), hwnd(hwnd), parent(parent)
	{
		set_font(hwnd, DEF_FONT);
	}

	virtual ~Component() {};// DestroyWindow(hwnd);

	void resize(RECT& rect)
	{
		if (type == STATIC) return;

		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;

		UINT flags = SWP_NOZORDER;

		if (type == DYNAMIC)
			flags = SWP_NOSIZE;

		SetWindowPos(hwnd, 0, x * nWidth, y * nHeight, width * nWidth + 1, height * nHeight + 1, flags);
	}

	void resize(float width, float height)
	{
		this->height = height;
		this->width = width;

		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;
		
		SetWindowPos(hwnd, 0, 0, 0, width * nWidth + 1, height * nHeight + 1, SWP_NOZORDER | SWP_NOMOVE);
	}

	void move(float x, float y)
	{
		this->x = x;
		this->y = y;

		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;

		SetWindowPos(hwnd, 0, x * nWidth, y * nHeight, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}


	void hide() { ShowWindow(hwnd, SW_HIDE); }
	void show() { ShowWindow(hwnd, SW_SHOWNOACTIVATE); }
};


struct Component_crt
{
	std::vector<std::pair<HWND, std::vector<Component>>> components;

	void add(HWND parent, Component& comp)
	{
		auto pointer = std::find_if(components.begin(), components.end(), [&parent](const std::pair<HWND, std::vector<Component>>& elem) {return elem.first == parent;});
		
		// if no such hwnd in storage
		if (pointer == components.end())
		{
			components.push_back(std::make_pair(parent, std::vector<Component>()));
			components.back().second.push_back(comp);
			return;
		}

		// add component to the parent
		pointer->second.push_back(comp);
	}

	void remove(HWND parent)
	{
		auto pointer = std::find_if(components.begin(), components.end(), [&parent](const std::pair<HWND, std::vector<Component>>& elem) {return elem.first == parent;});
		if (pointer == components.end()) return;

		// release components of this parent
		for (auto& comp : pointer->second)
		  DestroyWindow(comp.hwnd);

		components.erase(pointer);
	}

	void resize(HWND parent)
	{
		auto pointer = std::find_if(components.begin(), components.end(), [&parent](const std::pair<HWND, std::vector<Component>>& elem) {return elem.first == parent;});
		if (pointer == components.end()) return;
		
		RECT rect;
		GetClientRect(parent, &rect);

		for (auto& component : pointer->second)
			component.resize(rect);
	}

	void redraw(HWND parent)
	{
		auto pointer = std::find_if(components.begin(), components.end(), [&parent](const std::pair<HWND, std::vector<Component>>& elem) {return elem.first == parent; });
		if (pointer == components.end()) return;
		
		for (auto& component : pointer->second)
			RedrawWindow(component.hwnd, 0, 0, RDW_INVALIDATE | RDW_ALLCHILDREN);
	}

	auto& operator[] (HWND parent)
	{
		auto pointer = std::find_if(components.begin(), components.end(), [&parent](const std::pair<HWND, std::vector<Component>>& elem) {return elem.first == parent; });

		if (pointer == components.end())
		{
			components.push_back(std::make_pair(parent, std::vector<Component>()));
			return components.back().second;
		}

		return pointer->second;
	}

};
Component_crt components;


int global_id = 0;
struct Component_id
{
	int id;
	HWND parent;
	HWND hwnd;

	Component_id() : id(global_id++) {}

	Component* get() const
	{
		// this is how c++ work
		int comp_id = id;
		auto& comps = components[parent];
		auto iterator = find_if(comps.begin(), comps.end(), [comp_id](const Component& comp) { return comp.id == comp_id; });
		return &(*iterator);
	}

	void resize(float width, float height)
	{
		Component* comp = get();

		comp->height = height;
		comp->width = width;

		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;

		SetWindowPos(hwnd, 0, 0, 0, width * nWidth + 1, height * nHeight + 1, SWP_NOZORDER | SWP_NOMOVE);
	}

	void move(float x, float y)
	{
		Component* comp = get();

		comp->x = x;
		comp->y = y;

		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;

		SetWindowPos(hwnd, 0, x * nWidth, y * nHeight, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}


	void hide() { ShowWindow(hwnd, SW_HIDE); }
	void show() { ShowWindow(hwnd, SW_SHOWNOACTIVATE); }
};



// ========================================= WINDOW ========================================================
#define DEF_WINDOW	WS_OVERLAPPEDWINDOW | WS_VISIBLE

struct Window
{	
	static int name_id;
	static std::vector<std::pair<int, Window*>> windows;

	int class_id;
	HWND hwnd;
	HDC hdc;
	Canvas canvas;
	int min_w = 0, min_h = 0;
	int max_w = 600, max_h = 800;

	Window() {}
	Window(
		const std::wstring& window_name,
		int width,
		int height,
		std::function<LRESULT(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)> callback,
		UINT style = DEF_WINDOW,
		HWND parent = NULL
	)
	{
		init(window_name, width, height, callback, style, parent);
	}

	void init(
		const std::wstring& window_name,
		int width,
		int height,
		std::function<LRESULT(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)> callback,
		UINT style = DEF_WINDOW,
		HWND parent = NULL
	)
	{

		class_id = name_id++;
		wchar_t class_name[16];
		swprintf_s(class_name, L"class_%d", class_id);

		WNDCLASSEX wc;
		wc.cbSize = sizeof(wc);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInst;
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = class_name;
		wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);


		// global callback function
		wc.lpfnWndProc = [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)->LRESULT 
		{
			Args args = arguments.get(hwnd);
			Window* window = (Window*)args[0];
			if (window == NULL)	return DefWindowProc(hwnd, msg, wParam, lParam);

			PAINTSTRUCT ps;
			switch (msg)
			{
				case  WM_GETMINMAXINFO:
				{
					LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
					lpMMI->ptMinTrackSize.x = window->min_w;
					lpMMI->ptMinTrackSize.y = window->min_h;

					lpMMI->ptMaxTrackSize.x = window->max_w;
					lpMMI->ptMaxTrackSize.y = window->max_h;
					break;
				}


					// Keyboard
				case WM_SYSKEYDOWN:
					if (wParam == VK_ALT) Input::vk_alt = true;
					if (wParam == VK_F10) Input::vk_f10 = true;
					break;

					//case WM_SYSKEYUP:
					//	break;

				case WM_KEYDOWN:
					Input::pressed_any = true;
					Input::keys[wParam] += 1;
					Input::key_was_pressed[wParam] = true;
					break;

				case WM_KEYUP:
					Input::keys[wParam] = 0;
					break;

				case WM_CHAR:
					Input::char_buffer[(wchar_t)wParam] = true;
					break;


					// Mouse
				case WM_MOUSEMOVE:
				{
					float xPos = GET_X_LPARAM(lParam);
					float yPos = GET_Y_LPARAM(lParam);
					Mouse::pos_x = xPos / window->canvas.width;
					Mouse::pos_y = 1.0f - yPos / window->canvas.height;
				}break;


				case WM_RBUTTONDOWN:
				case WM_MBUTTONDOWN:
				case WM_LBUTTONDOWN:
				case WM_XBUTTONDOWN:
					Mouse::buttons[WM_LBUTTONDOWN - 512] += 1;
					Mouse::key_was_pressed[WM_LBUTTONDOWN - 512] = true;
					break;

				case WM_LBUTTONUP:
				case WM_MBUTTONUP:
				case WM_RBUTTONUP:
				case WM_XBUTTONUP:
					Mouse::buttons[WM_LBUTTONDOWN - 512] = 0;
					break;


				case WM_SIZE:
					window->canvas.resize(hwnd);
					components.resize(hwnd);
					break;
			}
			
			// user's callback
			LRESULT res = args.callback(hwnd, msg, wParam, lParam, args);
			
			switch (msg)
			{
				case WM_CLOSE:
					safe_release(window);
					return 0;
			}

			return res;
		};


		if (!RegisterClassEx(&wc))
		{
			MessageBox(NULL, L"Cannot register class", L"Error", MB_OK);
			assert(false);
		}

		hwnd = CreateWindow(wc.lpszClassName, window_name.c_str(), style, CW_USEDEFAULT, CW_USEDEFAULT, width, height, parent, (HMENU)0, (HINSTANCE)hInst, NULL);
		hdc = GetDC(hwnd);

		// set window def ssize
		set_min_max_size(width, height, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));

		// add window to the widnow holder
		windows.push_back(std::make_pair(class_id, this));

		// put window ptr to global holder
		arguments.add(hwnd, this, NULL, callback);

		// repeat messages
		SendMessage(hwnd, WM_CREATE, 0, 0);
		SendMessage(hwnd, WM_SIZE, 0, 0);
		SendMessage(hwnd, WM_PAINT, 0, 0);
	}

	virtual ~Window()
	{
		int id = class_id;
		auto pointer = std::find_if(windows.begin(), windows.end(), [id](const std::pair<int, Window*>& a) {return id == a.first; });
		windows.erase(pointer);

		ReleaseDC(hwnd, hdc);
		arguments.remove(hwnd);
		components.remove(hwnd);

		if (windows.size() == 0)
			PostQuitMessage(0);
	}

	void close()
	{
		SendMessage(hwnd, WM_CLOSE, 0, 0);
	}

	void render_canvas(const PAINTSTRUCT& ps)
	{


		HDC hdc = GetDC(NULL);
		HDC memDC = CreateCompatibleDC(hdc);
		HBITMAP hBmp = CreateHBITMAPfromByteArray(hdc, canvas.width, canvas.height, canvas.data);
		
		HGDIOBJ oldBMP = SelectObject(memDC, hBmp);
		
		const RECT& rect = ps.rcPaint;
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;
		
		// redraw area
		BitBlt(ps.hdc, rect.left, rect.top, width, height, memDC, rect.left, rect.top, SRCCOPY);
		
		SelectObject(memDC, oldBMP);
		DeleteDC(memDC);
		ReleaseDC(NULL, hdc);
	}

	void render_canvas()
	{
		StretchDIBits(hdc, 0, 0, canvas.width, canvas.height, 0, 0, canvas.width, canvas.height, canvas.data, &canvas.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
	}

	void redraw()
	{
		//RedrawWindow(getHWND(), 0, 0, RDW_INVALIDATE | RDW_ALLCHILDREN);
		SendMessage(hwnd, WM_PAINT, 0, 0);
	}

	int height()
	{
		return canvas.height;
	}

	int widht()
	{
		return canvas.width;
	}

#define MAX_WIN_SIZE -1
#define OLD_WIN_SIZE -2

	void set_min_max_size(int minw, int minh, int maxw = OLD_WIN_SIZE, int maxh = OLD_WIN_SIZE)
	{
		min_w = max(0, minw);
		min_h = max(0, minh);
		

		if (maxw == MAX_WIN_SIZE)
			max_w = GetSystemMetrics(SM_CXSCREEN);
		else if (maxw != OLD_WIN_SIZE)
			max_w = maxw;

		if (maxh == MAX_WIN_SIZE)
			max_h = GetSystemMetrics(SM_CYSCREEN);
		else if (maxh != OLD_WIN_SIZE)
			max_h = maxh;
	}

	HWND getHWND() { return hwnd; }

	// static elements
	static Window* get_window(int id)
	{
		auto pointer = std::find_if(windows.begin(), windows.end(), [id](const std::pair<int, Window*>& a) {return id == a.first; });
		return pointer != windows.end() ? pointer->second : NULL;
	}

	static void close(int id)
	{
		Window* window = Window::get_window(id);
		if (window) SendMessage(window->hwnd, WM_CLOSE, 0, 0);
	}


	static void default_msg_proc() {
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	static void wait_msg_proc() {
		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
};

// init static elems
int Window::name_id = 0;
std::vector<std::pair<int, Window*>> Window::windows = std::vector<std::pair<int, Window*>>();



// ==============================  Window Components =================================================

// ================== Button =====================
#define DEF_BUTTON WS_TABSTOP | WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE

struct Button : Component_id
{
	std::wstring text;

	Button() = default;
	Button(const Button& other) { Component* comp = other.get();  init(comp->parent, other.text, comp->x, comp->y, comp->width, comp->height, comp->type, comp->style); }
	Button& operator= (const Button& other) { Component* comp = other.get();  init(comp->parent, other.text, comp->x, comp->y, comp->width, comp->height, comp->type, comp->style);}

	Button(
		HWND parent,
		const std::wstring& text,
		float x,
		float y,
		float width,
		float height,
		UINT type = DYNAMIC,
		UINT style = DEF_BUTTON
	)
	{
		init(parent, text, x, y, width, height, type, style);
	}

	void init(
		HWND parent,
		const std::wstring& text,
		float x,
		float y,
		float width = 0.1f,
		float height = 0.1f,
		UINT type = DYNAMIC,
		UINT style = DEF_BUTTON
		)
	{
		this->parent = parent;
		this->text = text;

		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;

		hwnd = CreateWindow(
			L"BUTTON",  // Predefined class; Unicode assumed 
			text.c_str(), // Button text 
			style,     // Styles 
			x * nWidth,         // x position 
			(1.0f - y - height) * nHeight,         // y position 
			width * nWidth,     // Button width
			height * nHeight,    // Button height
			parent,    // Parent window
			(HMENU)id, // menu.
			hInst,
			NULL);      // Pointer not needed.

		Component comp(id, x, (1.0f - y - height), width, height, type, style, hwnd, parent);
		components.add(parent, comp);
	}

};

// =============== Radio Button ======================
#define DEF_RADIO	BS_AUTORADIOBUTTON | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE

struct RadioButton : Component_id
{
	std::wstring text;

	RadioButton() = default;
	RadioButton(const RadioButton& other) { Component* comp = other.get();  init(comp->parent, other.text, comp->x, comp->y, comp->width, comp->height, comp->type, comp->style); }
	RadioButton& operator= (const RadioButton& other) { Component* comp = other.get();  init(comp->parent, other.text, comp->x, comp->y, comp->width, comp->height, comp->type, comp->style); }

	RadioButton(HWND parent, const std::wstring& text, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = DYNAMIC, UINT style = DEF_RADIO)
	{
		init(parent, text, x, y, width, height, type, style);
	}

	void init(HWND parent, const std::wstring& text, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = DYNAMIC, UINT style = DEF_RADIO)
	{
		this->parent = parent;
		this->text = text;

		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;

		hwnd = CreateWindow(L"Button", text.c_str(), style,
			x * nWidth, (1.0f - y - height) * nHeight, width * nWidth, height * nHeight, parent, (HMENU)id, hInst, NULL);

		Component comp(id, x, (1.0f - y - height), width, height, type, style, hwnd, parent);
		components.add(parent, comp);
	}

	int chosed()
	{
		return SendMessage(hwnd, BM_GETCHECK, 0, 0);
	}

};


// ================ Check Button =================
#define DEF_CHECK	BS_AUTOCHECKBOX | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE

struct CheckBox : Component_id
{
	std::wstring text;

	CheckBox() = default;
	CheckBox(const CheckBox& other) { Component* comp = other.get();  init(comp->parent, other.text, comp->x, comp->y, comp->width, comp->height, comp->type, comp->style); }
	CheckBox& operator= (const CheckBox& other) { Component* comp = other.get();  init(comp->parent, other.text, comp->x, comp->y, comp->width, comp->height, comp->type, comp->style); }

	CheckBox(HWND parent, const std::wstring& text, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = DYNAMIC, UINT style = DEF_CHECK)
	{
		init(parent, text, x, y, width, height, type, style);
	}

	void init(HWND parent, const std::wstring& text, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = DYNAMIC, UINT style = DEF_CHECK)
	{
		this->parent = parent;
		this->text = text;

		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;

		hwnd = CreateWindow(L"Button", text.c_str(), style,
			x* nWidth, (1.0f - y - height) * nHeight, width* nWidth, height* nHeight, parent, (HMENU)id, hInst, NULL);

		Component comp(id, x, (1.0f - y - height), width, height, type, style, hwnd, parent);
		components.add(parent, comp);
	}

	bool checked()
	{
		return SendMessage(hwnd, BM_GETCHECK, 0, 0) == BST_CHECKED;
	}

};

// =================== ComboBox ========================
#define DEF_COMBO	CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE

struct ComboBox : Component_id
{
	ComboBox() = default;
	ComboBox(const ComboBox& other) { Component* comp = other.get();  init(comp->parent, comp->x, comp->y, comp->width, comp->height, comp->type, comp->style); }
	ComboBox& operator= (const ComboBox& other) { Component* comp = other.get();  init(comp->parent, comp->x, comp->y, comp->width, comp->height, comp->type, comp->style); }

	ComboBox(HWND parent, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = DYNAMIC, UINT style = DEF_COMBO)
	{
		init(parent, x, y, width, height, type, style);
	}

	void init(HWND parent, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = DYNAMIC, UINT style = DEF_COMBO)
	{
		this->parent = parent;

		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;

		hwnd = CreateWindow(L"ComboBox", L"combo", style,
			x* nWidth, (1.0f - y - height) * nHeight, width* nWidth, height* nHeight, parent, (HMENU)id, hInst, NULL);

		Component comp(id, x, (1.0f - y - height), width, height, type, style, hwnd, parent);
		components.add(parent, comp);
	}

	void add(const std::wstring& element)
	{
		// Add string to combobox.
		SendMessage(hwnd, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)element.c_str());

		// Send the CB_SETCURSEL message to display an initial item 
		//  in the selection field  
		SendMessage(hwnd, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

	}

	void add(const std::vector<std::wstring>& elements)
	{
		// Add strings to combobox.
		for (auto& element : elements)
			SendMessage(hwnd, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)element.c_str());

		SendMessage(hwnd, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
	}

	void set_selected(int i)
	{
		ComboBox_SetCurSel(hwnd, i);
	}

	UINT choosed_index()
	{
		return SendMessage(hwnd, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
	}
	
	std::wstring choosed()
	{
		int ItemIndex = SendMessage(hwnd, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
		TCHAR  ListItem[256];
		(TCHAR)SendMessage(hwnd, (UINT)CB_GETLBTEXT, (WPARAM)ItemIndex, (LPARAM)ListItem);
		return std::wstring(ListItem);
	}

	void remove(int idx)
	{
		SendMessage(hwnd, CB_DELETESTRING, (WPARAM)0, (LPARAM)0);
	}

	void remove(const std::wstring& name)
	{
		int idx = SendMessage(hwnd, CB_FINDSTRING, (WPARAM)0, (LPARAM)0);
		SendMessage(hwnd, CB_DELETESTRING, (WPARAM)idx, (LPARAM)0);
	}

	void clear()
	{
		int count = SendMessage(hwnd, CB_GETCOUNT, (WPARAM)0, (LPARAM)0);
		for (int i = 0; i < count; i++)
			SendMessage(hwnd, CB_DELETESTRING, (WPARAM)0, (LPARAM)0);

		ComboBox_SetText(hwnd, L"");
	}

};


// ================ Label =================
#define DEF_LABEL (WS_CHILD | WS_VISIBLE  | SS_LEFT)

struct Label : Component_id
{
	std::wstring text;

	Label() = default;
	Label(const Label& other) { Component* comp = other.get();  init(comp->parent, other.text, comp->x, comp->y, comp->width, comp->height, comp->type, comp->style); }
	Label& operator= (const Label& other) { Component* comp = other.get();  init(comp->parent, other.text, comp->x, comp->y, comp->width, comp->height, comp->type, comp->style); }

	Label(HWND parent, const std::wstring& text, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = DYNAMIC, UINT style = DEF_LABEL)
	{
		init(parent, text, x, y, width, height, type, style);
	}

	void init(HWND parent, const std::wstring& text, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = DYNAMIC, UINT style = DEF_LABEL)
	{
		this->parent = parent;
		this->text = text;

		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;

		hwnd = CreateWindow(L"static", L"label", style,
		x* nWidth, (1.0f - y - height) * nHeight, width* nWidth, height* nHeight, parent, (HMENU)id, hInst, NULL);

		Component comp(id, x, (1.0f - y - height), width, height, type, style, hwnd, parent);
		components.add(parent, comp);
		SetWindowText(hwnd, text.c_str());
	}

	void set_text(const std::wstring& text)
	{
		SetWindowText(hwnd, text.c_str());
	}

};

// ========================= Text ================================
#define DEF_TEXT WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL

struct Text : Component_id
{
	int cap = 10;
	TCHAR* text = NULL;
	
	Text() = default;
	Text(const Text& other) { Component* comp = other.get();  init(comp->parent, comp->x, comp->y, comp->width, comp->height, comp->type, comp->style); }
	Text& operator= (const Text& other) { Component* comp = other.get();  init(comp->parent, comp->x, comp->y, comp->width, comp->height, comp->type, comp->style); }

	Text(HWND parent, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = DYNAMIC, UINT style = DEF_TEXT)
	{
		init(parent, x, y, width, height, type, style);
	}

	void init(HWND parent, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = DYNAMIC, UINT style = DEF_TEXT)
	{
		this->parent = parent;
		text = (TCHAR*)::operator new(cap * sizeof(TCHAR));

		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;

		hwnd = CreateWindow(L"edit", L"", style,
		x* nWidth, (1.0f - y - height) * nHeight, width* nWidth, height* nHeight, parent, (HMENU)id, hInst, NULL);
		
		Component comp(id, x, (1.0f - y - height), width, height, type, style, hwnd, parent);
		components.add(parent, comp);
	}

	TCHAR* get_text()
	{
		int nLength = GetWindowTextLength(hwnd);
		if (nLength > 0)
		{
			if (nLength > cap)
			{
				cap = nLength + 1;
				::operator delete(text);
				text = (TCHAR*)::operator new(cap * sizeof(TCHAR));
			}
			GetWindowText(hwnd, text, nLength + 1);
			return text;
		}
		return NULL;
	}

	void set_text(const TCHAR* text)
	{
		SetWindowText(hwnd, text);
	}

	void clear()
	{
		*text = '\0';
		SetWindowText(hwnd, NULL);
	}
		
	~Text() {  ::operator delete(text); }
};


// ========================= Table ================================ Not a table actually and very slow, because rows*cols windows's hwndls are used
#define TOTAL_SIZE 0
#define CELL_SIZE 1

struct Table : Component_id
{
	int cap_col = 0, cap_row = 0;
	int cols = 0, rows = 0;
	int size_type;
	std::vector<Label> col_labels;
	std::vector<Label> row_labels;
	std::vector<Text> table;

	Table() = default;
	Table(HWND paretn, int max_row, int max_col, float x, float y, float width, float height, int size_type = TOTAL_SIZE, int type = DYNAMIC)
	{
		init(paretn, max_row, max_col, x, y, width, height, size_type, type);
	}

	void init(HWND parent, int max_row, int max_col, float x, float y, float width, float height, int size_type = TOTAL_SIZE, int type = DYNAMIC)
	{
		this->cap_row = max_row;
		this->cap_col = max_col;
		this->size_type = size_type;
		this->parent = parent;

		UINT lStyle = WS_CHILD | WS_VISIBLE | SS_CENTER;
		UINT tStyle = WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER;

		col_labels.resize(max_col, Label(parent, L"", 0, 0, 0, 0, type, lStyle));
		row_labels.resize(max_row, Label(parent, L"", 0, 0, 0, 0, type, lStyle));
		table.resize(max_row * max_col, Text(parent, 0, 0, 0, 0, type, tStyle));

		for (int i = 0; i < max_col; i++)
			col_labels[i].hide();

		for (int i = 0; i < max_row; i++)
			row_labels[i].hide();

		for (int i = 0; i < max_col * max_row; i++)
			table[i].hide();

		// just indicator of this class
		this->hwnd = hwnd = CreateWindow(L"BUTTON", L"", 0, 0, 0, 0, 0, parent, NULL, hInst, NULL);

		Component comp(id, x, y, width, height, type, STATIC, hwnd, parent);
		components.add(parent, comp);
	}

	void create(const std::vector<std::wstring>& text_rows, const std::vector<std::wstring>& text_cols)
	{
		if (text_rows.size() > cap_row || text_cols.size() > cap_col) return;
		if (text_rows.size() == 0 && text_cols.size() == 0) return;
		
		Component* comp = get();

		// hide old elements
		for (int i = 0; i < cols; i++)
			col_labels[i].hide();

		for (int i = 0; i < rows; i++)
			row_labels[i].hide();

		for (int i = 0; i < cols * rows; i++)
			table[i].hide();

		// rename and move elements for new pos
		rows = text_rows.size();
		cols = text_cols.size();

		for (int i = 0; i < rows; i++)
			row_labels[i].set_text(text_rows[i]);

		for (int i = 0; i < cols; i++)
			col_labels[i].set_text(text_cols[i]);
		
		float cell_w, cell_h;
		switch (size_type)
		{
			case TOTAL_SIZE:
			{
				cell_w = comp->width / (1 + cols);
				cell_h = comp->height /(1 +  rows);
			}break;
			case CELL_SIZE:
			{
				cell_h = comp->height;
				cell_w = comp->width;
			}break;
		}

		for (int i = 0; i < cols; i++)
		{
			col_labels[i].move(comp->x + cell_w * i + cell_w, comp->y);
			col_labels[i].resize(cell_w, cell_h);
			col_labels[i].show();
		}
		for (int i = 0; i < rows; i++)
		{
			row_labels[i].move(comp->x, comp->y + cell_h * i + cell_h);
			row_labels[i].resize(cell_w, cell_h);
			row_labels[i].show();
		}

		rows = rows ? rows : 1;
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < cols; j++)
			{
				table[i * cols + j].move(comp->x + cell_w * j + cell_w, comp->y + cell_h * i + cell_h);
				table[i * cols + j].resize(cell_w, cell_h);
				table[i * cols + j].show();
			}
		}
	}

	Text* operator[] (int i)
	{
		return table.data() + i * cols;
	}

	std::vector<TCHAR*> get_data()
	{
		std::vector<TCHAR*> data;

		for (int i = 0; i < rows; i++)
			for (int j = 0; j < cols; j++)
				data.push_back(table[i * cols + j].get_text());

		return data;
	}

	void clear()
	{
		for (int i = 0; i < cap_row; i++)
			for (int j = 0; j < cap_col; j++)
				table[i * cap_col + j].clear();
	}

};


// ==================== ListView =========================
#define DEF_LISTVIEW	WS_CHILD | LVS_REPORT | WS_VISIBLE

struct ListView : Component_id
{
	int columns = 0;

	ListView() = default;
	ListView(HWND parent, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = DYNAMIC, UINT style = DEF_LISTVIEW)
	{
		init(parent, x, y, width, height, type, style);
	}

	void init(HWND parent, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = DYNAMIC, UINT style = DEF_LISTVIEW)
	{

		INITCOMMONCONTROLSEX icex;
		icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
		icex.dwICC = ICC_LISTVIEW_CLASSES;
		InitCommonControlsEx(&icex);


		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;

		hwnd = CreateWindow(WC_LISTVIEW, L"", style,
			x * nWidth, (1.0f - y - height) * nHeight, width * nWidth, height * nHeight,
			parent, (HMENU)id, hInst, NULL);

		ListView_SetExtendedListViewStyleEx(hwnd, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

		this->parent = parent;

		Component comp(id, x, (1.0f - y - height), width, height, type, style, hwnd, parent);
		components.add(parent, comp);
	}

	void add_columns(std::vector<std::wstring> columns)
	{
		Component* comp = get();

		this->columns = columns.size();
		LVCOLUMN lvc;

		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;

		int colum_size = nWidth * comp->width / columns.size();

		// Initialize the LVCOLUMN structure.
		// The mask specifies that the format, width, text,
		// and subitem members of the structure are valid.
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM | LVIF_COLFMT;

		// Add the columns.
		for (int iCol = 0; iCol < columns.size(); iCol++)
		{
			lvc.iSubItem = iCol;
			lvc.pszText = (LPWSTR)columns[iCol].c_str();
			lvc.cx = colum_size;               // Width of column in pixels.

			if (iCol < 2)
				lvc.fmt = LVCFMT_LEFT;  // Left-aligned column.
			else
				lvc.fmt = LVCFMT_RIGHT; // Right-aligned column.

			// Insert the columns into the list view.
			if (ListView_InsertColumn(hwnd, iCol, &lvc) == -1)
				doutput("listview insert problem");
		}
	}

	void add_row(std::vector<std::wstring> row)
	{
		int textMaxLen = 10;

		int iLastIndex = ListView_GetItemCount(hwnd);

		LVITEM lvi;
		lvi.mask = LVIF_TEXT;
		lvi.cchTextMax = textMaxLen;
		lvi.iItem = iLastIndex;
		lvi.pszText = (LPWSTR)row[0].c_str();
		lvi.iSubItem = 0;

		if (ListView_InsertItem(hwnd, &lvi) != -1)
			for (int i = 1; i < row.size(); i++)
				ListView_SetItemText(hwnd, iLastIndex, i, (LPWSTR)row[i].c_str());
		
			// align the table
		ListView_SetColumnWidth(hwnd, 1, LVSCW_AUTOSIZE);
	}

	void add_rows(std::vector<std::vector<std::wstring>> rows)
	{
		int textMaxLen = 10;

		for (int i = 0; i < rows.size(); i++)
		{
			int iLastIndex = ListView_GetItemCount(hwnd);

			LVITEM lvi;
			lvi.mask = LVIF_TEXT;
			lvi.cchTextMax = textMaxLen;
			lvi.iItem = iLastIndex;
			lvi.pszText = (LPWSTR)rows[i][0].c_str();
			lvi.iSubItem = 0;

			if (ListView_InsertItem(hwnd, &lvi) != -1)
				for (int j = 1; j < rows[i].size(); j++)
					ListView_SetItemText(hwnd, iLastIndex, j, (LPWSTR)rows[i][j].c_str());

			// align the table
			ListView_SetColumnWidth(hwnd, 1, LVSCW_AUTOSIZE);
		}
	}

	void space(int i = 0)
	{
		wchar_t buffer[] = L"";
		
		for (int j = 1; j < i; j++)
		{
			int iLastIndex = ListView_GetItemCount(hwnd);

			LVITEM lvi;
			lvi.mask = LVIF_TEXT;
			lvi.cchTextMax = 10;
			lvi.iItem = iLastIndex;
			lvi.pszText = (LPWSTR)buffer;
			lvi.iSubItem = 0;

			if (ListView_InsertItem(hwnd, &lvi) != -1)
				ListView_SetItemText(hwnd, iLastIndex, j, (LPWSTR)buffer);
		}

	}

	void clear()
	{
		ListView_DeleteAllItems(hwnd);
	}

	void remove_row(int i)
	{
		ListView_DeleteItem(hwnd, i);
	}

	int rows()
	{
		return ListView_GetItemCount(hwnd);
	}
};


