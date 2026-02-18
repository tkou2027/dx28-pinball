#pragma once
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "render/directx.h"
#include "render/config/model_desc.h"
#include "render/config/model_geometry.h"
#include "render/resource/model_data.h"

template <typename T>
class ResourcePool;

struct StructureBuffer
{
};

class ModelLoader
{
public:
	ModelLoader();
	~ModelLoader();
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* context);
	const ModelData& GetModel(int model_id) const;
	const ModelDataSkinned& GetModelSkinned(int model_id_skinned) const;
	const AnimationData& GetAnimation(int animation_id) const;
	// bone matrix resource
	int CreateModelDataSkinnedMatrix(int model_id_skinned);
	void UpdateModelSkinnedMatrix(int model_id_skinned_frame, const std::vector<DirectX::XMFLOAT4X4>& bone_matrices) const;
	const ModelDataSkinnedFrame& GetModelSkinnedMatrix(int model_id_skinned_frame) const;
	// load from file
	void GetOrLoadModel(ModelDesc& model_desc);
	// meshes generated with code
	void LoadMeshGeometry(ModelDesc& model_desc, const MeshGeometry& mesh);
	void LoadMeshGeometryUnique(ModelDesc& model_desc, const MeshGeometry& mesh);
	const MeshGeometry& GetMeshGeometry(int mesh_geometry_id) const;
	MeshGeometry& GetMeshGeometry(int mesh_geometry_id);
	// structured buffer
	int AddBufferView(BufferView buffer_view);
	BufferView& GetBufferView(int buffer_view_id);
	const BufferView& GetBufferView(int buffer_view_id) const;
private:
	// create
	int GetOrLoadModel(const std::string& filename);
	void GetOrLoadModelDynamic(const std::string& filename, int& mode_id, int& mode_id_dynamic);
	int GetOrLoadAnimation(const std::string filename, const ModelDataSkinned& model_data_dynamic);
	int CreateModelFromGeometry(
		const std::string& key,
		const MeshGeometry& mesh_geometry);
	// assimp util
	class AiModelLoader;
	std::unique_ptr<AiModelLoader> m_model_loader{ nullptr };
	// resources
	std::unique_ptr<ResourcePool<ModelData> > m_models_pool;
	std::unique_ptr<ResourcePool<ModelDataSkinned> > m_models_skinned_pool;
	std::unique_ptr<ResourcePool<ModelDataSkinnedFrame> > m_models_skinned_matrix_pool;
	std::unique_ptr<ResourcePool<AnimationData> > m_animations_pool;
	std::unique_ptr<ResourcePool<MeshGeometry> > m_mesh_geometry_pool;
	std::unique_ptr<ResourcePool<BufferView> > m_buffer_view_pool;
	// 注意！初期化で外部から設定されるもの。Release不要。
	ID3D11Device* m_device{ nullptr };
	ID3D11DeviceContext* m_context{ nullptr };
};