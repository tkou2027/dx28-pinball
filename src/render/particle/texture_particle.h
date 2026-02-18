#pragma once
#include <DirectXMath.h>
#include "render/directx.h"
#include "render/resource/shader.h"
#include "render/config/texture_resource_id.h"
#include "math/vector2.h"

enum class TextureParticleShape
{
    PLANE = 0,
    CYLINDER = 1,
    // other shapes...
};

struct TextureParticleData
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 velocity;
    //DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT2 uv;
    float life;
    float delay;
};

struct TextureParticleConfig
{
    // TODO: shape type
    TextureParticleShape shape;
    unsigned int num_width;
    unsigned int num_height;
	int buffer_view_id{ -1 };
    TextureResourceId texture_id{};
    TextureResourceId local_texture_id{};
};

class TextureParticleManager
{
public:
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* context);
    int InitializeOrResetParticle(const TextureParticleConfig& config, const DirectX::XMFLOAT4X4& world_mat);
	void UpdateParticles(const TextureParticleConfig& config, float delta_time);
private:
    void ResetComputeShader();
    class ModelLoader& GetModelLoader() const;
    // init / update
    Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_cs_init{};
    Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_cs_update_crush{};
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_cb_init{ nullptr };
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_cb_update_crush{ nullptr };

    ID3D11Device* m_device{ nullptr };
    ID3D11DeviceContext* m_context{ nullptr };
};