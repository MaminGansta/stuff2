

void doutput(const char* format)
{
#if _DEBUG
	OutputDebugStringA(format);
#endif
}

template <size_t buffer_size = 128>
void doutput(const char* format, ...)
{
#if _DEBUG
	char log[buffer_size];
	va_list args;
	va_start(args, format);
	vsprintf_s(log, format, args);
	OutputDebugStringA(log);
	va_end(args);
#endif
}

template <size_t buffer_size = 128>
void doutput(const wchar_t* format, ...)
{
#if _DEBUG
	wchar_t log[buffer_size];
	va_list args;
	va_start(args, format);
	vswprintf_s(log, format, args);
	OutputDebugStringW(log);
	va_end(args);
#endif
}





void output(const char* format)
{
	OutputDebugStringA(format);
}

template <size_t buffer_size = 128>
void output(const char* format, ...)
{
	char log[buffer_size];
	va_list args;
	va_start(args, format);
	vsprintf_s(log, format, args);
	OutputDebugStringA(log);
	va_end(args);
}

template <size_t buffer_size = 128>
void output(const wchar_t* format, ...)
{
	wchar_t log[buffer_size];
	va_list args;
	va_start(args, format);
	vswprintf_s(log, format, args);
	OutputDebugStringW(log);
	va_end(args);
}

