#include "editor.h"
#include "editor_ui.h"

Editor::Editor() = default;
Editor::~Editor() = default;

void Editor::Initialize()
{
	m_editor_ui = std::make_unique<EditorUI>();
	m_editor_ui->Initialize();
}

void Editor::Finalize()
{
	m_editor_ui->Finalize();
}

EditorUI& Editor::GetEditorUI()
{
	return *m_editor_ui;
}

const EditorUI& Editor::GetEditorUI() const
{
	return *m_editor_ui;
}
