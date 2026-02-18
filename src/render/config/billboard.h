#pragma once
#include "billboard_desc.h"
#include "math/transform3d.h"
#include "uv_animation_state.h"

class BillboardInstance
{
public:
	BillboardInstance() = default;
	BillboardInstance(const Vector2 size, const Vector3 position, TransformNode3D* parent)
		: m_size(size), m_position(position), m_transform_parent(parent) {
	};
	bool GetActive() const { return m_active; }
	void SetActive(bool active) { m_active = active; }
	void SetPosition(const Vector3 position) { m_position = position; }
	void SetSize(const Vector2 size) { m_size = size; }
	void Update(const BillboardDesc& desc);
	// computed
	Vector3 GetPosition() const;
	Vector2 GetSize() const;
	const UVRect& GetUV(const BillboardDesc& desc) const;
private:
	bool m_active{ false };
	Vector3 m_position{};
	Vector2 m_size{};
	TransformNode3D* m_transform_parent{ nullptr };
	UVAnimationState m_uv_animation_state{};
};
