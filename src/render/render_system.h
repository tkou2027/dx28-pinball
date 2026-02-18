#pragma once

#include <memory>
#include <wrl/client.h> // ComPtr
#include "render/directx.h"
#include "render/render_swap_data.h"

class RenderStates;
class RenderResource;
class RenderScene;
class RenderPass;
class RenderPathManager;
class ParticleSystem;
class RenderSystem
{
public:
	RenderSystem();
	~RenderSystem();
	void Initialize(HWND hWnd, UINT screen_width, UINT screen_height);
	void Finalize();
	void Draw();
	void Present();

	ID3D11Device* GetDevice() { return m_device.Get(); }
	ID3D11DeviceContext* GetDeviceContext() { return m_context.Get(); }
	IDXGISwapChain* GetSwapChain() { return m_swap_chain.Get(); }

	RenderSwapContext& GetSwapContext() { return m_render_swap_context; }

	const RenderStates& GetRenderStates() const { return *m_render_states; }
	RenderResource& GetRenderResource() { return *m_render_resource; }
	RenderScene& GetRenderScene() { return *m_render_scene; }
	ParticleSystem& GetParticleSystem() { return *m_particle_system; }
	RenderPathManager& GetRenderPathManager() { return *m_render_path_manager; }
	void Resize(uint32_t width, uint32_t height);
private:
	void InitializeDirectX(HWND hWnd, UINT screen_width, UINT screen_height);
	Microsoft::WRL::ComPtr<ID3D11Device> m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swap_chain;

	std::unique_ptr<RenderStates> m_render_states{};
	std::unique_ptr<RenderResource> m_render_resource{};
	std::unique_ptr<RenderScene> m_render_scene{};
	std::unique_ptr<RenderPass> m_render_pass{};
	std::unique_ptr<ParticleSystem> m_particle_system{};

	std::unique_ptr<RenderPathManager> m_render_path_manager{};
	RenderSwapContext m_render_swap_context{};
};