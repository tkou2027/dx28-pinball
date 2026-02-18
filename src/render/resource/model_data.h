#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <wrl/client.h>
#include "assimp/matrix4x4.h"
#include "assimp/anim.h"


// static mesh ====
struct MeshData
{
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	// vertex buffer
	ComPtr<ID3D11Buffer> m_positions;
	std::vector<ComPtr<ID3D11Buffer>> m_pTexcoordArrays;
	ComPtr<ID3D11Buffer> m_normals;
	ComPtr<ID3D11Buffer> m_tangents;
	ComPtr<ID3D11Buffer> m_colors;

	// index buffer
	ComPtr<ID3D11Buffer> m_indices;
	uint32_t m_vertex_count{ 0 };
	uint32_t m_index_count{ 0 };
	uint32_t m_material_index{ 0 };

	DirectX::BoundingBox m_BoundingBox;
};

struct ModelTextureData
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_diffuse;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_metallic;
};

struct ModelData
{
	std::vector<MeshData> meshes;
	std::vector<ModelTextureData> textures;
};

struct BoneInfo
{
	int id;
	DirectX::XMFLOAT4X4 offset;
	aiMatrix4x4 offset_local;
};


// dynamic ====
struct MeshDataSkinned
{
	// vertex additional info
	Microsoft::WRL::ComPtr<ID3D11Buffer> bone_ids;
	Microsoft::WRL::ComPtr<ID3D11Buffer> bone_weights;
};

struct ModelDataSkinned
{
	std::vector<MeshDataSkinned> meshes_skinned;
	// model bone data
	Microsoft::WRL::ComPtr<ID3D11Buffer> bone_matrix_buffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> bone_matrix_srv;
	std::unordered_map<std::string, BoneInfo> bone_info_map; // cpu side, node name to bone info
	std::vector<aiMatrix4x4> bone_offset_local; // cpu side
};

// calculated matrix at some frame
struct ModelDataSkinnedFrame
{
	Microsoft::WRL::ComPtr<ID3D11Buffer> bone_matrix_buffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> bone_matrix_srv;
};

struct AnimatedNode
{
	int index{ -1 };
	int parent_index{ -1 };
	int animation_index{ -1 };
	int bone_index{ -1 };
	aiMatrix4x4 transform_local;
	aiMatrix4x4 offset;
};

struct NodeAnimationKeys
{
	std::vector<aiVectorKey> position_keys;
	std::vector<aiVectorKey> scaling_keys;
	std::vector<aiQuatKey> rotation_keys;
};

struct AnimationData
{
	// TODO: time
	float duration{ 0.0f };
	float ticks_per_second{ 0.0f };
	std::vector<NodeAnimationKeys> node_keys;
	std::vector<AnimatedNode> animated_nodes;
};

struct BufferView
{
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> buffer_srv;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> buffer_uav; // gpu update
};