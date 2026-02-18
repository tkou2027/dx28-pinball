#pragma once
#include "model_desc.h"
#include "material_desc.h"
#include "uv_animation_state.h"

//struct ModelRenderInfo
//{
//	// model
//	ModelDesc model_desc;
//
//	// material
//	MaterialDesc m_material_desc; // ?
//	// animation
//	// std::vector<DirectX::XMFLOAT4X4> bones_matrix; // ?
//	int model_skinning_matrix_id{ -1 };
//
//	// per-instance data
//	// transform
//	DirectX::XMFLOAT4X4 model_matrix;
//	// uv animation
//	Vector2 uv_offset{};
//	Vector2 uv_size{ 1.0f, 1.0f };
//};

enum class ModelType : uint16_t
{
	STATIC,
	SKINNED,
	INSTANCED,

	MAX
};

struct ModelRenderKey
{
	ModelType model_type{ ModelType::STATIC };
	// material ====
	// MaterialDesc material_desc;
	int material_id{ -1 };
	// model ====
	// static
	int model_id{ -1 };
	// skeletal animation
	int16_t model_skinning_id{ -1 };
	int16_t model_skinning_matrix_id{ -1 };
};

struct ModelRenderInstance
{
	// per-instance data
	// transform
	DirectX::XMFLOAT4X4 model_matrix;
	// uv animation
	Vector2 uv_offset{};
	Vector2 uv_size{ 1.0f, 1.0f };
};

struct ModelRenderInfo
{
	ModelRenderKey key{};
	ModelRenderInstance instance{};
};

struct ModelRenderInfoInstanced
{
	ModelRenderKey key{};
	int instance_buffer_id{ -1 };
};

struct AnimationState
{
	// animation...
	int   animation_index{ 0 };
	bool  playing{ false };
	float animation_t{ 0.0f };
	int model_skinning_matrix_id{ -1 };
	std::vector<DirectX::XMFLOAT4X4> bones_matrix; // ?

	void Initialize(const ModelDesc& model_desc);
	void Update(const ModelDesc& model_desc);
};

class Model
{
public:
	Model() = default;
	Model(const ModelDesc& model_desc, const MaterialDesc& material_desc, TransformNode3D* parent);
	Model(const ModelDesc& model_desc, int material_id, TransformNode3D* parent);
	TransformNode3D& GetTransform() { return m_transform; }
	const TransformNode3D& GetTransform() const { return m_transform; }
	ModelDesc& GetModelDesc() { return m_model_desc; }
	const ModelDesc& GetModelDesc() const { return m_model_desc; }
	MaterialDesc& GetMaterialDesc();
	const MaterialDesc& GetMaterialDesc() const;
	UVAnimationState& GetUVAnimationState() { return m_uv_animation_state; }
	const UVAnimationState& GetUVAnimationState() const { return m_uv_animation_state; }
	bool GetActive() const { return m_active; }
	void SetActive(bool active) { m_active = active; }
	void Update()
	{
		// update animationÅc
		UpdateAnimation();
		// update uv animationÅc
		UpdateUVAnimation();
	}
	void GetRenderKey(ModelRenderKey& key) const;
	void GetRenderInstance(ModelRenderInstance& instance);
private:
	void UpdateAnimation();
	void UpdateUVAnimation();
	bool m_active{ false };
	ModelDesc m_model_desc{};
	// MaterialDesc m_material_desc{};
	int m_material_id{ -1 };
	TransformNode3D m_transform{};
	// states to update
	AnimationState m_animation_state;
	UVAnimationState m_uv_animation_state{};
	bool m_instanced{ false };
};
