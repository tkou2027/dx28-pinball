#pragma once
#include "math/transform3d.h"

class MoveController
{
public:
	static void MoveTransform(TransformNode3D& transform,
		const Vector3& from_position, const Vector3 to_position, float t);
};