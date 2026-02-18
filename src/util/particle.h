#pragma once
#include "math/transform2d.h"

struct ParticleConfig
{
	Transform2D transform;
	Transform2D velocity;
	//Sprite* sprite{};
	int life{};
	//Sprite sprite;
};

class Particle
{
public:
	void Initialize(const ParticleConfig& particleConfig);
	void Update(const Vector2& acc);
	bool IfDead() const;
	Transform2D* GetTransformPtr() { return &m_transform; }
private:
	Transform2D m_transform{};
	Transform2D m_velocity{};
	// Sprite* m_sprite{ nullptr };
	int m_life{ 0 };
	bool m_dead{ false };
};