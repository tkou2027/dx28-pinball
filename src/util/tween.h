#pragma once

class Tween
{
public:
	enum class TweenFunction
	{
		LINEAR,
		EASE_IN_QUAD,
		EASE_OUT_QUAD,
		EASE_OUT_BACK,
		EASE_IN_OUT_QUAD
	};
	static float EaseFunc(TweenFunction function, float t);
};