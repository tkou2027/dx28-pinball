#include "pass_sky.h"

#include "render/render_common.h"
#include "render/render_scene.h"
#include "render/render_resource.h"
#include "render/render_states.h"

#include "vertex_common.h"

void PassSky::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	PassBase::Initialize(pDevice, pContext);
	// shaders
	m_vertex_shader = Shader::CreateShaderVertex(m_device, L"skybox_vertex.cso", Shader::InputLayoutType::MESH_STATIC);
	m_pixel_shader = Shader::CreateShaderPixel(m_device, L"skybox_pixel.cso");
}

void PassSky::Draw()
{
	SetInfoPerDraw();
	// const auto& render_entities = GetRenderScene().m_objects_main;
	const auto& material_resource = GetMaterialResourceManager();
	// for (const auto& model : render_entities)
	const auto& renderable_manager = GetRenderScene().GetRenderablesManager();
	for (const auto& index : m_mesh_indices_static)
	{
		const auto& model = renderable_manager.m_models.at(index);
		const auto& material_desc = material_resource.GetMaterialDesc(model.key.material_id);
		if (!material_desc.IfTechnique(RenderTechnique::SKYBOX, m_camera_render_layer))// TODO
		{
			continue;
		}
		SetInfoPerModel(model);
		VertexCommon::DrawModelStatic(m_context, model, m_vertex_shader.layout_type);
	}
}

void PassSky::ResetRenderableIndices(CameraRenderLayer layer)
{
	m_mesh_indices_static.clear();
	m_camera_render_layer = layer;
}

void PassSky::AddRenderableIndex(size_t mesh_index, ModelType model_type, const MaterialDesc& material_desc)
{
	if (!material_desc.IfTechnique(RenderTechnique::SKYBOX, m_camera_render_layer) || model_type != ModelType::STATIC)
	{
		return;
	}
	m_mesh_indices_static.push_back(mesh_index);
}

void PassSky::SetInfoPerDraw()
{
	m_context->VSSetShader(m_vertex_shader.vertex_shader.Get(), nullptr, 0);
	m_context->PSSetShader(m_pixel_shader.Get(), nullptr, 0);
	m_context->IASetInputLayout(m_vertex_shader.input_layout.Get());

	const auto& render_resource = GetRenderResource();
	m_context->VSSetConstantBuffers(0, 1, render_resource.m_buffer_per_projection.GetAddressOf());
	m_context->VSSetConstantBuffers(1, 1, render_resource.m_buffer_per_view.GetAddressOf());

	const auto& render_states = GetRenderStates();
	m_context->RSSetState(render_states.m_rs_cull_front.Get());
	m_context->OMSetDepthStencilState(render_states.m_dss_depth_read.Get(), 0);
	m_context->OMSetBlendState(render_states.m_bs_disabled.Get(),
		render_states.m_blend_factor_zero, render_states.m_sample_mask_all);

	m_context->PSSetSamplers(0, 1, render_states.m_ss_linear_clamp.GetAddressOf());
}

void PassSky::SetInfoPerModel(const ModelRenderInfo& model)
{
	const auto& material_desc = GetMaterialResourceManager().GetMaterialDesc(model.key.material_id);
	const auto& material = material_desc.GetTechnique<TechniqueDescSkybox>(m_camera_render_layer);

	const auto& texture_loader = GetTextureLoader();
	auto srv = texture_loader.GetTexture(material.skybox_texture_id);
	m_context->PSSetShaderResources(1, 1, srv.GetAddressOf());
}
