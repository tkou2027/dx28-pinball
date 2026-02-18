#include "component_camera.h"
#include "object/game_object.h"

#include "global_context.h"
#include "render/render_system.h"
#include "math/camera_math.h"

using namespace DirectX;

void ComponentCamera::InitializeCamera(
	const CameraUsageConfig& config, const CameraShapeConfig& shape)
{
	assert(!m_initialized);
	m_initialized = true;
	m_usage_config = config;
	m_shape_config = shape;
	auto& render_swap_data = g_global_context.m_render_system->GetSwapContext().GetSwapData();
	render_swap_data.camera_data.cameras_to_add.push_back(m_usage_config);
}

void ComponentCamera::Update()
{
	m_position = m_object.lock()->GetTransform().GetPositionGlobal();
	auto& render_swap_data = g_global_context.m_render_system->GetSwapContext().GetSwapData();
	CameraUpdateData update_data{};
	update_data.config = m_usage_config;
	update_data.shape = m_shape_config;
	update_data.position = m_position;
	update_data.target = m_target;
	update_data.up = m_up;
	render_swap_data.camera_data.cameras_to_update.push_back(update_data);
}

void ComponentCamera::Finalize()
{
	auto& render_swap_data = g_global_context.m_render_system->GetSwapContext().GetSwapData();
	render_swap_data.camera_data.cameras_to_remove.push_back(m_usage_config);
}

void ComponentCamera::SetViewMatrix(const DirectX::XMMATRIX& view_matrix)
{
	CameraMath::GetVectorsFromViewMatrix(
		view_matrix,
		m_position,
		m_target,
		m_up
	);
}

void ComponentCamera::GetDirectionXZNormalized(Vector3& out_forward, Vector3& out_right) const
{
	CameraMath::CalculateDirectionXZNormalized(
		m_object.lock()->GetTransform().GetPositionGlobal(),
		m_target,
		out_forward,
		out_right
	);
}

//Vector3 ComponentCamera::GetForward() const
//{
//	auto obj = m_object.lock();
//	assert(obj);
//
//	// world-space forward = target - position
//	const Vector3 pos = obj->GetTransform().GetPositionGlobal();
//	XMVECTOR posV = pos.ToXMVECTOR();
//	XMVECTOR targetV = m_target.ToXMVECTOR();
//
//	XMVECTOR dir = XMVectorSubtract(targetV, posV);
//	// if direction is degenerate return zero vector
//	if (XMVector3Equal(dir, XMVectorZero()))
//	{
//		return Vector3{};
//	}
//
//	// flatten Y so movement stays on the horizontal plane, then normalize
//	XMFLOAT3 dirF;
//	XMStoreFloat3(&dirF, dir);
//	dirF.y = 0.0f;
//	XMVECTOR dirFlat = XMLoadFloat3(&dirF);
//	if (XMVector3Equal(dirFlat, XMVectorZero()))
//	{
//		return Vector3{};
//	}
//	dirFlat = XMVector3Normalize(dirFlat);
//
//	return Vector3::FromXMVECTOR(dirFlat);
//}
//
//Vector3 ComponentCamera::GetRight() const
//{
//	auto obj = m_object.lock();
//	assert(obj);
//
//	// compute forward in world space first (not the view matrix)
//	const Vector3 pos = obj->GetTransform().GetPositionGlobal();
//	XMVECTOR posV = pos.ToXMVECTOR();
//	XMVECTOR targetV = m_target.ToXMVECTOR();
//	XMVECTOR upV = m_up.ToXMVECTOR();
//
//	XMVECTOR forward = XMVectorSubtract(targetV, posV);
//	// flatten forward on XZ and normalize
//	XMFLOAT3 fF;
//	XMStoreFloat3(&fF, forward);
//	fF.y = 0.0f;
//	XMVECTOR fFlat = XMLoadFloat3(&fF);
//	if (XMVector3Equal(fFlat, XMVectorZero()))
//	{
//		// fallback: use right from up vector (camera looking straight up/down)
//		XMVECTOR fallbackRight = XMVector3Normalize(XMVector3Cross(upV, XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)));
//		XMFLOAT3 rf;
//		XMStoreFloat3(&rf, fallbackRight);
//		Vector3 right_vec = Vector3::FromXMVECTOR(XMLoadFloat3(&rf));
//		right_vec.y = 0.0f;
//		right_vec.Normalize();
//		return right_vec;
//	}
//
//	fFlat = XMVector3Normalize(fFlat);
//
//	// right = cross(up, forward) (gives camera right in world space)
//	XMVECTOR right = XMVector3Cross(upV, fFlat);
//	// project to horizontal plane and normalize
//	XMFLOAT3 rF;
//	XMStoreFloat3(&rF, right);
//	rF.y = 0.0f;
//	XMVECTOR rFlat = XMLoadFloat3(&rF);
//	if (XMVector3Equal(rFlat, XMVectorZero()))
//	{
//		return Vector3{};
//	}
//	rFlat = XMVector3Normalize(rFlat);
//
//	return Vector3::FromXMVECTOR(rFlat);
//}
