#include "model_state.h"
#include "global_context.h"
#include "render/render_system.h"
#include "render/render_resource.h"
#include "render/resource/model_loader.h"
#include "platform/timer.h"

#include "util/debug_ostream.h"
#include "math/assimp_math.h"

static void UpdateMatrix(
	const AnimationData& animation_data,
	const ModelDataSkinned& model_data_dynamic,
	const float animation_time,
	std::vector<DirectX::XMFLOAT4X4>& bones_matrix)
{
	size_t num_bones = model_data_dynamic.bone_offset_local.size();
	bones_matrix.clear();
	bones_matrix.resize(num_bones); // TODO

	std::vector<aiMatrix4x4> global_transforms;
	global_transforms.resize(animation_data.animated_nodes.size());
	int node_id{ 0 };
	for (const auto& node : animation_data.animated_nodes)
	{
		// update local
		aiMatrix4x4 trans_local = node.transform_local;
		if (node.animation_index >= 0)
		{
			auto& anim_node = animation_data.node_keys[node.animation_index];
			aiVector3D position = AssimpMath::InterpolateVector3(anim_node.position_keys, animation_time);
			aiVector3D scaling = AssimpMath::InterpolateVector3(anim_node.scaling_keys, animation_time);
			aiQuaternion rotation = AssimpMath::InterpolateQuat(anim_node.rotation_keys, animation_time);
			//aiVector3D position = anim_node.position_keys[0].mValue;
			//aiVector3D scaling = anim_node.scaling_keys[0].mValue;
			// aiQuaternion rotation = anim_node.rotation_keys[0].mValue;

			trans_local = AssimpMath::BuildTransformMatrix(scaling, rotation, position);
		}

		aiMatrix4x4 global_transform; // identity
		// update offset
		if (node.parent_index >= 0)
		{
			global_transform = global_transforms[node.parent_index] * trans_local;
		}
		global_transforms[node_id] = global_transform;

		// save to bone matrix
		if (node.bone_index >= 0)
		{
			auto bone_mat = global_transform * model_data_dynamic.bone_offset_local[node.bone_index];
			bones_matrix[node.bone_index] = AssimpMath::AiToXMFloat4x4Transpose(bone_mat);
		}
		++node_id;
	}
}


Model::Model(const ModelDesc& model_desc, const MaterialDesc& material_desc, TransformNode3D* parent)
{
	m_model_desc = model_desc;

	auto& material_manager = g_global_context.m_render_system->GetRenderResource().GetMaterialManager();
	m_material_id = material_manager.AddMaterialDescWithGeneratedKey(material_desc);
	// TODO: material id

	m_transform.SetParent(parent);
	m_active = true;
}

Model::Model(const ModelDesc& model_desc, int material_id, TransformNode3D* parent)
{
	m_model_desc = model_desc;
	m_material_id = material_id;
	m_transform.SetParent(parent);
	m_instanced = true; // instanced by default when setting material with id
	m_active = true;
}

void Model::UpdateAnimation()
{
	if (!m_model_desc.has_animation)
	{
		return;
	}
	m_animation_state.Update(m_model_desc);

	//const auto& animation_desc = m_model_desc.animations_desc[m_animation_state.animation_index];
	//const auto& model_loader = g_global_context.m_render_system->GetRenderResource().GetModelLoader();
	//auto& animation_data = model_loader.GetAnimation(animation_desc.animation_id);
	//auto& model_data_dynamic = model_loader.GetModelSkinned(m_model_desc.model_skinning_id);

	//if (animation_data.duration > 0.0f)
	//{
	//	const float delta_time = g_global_context.m_timer->GetDeltaTime();
	//	m_animation_state.animation_t += animation_data.ticks_per_second * delta_time;
	//	if (m_animation_state.animation_t >= animation_data.duration - 0.01f)
	//	{
	//		if (animation_desc.play_loop)
	//		{
	//			m_animation_state.animation_t -= animation_data.duration;
	//			m_animation_state.animation_t = Math::Max(0.0f, m_animation_state.animation_t);
	//			m_animation_state.animation_t = 0;
	//		}
	//		else
	//		{
	//			m_animation_state.animation_t = animation_data.duration;
	//		}
	//	}
	//}
	//UpdateMatrix(animation_data, model_data_dynamic, m_animation_state.animation_t, m_animation_state.bones_matrix);
}

