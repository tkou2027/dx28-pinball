#pragma once
#include <vector>
#include <cassert>
class TransformNode
{
public:
	virtual ~TransformNode() = default;
	void MarkDirty()
	{
		if (m_dirty)
		{
			return;
		}
		m_dirty = true;
		for (auto& child : m_children)
		{
			child->MarkDirty();
		}
	}
	void SetParent(TransformNode* parent)
	{
		if (!parent)
		{
			// TODO: if m_parent
			m_parent = nullptr;
			return;
		}
		// TODO: if m_parent
		parent->AddChild(this);
	}

	void AddChild(TransformNode* child)
	{
		assert(child);
		// TODO: check if already in
		m_children.push_back(child);
		child->m_parent = this;
	}
protected:
	// dirty flag
	bool m_dirty{ true };
	// hierarchy
	TransformNode* m_parent{ nullptr };
	std::vector<TransformNode*> m_children{};
};