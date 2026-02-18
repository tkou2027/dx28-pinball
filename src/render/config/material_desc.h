#pragma once
#include <string>
#include <vector>
#include <array>
#include <variant>
#include "math/vector3.h"
#include "math/vector4.h"
#include "render/config/camera_render_layer.h"
#include "render/config/texture_resource_id.h"

// customizable per project ====
enum class RenderTechnique
{
	DEFAULT,
	CAST_SHADOW,
	SKYBOX,
	// deferred
	DEFERRED,
	DEFERRED_FLOOR,
	DEFERRED_CEL,
	// forward
	FORWARD_UNLIT,
	FORWARD_SILHOUETTE,
	FORWARD_SCREEN,
	FORWARD_GLASS,
	FORWARD_SCREEN_BACKGROUND,
	FORWARD_PROJECTOR,
	// count
	MAX
};

enum class CullType
{
	CULL_BACK,
	CULL_FRONT,
	CULL_NONE
};

enum class ShadingModel
{
	LIT,
	UNLIT
};

struct TechniqueDescDefault
{
	static constexpr RenderTechnique TYPE{ RenderTechnique::DEFAULT };
	// base color / albedo
	Vector3 base_color{ 1.0f, 1.0f, 1.0f };
	TextureResourceId albedo_texture_id{};
	// normal map
	TextureResourceId normal_texture_id{}; // TODO: default normal map
	// metallic
	float metallic{ 0.0f };
	TextureResourceId metallic_texture_id{}; // TODO: default metallic map
	// roughness
	float roughness{ 1.0f };
	TextureResourceId roughness_texture_id{}; // TODO: default roughness map
	// specular
	float specular{ 0.0f };
	// emission
	Vector3 emission_color{ 0.0f, 0.0f, 0.0f };
	float emission_intensity{ 0.0f };
	TextureResourceId emission_texture_id{}; // TODO: default emission map
	// flags
	ShadingModel shading_model{ ShadingModel::LIT };
	CullType cull_type{ CullType::CULL_BACK };
	bool no_deferred{ false };
	bool pre_pass{ true };
	int8_t stencil_write{ -1 };
	int8_t stencil_read{ -1 };
};

struct TechniqueDescCastShadow
{
	static constexpr RenderTechnique TYPE{ RenderTechnique::CAST_SHADOW };
};

struct TechniqueDescSkybox
{
	static constexpr RenderTechnique TYPE{ RenderTechnique::SKYBOX };
	TextureResourceId skybox_texture_id{};
};

struct TechniqueDescDeferred
{
	static constexpr RenderTechnique TYPE{ RenderTechnique::DEFERRED };
};

struct TechniqueDescDeferredFloor
{
	static constexpr RenderTechnique TYPE{ RenderTechnique::DEFERRED_FLOOR };
	float radius_outer{ 0.0f };
	float radius_inner{ 0.0f };
};

struct TechniqueDescDeferredCel
{
	static constexpr RenderTechnique TYPE{ RenderTechnique::DEFERRED_CEL };
};

struct TechniqueDescForwardUnlit
{
	static constexpr RenderTechnique TYPE{ RenderTechnique::FORWARD_UNLIT };
};

struct TechniqueDescForwardSilhouette
{
	static constexpr RenderTechnique TYPE{ RenderTechnique::FORWARD_SILHOUETTE };
};

struct TechniqueDescForwardScreen
{
	static constexpr RenderTechnique TYPE{ RenderTechnique::FORWARD_SCREEN };
	TextureResourceId screen_pixel_texture_id{};
	TextureResourceId world_texture_id{};
	Vector3 emission_color{ 1.0f, 1.0f, 1.0f };
	float emission_intensity{ 1.0f };
	Vector2 screen_pixels_scale{ 1.0f, 1.0f };
};

struct TechniqueDescForwardGlass
{
	static constexpr RenderTechnique TYPE{ RenderTechnique::FORWARD_GLASS };
	TextureResourceId env_texture_id{};
};

struct TechniqueDescForwardScreenBackground
{
	static constexpr RenderTechnique TYPE{ RenderTechnique::FORWARD_SCREEN_BACKGROUND };
	float center_u{};
	float center_v{};
	float radius{};
	float thickness{};
	float time{};
	float rotation{};
	float stripe_density{ 300.0f };
	float stripe_offset{ 0.0f };
	
	Vector4 color_inside;
	Vector4 color_stripe_positive;
	Vector4 color_stripe_negative;
};

struct TechniqueDescForwardProjector
{
	static constexpr RenderTechnique TYPE{ RenderTechnique::FORWARD_PROJECTOR };
	// camera
	DirectX::XMFLOAT4X4 projector_mat_view_proj{};
	Vector3 projector_position_w{};
	// texture
	TextureResourceId projectior_texture_id{};
	float intensity{ 1.0f };
};

