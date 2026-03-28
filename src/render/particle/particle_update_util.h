#pragma once
#include <DirectXMath.h>
#include "render/directx.h"
#include "render/particle/particle_data.h"

// utility functions for particle init and update
class ParticleUpdateUtil
{
public:
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* context);
	void InitializeEmitterParticles(const EmitterData& emitter) const;
	void InitializeEmitterBurstUpdate(const EmitterUpdateData& update_data, const EmitterData& emitter) const;
	void UpdateEmitterBurstUpdate(const EmitterUpdateData& update_data, const EmitterData& emitter) const;
private:
	void ResetComputeShader() const;
	// init
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_cs_init{};
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_cb_init{};
	// burst init
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_cs_burst_init{};
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_cb_burst_init{};

	// burst update
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_cs_update_crush{};
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_cb_update_crush{};
	ID3D11Device* m_device{ nullptr };
	ID3D11DeviceContext* m_context{ nullptr };
};
