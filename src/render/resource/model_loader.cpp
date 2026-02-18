#include "model_loader.h"
#include <array>
// model
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "render/resource/texture_loader.h"
#include "render/dx_trace.h"
#include "render/resource/buffer.h"
#include "util/debug_ostream.h"
#include "util/resource_pool.h"

#pragma comment (lib, "assimp-vc143-mt.lib") 
//#if defined _DEBUG
//#pragma comment (lib, "assimp-vc143-mtd.lib") 
//#else
//#pragma comment (lib, "assimp-vc143-mt.lib") 
//#endif // _DEBUG

using namespace DirectX;
using namespace  Microsoft::WRL;

class ModelLoader::AiModelLoader
{
public:
	AiModelLoader(ID3D11Device* device, ID3D11DeviceContext* context) :
		m_device(device), m_context(context) {
	}
	struct LoadingContext
	{
		const aiScene* scene{ nullptr };
		std::string root_path;
		std::string model_path;
	};
	// load scene file
	bool LoadScene(const std::string& filename, Assimp::Importer& importer, LoadingContext& loading_context) const;
	// load models
	void LoadModelData(const std::string& filename, ModelData& model_data) const;
	void LoadModelDataDynamic(const std::string& filename, ModelDataSkinned& mode_data_dynamic) const;
	void LoadModelDataDynamic(const std::string& filename, ModelData& model_data, ModelDataSkinned& mode_data_dynamic) const;
	void LoadAnimationData(const std::string& filename, const ModelDataSkinned& model_data_dynamic, AnimationData& animation_data) const;
private:
	void LoadModelData(const LoadingContext& loading_context, ModelData& model_data) const;
	// load and create resources ====
	// static
	void LoadMesh(const LoadingContext& loading_context, const aiMesh* mesh, MeshData& mesh_data) const;
	void LoadMaterial(const LoadingContext& loading_context, const aiMaterial* material, ModelTextureData& mesh_texture_data) const;
	ComPtr<ID3D11ShaderResourceView> LoadTextureOfType(
		const LoadingContext& loading_context, const aiMaterial* material, enum aiTextureType type) const;
	// dynamic
	void LoadBones(const LoadingContext& loading_context, ModelDataSkinned& model_data_dynamic) const;
	void LoadMeshBones(
		const LoadingContext& loading_context, const aiMesh* mesh,
		std::unordered_map<std::string, BoneInfo>& bone_info_map,
		MeshDataSkinned& mesh_data_dynamic) const;

	ID3D11Device* m_device{ nullptr };
	ID3D11DeviceContext* m_context{ nullptr };
};

// TODO: static animation utils
static DirectX::XMFLOAT4X4 AiToXMFloat4x4(const aiMatrix4x4& m)
{
	// transpose
	DirectX::XMFLOAT4X4 result(
		m.a1, m.b1, m.c1, m.d1,
		m.a2, m.b2, m.c2, m.d2,
		m.a3, m.b3, m.c3, m.d3,
		m.a4, m.b4, m.c4, m.d4
	);
	return result;
}

static void LoadAssimpHierarchy(
	const aiNode* node, const aiMatrix4x4& trans_parent,
	std::unordered_map<std::string, BoneInfo>& bone_info_map)
{
	std::string node_name(node->mName.data);

	aiMatrix4x4 trans_node(node->mTransformation);
	aiMatrix4x4 trans_global = trans_parent * trans_node;

	auto find_it = bone_info_map.find(node_name);
	if (find_it != bone_info_map.end()) {
		auto& bone_info = find_it->second;
		bone_info.offset = AiToXMFloat4x4(trans_global * bone_info.offset_local);
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		LoadAssimpHierarchy(node->mChildren[i], trans_global, bone_info_map);
	}
}

static void LoadAssimpHierarchyAnimation(
	const aiNode* node,
	int parent_index,
	const std::unordered_map<std::string, BoneInfo>& bone_info_map,
	const std::unordered_map<std::string, int>& anim_node_map,
	std::vector<AnimatedNode>& node_list)
{
	std::string node_name(node->mName.data);

	// animation
	AnimatedNode animated_node{};
	animated_node.index = node_list.size();
	animated_node.parent_index = parent_index;
	// find animation index
	{
		auto find_it = anim_node_map.find(node_name);
		if (find_it != anim_node_map.end())
		{
			animated_node.animation_index = find_it->second;
		}
	}
	// find bone index
	{
		auto find_it = bone_info_map.find(node_name);
		if (find_it != bone_info_map.end())
		{
			animated_node.bone_index = find_it->second.id;
		}
	}
	// local transform
	animated_node.transform_local = node->mTransformation;

	node_list.push_back(animated_node);
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		LoadAssimpHierarchyAnimation(node->mChildren[i], animated_node.index, bone_info_map, anim_node_map, node_list);
	}
}

