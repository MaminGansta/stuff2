#pragma once

#pragma comment( lib, "comctl32.lib" )

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


#define safe_release(ptr) (delete ptr, ptr = nullptr)
#define safe_releaseArr(ptr) (delete[] ptr, ptr = nullptr)

#ifndef PI
#define PI 3.14159265359f
#endif

#include <Windows.h>
#include <CommCtrl.h>
#include <windowsx.h>

#include <cstdint>
#include <cstdio>
#include <wchar.h>
#include <tchar.h>

#include <array>
#include <vector>
#include <algorithm>
#include <functional>


void doutput(const char* format, ...)
{
#if _DEBUG
	char log[128];
	va_list args;
	va_start(args, format);
	vsprintf_s(log, format, args);
	OutputDebugStringA(log);
	va_end(args);
#endif
}

void doutput(const wchar_t* format, ...)
{
#if _DEBUG
	wchar_t log[128];
	va_list args;
	va_start(args, format);
	vswprintf_s(log, format, args);
	OutputDebugStringW(log);
	va_end(args);
#endif
}


void output(const char* format, ...)
{
	char log[128];
	va_list args;
	va_start(args, format);
	vsprintf_s(log, format, args);
	OutputDebugStringA(log);
	va_end(args);
}

void output(const wchar_t* format, ...)
{
	wchar_t log[128];
	va_list args;
	va_start(args, format);
	vswprintf_s(log, format, args);
	OutputDebugStringW(log);
	va_end(args);
}




#undef small
#include "libs/smallLib.cpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_WINDOWS_UTF8
#include "libs/stb_image.h"

// globals
HINSTANCE hInst;

#ifndef MAX_THREADS
#define MAX_THREADS 2
#endif

// unity build
#include "libs/thread_pool.cpp"
thread_pool workers(MAX_THREADS);

// gui layout
#include "image/canvas.cpp"
#include "win_api/window.cpp"
#include "image/image.cpp"
#include "image/draw.cpp"
#include "input/input.cpp"
#include "libs/timer.cpp"
#include "libs/time.cpp"
#include "input/io.cpp"

// image tarnsformation
#include "operators/color/histogram.cpp"
#include "operators/image_conversion.cpp"
#include "operators/median_filter.cpp"
#include "operators/color/auto_contrast.cpp"
#include "operators/color/gray_world.cpp"
#include "operators/color/histogram_alignment.cpp"
#include "operators/convolutions/convolution.cpp"

// high_lavel_gui elements
#include "high_lavel_gui/image_window.cpp"

void al_init(HINSTANCE hInstance)
{
	hInst = hInstance;
	init_time = high_resolution_clock::now();
}
