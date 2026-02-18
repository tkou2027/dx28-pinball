#pragma once
#include <cstdint>

enum class TextureType : uint8_t
{
	STATIC,
	CAMERA
};

enum class TexturePlaceholder : uint8_t
{
	WHITE,
	NORMAL,
	METALLIC,
	ROUGHNESS,
	// counter
	MAX
};

enum class TextureUsageType
{
	RENDER_TARGET,
	DEPTH_STENCIL,
	BACK_BUFFER
};

struct TextureResourceId
{
	int id{ -1 };
	TextureType type{ TextureType::STATIC };
	TexturePlaceholder placeholder{ TexturePlaceholder::WHITE };

	bool operator==(const TextureResourceId& other) const noexcept
	{
		return id == other.id
			&& type == other.type
			&& placeholder == other.placeholder;
	}

	bool operator!=(const TextureResourceId& other) const noexcept
	{
		return !(*this == other);
	}

	bool operator<(const TextureResourceId& other) const noexcept
	{
		// order by type, then placeholder, then id
		return std::tie(type, placeholder, id) < std::tie(other.type, other.placeholder, other.id);
	}

	bool IfValid() const noexcept
	{
		return id >= 0;
	}
};