// assimp loading
bool ModelLoader::AiModelLoader::LoadScene(const std::string& filename, Assimp::Importer& importer, LoadingContext& loading_context) const
{
	importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE | aiPrimitiveType_POINT);
	const aiScene* scene = importer.ReadFile(filename,
		aiProcessPreset_TargetRealtime_MaxQuality |
		aiProcess_ConvertToLeftHanded |
		aiProcess_Triangulate |
		aiProcess_ImproveCacheLocality |
		aiProcess_SortByPType |
		aiProcess_GenSmoothNormals |
		aiProcess_FlipUVs
	);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		hal::dout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		loading_context.scene = nullptr;
		return false;
	}
	loading_context.scene = scene;
	loading_context.model_path = filename;
	loading_context.root_path = filename.substr(0, filename.find_last_of('/'));
	return true;
}

void ModelLoader::AiModelLoader::LoadModelData(const std::string& filename, ModelData& model_data) const
{
	Assimp::Importer importer;
	LoadingContext loading_context;
	if (!LoadScene(filename, importer, loading_context))
	{
		hal::dout << "Failed to load model " << filename << std::endl;
		return;
	}

	LoadModelData(loading_context, model_data);
}

void ModelLoader::AiModelLoader::LoadModelDataDynamic(const std::string& filename, ModelDataSkinned& mode_data_dynamic) const
{
	Assimp::Importer importer;
	LoadingContext loading_context;
	if (!LoadScene(filename, importer, loading_context))
	{
		hal::dout << "Failed to load dynamic model " << filename << std::endl;
		return;
	}
	// dynamic
	LoadBones(loading_context, mode_data_dynamic);
}

void ModelLoader::AiModelLoader::LoadModelDataDynamic(const std::string& filename, ModelData& model_data, ModelDataSkinned& mode_data_dynamic) const
{
	Assimp::Importer importer;
	LoadingContext loading_context;
	if (!LoadScene(filename, importer, loading_context))
	{
		hal::dout << "Failed to load dynamic model " << filename << std::endl;
		return;
	}
	// static
	LoadModelData(loading_context, model_data);
	// dynamic
	LoadBones(loading_context, mode_data_dynamic);
}

void ModelLoader::AiModelLoader::LoadModelData(const LoadingContext& loading_context, ModelData& model_data) const
{
	auto scene = loading_context.scene;
	// static meshes ====
	model_data.meshes.clear();
	model_data.meshes.reserve(scene->mNumMeshes);
	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		MeshData mesh{};
		LoadMesh(loading_context, scene->mMeshes[i], mesh);
		model_data.meshes.push_back(mesh);
	}
	// material ====
	model_data.textures.clear();
	model_data.textures.reserve(scene->mNumMaterials);
	for (unsigned int i = 0; i < scene->mNumMaterials; i++)
	{
		ModelTextureData model_textures{};
		LoadMaterial(loading_context, scene->mMaterials[i], model_textures);
		model_data.textures.push_back(model_textures);
	}
}

