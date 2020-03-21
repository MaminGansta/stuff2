
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
void components_remove (HWND parent, HWND handle);

#define STATIC 0
#define DYNAMIC 1
#define RESIZABLE 2

struct Component
{
	int id;
	float x, y, width, height;
	UINT type;
	UINT style;
	HWND handle;
	HWND parent;

	Component() = default;
	~Component() { components_remove(parent, handle); DestroyWindow(handle); }

	void resize()
	{
		if (type == STATIC)
			return;

		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;

		UINT flags = SWP_NOZORDER;

		if (type == DYNAMIC)
			flags = SWP_NOSIZE;

		SetWindowPos(handle, 0, x * nWidth, y * nHeight, width * nWidth, height * nHeight, flags);
	}

	void resize(float width, float height)
	{
		this->height = height;
		this->width = width;

		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;
		
		SetWindowPos(handle, 0, 0, 0, width * nWidth, height * nHeight, SWP_NOZORDER | SWP_NOMOVE);
	}

	void move(float x, float y)
	{
		this->x = x;
		this->y = y;

		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;

		SetWindowPos(handle, 0, x * nWidth, y * nHeight, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}


	void hide() { ShowWindow(handle, SW_HIDE); }
	void show() { ShowWindow(handle, SW_SHOWNOACTIVATE); }
};


struct Component_crt
{
	std::unordered_map<HWND, std::vector<Component*>> components;

	void add(HWND parent, Component* comp)
	{
		components[parent].push_back(comp);
	}

	void remove(HWND parent)
	{
		components.erase(parent);
	}

	void update(HWND parent)
	{
		for (auto& component : components[parent])
			component->resize();
	}

	void redraw(HWND parent)
	{
		for (auto& component : components[parent])
			RedrawWindow(component->handle, 0, 0, RDW_INVALIDATE | RDW_NOCHILDREN);
	}

	auto& operator[] (HWND hwnd)
	{
		return components[hwnd];
	}

};
Component_crt components;


// connect component distructor and comp_Control
void components_remove(HWND parent, HWND handle)
{
	auto pointer = std::find_if(components[parent].begin(), components[parent].end(), [handle](Component* comp) { return comp->handle == handle; });
	if (pointer == components[parent].end()) return;
	components[parent].erase(pointer);
}


// =============================== HWND HANDLER ======================================
struct HWND_constainer
{
	int gen_id = 0;
	std::vector<std::pair<int, HWND>> handles;

	int add(HWND handle)
	{
		handles.push_back(std::make_pair(gen_id, handle));
		return gen_id++;
	}

	void remove(int id)
	{
		auto handle = std::find_if(handles.begin(), handles.end(), [id](std::pair<int, HWND> in) {return in.first == id; });
		arguments.remove(handle->second);
		DestroyWindow(handle->second);
		handles.erase(handle);
	}

	HWND operator [](int id)
	{
		auto res = std::find_if(handles.begin(), handles.end(), [id](std::pair<int, HWND> in) {return in.first == id; });

		if (res == handles.end())
			return NULL;

		return res->second;
	}

	int size() { return handles.size(); };
};

HWND_constainer handles;



// ========================================= WINDOW ========================================================
#define DEF_WINDOW (WS_OVERLAPPEDWINDOW | WS_VISIBLE)

struct Window
{	
	int class_id;
	static int name_id;
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
		HWND parent = NULL,
		void* arg_ptr = NULL,
		int id = 0
	)
	{
		init(window_name, width, height, callback, style, parent, arg_ptr, id);
	}

	void init(
		const std::wstring& window_name,
		int width,
		int height,
		std::function<LRESULT(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)> callback,
		UINT style = DEF_WINDOW,
		HWND parent = NULL,
		void* arg_ptr = NULL,
		int id = 0
	)
	{
		wchar_t class_name[16];
		swprintf_s(class_name, L"class_%d", name_id++);
		std::wstring name(class_name);

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
		wc.lpszClassName = name.c_str();
		//  wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
		wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);


