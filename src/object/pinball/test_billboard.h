#pragma once
#include "object/game_object.h"
#include "component/render/component_renderer_billboard.h"

#include "global_context.h"
#include "render/render_system.h"
#include "render/render_resource.h"
#include "render/resource/texture_loader.h"

#include "math/vector2.h"
#include "math/vector3.h"

class TestBillboard : public GameObject
{
public:
	void Initialize() override
	{
		auto& comp = m_components.Add<ComponentRendererBillboard>(m_comp_id_render);

		// prepare billboard descriptor using particle texture
		BillboardDesc desc{};
		desc.texture_id = GetTextureLoader().GetOrLoadTextureFromFile("asset/texture/particle.png");
		desc.has_uv_animation = false;
		comp.SetBillboardDesc(desc);

		// create a small grid of billboards for testing
		const int cols = 30;
		const int rows = 30;
		const int total = cols * rows;
		comp.SetSize(total);

		const float spacing = 1.0f;
		const float start_x = -((cols - 1) * spacing) * 0.5f;
		const float start_z = -((rows - 1) * spacing) * 0.5f;
		for (int r = 0; r < rows; ++r)
		{
			for (int c = 0; c < cols; ++c)
			{
				int idx = r * cols + c;
				auto& inst = comp.GetInstance(idx);
				// activate
				inst.SetActive(true);
				// set size (width, height)
				inst.SetSize(Vector2{ 1.0f, 1.0f });
				// compute position above ground
				Vector3 pos{ start_x + c * spacing, 1.0f, start_z + r * spacing };
				inst.SetPosition(pos);
			}
		}
	}
private:
	ComponentId m_comp_id_render;
};