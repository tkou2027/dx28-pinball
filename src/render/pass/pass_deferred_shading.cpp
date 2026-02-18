#include "pass_deferred_shading.h"
#include "render/render_states.h"
#include "render/render_resource.h"
#include "render/resource/render_attachment.h"
#include "pass_forward.h"

#include "render/render_scene.h"


#include "global_context.h"
#include "config/preset_manager.h"
#include "render/pass/vertex_common.h"

using namespace DirectX;

void PassDeferredShading::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	PassBase::Initialize(device, context);

	m_vs = Shader::CreateShaderVertex(
		m_device, L"vertex_fullscreen.cso", Shader::InputLayoutType::MESH_STATIC);
	m_ps = Shader::CreateShaderPixel(m_device, L"pixel_deferred_shading.cso");

	m_vs_local_light = Shader::CreateShaderVertex(
		m_device, L"vertex_geometry_instanced.cso", Shader::InputLayoutType::MESH_STATIC);
		// m_device, L"vertex_geometry_static.cso", Shader::InputLayoutType::MESH_STATIC);

	m_ps_local_light_shading = Shader::CreateShaderPixel(m_device, L"pixel_deferred_shading_single_light.cso");
	m_ps_debug = Shader::CreateShaderPixel(m_device, L"pixel_debug_red.cso");

	SetIBLTextures(
		"asset/texture/ibl/testDiffuseHDR.dds",
		"asset/texture/ibl/testSpecularHDR.dds",
		"asset/texture/ibl/testBrdf.dds"
	);
}

void PassDeferredShading::Draw()
{
	DrawLocalLights();

	m_context->VSSetShader(m_vs.vertex_shader.Get(), nullptr, 0);
	m_context->PSSetShader(m_ps.Get(), nullptr, 0);
	m_context->IASetInputLayout(m_vs.input_layout.Get());

	const auto& render_resource = GetRenderResource();
	m_context->PSSetConstantBuffers(0, 1, render_resource.m_buffer_per_projection.GetAddressOf());
	m_context->PSSetConstantBuffers(1, 1, render_resource.m_buffer_per_view.GetAddressOf());
	m_context->PSSetConstantBuffers(2, 1, render_resource.m_buffer_lights.GetAddressOf());
	// m_context->PSSetConstantBuffers(3, 1, render_resource.m_buffer_light_shadows.GetAddressOf());
	// texture
	const auto& render_states = GetRenderStates();
	m_context->RSSetState(render_states.m_rs_cull_back.Get());

	m_context->PSSetSamplers(0, 1, render_states.m_ss_linear_clamp.GetAddressOf());
	m_context->PSSetSamplers(1, 1, render_states.m_ss_compare_less_linear_border.GetAddressOf());
	m_context->PSSetSamplers(2, 1, render_states.m_ss_linear_wrap.GetAddressOf());
	m_context->PSSetSamplers(3, 1, render_states.m_ss_linear_clamp.GetAddressOf());
	m_context->PSSetShaderResources(0, GEO_BUFFER_NUM, m_srv_g_buffer);
	//m_context->PSSetShaderResources(GEO_BUFFER_NUM, 1, &m_srv_shadow);
	// ramp map texture
	//{
	//	auto& texture_loader = GetTextureLoader();
	//	const auto texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/test_ramp.png");
	//	const auto texture_view = texture_loader.GetTexture(texture_id);
	//	m_context->PSSetShaderResources(GEO_BUFFER_NUM + 1, 1, texture_view.GetAddressOf());
	//}
	{
		auto& texture_loader = GetTextureLoader();
		const auto srv_diffuse = texture_loader.GetTexture(m_texture_ibl_diffuse);
		m_context->PSSetShaderResources(GEO_BUFFER_NUM, 1, srv_diffuse.GetAddressOf());
		const auto srv_specular = texture_loader.GetTexture(m_texture_ibl_specular);
		m_context->PSSetShaderResources(GEO_BUFFER_NUM + 1, 1, srv_specular.GetAddressOf());
		const auto srv_brdf_lut = texture_loader.GetTexture(m_texture_ibl_brdf_lut);
		m_context->PSSetShaderResources(GEO_BUFFER_NUM + 2, 1, srv_brdf_lut.GetAddressOf());
	}
	// om
	m_context->OMSetDepthStencilState(render_states.m_dss_depth_disabled.Get(), 0);
	// adding lights
	m_context->OMSetBlendState(render_states.m_bs_add.Get(),
		render_states.m_blend_factor_zero, render_states.m_sample_mask_all);
	// draw
	m_context->Draw(3, 0); // draw full-screen triangle
}