void ModelLoader::AiModelLoader::LoadMesh(const LoadingContext& loading_context, const aiMesh* mesh, MeshData& mesh_data) const
{
	// buffer
	CD3D11_BUFFER_DESC buffer_desc(0, D3D11_BIND_VERTEX_BUFFER);
	D3D11_SUBRESOURCE_DATA init_data{ nullptr, 0, 0 };

	// mesh vertex buffer =====
	const uint32_t numVertices{ mesh->mNumVertices };
	mesh_data.m_vertex_count = numVertices;
	// position
	if (numVertices > 0)
	{
		init_data.pSysMem = mesh->mVertices;
		buffer_desc.ByteWidth = mesh->mNumVertices * sizeof(DirectX::XMFLOAT3);
		m_device->CreateBuffer(&buffer_desc, &init_data, mesh_data.m_positions.GetAddressOf());
	}
	// texture
	uint32_t numUVs{ 0 };
	while (mesh->mTextureCoords[numUVs])
	{
		++numUVs;
	}
	if (numUVs > 0)
	{
		mesh_data.m_pTexcoordArrays.resize(numUVs);
		for (uint32_t i = 0; i < numUVs; ++i)
		{
			std::vector<DirectX::XMFLOAT2> uvs(numVertices);
			for (uint32_t j = 0; j < numVertices; ++j)
			{
				memcpy_s(&uvs[j], sizeof(DirectX::XMFLOAT2),
					mesh->mTextureCoords[i] + j, sizeof(DirectX::XMFLOAT2));
			}
			init_data.pSysMem = uvs.data();
			buffer_desc.ByteWidth = numVertices * sizeof(DirectX::XMFLOAT2);
			m_device->CreateBuffer(&buffer_desc, &init_data, mesh_data.m_pTexcoordArrays[i].GetAddressOf());
			break; // TODO: assuming one set of uv
		}
	}
	// normal
	if (mesh->HasNormals())
	{
		init_data.pSysMem = mesh->mNormals;
		buffer_desc.ByteWidth = numVertices * sizeof(DirectX::XMFLOAT3);
		m_device->CreateBuffer(&buffer_desc, &init_data, mesh_data.m_normals.GetAddressOf());
	}

	// mesh index buffer ====
	const uint32_t numFaces{ mesh->mNumFaces };
	const uint32_t numIndices{ numFaces * 3 };
	if (numIndices < 65535)
	{
		std::vector<uint16_t> indices(numIndices);
		for (size_t i = 0; i < numFaces; ++i)
		{
			indices[i * 3] = static_cast<uint16_t>(mesh->mFaces[i].mIndices[0]);
			indices[i * 3 + 1] = static_cast<uint16_t>(mesh->mFaces[i].mIndices[1]);
			indices[i * 3 + 2] = static_cast<uint16_t>(mesh->mFaces[i].mIndices[2]);
		}
		buffer_desc = CD3D11_BUFFER_DESC(numIndices * sizeof(uint16_t), D3D11_BIND_INDEX_BUFFER);
		init_data.pSysMem = indices.data();
		m_device->CreateBuffer(&buffer_desc, &init_data, mesh_data.m_indices.GetAddressOf());
	}
	else
	{
		std::vector<uint32_t> indices(numIndices);
		for (size_t i = 0; i < numFaces; ++i)
		{
			memcpy_s(indices.data() + i * 3, sizeof(uint32_t) * 3,
				mesh->mFaces[i].mIndices, sizeof(uint32_t) * 3);
		}
		buffer_desc = CD3D11_BUFFER_DESC(numIndices * sizeof(uint32_t), D3D11_BIND_INDEX_BUFFER);
		init_data.pSysMem = indices.data();
		m_device->CreateBuffer(&buffer_desc, &init_data, mesh_data.m_indices.GetAddressOf());
	}
	mesh_data.m_index_count = numIndices;

	// materials ====
	mesh_data.m_material_index = mesh->mMaterialIndex;
}

