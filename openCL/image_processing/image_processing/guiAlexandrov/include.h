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

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <wchar.h>
#include <tchar.h>
#include <cmath>
#include <malloc.h>


// some output
#include "libs/log.cpp"

#ifdef SMALL_LIB
#undef small
#include "libs/smallLib.cpp"
#endif


#define STB_IMAGE_IMPLEMENTATION
#define STBI_WINDOWS_UTF8
#include "libs/stb_image.h"


// globals
HINSTANCE hInst;

#ifndef MAX_THREADS
#define MAX_THREADS 8
#endif

// unity build

// thread pool
#include "libs/thread_pool.cpp"
thread_pool workers(MAX_THREADS);

// gui layout
#include "image/color.cpp"
#include "image/canvas.cpp"
#include "image/image.cpp"
#include "image/draw.cpp"
#include "image/text.cpp"

#include "input/input.cpp"
#include "input/io.cpp"
#include "libs/time.cpp"
#include "win_api/window.cpp"

// image tarnsformations
#ifdef IMAGE_PROCESSING
#include "operators/color/histogram.cpp"
#include "operators/image_conversion.cpp"
#include "operators/median_filter.cpp"
#include "operators/color/auto_contrast.cpp"
#include "operators/color/gray_world.cpp"
#include "operators/color/histogram_alignment.cpp"
#include "operators/convolutions/convolution.cpp"
#endif


// high_lavel_gui elements
#include "high_lavel_gui/image_window.cpp"
#include "high_lavel_gui/file_explorer_window.cpp"


void al_init(HINSTANCE hInstance)
{
	hInst = hInstance;
	init_time = high_resolution_clock::now();
}