void PassDeferredShading::SetGBuffer(const RenderAttachment& geo_buffer)
{
	for (int i = 0; i < GEO_BUFFER_NUM - 1; i++)
	{
		m_srv_g_buffer[i] = geo_buffer.GetTextureColor<TextureResource2D>(i)->GetShaderResourceView().Get();
	}
	// depth stencil
	m_srv_g_buffer[GEO_BUFFER_NUM - 1] = geo_buffer.GetTextureDepthStencil()->GetShaderResourceView().Get();
}

void PassDeferredShading::SetTextureShadow(ID3D11ShaderResourceView* srv)
{
	m_srv_shadow = srv;
}

void PassDeferredShading::SetInputResource(
	ID3D11ShaderResourceView* g_buffer_a,
	ID3D11ShaderResourceView* g_buffer_b,
	ID3D11ShaderResourceView* g_buffer_c,
	ID3D11ShaderResourceView* g_buffer_depth
)
{
	m_srv_g_buffer[0] = g_buffer_a;
	m_srv_g_buffer[1] = g_buffer_b;
	m_srv_g_buffer[2] = g_buffer_c;
	m_srv_g_buffer[3] = g_buffer_depth;
}

void PassDeferredShading::SetIBLTextures(const std::string& diffuse, const std::string& specular, const std::string& brdf_lut)
{
	auto& texture_loader = GetTextureLoader();
	m_texture_ibl_diffuse = texture_loader.GetOrLoadTextureCubeFromFileDDS(diffuse);
	m_texture_ibl_specular = texture_loader.GetOrLoadTextureCubeFromFileDDS(specular);
	m_texture_ibl_brdf_lut = texture_loader.GetOrLoadTextureFromFileDDS(brdf_lut);
}

