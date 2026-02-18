#include "transform2d.h"

void TransformNode2D::SetPosition(const Vector2& position)
{
	m_transform_local.position = position;
	MarkDirty();
}

void TransformNode2D::SetScale(const Vector2& scale)
{
	m_transform_local.scale = scale;
	MarkDirty();
}

void TransformNode2D::SetPositionX(float x)
{
	SetPosition({ x, m_transform_local.position.y });
}	

void TransformNode2D::SetPositionY(float y)
{
	SetPosition({ m_transform_local.position.x, y });
}

void TransformNode2D::SetScaleX(float x)
{
	SetScale({ x, m_transform_local.scale.y });
}

void TransformNode2D::SetScaleY(float y)
{
	SetScale({ m_transform_local.scale.x, y });
}

void TransformNode2D::SetRotation(float rotation)
{
	m_transform_local.rotation = rotation;
	MarkDirty();
}

Transform2D TransformNode2D::GetTransformGlobal()
{
	if (m_dirty)
	{
		RecalculateCached();
	}
	return m_transform_global;
}

void TransformNode2D::RecalculateCached()
{
	if (auto parent = m_parent)
	{
		m_transform_global = Transform2D::Join(dynamic_cast<TransformNode2D*>(m_parent)->GetTransformGlobal(), m_transform_local);
	}
	else
	{
		m_transform_global = m_transform_local;
	}
}