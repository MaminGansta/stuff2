
#include "color.h"


template <>
float chanel_clip<float>(float color) { return __min(__max(color, 0.0f), 1.0f); }

template <>
int chanel_clip<int>(int color) { return __min(__max(color, 0), 255); }

template <>
uint8_t chanel_clip<uint8_t>(uint8_t color) { return __min(__max(color, 0), 255); }


namespace color
{
	template <>
	float max_val<float>() { return 1.0f; }

	template <>
	uint8_t max_val<uint8_t>() { return 255; }
}

template struct Color_base<uint8_t>;
template struct Color_base<float>;