		// global callback function
		wc.lpfnWndProc = [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)->LRESULT 
		{
			Args args = arguments.get(hwnd);
			Window* window = (Window*)args[0];
			if (window == NULL) return DefWindowProc(hwnd, msg, wParam, lParam);

			switch (msg)
			{
				case WM_SIZE:
				{
					window->canvas.resize(hwnd);
					components.update(hwnd);
				}break;
				case WM_PAINT:
				{
					components.redraw(hwnd);
				}break;
				case  WM_GETMINMAXINFO:
				{
					LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
					lpMMI->ptMinTrackSize.x = window->min_w;
					lpMMI->ptMinTrackSize.y = window->min_h;
				
					lpMMI->ptMaxTrackSize.x = window->max_w;
					lpMMI->ptMaxTrackSize.y = window->max_h;
				}break;
			}
			
			// user's callback
			return args.callback(hwnd, msg, wParam, lParam, args);
		};


		if (!RegisterClassEx(&wc))
		{
			MessageBox(NULL, L"Cannot register class", L"Error", MB_OK);
			assert(false);
		}

		HWND handle = CreateWindow(wc.lpszClassName, window_name.c_str(), style, CW_USEDEFAULT, CW_USEDEFAULT, width, height, parent, (HMENU)id, (HINSTANCE)hInst, NULL);