void  ModelLoader::AiModelLoader::LoadMaterial(const LoadingContext& loading_context, const aiMaterial* material, ModelTextureData& mesh_texture_data) const
{
	mesh_texture_data.m_diffuse = LoadTextureOfType(loading_context, material, aiTextureType_DIFFUSE);
	// temp
	if (!mesh_texture_data.m_diffuse.Get())
	{
		mesh_texture_data.m_diffuse = TextureLoader::LoadTextureFromFile(
			m_device, "asset/texture/placeholder/white.png", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	}
	mesh_texture_data.m_metallic = LoadTextureOfType(loading_context, material, aiTextureType_METALNESS);
	if (!mesh_texture_data.m_metallic.Get())
	{
		mesh_texture_data.m_metallic = TextureLoader::LoadTextureFromFile(
			m_device, "asset/texture/placeholder/black.png", DXGI_FORMAT_R8G8B8A8_UNORM);
	}

	// TODO: other textures
	// debug
	for (aiTextureType type = aiTextureType_DIFFUSE; type <= aiTextureType_ANISOTROPY; type = aiTextureType(type + 1))
	{
		if (material->GetTextureCount(type) == 0)
		{
			continue;
		}
		aiString texture_path;
		if (material->GetTexture(type, 0, &texture_path) == AI_SUCCESS)
		{
			const aiTexture* embeddedTex = loading_context.scene->GetEmbeddedTexture(texture_path.C_Str());
			if (embeddedTex)
			{
				hal::dout << "Embedded texture found: " << type << " " << loading_context.model_path << " " << texture_path.C_Str() << std::endl;
			}
			else
			{
				std::string texFile = loading_context.root_path + '/' + texture_path.C_Str();
				hal::dout << "Texture file found: " << type << " " << loading_context.model_path << " " << texFile.c_str() << std::endl;
			}
		}
	}
}

ComPtr<ID3D11ShaderResourceView> ModelLoader::AiModelLoader::LoadTextureOfType(
	const LoadingContext& loading_context, const aiMaterial* material, enum aiTextureType type) const
{
	if (material->GetTextureCount(type) == 0)
	{
		return nullptr;
	}

	aiString texture_path;
	if (material->GetTexture(type, 0, &texture_path) == AI_SUCCESS)
	{

		const aiTexture* embeddedTex = loading_context.scene->GetEmbeddedTexture(texture_path.C_Str());
		if (embeddedTex)
		{
			hal::dout << "Embedded texture found: " << type << texture_path.C_Str() << std::endl;
			if (embeddedTex->mHeight == 0)
			{
				bool srgb = type == aiTextureType_DIFFUSE;
				auto texture = TextureLoader::LoadTextureFromMemory(m_device, embeddedTex, srgb);
				assert(texture.Get());
				return texture;
			}
			else
			{
				return nullptr;
			}
		}
		else
		{
			DXGI_FORMAT format = type == aiTextureType_DIFFUSE ?
				DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
			std::string texFile = loading_context.root_path + '/' + texture_path.C_Str();
			hal::dout << "Texture file found: " << texFile.c_str() << std::endl;
			auto texture = TextureLoader::LoadTextureFromFile(m_device, texFile, format);
			assert(texture.Get());
			return texture;
		}
	}
	return nullptr;
}

void ModelLoader::AiModelLoader::LoadBones(const LoadingContext& loading_context, ModelDataSkinned& model_data_dynamic) const
{
	// load mesh's bone info
	auto scene = loading_context.scene;
	model_data_dynamic.meshes_skinned.clear();
	model_data_dynamic.meshes_skinned.reserve(scene->mNumMeshes);
	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		MeshDataSkinned mesh_dynamic{};
		LoadMeshBones(loading_context, scene->mMeshes[i], model_data_dynamic.bone_info_map, mesh_dynamic);
		model_data_dynamic.meshes_skinned.push_back(mesh_dynamic);
	}
	// load bone hierarchy ====
	aiMatrix4x4 global_inverse_tranform = scene->mRootNode->mTransformation;
	global_inverse_tranform = global_inverse_tranform.Inverse();
	LoadAssimpHierarchy(scene->mRootNode, global_inverse_tranform, model_data_dynamic.bone_info_map);
	// create buffers
	auto& bone_info_map = model_data_dynamic.bone_info_map;
	const int MAX_BONE_NUM = 200;
	const int bone_num = 200;// bone_info_map.size(); // TODO
	assert(bone_num <= MAX_BONE_NUM);
	{
		//D3D11_BUFFER_DESC desc{};
		//desc.ByteWidth = sizeof(DirectX::XMFLOAT4X4) * MAX_BONE_NUM;
		//desc.Usage = D3D11_USAGE_DYNAMIC; //D3D11_USAGE_DEFAULT;
		//desc.Usage = D3D11_USAGE_DEFAULT;
		//desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		//desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // 0;
		//desc.CPUAccessFlags = 0;
		//desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		//desc.StructureByteStride = sizeof(DirectX::XMFLOAT4X4);
		//HR(m_device->CreateBuffer(&desc, nullptr, model_data_dynamic.bone_matrix_buffer.GetAddressOf()));

		//D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		//srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		//srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		//srvDesc.Buffer.ElementOffset = 0;
		//srvDesc.Buffer.ElementWidth = MAX_BONE_NUM;


		//HR(m_device->CreateShaderResourceView(
		//	model_data_dynamic.bone_matrix_buffer.Get(), &srvDesc, model_data_dynamic.bone_matrix_srv.GetAddressOf()));


		Buffer::CreateStructuredBuffer(
			m_device,
			sizeof(DirectX::XMFLOAT4X4),
			bone_num,
			model_data_dynamic.bone_matrix_buffer,
			model_data_dynamic.bone_matrix_srv
		);
	}

	{
		auto& bone_info_map = model_data_dynamic.bone_info_map;
		std::array<DirectX::XMFLOAT4X4, MAX_BONE_NUM> bone_matrix{};
		model_data_dynamic.bone_offset_local.clear();
		model_data_dynamic.bone_offset_local.resize(bone_num); // TODO
		for (auto it = bone_info_map.begin(); it != bone_info_map.end(); ++it)
		{
			DirectX::XMMATRIX trans = XMLoadFloat4x4(&it->second.offset);
			DirectX::XMStoreFloat4x4(&bone_matrix[it->second.id], XMMatrixTranspose(trans));

			model_data_dynamic.bone_offset_local[it->second.id] = it->second.offset_local;
		}
		//m_context->UpdateSubresource(
		//	model_data_dynamic.bone_matrix_buffer.Get(), 0, nullptr, bone_matrix.data(), 0, 0);
		D3D11_MAPPED_SUBRESOURCE msr;
		m_context->Map(model_data_dynamic.bone_matrix_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
		memcpy(msr.pData, bone_matrix.data(), sizeof(DirectX::XMFLOAT4X4) * bone_num);
		m_context->Unmap(model_data_dynamic.bone_matrix_buffer.Get(), 0);
	}
}

void ModelLoader::AiModelLoader::LoadMeshBones(
	const LoadingContext& loading_context, const aiMesh* mesh,
	std::unordered_map<std::string, BoneInfo>& bone_info_map, MeshDataSkinned& mesh_data_dynamic) const
{
	// data to be passed to buffer
	const auto num_vertices{ mesh->mNumVertices };
	std::vector<XMINT4> bone_ids(num_vertices, { -1, -1, -1, -1 });
	std::vector<XMFLOAT4> bone_weights(num_vertices, { 0.0f, 0.0f, 0.0f, 0.0f });
	// bone id and weight ====
	int bone_cnt{ 0 };
	for (int i = 0; i < mesh->mNumBones; ++i)
	{
		int bone_id = -1;
		std::string boneName = mesh->mBones[i]->mName.C_Str();
		if (bone_info_map.find(boneName) == bone_info_map.end())
		{
			BoneInfo bone_info;
			bone_info.id = bone_cnt;
			bone_info.offset_local = mesh->mBones[i]->mOffsetMatrix;
			bone_info.offset = AiToXMFloat4x4(bone_info.offset_local);
			bone_info_map[boneName] = bone_info;
			bone_id = bone_cnt;
			bone_cnt++;
		}
		else
		{
			bone_id = bone_info_map[boneName].id;
		}
		assert(bone_id != -1);
		auto weights = mesh->mBones[i]->mWeights;
		int numWeights = mesh->mBones[i]->mNumWeights;
		// update vertex data, for each vertex
		for (int vertex_index = 0; vertex_index < numWeights; ++vertex_index)
		{
			int vertex_id = weights[vertex_index].mVertexId;
			float weight = weights[vertex_index].mWeight;
			assert(vertex_id <= mesh->mNumVertices);
			// find next empty slot of the vertex
			if (bone_ids[vertex_id].x < 0)
			{
				bone_ids[vertex_id].x = bone_id;
				bone_weights[vertex_id].x = weight;
			}
			else if (bone_ids[vertex_id].y < 0)
			{
				bone_ids[vertex_id].y = bone_id;
				bone_weights[vertex_id].y = weight;
			}
			else if (bone_ids[vertex_id].z < 0)
			{
				bone_ids[vertex_id].z = bone_id;
				bone_weights[vertex_id].z = weight;
			}
			else if (bone_ids[vertex_id].w < 0)
			{
				bone_ids[vertex_id].w = bone_id;
				bone_weights[vertex_id].w = weight;
			}
		}
	}
	// create buffers
	{
		CD3D11_BUFFER_DESC buffer_desc(0, D3D11_BIND_VERTEX_BUFFER);
		D3D11_SUBRESOURCE_DATA init_data{ nullptr, 0, 0 };
		// bone id
		init_data.pSysMem = bone_ids.data();
		buffer_desc.ByteWidth = num_vertices * sizeof(XMINT4);
		HR(m_device->CreateBuffer(&buffer_desc, &init_data, mesh_data_dynamic.bone_ids.GetAddressOf()));
		// bone weight
		init_data.pSysMem = bone_weights.data();
		buffer_desc.ByteWidth = num_vertices * sizeof(XMFLOAT4);
		HR(m_device->CreateBuffer(&buffer_desc, &init_data, mesh_data_dynamic.bone_weights.GetAddressOf()));
	}
}

void ModelLoader::AiModelLoader::LoadAnimationData(
	const std::string& filename, const ModelDataSkinned& model_data_dynamic, AnimationData& animation_data) const
{
	Assimp::Importer importer;
	importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
	LoadingContext loading_context;
	if (!LoadScene(filename, importer, loading_context))
	{
		hal::dout << "Failed to load model " << filename << std::endl;
		return;
	}

	auto scene = loading_context.scene;
	if (!scene->HasAnimations() || !scene->mAnimations[0])
	{
		hal::dout << "ERROR::ASSIMP::" << "no animation" << std::endl;
		return;
	}

	aiAnimation* pAnimation = scene->mAnimations[0];

	animation_data.duration = static_cast<float>(pAnimation->mDuration);
	animation_data.ticks_per_second = static_cast<float>(pAnimation->mTicksPerSecond);

	std::unordered_map<std::string, int> anim_node_map;
	animation_data.node_keys.clear();
	animation_data.node_keys.reserve(pAnimation->mNumChannels);
	int index{ 0 };
	for (int i = 0; i < pAnimation->mNumChannels; i++)
	{
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];
		std::string anim_node_name{ pNodeAnim->mNodeName.data };
		if (anim_node_map.find(anim_node_name) == anim_node_map.end())
		{
			anim_node_map.emplace(anim_node_name, index);
			index++;

			// load data
			NodeAnimationKeys animation_node{};
			animation_node.position_keys.resize(pNodeAnim->mNumPositionKeys);
			for (int j = 0; j < pNodeAnim->mNumPositionKeys; j++)
			{
				//animation_node.position_keys[j] = aiVectorKey{ pNodeAnim->mPositionKeys[j] };
				animation_node.position_keys[j].mTime = pNodeAnim->mPositionKeys[j].mTime;
				animation_node.position_keys[j].mValue = pNodeAnim->mPositionKeys[j].mValue;
			}
			animation_node.scaling_keys.resize(pNodeAnim->mNumScalingKeys);
			for (int j = 0; j < pNodeAnim->mNumScalingKeys; j++)
			{
				//animation_node.scaling_keys[j] = aiVectorKey{ pNodeAnim->mScalingKeys[j] };
				animation_node.scaling_keys[j].mTime = pNodeAnim->mScalingKeys[j].mTime;
				animation_node.scaling_keys[j].mValue = pNodeAnim->mScalingKeys[j].mValue;
			}
			animation_node.rotation_keys.resize(pNodeAnim->mNumRotationKeys);
			for (int j = 0; j < pNodeAnim->mNumRotationKeys; j++)
			{
				//animation_node.rotation_keys[j] = aiQuatKey{ pNodeAnim->mRotationKeys[j] };
				animation_node.rotation_keys[j].mTime = pNodeAnim->mRotationKeys[j].mTime;
				animation_node.rotation_keys[j].mValue = pNodeAnim->mRotationKeys[j].mValue;
			}

			animation_data.node_keys.push_back(animation_node);
		}
	}
	LoadAssimpHierarchyAnimation(scene->mRootNode, -1, model_data_dynamic.bone_info_map, anim_node_map, animation_data.animated_nodes);
}

