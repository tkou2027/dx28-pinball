#include "pass_shadow.h"

#include <DirectXMath.h>
#include "config/constant.h"
#include "platform/graphics.h"

#include "global_context.h"
#include "render/render_system.h"
#include "render/render_common.h"
#include "render/render_scene.h"
#include "render/render_resource.h"
#include "render/render_states.h"
#include "render/resource/buffer.h"

#include "shader_setting.h"

template <class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;
using namespace DirectX;

namespace
{
	struct BufferPerShadow
	{
		DirectX::XMFLOAT4X4 matrix_light_view_proj;
	};
}

void PassShadow::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	PassBase::Initialize(device, context);
	// shaders
	m_vertex_shaders[VS_INDEX_MESH_STATIC] = Shader::CreateShaderVertex(m_device,
		L"vertex_shadow_static.cso", Shader::InputLayoutType::MESH_STATIC);
	m_vertex_shaders[VS_INDEX_MESH_SKINNED] = Shader::CreateShaderVertex(m_device,
		L"vertex_shadow_skinned.cso", Shader::InputLayoutType::MESH_SKINNED);
	m_pixel_shader = Shader::CreateShaderPixel(m_device, L"pixel_none.cso");

	m_buffer_per_shadow = Buffer::CreateConstantBuffer(m_device, sizeof(BufferPerShadow));
}

void PassShadow::Draw()
{
	SetInfoPerDraw();

	const auto& render_scene = g_global_context.m_render_system->GetRenderScene();
	SetInfoPerShader(VS_INDEX_MESH_STATIC);
	const auto& material_resource = GetMaterialResourceManager();
	for (const auto& model : render_scene.m_objects_main)
	{
		const auto& material_desc = material_resource.GetMaterialDesc(model.key.material_id);
		if (!material_desc.IfTechnique(RenderTechnique::CAST_SHADOW, CameraRenderLayer::DEFAULT)) // TODO
		{
			continue;
		}
		SetInfoPerModel(model);
		DrawModelStatic(model);
	}

	SetInfoPerShader(VS_INDEX_MESH_SKINNED);
	for (const auto& model : render_scene.m_objects_main_skinned)
	{
		const auto& material_desc = material_resource.GetMaterialDesc(model.key.material_id);
		if (!material_desc.IfTechnique(RenderTechnique::CAST_SHADOW, CameraRenderLayer::DEFAULT)) // TODO
		{
			continue;
		}
		SetInfoPerModel(model);
		DrawModelDynamic(model);
	}
}

void PassShadow::SetInfoPerDraw()
{
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_context->PSSetShader(m_pixel_shader.Get(), nullptr, 0);

	const auto& render_states = GetRenderStates();
	const auto& render_resource = GetRenderResource();
	// geometry
	m_context->VSSetConstantBuffers(0, 1, render_resource.m_buffer_per_projection.GetAddressOf());
	m_context->VSSetConstantBuffers(1, 1, render_resource.m_buffer_per_view.GetAddressOf());
	m_context->VSSetConstantBuffers(2, 1, render_resource.m_buffer_per_mesh.GetAddressOf());
	// m_context->VSSetConstantBuffers(3, 1, render_resource.m_buffer_light_shadows.GetAddressOf());
	//m_context->VSSetConstantBuffers(3, 1, m_buffer_per_shadow.GetAddressOf());
	// light projection
	//{
	//	BufferPerShadow cb{};
	//	// TODO
	//	const auto target = Vector3{ 0.0f, 0.0f, 0.0f };
	//	const auto light_pos = g_shader_setting.light_direction.GetNormalized() * -200.0f;
	//	const auto up = Vector3{ 0.0f, 1.0f, 0.0f };

	//	XMMATRIX light_view = XMMatrixLookAtLH(
	//		XMVectorSet(light_pos.x, light_pos.y, light_pos.z, 1.0f),
	//		XMVectorSet(target.x, target.y, target.z, 1.0f),
	//		XMVectorSet(up.x, up.y, up.z, 0.0f)
	//	);

	//	float ortho_width = 300.0f;
	//	float ortho_height = 300.0f;
	//	float near_plane = 0.1f;
	//	float far_plane = 300.0f;
	//	XMMATRIX light_proj = XMMatrixOrthographicLH(ortho_width, ortho_height, near_plane, far_plane);
	//	XMStoreFloat4x4(&cb.matrix_light_view_proj, XMMatrixTranspose(light_view * light_proj));

	//	m_context->UpdateSubresource(m_buffer_per_shadow.Get(), 0, nullptr, &cb, 0, 0);
	//}
	// rs
	m_context->RSSetState(render_states.m_rs_cull_back.Get());
	m_context->RSSetState(render_states.m_rs_depth_bias.Get());
	// ps
	m_context->PSSetSamplers(0, 1, render_states.m_ss_linear_clamp.GetAddressOf());
	// om
	m_context->OMSetDepthStencilState(render_states.m_dss_depth_enabled.Get(), 0);
}

void PassShadow::SetInfoPerShader(VertexShaderIndex vertex_index)
{
	auto& vertex_shader_input = m_vertex_shaders[vertex_index];
	// 頂点シェーダーとピクセルシェーダーを描画パイプラインに設定
	m_context->VSSetShader(vertex_shader_input.vertex_shader.Get(), nullptr, 0);
	// 頂点レイアウトを描画パイプラインに設定
	m_context->IASetInputLayout(vertex_shader_input.input_layout.Get());
}

void PassShadow::SetInfoPerModel(const ModelRenderInfo& entity)
{
	// per object
	{
		BufferPerMesh cb{};
		XMMATRIX world = XMLoadFloat4x4(&entity.instance.model_matrix);
		XMStoreFloat4x4(&cb.matrix_model, XMMatrixTranspose(world));
		XMStoreFloat4x4(&cb.matrix_model_inverse_transpose, XMMatrixInverse(nullptr, world));

		// 定数バッファに行列をセット
		const auto& render_resource = GetRenderResource();
		m_context->UpdateSubresource(render_resource.m_buffer_per_mesh.Get(), 0, nullptr, &cb, 0, 0);
	}
}
