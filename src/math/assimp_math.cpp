#include "assimp_math.h"

namespace AssimpMath
{
	DirectX::XMFLOAT4X4 AiToXMFloat4x4(const aiMatrix4x4& m)
	{
		// transpose
		DirectX::XMFLOAT4X4 result(
			m.a1, m.b1, m.c1, m.d1,
			m.a2, m.b2, m.c2, m.d2,
			m.a3, m.b3, m.c3, m.d3,
			m.a4, m.b4, m.c4, m.d4
		);
		return result;
	}

	DirectX::XMFLOAT4X4 AiToXMFloat4x4Transpose(const aiMatrix4x4& m)
	{
		DirectX::XMFLOAT4X4 result(
			m.a1, m.a2, m.a3, m.a4,
			m.b1, m.b2, m.b3, m.b4,
			m.c1, m.c2, m.c3, m.c4,
			m.d1, m.d2, m.d3, m.d4
		);
		return result;
	}

	aiMatrix4x4 BuildTransformMatrix(const aiVector3D& scaling,
		const aiQuaternion& rotationQ,
		const aiVector3D& translation)
	{
		aiMatrix4x4 matScaling;
		aiMatrix4x4::Scaling(scaling, matScaling);

		aiMatrix4x4 matRotation = aiMatrix4x4(rotationQ.GetMatrix());

		aiMatrix4x4 matTranslation;
		aiMatrix4x4::Translation(translation, matTranslation);

		aiMatrix4x4 result = matTranslation * matRotation * matScaling;

		return result;
	}

	aiVector3D InterpolateVector3(const std::vector<aiVectorKey>& keys, float animation_time)
	{
		const size_t size{ keys.size() };
		if (size == 1)
		{
			return keys[0].mValue;
		}

		size_t index{ 0 };
		for (; index < size - 1; index++)
		{
			if (animation_time < static_cast<float>(keys[index + 1].mTime))
			{
				break;
			}
		}
		size_t next_index{ index + 1 };
		assert(next_index < size);
		float delta_time = keys[next_index].mTime - keys[index].mTime;
		float factor = (animation_time - (float)keys[index].mTime) / delta_time;
		assert(factor >= 0.0f && factor <= 1.0f);
		const aiVector3D& Start = keys[index].mValue;
		const aiVector3D& End = keys[next_index].mValue;
		return Start * (1.0f - factor) + End * factor; // TODO
	}

	aiQuaternion InterpolateQuat(const std::vector<aiQuatKey>& keys, float animation_time)
	{
		const size_t size{ keys.size() };
		if (size == 1)
		{
			return keys[0].mValue;
		}

		size_t index{ 0 };
		for (; index < size - 1; index++)
		{
			if (animation_time < static_cast<float>(keys[index + 1].mTime))
			{
				break;
			}
		}
		size_t next_index{ index + 1 };
		assert(next_index < size);
		float delta_time = keys[next_index].mTime - keys[index].mTime;
		float factor = (animation_time - (float)keys[index].mTime) / delta_time;
		assert(factor >= 0.0f && factor <= 1.0f);
		const aiQuaternion& Start = keys[index].mValue;
		const aiQuaternion& End = keys[next_index].mValue;
		aiQuaternion res;
		aiQuaternion::Interpolate(res, Start, End, factor);
		res = res.Normalize();
		return res;
	}
}