// ==== model loader start =====
ModelLoader::ModelLoader() = default;
ModelLoader::~ModelLoader() = default;

void ModelLoader::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	// デバイスとデバイスコンテキストの保存
	m_device = device;
	m_context = context;

	m_model_loader = std::make_unique<AiModelLoader>(m_device, m_context);

	m_models_pool = std::make_unique<ResourcePool<ModelData> >();
	m_models_skinned_pool = std::make_unique<ResourcePool<ModelDataSkinned> >();
	m_models_skinned_matrix_pool = std::make_unique<ResourcePool<ModelDataSkinnedFrame> >();
	m_animations_pool = std::make_unique<ResourcePool<AnimationData> >();
	m_mesh_geometry_pool = std::make_unique<ResourcePool<MeshGeometry> >();
	m_buffer_view_pool = std::make_unique<ResourcePool<BufferView> >();
}

const ModelData& ModelLoader::GetModel(int model_id) const
{
	return m_models_pool->Get(model_id);
}

const ModelDataSkinned& ModelLoader::GetModelSkinned(int model_id_dyamic) const
{
	return m_models_skinned_pool->Get(model_id_dyamic);
}

const AnimationData& ModelLoader::GetAnimation(int animation_id) const
{
	return m_animations_pool->Get(animation_id);
}

