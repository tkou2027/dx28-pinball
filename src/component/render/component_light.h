#pragma once
#include "component/component.h"
#include "math/vector3.h"

// TODO light type
class ComponentLight : public Component
{
public:
	ComponentLight() { m_layer = ComponentLayer::LIGHT; }
	virtual ~ComponentLight() {}
	void SetColor(const Vector3& color) { m_color = color; }
	Vector3 GetColor() const { return m_color; }
	Vector3 GetPosition() const;
private:
	Vector3 m_color{};
};