void Model::UpdateUVAnimation()
{
	m_uv_animation_state.Update(m_model_desc.uv_animation_desc);
}

MaterialDesc& Model::GetMaterialDesc()
{
	auto& material_manager = g_global_context.m_render_system->GetRenderResource().GetMaterialManager();
	return material_manager.GetMaterialDesc(m_material_id);
}

const MaterialDesc& Model::GetMaterialDesc() const
{
	auto& material_manager = g_global_context.m_render_system->GetRenderResource().GetMaterialManager();
	return material_manager.GetMaterialDesc(m_material_id);
}

void Model::GetRenderKey(ModelRenderKey& key) const
{
	key.material_id = m_material_id;
	key.model_id = m_model_desc.model_id;
	if (m_model_desc.has_animation)
	{
		key.model_type = ModelType::SKINNED;
		key.model_skinning_id = m_model_desc.model_skinning_id;
		key.model_skinning_matrix_id = m_animation_state.model_skinning_matrix_id;
	}
	else
	{
		key.model_type = m_instanced ? ModelType::INSTANCED : ModelType::STATIC;
		key.model_skinning_id = -1;
		key.model_skinning_matrix_id = -1;
	}
}

void Model::GetRenderInstance(ModelRenderInstance& instance)
{
	instance.model_matrix = m_transform.GetFloat4x4Global();
	
	const auto& frame_animations = m_model_desc.uv_animation_desc;
	const auto uv_rect = m_uv_animation_state.GetUVRect(m_model_desc.uv_animation_desc);
	instance.uv_offset = uv_rect.uv_offset;
	instance.uv_size = uv_rect.uv_size;
}

void AnimationState::Initialize(const ModelDesc& model_desc)
{
	if (!model_desc.has_animation)
	{
		return;
	}
	assert(model_skinning_matrix_id == -1);
	// create matrix resource
	auto& model_loader = g_global_context.m_render_system->GetRenderResource().GetModelLoader();
	model_skinning_matrix_id = model_loader.CreateModelDataSkinnedMatrix(model_desc.model_skinning_id);
}

void AnimationState::Update(const ModelDesc& model_desc)
{
	if (!model_desc.has_animation)
	{
		return;
	}

	if (model_skinning_matrix_id == -1)
	{
		Initialize(model_desc);
	}

	//if (!playing)
	//{
	//	return;
	//}

	const auto& animation_desc = model_desc.animations_desc.at(animation_index);
	const auto& model_loader = g_global_context.m_render_system->GetRenderResource().GetModelLoader();
	auto& animation_data = model_loader.GetAnimation(animation_desc.animation_id);
	auto& model_data_dynamic = model_loader.GetModelSkinned(model_desc.model_skinning_id);

	if (animation_data.duration > 0.0f)
	{
		const float delta_time = g_global_context.m_timer->GetDeltaTime();
		animation_t += animation_data.ticks_per_second * delta_time;
		if (animation_t >= animation_data.duration - 0.01f)
		{
			if (animation_desc.play_loop)
			{
				animation_t -= animation_data.duration;
				animation_t = Math::Max(0.0f, animation_t);
				animation_t = 0;
			}
			else
			{
				animation_t = animation_data.duration;
			}
		}
	}
	std::vector<DirectX::XMFLOAT4X4> bones_matrix;
	UpdateMatrix(animation_data, model_data_dynamic, animation_t, bones_matrix);
	// update buffer
	model_loader.UpdateModelSkinnedMatrix(model_skinning_matrix_id, bones_matrix);
}