int ModelLoader::CreateModelDataSkinnedMatrix(int model_id_skinned)
{
	const auto& model_data_dynamic = GetModelSkinned(model_id_skinned);
	size_t bone_num = model_data_dynamic.bone_info_map.size();
	ModelDataSkinnedFrame data{};
	Buffer::CreateStructuredBuffer(
		m_device,
		sizeof(DirectX::XMFLOAT4X4),
		bone_num,
		data.bone_matrix_buffer,
		data.bone_matrix_srv);
	int id = m_models_skinned_matrix_pool->AddUniqueWithGeneratedKey("", data);
	return id;
}

void ModelLoader::UpdateModelSkinnedMatrix(int model_id_skinned_frame, const std::vector<DirectX::XMFLOAT4X4>& bone_matrices) const
{
	auto& data = m_models_skinned_matrix_pool->Get(model_id_skinned_frame);
	// query buffer size
	D3D11_BUFFER_DESC desc{};
	data.bone_matrix_buffer->GetDesc(&desc);
	size_t bufferBytes = desc.ByteWidth;
	size_t copyBytes = std::min(bufferBytes, bone_matrices.size() * sizeof(DirectX::XMFLOAT4X4));

	D3D11_MAPPED_SUBRESOURCE msr;
	m_context->Map(data.bone_matrix_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, bone_matrices.data(), copyBytes);
	m_context->Unmap(data.bone_matrix_buffer.Get(), 0);
}

