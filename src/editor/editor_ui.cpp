#include "editor_ui.h"

#include "global_context.h"
#include "platform/window.h"
#include "render/render_system.h"
#include "render/render_scene.h"
#include "render/render_resource.h"
#include "scene/scene_manager.h"
#include "render/render_path.h"

#pragma comment (lib, "imgui_release.lib") 

#include "render/shader_setting.h"

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

void EditorUI::Update()
{
	//ImGuiIO& io = ImGui::GetIO();
	//auto& window = g_global_context.m_window;
	//io.DisplaySize = ImVec2(static_cast<float>(window->GetWidth()), static_cast<float>(window->GetHeight()));

	m_unique_id = 0;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Renderer Editor");

	// game objects ====
	ImGui::BeginChild("Settings", ImVec2(m_default_width, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
	ImGui::Text("Settings");
	DrawSceneObjects();
	DrawShaderSetting();
	ImGui::EndChild();
	// game objects ====

	ImGui::SameLine(); // next column
	// render textures ====
	ImGui::BeginChild("Render Textures", ImVec2(m_default_width, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
	ImGui::Text("Render Textures");
	DrawRenderTextures();
	ImGui::EndChild();
	// render textures ====

	ImGui::SameLine(); // next column
	// render paths ====
	ImGui::BeginChild("Render Paths", ImVec2(m_default_width, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
	ImGui::Text("Render Paths");
	DrawRenderPaths();
	ImGui::EndChild();
	// render paths ====
	ImGui::End();

	ImGui::Render();
}

void EditorUI::Draw() const
{
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void EditorUI::DrawShaderSetting()
{
	// TODO: deprecate this and use editor items instead
	if (ImGui::CollapsingHeader("Lights", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::SliderFloat3("Light Dir", &g_shader_setting.light_direction.x, -10.0f, 10.0f);
		ImGui::ColorEdit3("Light Color", &g_shader_setting.light_color.x);
		ImGui::SliderFloat("Light Intencity", &g_shader_setting.light_intensity, 0.0f, 100.0f);
	}

	if (ImGui::CollapsingHeader("Screen Space Reflection", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::SliderFloat("Max Distance", &g_shader_setting.ssr_setting.max_distance, 1.0f, 64.0f);
		ImGui::SliderFloat("Stride", &g_shader_setting.ssr_setting.stride, 1.0f, 30.0f);
		ImGui::SliderFloat("Thickness", &g_shader_setting.ssr_setting.thickness, 0.0f, 2.0f);
		ImGui::SliderInt("Steps", &g_shader_setting.ssr_setting.steps, 1, 200);
	}

	if (ImGui::CollapsingHeader("Ramp Map", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::SliderInt("Color", &g_shader_setting.cel_offset, 0.0f, 6.0f);
	}
}

void EditorUI::DrawSceneObjects()
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
	DrawEditorItems(items);
}

void EditorUI::DrawRenderTextures()
{
	auto& camera_manager = g_global_context.m_render_system->GetRenderScene().GetCameraManager();
	const auto cameras = camera_manager.GetActiveCameras();
	for (const auto& camera : cameras)
	{
		const auto& camera_usage = camera->GetUsageConfig();
		if (ImGui::CollapsingHeader(camera_usage.render_camera_key.c_str()))
		{
			std::vector<EditorItem> items;
			camera->GetEditorItems(items);
			DrawEditorItems(items);
		}
	}
}

void EditorUI::DrawRenderPaths()
{
	auto& path_manager = g_global_context.m_render_system->GetRenderPathManager();
	auto& camera_manager = g_global_context.m_render_system->GetRenderScene().GetCameraManager();
	const auto cameras = camera_manager.GetActiveCameras();
	for (const auto& camera : cameras)
	{
		const auto& camera_usage = camera->GetUsageConfig();
		if (ImGui::CollapsingHeader(camera_usage.render_camera_key.c_str()))
		{
			std::vector<EditorItem> items;
			const size_t path_id = camera_usage.render_path_id;
			const auto& path = path_manager.GetRenderPath(path_id);
			path.GetEditorItems(camera_usage.render_camera_key, items);

			// ImGui::Begin(("Render Path: " + camera->GetUsageConfig().render_camera_key).c_str());

			DrawEditorItems(items);
		}
	}
}

// imgiu utils for drawing editor items
void EditorUI::DrawEditorItems(const std::vector<EditorItem>& items)
{
	for (const auto& item : items)
	{
		bool has_label = item.label.length() > 0;
		ImGui::PushID(m_unique_id++);
		if (has_label ? ImGui::TreeNode(item.label.c_str()) : true)
		{
			for (const auto& image : item.images)
			{
				DrawEditorImage(image);
			}
			for (const auto& property : item.properties)
			{
				DrawEditorProperty(property);
			}
			if (has_label)
			{
				ImGui::TreePop();
			}
		}
		ImGui::PopID();
	}
}

void EditorUI::DrawEditorImage(const EditorImage& image)
{
	const float width = ImGui::GetContentRegionAvail().x;
	ImGui::PushID(m_unique_id++);
	if (ImGui::TreeNode(image.label.c_str()))
	{
		ImGui::Image(
			(ImTextureID)image.srv_ptr,
			ImVec2{ width, width / image.aspect_ratio },
			ImVec2{ 0.0f, 0.0f },
			ImVec2{ 1.0f, 1.0f }
		);
		ImGui::TreePop();
	}
	ImGui::PopID();
}

void EditorUI::DrawEditorProperty(const EditorProperty& property) const
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
