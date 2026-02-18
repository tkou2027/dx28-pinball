#pragma once
#include "vector2.h"
#include "transform_node.h"

struct Transform2D
{
	Vector2 position{ 0.0f, 0.0f };
	Vector2 scale{ 1.0f, 1.0f };
	float rotation{ 0.0f };

	static Transform2D Join(const Transform2D& parent, const Transform2D& local)
	{
		const float parent_sin{ sinf(parent.rotation) };
		const float parent_cos{ cosf(parent.rotation) };
		return Transform2D{
			Vector2{
				parent.position.x + parent_cos * local.position.x - parent_sin * local.position.y,
				parent.position.y + parent_sin * local.position.x + parent_cos * local.position.y
			},
			Vector2{
				parent.scale.x * local.scale.x,
				parent.scale.y * local.scale.y
			},
			parent.rotation + local.rotation
		};
	}
};

class TransformNode2D : public TransformNode
{
public:
	// set local
	void SetPosition(const Vector2& position);
	void SetScale(const Vector2& scale);
	void SetRotation(float rotation);
	void SetPositionX(float x);
	void SetPositionY(float y);
	void SetScaleX(float x);
	void SetScaleY(float y);
	// get local
	const Vector2& GetPosition() const { return m_transform_local.position; }
	const Vector2& GetScale() const { return m_transform_local.scale; }
	float GetRotation() const { return m_transform_local.rotation; }
	// get global
	Transform2D GetTransformGlobal();
private:
	void RecalculateCached();
	Transform2D m_transform_local{};
	Transform2D m_transform_global{};
};