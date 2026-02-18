#pragma once
#include "util/sprite.h"
#include "util/particle.h"

template <int CAPACITY>
class ParticleSystem
{
public:
	void AddParticle(const ParticleConfig& particle_config)
	{
		assert(m_count <= CAPACITY - 1, "Particle list full!");
		m_particles[m_count].Initialize(particle_config);
		m_particle_sprites[m_count] = particle_config.sprite;
		m_particle_sprites[m_count].m_transform_2d = m_particles[m_count].GetTransformPtr();
		m_count++;
	}
	void Update()
	{
		// remove dead
		for (int i = 0; i < m_count; i++)
		{
			if (m_particles[i].IfDead())
			{
				m_particles[i] = m_particles[m_count - 1];
				m_particle_sprites[i] = m_particle_sprites[m_count - 1];
				m_particle_sprites[m_count - 1].m_active = false;
				--m_count;
				--i;
			}
		}
		// update
		for (int i = 0; i < m_count; i++)
		{
			m_particles[i].Update(m_acc);
		}
	}
	void SetAcceleration(const Vector2& acc) { m_acc = acc; }
	Sprite* GetParticleSprites() { return m_particle_sprites; }
private:
	Particle m_particles[CAPACITY];
	Sprite m_particle_sprites[CAPACITY];
	int m_count{ 0 };
	Vector2 m_acc{};
};


