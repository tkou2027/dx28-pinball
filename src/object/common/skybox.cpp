#include "skybox.h"
#include "config/preset_manager.h"
#include "component/render/component_renderer_mesh.h"

#include "global_context.h"
#include "render/render_system.h"
#include "render/render_resource.h"

void Skybox::Initialize()
{
	// ModelDesc model_desc{ "asset/models/cube2.fbx" };
	const auto& model_desc = GetPresetManager().GetModelDesc("geo/unit_cube");

	MaterialDesc mat_desc{};
	TechniqueDescSkybox mat_skybox{};
	auto& texture_loader = g_global_context.m_render_system->GetRenderResource().GetTextureLoader();
	mat_skybox.skybox_texture_id = texture_loader.GetOrLoadTextureCubeFromFile(
		"asset/texture/skybox/ablaze",
		{
			L"asset/texture/skybox/ablaze/right.png",
			L"asset/texture/skybox/ablaze/left.png",
			L"asset/texture/skybox/ablaze/top.png",
			L"asset/texture/skybox/ablaze/bottom.png",
			L"asset/texture/skybox/ablaze/front.png",
			L"asset/texture/skybox/ablaze/back.png"
		}
	);
	//mat_skybox.skybox_texture_id = texture_loader.GetOrLoadTextureCubeFromFile(
	//	"asset/texture/skybox/blink",
	//	{
	//		L"asset/texture/skybox/blink/right.png",
	//		L"asset/texture/skybox/blink/left.png",
	//		L"asset/texture/skybox/blink/top.png",
	//		L"asset/texture/skybox/blink/bottom.png",
	//		L"asset/texture/skybox/blink/front.png",
	//		L"asset/texture/skybox/blink/back.png"
	//	}
	//);
	// mat_skybox.skybox_texture_id = texture_loader.GetOrCreateRenderTextureId("reflect");
	mat_desc.SetTechnique(mat_skybox);
	// // TODO!!
	//state.m_model_desc.material_desc.skybox_desc.texture_id = texture_loader.GetOrLoadTextureCubeFromFile(
	//	"asset/texture/skybox/ablaze",
	//	{
	//		L"asset/texture/skybox/ablaze/right.png",
	//		L"asset/texture/skybox/ablaze/left.png",
	//		L"asset/texture/skybox/ablaze/top.png",
	//		L"asset/texture/skybox/ablaze/bottom.png",
	//		L"asset/texture/skybox/ablaze/front.png",
	//		L"asset/texture/skybox/ablaze/back.png"
	//	}
	//);
	Model state{ model_desc ,mat_desc ,&m_transform };
	auto& render_mesh = m_components.Add<ComponentRendererMesh>();
	render_mesh.AddModel(state);

}