const ModelDataSkinnedFrame& ModelLoader::GetModelSkinnedMatrix(int model_id_skinned_frame) const
{
	return m_models_skinned_matrix_pool->Get(model_id_skinned_frame);
}

void ModelLoader::GetOrLoadModel(ModelDesc& model_desc)
{
	if (model_desc.has_animation)
	{
		GetOrLoadModelDynamic(model_desc.model_file, model_desc.model_id, model_desc.model_skinning_id);
		const auto& model_data_dynamic = GetModelSkinned(model_desc.model_skinning_id);
		for (auto& anim_desc : model_desc.animations_desc)
		{
			anim_desc.animation_id = GetOrLoadAnimation(anim_desc.animation_file, model_data_dynamic);
		}
	}
	else
	{
		model_desc.model_id = GetOrLoadModel(model_desc.model_file);
	}
}

void ModelLoader::LoadMeshGeometry(ModelDesc& model_desc, const MeshGeometry& mesh)
{
	// add geometry data
	int geometry_id = m_mesh_geometry_pool->GetId(model_desc.model_file);
	assert(geometry_id < 0); // already exists
	geometry_id = m_mesh_geometry_pool->AddIfNotExists(model_desc.model_file, mesh);
	model_desc.geometry_id = geometry_id;

	// add model data
	model_desc.model_id = CreateModelFromGeometry(model_desc.model_file, mesh);
}

void ModelLoader::LoadMeshGeometryUnique(ModelDesc& model_desc, const MeshGeometry& mesh)
{
	// add geometry data with unique generated key
	model_desc.model_file = m_mesh_geometry_pool->GetGeneratedKey(model_desc.model_file);
	int geometry_id = m_mesh_geometry_pool->AddUnique(model_desc.model_file, mesh);
	model_desc.geometry_id = geometry_id;
	// add model data
	model_desc.model_id = CreateModelFromGeometry(model_desc.model_file, mesh);
}

const MeshGeometry& ModelLoader::GetMeshGeometry(int mesh_geometry_id) const
{
	return m_mesh_geometry_pool->Get(mesh_geometry_id);
}

MeshGeometry& ModelLoader::GetMeshGeometry(int mesh_geometry_id)
{
	return m_mesh_geometry_pool->Get(mesh_geometry_id);
}

int ModelLoader::AddBufferView(BufferView buffer_view)
{
	return m_buffer_view_pool->AddUniqueWithGeneratedKey("buffer_view", buffer_view);
}

BufferView& ModelLoader::GetBufferView(int buffer_view_id)
{
	return m_buffer_view_pool->Get(buffer_view_id);
}

const BufferView& ModelLoader::GetBufferView(int buffer_view_id) const
{
	return m_buffer_view_pool->Get(buffer_view_id);
}

int ModelLoader::GetOrLoadModel(const std::string& filename)
{
	// try get
	int id = m_models_pool->GetId(filename);
	if (id >= 0)
	{
		return id;
	}
	// create
	ModelData model_data{};
	m_model_loader->LoadModelData(filename, model_data);
	id = m_models_pool->AddIfNotExists(filename, model_data);
	return id;
}

