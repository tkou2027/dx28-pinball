#pragma once
#include <vector>
#include <DirectXMath.h>
#include <assimp/matrix4x4.h>
#include <assimp/anim.h>

namespace AssimpMath
{
	DirectX::XMFLOAT4X4 AiToXMFloat4x4(const aiMatrix4x4& m);
	DirectX::XMFLOAT4X4 AiToXMFloat4x4Transpose(const aiMatrix4x4& m);
	aiMatrix4x4 BuildTransformMatrix(
		const aiVector3D& scaling,
		const aiQuaternion& rotationQ,
		const aiVector3D& translation);
	aiVector3D InterpolateVector3(const std::vector<aiVectorKey>& keys, float animation_time);
	aiQuaternion InterpolateQuat(const std::vector<aiQuatKey>& keys, float animation_time);
}