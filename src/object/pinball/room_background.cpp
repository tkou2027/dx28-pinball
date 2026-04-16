#include "room_background.h"
#include "math/hex.h"
#include "object/game_object_list.h"
#include "object/effect/test_texture.h"

void RoomBackground::InitializeConfig()
{
	Hex hex{};
	hex.SetSize(2.8f);

	for (int r = 30; r <= 35; r++)
	{
		std::vector<HexCoord> coords = HexCoord::GenerateRing(r);
		for (int i = 0; i < coords.size(); ++i)
		{
			float pos_x = hex.HexToX(coords[i]);
			float pos_z = hex.HexToZ(coords[i]);
			float pos_y = (r - 20.0f) * 1.5f;
			auto effect = GetOwner().CreateGameObject<TestTexture>();
			effect->GetTransform().SetPosition({ pos_x, pos_y, pos_z });
			effect->GetTransform().SetScale(0.15f);

			float rotation_y = std::atan2(-pos_x, -pos_z);
			float dist_xz = std::sqrt(pos_x * pos_x + pos_z * pos_z);
			float rotation_x = std::atan2(dist_xz, -pos_y);
			//effect->GetTransform().SetRotationEuler({ rotation_x, rotation_y, 0.0f });
			effect->GetTransform().SetRotationYOnly(rotation_y);
		}
	}
}