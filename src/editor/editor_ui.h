#pragma once

#include <windows.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "editor_item.h"

class EditorUI
{
public:
	void Initialize();
	void Finalize();
	void Update();
	void Draw() const;
	LRESULT ProcessMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const;
private:
	void DrawShaderSetting();
	void DrawSceneObjects();
	void DrawRenderTextures();
	void DrawRenderPaths();
	// utils for drawing editor items
	void DrawEditorItems(const std::vector<EditorItem>& items);
	void DrawEditorImage(const EditorImage& image);
	void DrawEditorProperty(const EditorProperty& property) const;
	float m_default_width{ 400.0f };
	int m_unique_id{};
};