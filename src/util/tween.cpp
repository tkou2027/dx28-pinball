#include "tween.h"
#include "math/math.h"

namespace
{
	float CalcEaseOutBack(float t)
	{
		constexpr float c4 = (2 * Math::PI) / 3;

		//return t == 0
		//	? 0
		//	: t == 1
		//	? 1
		//	: powf(2, -10 * t) * sinf((t * 10 - 0.75) * c4) + 1;

		// easeOutBack
		constexpr float c1 = 1.70158;
		constexpr float c3 = c1 + 1;

		return 1 + c3 * pow(t - 1, 3) + c1 * pow(t - 1, 2);

		//constexpr float n1{ 7.5625f };
		//constexpr float d1{ 2.75f };

		//if (t < 1.0f / d1) {
		//	return n1 * t * t;
		//}
		//else if (t < 2.0f / d1) {
		//	return n1 * (t -= 1.5 / d1) * t + 0.75f;
		//}
		//else if (t < 2.5f / d1) {
		//	return n1 * (t -= 2.25f / d1) * t + 0.9375f;
		//}
		//else {
		//	return n1 * (t -= 2.625f / d1) * t + 0.984375f;
		//}
	}

	float CalcEaseInOutQuad(float t)
	{
		// easeInOutQuad
		return t < 0.5 ? 2 * t * t : 1 - powf(-2 * t + 2, 2) / 2;

		//constexpr float c1 = 1.70158;
		//constexpr float c2 = c1 * 1.525;

		//return t < 0.5
		//	? (powf(2 * t, 2) * ((c2 + 1) * 2 * t - c2)) / 2
		//	: (powf(2 * t - 2, 2) * ((c2 + 1) * (t * 2 - 2) + c2) + 2) / 2;
	}
}

float Tween::EaseFunc(TweenFunction function, float t)
{
	switch (function)
	{
	case TweenFunction::EASE_IN_QUAD:
	{
		return t * t;
	}
	case TweenFunction::EASE_OUT_QUAD:
	{
		return 1 - (1 - t) * (1 - t);
	}
	case TweenFunction::EASE_OUT_BACK:
	{
		return CalcEaseOutBack(t);
	}
	case TweenFunction::EASE_IN_OUT_QUAD:
	{
		return CalcEaseInOutQuad(t);
	}
	default:
	{
		return t;
	}
	}
}