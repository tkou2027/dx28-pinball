#pragma once
#include <cstdint>

// defines if object(material) will be rendered by camera
enum class CameraRenderLayer : uint8_t
{
	DEFAULT = 0,
	REFLECTED = 1,
	MONITOR = 2,
	SCREEN_MAIN = 3,
	SCREEN_STATUS = 4,

	MAX
};

class CameraRenderLayerMask
{
public:
	static constexpr uint32_t DEFAULT{ 1 << static_cast<uint8_t>(CameraRenderLayer::DEFAULT) };
	static constexpr uint32_t REFLECTED{ 1 << static_cast<uint8_t>(CameraRenderLayer::REFLECTED) };
	static constexpr uint32_t MONITOR{ 1 << static_cast<uint8_t>(CameraRenderLayer::MONITOR) };
	static constexpr uint32_t SCREEN_MAIN{ 1 << static_cast<uint8_t>(CameraRenderLayer::SCREEN_MAIN) };
	static constexpr uint32_t SCREEN_STATUS{ 1 << static_cast<uint8_t>(CameraRenderLayer::SCREEN_STATUS) };

	static constexpr uint32_t NONE{ 0u };
	static constexpr uint32_t ALL{ 0xFFFFFFFFu };

	static uint32_t GetLayerMask(CameraRenderLayer layer)
	{
		return 1 << static_cast<uint8_t>(layer);
	}

	static bool IfLayerInMask(uint32_t mask, CameraRenderLayer layer)
	{
		uint32_t layer_mask = GetLayerMask(layer);
		return (mask & layer_mask) != 0;
	}
};