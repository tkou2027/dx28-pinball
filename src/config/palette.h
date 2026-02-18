#pragma once
#include "math/vector4.h"

struct Palette
{
	Vector4 color_theme{};
	Vector4 color_theme_accent{};
	Vector4 color_player{};
	Vector4 color_danger{};
};

extern Palette g_palette;