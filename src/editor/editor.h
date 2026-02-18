#pragma once
#include <memory>

class EditorUI;
class Editor
{
public:
	Editor();
	~Editor();
	void Initialize();
	void Finalize();
	EditorUI& GetEditorUI();
	const EditorUI& GetEditorUI() const;
private:
	std::unique_ptr<EditorUI> m_editor_ui;
};