void PassDeferredShading::DrawLocalLights()
{
	m_context->VSSetShader(m_vs_local_light.vertex_shader.Get(), nullptr, 0);
	m_context->IASetInputLayout(m_vs_local_light.input_layout.Get());
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_context->PSSetShader(m_ps_local_light_shading.Get(), nullptr, 0);

	const auto& render_resource = GetRenderResource();
	// vs constant buffer
	m_context->VSSetConstantBuffers(0, 1, render_resource.m_buffer_per_projection.GetAddressOf());
	m_context->VSSetConstantBuffers(1, 1, render_resource.m_buffer_per_view.GetAddressOf());
	// m_context->VSSetConstantBuffers(2, 1, render_resource.m_buffer_per_mesh.GetAddressOf());

	// ps constant buffer
	m_context->PSSetConstantBuffers(0, 1, render_resource.m_buffer_per_projection.GetAddressOf());
	m_context->PSSetConstantBuffers(1, 1, render_resource.m_buffer_per_view.GetAddressOf());
	m_context->PSSetConstantBuffers(2, 1, render_resource.m_buffer_per_screen_size.GetAddressOf());
	m_context->PSSetConstantBuffers(3, 1, render_resource.m_buffer_lights.GetAddressOf());
	// ps texture (g-buffer)
	m_context->PSSetShaderResources(0, GEO_BUFFER_NUM, m_srv_g_buffer);

	const auto& render_states = GetRenderStates();
	// cull front, depth test greater equal
	m_context->RSSetState(render_states.m_rs_cull_front.Get());
	m_context->OMSetDepthStencilState(render_states.m_dss_depth_read_geq.Get(), 0);
	// add lights
	m_context->OMSetBlendState(render_states.m_bs_add.Get(),
		render_states.m_blend_factor_zero, render_states.m_sample_mask_all);

	auto& lights = GetRenderScene().m_light_models;
	VertexCommon::DrawModelInstanced(m_context, lights, 0, lights.size(), m_vs_local_light.layout_type);

	// // pass 2: draw shading ====
	// m_context->PSSetShader(m_ps_debug.Get(), nullptr, 0);
	// m_context->PSSetShaderResources(0, GEO_BUFFER_NUM, m_srv_g_buffer);
	// m_context->PSSetConstantBuffers(2, 1, render_resource.m_buffer_lights.GetAddressOf());
	// stencil = 0
	// m_context->OMSetDepthStencilState(render_states.m_dss_depth_enabled_stencil_read.Get(), 0);
	// VertexCommon::DrawModelInstanced(m_context, lights, 0, lights.size());
	//for (const auto& model : lights)
	//{
	//	BufferPerMesh cb{};
	//	XMMATRIX world = XMLoadFloat4x4(&model.instance.model_matrix);
	//	XMStoreFloat4x4(&cb.matrix_model, XMMatrixTranspose(world));
	//	XMStoreFloat4x4(&cb.matrix_model_inverse_transpose, XMMatrixInverse(nullptr, world));
	//	cb.uv_offset = XMFLOAT2(model.instance.uv_offset.x, model.instance.uv_offset.y);
	//	cb.uv_size = XMFLOAT2(model.instance.uv_size.x, model.instance.uv_size.y);
	//
	//	// 定数バッファに行列をセット
	//	const auto& render_resource = GetRenderResource();
	//	m_context->UpdateSubresource(render_resource.m_buffer_per_mesh.Get(), 0, nullptr, &cb, 0, 0);
	//
	//	VertexCommon::DrawModelStatic(m_context, model);
	//}


	//std::vector<ModelRenderInfo> local_lights;
	//const auto& render_scene = GetRenderScene();
	//const auto& lights = render_scene.m_lights;
	//const auto& model_desc = g_global_context.m_preset_manager->GetModelDesc("geo/unit_cube");
	//const auto& model_loader = GetModelLoader();

	//local_lights.reserve(lights.num_point_lights);
	//for (int i = 0; i < lights.num_point_lights; i++)
	//{
	//	const auto& light = lights.point_lights[i];
	//	ModelRenderInfo light_model{};

	//	light_model.key.model_type = ModelType::STATIC;
	//	light_model.key.model_id = model_desc.model_id;
	//	{
	//		// build model matrix from light position (no rotation) and small scale
	//		const float scale = 100.0f;
	//		XMMATRIX matScale = XMMatrixScaling(scale, scale, scale);
	//		XMMATRIX matTrans = XMMatrixTranslation(light.position_w.x, light.position_w.y, light.position_w.z);
	//		XMMATRIX matModel = XMMatrixMultiply(matScale, matTrans);
	//		XMStoreFloat4x4(&light_model.instance.model_matrix, matModel);

	//		BufferPerMesh cb{};
	//		XMMATRIX world = XMLoadFloat4x4(&light_model.instance.model_matrix);
	//		XMStoreFloat4x4(&cb.matrix_model, XMMatrixTranspose(world));
	//		XMStoreFloat4x4(&cb.matrix_model_inverse_transpose, XMMatrixInverse(nullptr, world));
	//		cb.uv_offset = XMFLOAT2(light_model.instance.uv_offset.x, light_model.instance.uv_offset.y);
	//		cb.uv_size = XMFLOAT2(light_model.instance.uv_size.x, light_model.instance.uv_size.y);

	//		// 定数バッファに行列をセット
	//		const auto& render_resource = GetRenderResource();
	//		m_context->UpdateSubresource(render_resource.m_buffer_per_mesh.Get(), 0, nullptr, &cb, 0, 0);
	//	}

	//	local_lights.push_back(light_model);

	//	VertexCommon::DrawModelStatic(m_context, light_model);
	//}

	// VertexCommon::DrawModelInstanced(m_context, local_lights, 0, lights.num_point_lights);
}
