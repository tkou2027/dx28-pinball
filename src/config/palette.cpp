#include "palette.h"

namespace
{
	// CIE SRGB to Linear conversion with help of AI
    float SRGBToLinear(float c)
    {
        return (c <= 0.04045f)
            ? c / 12.92f
            : powf((c + 0.055f) / 1.055f, 2.4f);
    }

    Vector4 SRGBToLinear(const Vector4& c)
    {
        return {
            SRGBToLinear(c.x),
            SRGBToLinear(c.y),
            SRGBToLinear(c.z),
            c.w
        };
    }
}

Palette g_palette{
    // Vector4{  1.0f, 0.07059f, 0.30980f, 1.0f }, // theme #FF124F
    // Vector4{ 0.8595f, 0.0980f, 0.3085f, 1.0f },
    SRGBToLinear(Vector4{  1.0f, 0.07059f, 0.30980f, 1.0f }),
    SRGBToLinear(Vector4{  1.0f, 0.07059f, 0.30980f, 1.0f }), // accent
	SRGBToLinear(Vector4{ 0.81961f, 0.62745f, 0.71373f, 1.0f }), // player
	Vector4{ 1.0f, 0.0f, 0.0f, 1.0f } // danger
};