void ModelLoader::GetOrLoadModelDynamic(const std::string& filename, int& model_id, int& model_id_dynamic)
{
	// static
	model_id = m_models_pool->GetId(filename);
	// dynamic
	model_id_dynamic = m_models_skinned_pool->GetId(filename);
	if (model_id >= 0 && model_id_dynamic >= 0)
	{
		// both loaded
		return;
	}
	if (model_id >= 0)
	{
		// load static
		ModelDataSkinned model_data_dynamic{};
		m_model_loader->LoadModelDataDynamic(filename, model_data_dynamic);
		model_id_dynamic = m_models_skinned_pool->AddIfNotExists(filename, model_data_dynamic);
		return;
	}
	if (model_id_dynamic >= 0)
	{
		// load skinned
		ModelData model_data{};
		m_model_loader->LoadModelData(filename, model_data);
		model_id = m_models_pool->AddIfNotExists(filename, model_data);
		return;

	}
	// load both
	ModelData model_data{};
	ModelDataSkinned model_data_dynamic{};
	m_model_loader->LoadModelDataDynamic(filename, model_data, model_data_dynamic);
	model_id = m_models_pool->AddIfNotExists(filename, model_data);
	model_id_dynamic = m_models_skinned_pool->AddIfNotExists(filename, model_data_dynamic);
}

int ModelLoader::GetOrLoadAnimation(const std::string filename, const ModelDataSkinned& model_data_dynamic)
{
	// try get
	int id = m_animations_pool->GetId(filename);
	if (id >= 0)
	{
		return id;
	}
	// create
	AnimationData animation_data{};
	m_model_loader->LoadAnimationData(filename, model_data_dynamic, animation_data);
	id = m_animations_pool->AddIfNotExists(filename, animation_data);
	return id;
}

int ModelLoader::CreateModelFromGeometry(const std::string& key, const MeshGeometry& mesh_geometry)
{
	MeshData mesh_data{};
	CD3D11_BUFFER_DESC buffer_desc(0, D3D11_BIND_VERTEX_BUFFER);
	D3D11_SUBRESOURCE_DATA init_data{ nullptr, 0, 0 };

	const size_t numVertices{ mesh_geometry.positions.size() };
	// vertex
	mesh_data.m_vertex_count = numVertices;
	if (numVertices > 0)
	{
		// position
		buffer_desc.ByteWidth = numVertices * sizeof(Vector3);
		init_data.pSysMem = mesh_geometry.positions.data();
		m_device->CreateBuffer(&buffer_desc, &init_data, mesh_data.m_positions.GetAddressOf());
		// normal
		init_data.pSysMem = mesh_geometry.normals.data();
		m_device->CreateBuffer(&buffer_desc, &init_data, mesh_data.m_normals.GetAddressOf());
		// uv
		mesh_data.m_pTexcoordArrays.resize(1);
		buffer_desc.ByteWidth = numVertices * sizeof(Vector2);
		init_data.pSysMem = mesh_geometry.texcoords.data();
		m_device->CreateBuffer(&buffer_desc, &init_data, mesh_data.m_pTexcoordArrays[0].GetAddressOf());
		// tangent
		if (!mesh_geometry.tangents.empty())
		{
			// TODO: compress
			buffer_desc.ByteWidth = numVertices * sizeof(Vector4);
			init_data.pSysMem = mesh_geometry.tangents.data();
			m_device->CreateBuffer(&buffer_desc, &init_data, mesh_data.m_tangents.GetAddressOf());
		}
		// color
		if (!mesh_geometry.colors.empty())
		{
			buffer_desc.ByteWidth = numVertices * sizeof(Vector4);
			init_data.pSysMem = mesh_geometry.colors.data();
			m_device->CreateBuffer(&buffer_desc, &init_data, mesh_data.m_colors.GetAddressOf());
		}
	}

	// index
	const size_t numIndices{ mesh_geometry.indices.size() };
	mesh_data.m_index_count = numIndices;
	if (numIndices > 0)
	{
		if (numIndices < 65535)
		{
			std::vector<uint16_t> indices_16(numIndices);
			for (size_t i = 0; i < numIndices; ++i)
			{
				indices_16[i] = static_cast<uint16_t>(mesh_geometry.indices[i]);
			}
			buffer_desc = CD3D11_BUFFER_DESC(numIndices * sizeof(uint16_t), D3D11_BIND_INDEX_BUFFER);
			init_data.pSysMem = indices_16.data();
			m_device->CreateBuffer(&buffer_desc, &init_data, mesh_data.m_indices.GetAddressOf());
		}
		else
		{
			CD3D11_BUFFER_DESC index_buffer_desc = CD3D11_BUFFER_DESC(numIndices * sizeof(uint32_t), D3D11_BIND_INDEX_BUFFER);
			init_data.pSysMem = mesh_geometry.indices.data();
			m_device->CreateBuffer(&index_buffer_desc, &init_data, mesh_data.m_indices.GetAddressOf());
		}
	}

	// model data
	ModelData model_data{};
	model_data.meshes.push_back(mesh_data);
	int id = m_models_pool->AddUnique(key, model_data);
	return id;
}



// ==== model loader end =====
