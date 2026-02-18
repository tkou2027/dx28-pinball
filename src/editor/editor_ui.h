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
	void Update() const;
	void Draw() const;
	LRESULT ProcessMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const;
private:
	void DrawShaderSetting() const;
	void DrawSceneObjects() const;
	void DrawObjectEditorItems(const std::vector<EditorItem>& items) const;
};