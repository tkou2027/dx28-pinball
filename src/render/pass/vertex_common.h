#pragma once
#include <array>
#include "render/config/model_state.h"
#include "render/resource/shader.h"
class VertexCommon
{
public:
	struct ShaderConfig
	{
		std::wstring shader_name{};
		Shader::InputLayoutType input_layout_type{ Shader::InputLayoutType::NONE };
	};
	struct VertexCommonConfig
	{
		std::array<ShaderConfig, static_cast<size_t>(ModelType::MAX)> shader_configs{};
	};
	void Initialize(ID3D11Device* device);
	void Initialize(ID3D11Device* device, const VertexCommonConfig& config);
	bool SetVertexShaderAndInputLayout(ID3D11DeviceContext* context, ModelType type);
	static void DrawModelStatic(ID3D11DeviceContext* context, ModelRenderInfo render_key, Shader::InputLayoutType input_layout_type);
	static void DrawModelSkinned(ID3D11DeviceContext* context, ModelRenderInfo render_key, Shader::InputLayoutType input_layout_type);
	// static void DrawModelInstanced(ID3D11DeviceContext* context, ModelRenderKey render_key, const std::vector<ModelRenderInstance> instances);
	static void DrawModelInstanced(
		ID3D11DeviceContext* context, const std::vector<ModelRenderInfo> instances,
		unsigned int start_index, unsigned int end_index_exclude, Shader::InputLayoutType input_layout_type);
	Shader::InputLayoutType GetInputLayoutType(ModelType type) const
	{
		return m_vertex_inputs[static_cast<size_t>(type)].layout_type;
	}
private:
	static class ModelLoader& GetModelLoader();
	static void SetupVertexBuffer(ID3D11DeviceContext*, const class MeshData& mesh_data, Shader::InputLayoutType input_layout_type);
	static void SetupVertexBuffer(ID3D11DeviceContext*,
		const class MeshData& mesh_data,const class MeshDataSkinned mesh_data_skinned, Shader::InputLayoutType input_layout_type);
	std::array<Shader::VertexShaderInputLayout, static_cast<size_t>(ModelType::MAX)> m_vertex_inputs{};
};