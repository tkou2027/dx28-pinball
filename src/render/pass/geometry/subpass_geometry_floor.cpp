#include "subpass_geometry_floor.h"

#include "config/constant.h"
#include "platform/graphics.h"

#include "global_context.h"
#include "render/render_system.h"
#include "render/render_resource.h"
#include "render/render_states.h"
#include "render/render_scene.h"
#include "render/render_common.h"
#include "render/resource/buffer.h"

#include "shader_setting.h"
#include "render/dx_trace.h"

namespace
{
	struct FloorConfig
	{
		float radius_outer{};
		float radius_inner{};
		float _padding_1{};
		float _padding_2{};
	};
}

void PassGeometryFloor::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	PassBaseGeometry::Initialize(device, context);
	m_pixel_shader = Shader::CreateShaderPixel(m_device, L"pixel_geometry_floor.cso");
	m_cb_material = Buffer::CreateConstantBuffer(m_device, sizeof(MaterialDefault));
	m_cb_floor_config = Buffer::CreateConstantBuffer(m_device, sizeof(FloorConfig));
}

bool PassGeometryFloor::ShouldRender(const MaterialDesc& material_desc, ModelType model_type) const
{
	if (!material_desc.IfTechnique(RenderTechnique::DEFAULT, m_curr_render_layer)
		|| !material_desc.IfTechnique(RenderTechnique::DEFERRED_FLOOR, m_curr_render_layer))
	{
		return false;
	}
	const auto& material = material_desc.GetTechnique<TechniqueDescDefault>(m_curr_render_layer);
	return !material.no_deferred;
}

void PassGeometryFloor::SetInfoPerDraw()
{
	const auto& render_states = GetRenderStates();
	const auto& render_resource = GetRenderResource();
	// vs
	m_context->VSSetConstantBuffers(0, 1, render_resource.m_buffer_per_projection.GetAddressOf());
	m_context->VSSetConstantBuffers(1, 1, render_resource.m_buffer_per_view.GetAddressOf());
	m_context->VSSetConstantBuffers(2, 1, render_resource.m_buffer_per_mesh.GetAddressOf());
	// ps
	m_context->PSSetShader(m_pixel_shader.Get(), nullptr, 0);
	m_context->PSSetSamplers(0, 1, render_states.m_ss_linear_wrap.GetAddressOf());
	m_context->PSSetConstantBuffers(0, 1, m_cb_material.GetAddressOf());
	m_context->PSSetConstantBuffers(1, 1, m_cb_floor_config.GetAddressOf()); // floor config
	// om
	m_context->OMSetDepthStencilState(render_states.m_dss_depth_enabled.Get(), 0);
	m_context->OMSetBlendState(render_states.m_bs_disabled.Get(),
		render_states.m_blend_factor_zero, render_states.m_sample_mask_all);
}

void PassGeometryFloor::SetInfoPerModel(const ModelRenderInfo& model)
{
	// per object
	PassBaseGeometry::SetInfoPerModel(model);
	SetInfoPerMaterial(model.key);
}

void PassGeometryFloor::SetInfoPerMaterial(const ModelRenderKey& model_key)
{
	// per object material
	const auto& material_resource = GetMaterialResourceManager();
	const auto& material_desc = material_resource.GetMaterialDesc(model_key.material_id);
	const auto& material = material_desc.GetTechnique<TechniqueDescDefault>(m_curr_render_layer);
	{
		MaterialDefault cb{};
		cb.base_color = DirectX::XMFLOAT4{ material.base_color.x, material.base_color.y, material.base_color.z, 1.0f };
		cb.metallic = material.metallic;
		cb.specular = material.specular;
		cb.roughness = material.roughness;
		cb.shading_model = static_cast<UINT>(material.shading_model);
		cb.emission_color = material.emission_color.ToXMFLOAT3();
		cb.emission_intensity = material.emission_intensity;
		//cb.shading_model = entity.m_material_desc.default_desc.shading_model;
		// TODO
		m_context->UpdateSubresource(m_cb_material.Get(), 0, nullptr, &cb, 0, 0);
	}
	// textures
	const auto& render_states = GetRenderStates();
	auto& texture_loader = GetTextureLoader();

	auto albedo = texture_loader.GetTexture(material.albedo_texture_id);
	m_context->PSSetShaderResources(0, 1, albedo.GetAddressOf());
	auto emission = texture_loader.GetTexture(material.emission_texture_id);
	m_context->PSSetShaderResources(1, 1, emission.GetAddressOf());
	auto normal = texture_loader.GetTexture(material.normal_texture_id);
	m_context->PSSetShaderResources(2, 1, normal.GetAddressOf());
	auto metallic = texture_loader.GetTexture(material.metallic_texture_id);
	m_context->PSSetShaderResources(3, 1, metallic.GetAddressOf());
	auto roughness = texture_loader.GetTexture(material.roughness_texture_id);
	m_context->PSSetShaderResources(4, 1, roughness.GetAddressOf());

	// states
	switch (material.cull_type)
	{
	case CullType::CULL_BACK:
	{
		m_context->RSSetState(render_states.m_rs_cull_back.Get());
		break;
	}
	case CullType::CULL_FRONT:
	{
		m_context->RSSetState(render_states.m_rs_cull_front.Get());
		break;
	}
	case CullType::CULL_NONE:
	{
		m_context->RSSetState(render_states.m_rs_cull_none.Get());
		break;
	}
	}

	{
		const auto& material_floor = material_desc.GetTechnique<TechniqueDescDeferredFloor>(m_curr_render_layer);
		FloorConfig cb{};
		cb.radius_inner = material_floor.radius_inner;
		cb.radius_outer = material_floor.radius_outer;
		m_context->UpdateSubresource(m_cb_floor_config .Get(), 0, nullptr, &cb, 0, 0);
	}
}
