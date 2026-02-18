#include "editor_ui.h"

#include "global_context.h"
#include "platform/window.h"
#include "render/render_system.h"

#include "scene/scene_manager.h"

#pragma comment (lib, "imgui_release.lib") 

#include "shader_setting.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void EditorUI::Initialize()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigWindowsMoveFromTitleBarOnly = true;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(g_global_context.m_window->GetHandler());
	ImGui_ImplDX11_Init(
		g_global_context.m_render_system->GetDevice(),
		g_global_context.m_render_system->GetDeviceContext()
	);
}

void EditorUI::Finalize()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

LRESULT EditorUI::ProcessMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const
{
	return ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
}

void EditorUI::Update() const
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	
	DrawShaderSetting();
	DrawSceneObjects();

	ImGui::Render();
}

void EditorUI::Draw() const
{
	
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

static bool DrawEnemyEditor(std::vector<EditorEnemySetting>& list, bool& desiredReset)
{
	bool modified = false;

	for (int i = 0; i < list.size(); i++)
	{
		ImGui::PushID(i);
		ImGui::Text("Enemy %d", i);
		ImGui::Indent();

		if (ImGui::InputInt("Type", &list[i].type))
			modified = true;

		if (ImGui::InputFloat3("Position", &list[i].position.x))
			modified = true;

		ImGui::SameLine();
		if (ImGui::Button("Remove"))
		{
			list.erase(list.begin() + i);
			modified = true;
			ImGui::Unindent();
			ImGui::PopID();
			break;
		}

		ImGui::Unindent();
		ImGui::Separator();
		ImGui::PopID();
	}

	if (ImGui::Button("Add Enemy"))
	{
		list.push_back(EditorEnemySetting{});
		modified = true;
	}

	if (ImGui::Button("Reset Enemy"))
	{
		desiredReset = true;
		modified = true;
	}

	return modified;
}


static bool DrawLightEditor(std::vector<EditorLightSetting>& list)
{
	bool modified = false;

	for (int i = 0; i < list.size(); i++)
	{
		ImGui::PushID(i);
		ImGui::Text("Light %d", i);
		ImGui::Indent();

		if (ImGui::InputFloat3("Position", &list[i].position.x))
			modified = true;

		if (ImGui::ColorEdit3("Color", &list[i].color.x))
			modified = true;

		ImGui::SameLine();
		if (ImGui::Button("Remove"))
		{
			list.erase(list.begin() + i);
			modified = true;
			ImGui::Unindent();
			ImGui::PopID();
			break;
		}

		ImGui::Unindent();
		ImGui::Separator();
		ImGui::PopID();
	}

	if (ImGui::Button("Add"))
	{
		list.push_back(EditorLightSetting{});
		modified = true;
	}

	return modified;
}


void EditorUI::DrawShaderSetting() const
{
	ImGui::Begin("Test");
		
	if (ImGui::CollapsingHeader("Lights", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::SliderFloat3("Light Dir", &g_shader_setting.light_direction.x, -10.0f, 10.0f);
		ImGui::ColorEdit3("Light Color", &g_shader_setting.light_color.x);
		ImGui::SliderFloat("Light Intencity", &g_shader_setting.light_intensity, 0.0f, 20.0f);
		DrawLightEditor(
			g_shader_setting.lights
		);
	}

	if (ImGui::CollapsingHeader("Screen Space Reflection", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::SliderFloat("Max Distance", &g_shader_setting.ssr_setting.max_distance, 1.0f, 64.0f);
		ImGui::SliderFloat("Resolution", &g_shader_setting.ssr_setting.resolution, 0.0f, 1.0f);
		ImGui::SliderFloat("Thickness", &g_shader_setting.ssr_setting.thickness, 0.0f, 2.0f);
		ImGui::SliderInt("Steps", &g_shader_setting.ssr_setting.steps, 1, 32);
	}


	if (ImGui::CollapsingHeader("Ramp Map", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::SliderInt("Color", &g_shader_setting.cel_offset, 0.0f, 6.0f);
	}

	if (ImGui::CollapsingHeader("Camera"))
	{
		ImGui::SliderFloat("FOV", &g_shader_setting.camera_fov, 0.1f, 3.0f);
	}
	ImGui::End();
}

void EditorUI::DrawSceneObjects() const
{
	Scene* scene = g_global_context.m_scene_manager->GetCurrentScene();
	if (!scene)
	{
		return;
	}
	auto& objects = scene->GetObjects();
	std::vector<EditorItem> items;
	for (auto& object : objects)
	{
		object->GetEditorItem(items);
	}
	ImGui::Begin("Scene");
	DrawObjectEditorItems(items);
	ImGui::End();
}

void EditorUI::DrawObjectEditorItems(const std::vector<EditorItem>& items) const
{
	for (const auto& item : items)
	{
		if (ImGui::CollapsingHeader(item.label.c_str()))
		{
			for (const auto& property : item.properties)
			{
				switch (property.type)
				{
				case EditorPropertyType::FLOAT:
					ImGui::SliderFloat(
						property.label.c_str(),
						static_cast<float*>(property.data_ptr),
						property.min,
						property.max
					);
					break;
				case EditorPropertyType::FLOAT2:
					ImGui::SliderFloat2(
						property.label.c_str(),
						static_cast<float*>(property.data_ptr),
						property.min,
						property.max
					);
					break;
				case EditorPropertyType::FLOAT3:
					ImGui::SliderFloat3(
						property.label.c_str(),
						static_cast<float*>(property.data_ptr),
						property.min,
						property.max
					);
					break;
				case EditorPropertyType::INT:
					ImGui::SliderInt(
						property.label.c_str(),
						static_cast<int*>(property.data_ptr),
						static_cast<int>(property.min),
						static_cast<int>(property.max)
					);
					break;
				default:
					break;
				}
			}
		}
	}
}

//void EditorUI::DrawShaderSetting() const
//{
//	ImGui::Begin("Test");
//
//	if (ImGui::CollapsingHeader("Model"))
//	{
//		ImGui::Checkbox("Alicia", &g_shader_setting.alicia);
//	}
//	if (ImGui::CollapsingHeader("Cel"))
//	{
//		ImGui::Checkbox("Enable Cel", &g_shader_setting.enable_cel);
//		ImGui::Checkbox("Enable Phong", &g_shader_setting.enable_phong);
//		ImGui::SliderInt("Cel Color", &g_shader_setting.cel_offset, 0.0f, 6.0f);
//		ImGui::SliderFloat3("Light Dir", &g_shader_setting.light_direction.x, -10.0f, 10.0f);
//	}
//
//	if (ImGui::CollapsingHeader("Outline"))
//	{
//		ImGui::Checkbox("Enable Outline", &g_shader_setting.enable_outline);
//		ImGui::ColorEdit3("Color", &g_shader_setting.outline_color.x);
//		ImGui::SliderFloat("Width", &g_shader_setting.outline_width, 0.0f, 0.01f);
//		//ImGui::ColorEdit3("Color", (float*)&g_shaderSettings.outlineColor);
//	}
//
//	if (ImGui::CollapsingHeader("Camera"))
//	{
//		ImGui::SliderFloat("FOV", &g_shader_setting.camera_fov, 0.1f, 3.0f);
//	}
//
//	if (ImGui::CollapsingHeader("Portal", ImGuiTreeNodeFlags_DefaultOpen))
//	{
//		ImGui::SliderFloat3("Position", &g_shader_setting.mask_position.x, 0.0f, 80.0f);
//		ImGui::SliderFloat("Yaw", &g_shader_setting.mask_yaw, 0.0f, Math::TWO_PI);
//		ImGui::SliderFloat("Scale", &g_shader_setting.mask_scale, 1.0f, 100.0f);
//		ImGui::Checkbox("Scale Animation", &g_shader_setting.mask_animation);
//	}
//
//	if (ImGui::CollapsingHeader("Aim", ImGuiTreeNodeFlags_DefaultOpen))
//	{
//		ImGui::SliderFloat3("Aim Position", &g_shader_setting.aim_position.x, 0.0f, 80.0f);
//		ImGui::SliderFloat("Aim Yaw", &g_shader_setting.aim_yaw, 0.0f, Math::TWO_PI);
//	}
//
//	if (ImGui::CollapsingHeader("Aim", ImGuiTreeNodeFlags_DefaultOpen))
//	{
//		ImGui::SliderInt("Map Size X", &g_shader_setting.map_width_half, 1, 10);
//		ImGui::SliderInt("Map Size Z", &g_shader_setting.map_depth_half, 1, 10);
//	}
//
//	//if (ImGui::Button("Dash"))
//	//{
//	//	g_shader_setting.desired_dash = true;
//	//}
//
//	//if (ImGui::Button("Reset"))
//	//{
//	//	g_shader_setting.desired_reset = true;
//	//}
//
//	if (ImGui::CollapsingHeader("Enemies", ImGuiTreeNodeFlags_DefaultOpen))
//	{
//		DrawEnemyEditor(
//			g_shader_setting.enemies,
//			g_shader_setting.desired_enemy_reset
//		);
//	}
//
//	ImGui::End();
//}