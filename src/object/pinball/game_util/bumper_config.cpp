#include "bumper_config.h"

namespace
{
	// bumper presets
	BumperPreset bumper_preset_tri_bumper
	{
		3,      // sides
		5.0f,   // radius
		4.0f    // height
	};
	BumperPreset bumper_preset_quad_bumper
	{
		4,      // sides
		5.0f,   // radius
		5.0f    // height
	};
}

const BumperPreset g_bumper_presets[static_cast<size_t>(BumperType::MAX)]
{
	bumper_preset_tri_bumper,
	bumper_preset_quad_bumper
};
