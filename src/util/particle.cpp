#include "particle.h"

void Particle::Initialize(const ParticleConfig& config)
{
	m_transform = config.transform;
	m_velocity = config.velocity;
	// m_sprite = config.sprite;
	m_life = config.life;
	m_dead = false;
}

void Particle::Update(const Vector2& acc)
{
	if (m_life <= 0)
	{
		m_dead = true;
		return;
	}
	m_life--;
	// apply force
	m_velocity.position = m_velocity.position + acc;
	// move
	m_transform.position = m_transform.position + m_velocity.position;
	m_transform.scale.x *= m_transform.scale.x * m_velocity.scale.x;
	m_transform.scale.y *= m_transform.scale.y * m_velocity.scale.y;
	m_transform.rotation += m_velocity.rotation;
}

bool Particle::IfDead() const
{
	return m_dead;
}