		// set window def ssize
		set_min_max_size(0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));

		// put window ptr to global holder
		arguments.add(handle, this, arg_ptr, callback);

		// put handle to the global holder
		class_id = handles.add(handle);

		// repeat messages
		SendMessage(handle, WM_CREATE, 0, 0);
		SendMessage(handle, WM_SIZE, 0, 0);
		SendMessage(handle, WM_PAINT, 0, 0);

		hdc = GetDC(handle);
	}

	virtual ~Window() { components.remove(getHWND()); handles.remove(class_id); }

	void render_canvas()
	{
		StretchDIBits(hdc, 0, 0, canvas.width, canvas.height, 0, 0, canvas.width, canvas.height, canvas.memory, &canvas.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
	}

	void redraw()
	{
		//RedrawWindow(getHWND(), 0, 0, RDW_INVALIDATE | RDW_ALLCHILDREN);
		SendMessage(getHWND(), WM_PAINT, 0, 0);
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

	HWND getHWND() { return handles[class_id]; }

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

int Window::name_id = 0;



// ==============================  Window Components =================================================

// ================== Button =====================
#define DEF_BUTTON WS_TABSTOP | WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE

struct Button : Component
{
	std::wstring text;

	Button() = default;
	Button(const Button& btn) { init(btn.parent, btn.text, btn.id, btn.x, btn.y, btn.width, btn.height, btn.type, btn.style);  }
	Button& operator= (const Button& btn) { init(btn.parent, btn.text, btn.id, btn.x, btn.y, btn.width, btn.height, btn.type, btn.style); return *this; }

	Button(
		HWND parent,
		const std::wstring& text,
		int id,
		float x,
		float y,
		float width,
		float height,
		UINT type = DYNAMIC,
		UINT style = DEF_BUTTON
	)
	{
		init(parent, text, id, x, y, width, height, type, style);
	}

	void init(
		HWND parent,
		const std::wstring& text,
		int id,
		float x,
		float y,
		float width = 0.1f,
		float height = 0.1f,
		UINT type = DYNAMIC,
		UINT style = DEF_BUTTON
		)
	{
		this->id = id;
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		this->parent = parent;
		this->type = type;
		this->style = style;
		this->text = text;

		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;

		handle = CreateWindow(
			L"BUTTON",  // Predefined class; Unicode assumed 
			text.c_str(), // Button text 
			style,     // Styles 
			x * nWidth,         // x position 
			y * nHeight,         // y position 
			width * nWidth,     // Button width
			height * nHeight,    // Button height
			parent,    // Parent window
			(HMENU)id, // menu.
			hInst,
			NULL);      // Pointer not needed.

		components.add(parent, this);
	}

};

// =============== Radio Button ======================
#define DEF_RADIO	BS_AUTORADIOBUTTON | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE

struct RadioButton : Component
{
	std::wstring text;

	RadioButton() = default;
	RadioButton(const RadioButton& rBtn) { init(rBtn.parent, rBtn.text, rBtn.id, rBtn.x, rBtn.y, rBtn.width, rBtn.height, rBtn.type, rBtn.style); }
	RadioButton& operator= (const RadioButton& rBtn) { init(rBtn.parent, rBtn.text, rBtn.id, rBtn.x, rBtn.y, rBtn.width, rBtn.height, rBtn.type, rBtn.style); return *this; }

	RadioButton(HWND parent, const std::wstring& text, int id, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = DYNAMIC, UINT style = DEF_RADIO)
	{
		init(parent, text, id, x, y, width, height, type, style);
	}

	void init(HWND parent, const std::wstring& text, int id, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = DYNAMIC, UINT style = DEF_RADIO)
	{
		this->id = id;
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		this->parent = parent;
		this->type = type;
		this->style = style;
		this->text = text;


		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;

		handle = CreateWindow(L"Button", text.c_str(), style,
			x * nWidth, y * nHeight, width * nWidth, height * nHeight, parent, (HMENU)id, hInst, NULL);
	
		components.add(parent, this);
	}

};


// ================ Check Button =================
#define DEF_CHECK	BS_AUTOCHECKBOX | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE

struct CheckBox : Component
{
	std::wstring text;

	CheckBox() = default;
	CheckBox(const CheckBox& check) { init(check.parent, check.text, check.id, check.x, check.y, check.width, check.height, check.type, check.style); }
	CheckBox& operator= (const CheckBox& check) { init(check.parent, check.text, check.id, check.x, check.y, check.width, check.height, check.type, check.style); return *this; }

	CheckBox(HWND parent, const std::wstring& text, int id, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = DYNAMIC, UINT style = DEF_CHECK)
	{
		init(parent, text, id, x, y, width, height, type, style);
	}

	void init(HWND parent, const std::wstring& text, int id, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = DYNAMIC, UINT style = DEF_CHECK)
	{
		this->id = id;
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		this->parent = parent;
		this->type = type;
		this->text = text;

		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;

		handle = CreateWindow(L"Button", text.c_str(), style,
			x* nWidth, y* nHeight, width* nWidth, height* nHeight, parent, (HMENU)id, hInst, NULL);
	
		components.add(parent, this);
	}

};

// =================== ComboBox ========================
#define DEF_COMBO	CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE

struct ComboBox : Component
{
	ComboBox() = default;
	ComboBox(const ComboBox& combo) { init(combo.parent, combo.id, combo.x, combo.y, combo.width, combo.height, combo.type, combo.style); }
	ComboBox& operator= (const ComboBox& combo) { init(combo.parent, combo.id, combo.x, combo.y, combo.width, combo.height, combo.type, combo.style); return *this; }

	ComboBox(HWND parent, int id, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = DYNAMIC, UINT style = DEF_COMBO)
	{
		init(parent, id, x, y, width, height, type, style);
	}

	void init(HWND parent, int id, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = DYNAMIC, UINT style = DEF_COMBO)
	{
		this->id = id;
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		this->parent = parent;
		this->type = type;
		this->style = style;

		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;

		handle = CreateWindow(L"ComboBox", L"combo", style,
			x* nWidth, y* nHeight, width* nWidth, height* nHeight, parent, (HMENU)id, hInst, NULL);

		components.add(parent, this);
	}

	void add(const std::wstring& element)
	{
		// Add string to combobox.
		SendMessage(handle, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)element.c_str());

		// Send the CB_SETCURSEL message to display an initial item 
		//  in the selection field  
		SendMessage(handle, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

	}

	void add(std::vector<std::wstring> elements)
	{
		// Add strings to combobox.
		for (auto& element : elements)
			SendMessage(handle, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)element.c_str());

		SendMessage(handle, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

	}

	UINT choosed_index()
	{
		return SendMessage(handle, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
	}
	
	std::wstring choosed()
	{
		int ItemIndex = SendMessage(handle, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
		TCHAR  ListItem[256];
		(TCHAR)SendMessage(handle, (UINT)CB_GETLBTEXT, (WPARAM)ItemIndex, (LPARAM)ListItem);
		return std::wstring(ListItem);
	}

	void remove(int idx)
	{
		SendMessage(handle, CB_DELETESTRING, (WPARAM)0, (LPARAM)0);
	}

	void remove(const std::wstring& name)
	{
		int idx = SendMessage(handle, CB_FINDSTRING, (WPARAM)0, (LPARAM)0);
		SendMessage(handle, CB_DELETESTRING, (WPARAM)idx, (LPARAM)0);
	}

	void clear()
	{
		int count = SendMessage(handle, CB_GETCOUNT, (WPARAM)0, (LPARAM)0);
		for (int i = 0; i < count; i++)
			SendMessage(handle, CB_DELETESTRING, (WPARAM)0, (LPARAM)0);
	}

};


// ================ Label =================
#define DEF_LABEL (WS_CHILD | WS_VISIBLE  | SS_LEFT)

struct Label : Component
{
	std::wstring text;

	Label() = default;
	Label(const Label& lable) { init(lable.parent, lable.text, lable.id, lable.x, lable.y, lable.width, lable.height, lable.type, lable.style); }
	Label(Label&& label) {
		parent = label.parent;
		handle = label.handle;
		text = label.text;
		id = label.id;
		x = label.x;
		y = label.y;
		width = label.width;
		height = label.height;
		type = label.type;
		style = label.style;
		label.handle = 0;
		components.add(parent, this);
	}

	Label& operator= (const Label& lable) { init(lable.parent, lable.text, lable.id, lable.x, lable.y, lable.width, lable.height, lable.type, lable.style); return *this; }

	Label(HWND parent, const std::wstring& text, int id, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = DYNAMIC, UINT style = DEF_LABEL)
	{
		init(parent, text, id, x, y, width, height, type, style);
	}

	void init(HWND parent, const std::wstring& text, int id, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = DYNAMIC, UINT style = DEF_LABEL)
	{
		this->id = id;
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		this->parent = parent;
		this->type = type;
		this->style = style;
		this->text = text;

		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;

		handle = CreateWindow(L"static", L"label", style,
		x* nWidth, y* nHeight, width* nWidth, height* nHeight, parent, (HMENU)id, hInst, NULL);

		components.add(parent, this);
		SetWindowText(handle, text.c_str());
	}

	void set_text(const std::wstring& text)
	{
		SetWindowText(handle, text.c_str());
	}

};

// ========================= Text ================================
#define DEF_EDIT  WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL

struct Text : Component
{
	int cap = 10;
	TCHAR* text = NULL;
	
	Text() = default;
	Text(const Text& text) { init(text.parent, text.id, text.x, text.y, text.width, text.height, text.type, text.style); }
	Text(Text&& text) {
		parent = text.parent;
		handle = text.handle;

		id = text.id;
		x = text.x;
		y = text.y;
		width = text.width;
		height = text.height;
		type = text.type;
		style = text.style;
		text.handle = 0;
		components.add(parent, this);
	}

	Text& operator= (const Text& text) { init(text.parent, text.id, text.x, text.y, text.width, text.height, text.type, text.style); return *this; }

	Text(HWND parent, int id, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = DYNAMIC, UINT style = DEF_EDIT)
	{
		init(parent, id, x, y, width, height, type, style);
	}

	void init(HWND parent, int id, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = DYNAMIC, UINT style = DEF_EDIT)
	{
		text = (TCHAR*)::operator new(cap * sizeof(TCHAR));

		this->id = id;
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		this->parent = parent;
		this->type = type;
		this->style = style;

		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;

		handle = CreateWindow(L"edit", L"", style,
		x* nWidth, y* nHeight, width* nWidth, height* nHeight, parent, (HMENU)id, hInst, NULL);

		components.add(parent, this);
	}

	TCHAR* getText()
	{
		int nLength = GetWindowTextLength(handle);
		if (nLength > 0)
		{
			if (nLength > cap)
			{
				cap = nLength + 1;
				::operator delete(text);
				text = (TCHAR*)::operator new(cap * sizeof(TCHAR));
			}
			GetWindowText(handle, text, nLength + 1);
			return text;
		}
		return NULL;
	}

	void SetText(TCHAR* text)
	{
		SetWindowText(handle, text);
	}

	void clear()
	{
		*text = '\0';
		SetWindowText(handle, NULL);
	}
		
	~Text() {  ::operator delete(text); }
};


// ========================= Table ================================ Not a table actually and very slow, because rows*cols windows's handlels are used
#define TOTAL_SIZE 0
#define CELL_SIZE 1

struct Table : Component
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
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		this->cap_row = max_row;
		this->cap_col = max_col;
		this->size_type = size_type;
		this->parent = parent;
		this->type = type;

		UINT lStyle = WS_CHILD | WS_VISIBLE | SS_CENTER;
		UINT tStyle = WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER;

		col_labels.resize(max_col, Label(parent, L"", 0, 0, 0, 0, 0, DYNAMIC, lStyle));
		row_labels.resize(max_row, Label(parent, L"", 0, 0, 0, 0, 0, DYNAMIC, lStyle));
		table.resize(max_row * max_col, Text(parent, 0, 0, 0, 0, 0, DYNAMIC, tStyle));

		for (int i = 0; i < max_col; i++)
			col_labels[i].hide();

		for (int i = 0; i < max_row; i++)
			row_labels[i].hide();

		for (int i = 0; i < max_col * max_row; i++)
			table[i].hide();

		// just indicator of this class
		this->handle = handle = CreateWindow(L"BUTTON", L"", 0, 0, 0, 0, 0, parent, NULL, hInst, NULL);

		components.add(parent, this);
	}

	void create(const std::vector<std::wstring>& text_rows, const std::vector<std::wstring>& text_cols)
	{
		if (text_rows.size() > cap_row || text_cols.size() > cap_col) return;
		if (text_rows.size() == 0 && text_cols.size() == 0) return;
		
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
				cell_w = width / (1 + cols);
				cell_h = height /(1 +  rows);
			}break;
			case CELL_SIZE:
			{
				cell_w = width;
				cell_h = height;
			}break;
		}

		for (int i = 0; i < cols; i++)
		{
			col_labels[i].move(x + cell_w * i + cell_w, y);
			col_labels[i].resize(cell_w, cell_h);
			col_labels[i].show();
		}
		for (int i = 0; i < rows; i++)
		{
			row_labels[i].move(x, y + cell_h * i + cell_h);
			row_labels[i].resize(cell_w, cell_h);
			row_labels[i].show();
		}

		rows = rows ? rows : 1;
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < cols; j++)
			{
				table[i * cols + j].move(x + cell_w * j + cell_w, y + cell_h * i + cell_h);
				table[i * cols + j].resize(cell_w, cell_h);
				table[i * cols + j].show();
			}
		}
	}

	std::vector<TCHAR*> get_data()
	{
		std::vector<TCHAR*> data;

		for (int i = 0; i < rows; i++)
			for (int j = 0; j < cols; j++)
				data.push_back(table[i * cols + j].getText());

		return data;
	}

	void clear()
	{
		for (int i = 0; i < cap_row; i++)
			for (int j = 0; j < cap_col; j++)
				table[i * cap_col + j].clear();
	}

};


// ============== Some functions ======================

#define wtoi(str) ( str ? _wtoi(str) : 0)
#define wtof(str) ( str ? _wtof(str) : 0.0f)


void set_font_size(HWND handle, int size)
{
	HFONT hFont = CreateFont(size, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");
	SendMessage(handle, WM_SETFONT, WPARAM(hFont), FALSE);
}


void set_font(HWND handle, HFONT font)
{
	SendMessage(handle, WM_SETFONT, WPARAM(font), FALSE);
}

