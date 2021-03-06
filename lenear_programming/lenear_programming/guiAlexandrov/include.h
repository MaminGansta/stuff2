#pragma once

#pragma comment( lib, "comctl32.lib" )

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


#define safe_release(ptr) (delete ptr, ptr = nullptr)
#define safe_releaseArr(ptr) (delete[] ptr, ptr = nullptr)

#include <Windows.h>
#include <CommCtrl.h>
#include <windowsx.h>

#include <cstdint>
#include <cstdio>
#include <wchar.h>
#include <tchar.h>

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <functional>

#ifdef small
#undef small
#endif


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


#include "smallLib.cpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_WINDOWS_UTF8
#include "stb_image.h"

// globals
HINSTANCE hInst;

#ifndef MAX_THREADS
#define MAX_THREADS 2
#endif

// unity build
#include "thread_pool.cpp"
thread_pool workers(MAX_THREADS);

// gui layout
#include "canvas.cpp"
#include "window.cpp"
#include "image.cpp"
#include "draw.cpp"
#include "input.cpp"
#include "timer.cpp"
#include "time.cpp"
#include "io.cpp"



void al_init(HINSTANCE hInstance)
{
	hInst = hInstance;
	init_time = high_resolution_clock::now();
}
