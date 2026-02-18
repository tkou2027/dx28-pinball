#include "component_light.h"
#include "object/game_object.h"

Vector3 ComponentLight::GetPosition() const
{
	return m_object.lock()->GetTransform().GetPositionGlobal();
}