class TechniqueDesc
{
public:
	using Variant = std::variant<
		TechniqueDescDefault,
		TechniqueDescCastShadow,
		TechniqueDescSkybox,
		TechniqueDescDeferred,
		TechniqueDescDeferredFloor,
		TechniqueDescDeferredCel,
		TechniqueDescForwardUnlit,
		TechniqueDescForwardSilhouette,
		TechniqueDescForwardScreen,
		TechniqueDescForwardGlass,
		TechniqueDescForwardScreenBackground,
		TechniqueDescForwardProjector
	>;
	TechniqueDesc() : m_data(TechniqueDescDefault{}), layer_mask(CameraRenderLayerMask::DEFAULT) {}
	TechniqueDesc(const TechniqueDescDefault& desc) : m_data(desc), layer_mask(CameraRenderLayerMask::DEFAULT) {}
	TechniqueDesc(const TechniqueDescSkybox& desc) : m_data(desc), layer_mask(CameraRenderLayerMask::DEFAULT) {}
	TechniqueDesc(const TechniqueDescDeferred& desc) : m_data(desc), layer_mask(CameraRenderLayerMask::DEFAULT) {}
	TechniqueDesc(const TechniqueDescDeferredFloor& desc) : m_data(desc), layer_mask(CameraRenderLayerMask::DEFAULT) {}
	TechniqueDesc(const TechniqueDescDeferredCel&desc) : m_data(desc), layer_mask(CameraRenderLayerMask::DEFAULT) {}
	TechniqueDesc(const TechniqueDescForwardUnlit& desc) : m_data(desc), layer_mask(CameraRenderLayerMask::DEFAULT) {}
	TechniqueDesc(const TechniqueDescForwardSilhouette& desc) : m_data(desc), layer_mask(CameraRenderLayerMask::DEFAULT) {}
	TechniqueDesc(const TechniqueDescForwardScreen& desc) : m_data(desc), layer_mask(CameraRenderLayerMask::DEFAULT) {}
	TechniqueDesc(const TechniqueDescForwardGlass& desc) : m_data(desc), layer_mask(CameraRenderLayerMask::DEFAULT) {}
	TechniqueDesc(const TechniqueDescForwardScreenBackground& desc) : m_data(desc), layer_mask(CameraRenderLayerMask::DEFAULT) {}
	TechniqueDesc(const TechniqueDescForwardProjector& desc) : m_data(desc), layer_mask(CameraRenderLayerMask::DEFAULT) {}

	template<typename TDesc>
	const TDesc& Get() const
	{
		return  std::get<TDesc>(m_data);
	}
	template<typename TDesc>
	TDesc& Get()
	{
		return std::get<TDesc>(m_data);
	}
	RenderTechnique GetType() const
	{
		return static_cast<RenderTechnique>(m_data.index());
	}

	// Per-technique layer mask. Set by MaterialDesc::SetTechnique when assigning.
	uint32_t layer_mask{ CameraRenderLayerMask::DEFAULT };

private:
	Variant m_data;
};

// customizable per project end ====

class MaterialDesc
{
public:
	MaterialDesc();
	void SetTechnique(TechniqueDesc desc, uint32_t mask = CameraRenderLayerMask::DEFAULT);
	bool IfTechnique(RenderTechnique tech, CameraRenderLayer layer) const;
	// void SetRenderLayerMask(uint32_t mask) { m_render_layer_mask = mask; }
	bool IfRenderLayer(CameraRenderLayer layer) const;
	template<typename TDesc>
	TDesc& GetTechnique(CameraRenderLayer layer = CameraRenderLayer::DEFAULT)
	{
		RenderTechnique type = TDesc::TYPE;
		int index = m_techniques_index[static_cast<size_t>(layer)][static_cast<size_t>(type)];
		assert(index >= 0);
		return m_techniques[index].Get<TDesc>();
	}
	template<typename TDesc>
	const TDesc& GetTechnique(CameraRenderLayer layer = CameraRenderLayer::DEFAULT) const
	{
		RenderTechnique type = TDesc::TYPE;
		int index = m_techniques_index[static_cast<size_t>(layer)][static_cast<size_t>(type)];
		assert(index >= 0);
		return m_techniques[index].Get<TDesc>();
	}
private:
	static constexpr size_t MAX_TECHNIQUES{ static_cast<size_t>(RenderTechnique::MAX) };
	static constexpr size_t MAX_LAYERS{ static_cast<size_t>(CameraRenderLayer::MAX) };
	std::vector<TechniqueDesc> m_techniques{};
	std::array<std::array<int, MAX_TECHNIQUES>, MAX_LAYERS> m_techniques_index{};
	uint32_t m_render_layer_mask{ CameraRenderLayerMask::DEFAULT };
};
