#include "move_controller.h"
#include "util/tween.h"

void MoveController::MoveTransform(TransformNode3D& transform, const Vector3& from_position, const Vector3 to_position, float t)
{
	float tween_t = Tween::EaseFunc(Tween::TweenFunction::EASE_IN_OUT_QUAD, t);
	Vector3 new_position = Vector3::Interpolate(from_position, to_position, tween_t);
	transform.SetPosition(new_position);
}
