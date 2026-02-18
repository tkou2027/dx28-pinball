#pragma once
#include "collider_layer.h"

class CollisionMatrix {
public:
	// should be data but coding it for convinience...
    void Initialize();

    void SetLayerCollision(ColliderLayer::Type layer_a, ColliderLayer::Type layer_b, bool enable)
    {
        if (enable) {
            m_matrix[static_cast<size_t>(layer_a)] |= (1 << static_cast<int>(layer_b));
            m_matrix[static_cast<size_t>(layer_b)] |= (1 << static_cast<int>(layer_a));
        }
        else {
            m_matrix[static_cast<size_t>(layer_a)] &= ~(1 << static_cast<int>(layer_b));
            m_matrix[static_cast<size_t>(layer_b)] &= ~(1 << static_cast<int>(layer_a));
        }
    }

    uint32_t GetMaskOfLayer(ColliderLayer::Type layer) const
    {
        return m_matrix[static_cast<size_t>(layer)];
    }

    bool IfCollides(ColliderLayer::Type layer_a, ColliderLayer::Type layer_b) const
    {
        return (m_matrix[static_cast<size_t>(layer_a)] & (1 << static_cast<int>(layer_b))) != 0;
    }

    bool IfInMask(uint32_t mask, ColliderLayer::Type layer_b) const
    {
        return (mask & (1 << static_cast<int>(layer_b))) != 0;
    }

private:
    uint32_t m_matrix[static_cast<size_t>(ColliderLayer::Type::MAX)] = { 0 };
};