#pragma once
typedef size_t ParticleEmitterId;

class ParticleEmitterIdAllocator
{
public:
    static ParticleEmitterId Allocate();
    static void Reset();
	static constexpr ParticleEmitterId ID_INVALID = static_cast<ParticleEmitterId>(-1);
private:
    static ParticleEmitterId m_next_id; // TODO atomic
};