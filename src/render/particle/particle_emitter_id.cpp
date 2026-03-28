#include "particle_emitter_id.h"

ParticleEmitterId ParticleEmitterIdAllocator::m_next_id{ 0 };

ParticleEmitterId ParticleEmitterIdAllocator::Allocate()
{
    return m_next_id++;
}

void ParticleEmitterIdAllocator::Reset()
{
    m_next_id = 0;
}
