#pragma once

#include "Vector2.hpp"

v2 Lerp(const v2 &a, const v2 &b, const float t)
{
	return a + (b - a) * t;
}