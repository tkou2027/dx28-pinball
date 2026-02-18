#include "render_system.h"
#include "render/render_states.h"
#include "render/render_resource.h"
#include "render/render_pass.h"
#include "render/render_scene.h"
#include "render/render_path.h"
#include "render/particle/particle_system.h"
#include "config/camera_names.h"

#include "global_context.h"
#include "platform/graphics.h"
#include "scene/scene_manager.h"

RenderSystem::RenderSystem() = default;
RenderSystem::~RenderSystem() = default;

void RenderSystem::Initialize(HWND hWnd, UINT screen_width, UINT screen_height)
{
	InitializeDirectX(hWnd, screen_width, screen_height);
	ID3D11Device* device = m_device.Get();
	ID3D11DeviceContext* context = m_context.Get();
	IDXGISwapChain* swap_chain = m_swap_chain.Get();

	m_render_states = std::make_unique<RenderStates>();
	m_render_states->Initialize(device);

	m_render_resource = std::make_unique<RenderResource>();
	m_render_resource->Initialize(device, context, swap_chain);

	m_render_pass = std::make_unique<RenderPass>();
	m_render_pass->Initialize(device, context, swap_chain);

	m_render_path_manager = std::make_unique<RenderPathManager>();
	CameraPathConfig::InitializeRenderPath(*m_render_path_manager);
	// m_render_path_manager->Initialize();
	// m_render_path_manager->Initialize(device, context, swap_chain);

	m_render_scene = std::make_unique<RenderScene>();
	m_render_scene->Initialize(device, context);

	m_particle_system = std::make_unique<ParticleSystem>();
	m_particle_system->Initialize(device, context);
}

void RenderSystem::InitializeDirectX(HWND hWnd, UINT screen_width, UINT screen_height)
{
	// デバイス、デバイスコンテキスト、スワップチェン生成
	// デバイス　ー　抽象的GPU
	// デバイスコンテキスト　ー　GPUに命令を出す時に経由
	// スワップチェン　ー　ダブルバッファを管理
	DXGI_SWAP_CHAIN_DESC sd{};
	sd.BufferCount = 1;
	sd.BufferDesc.Width = screen_width;
	sd.BufferDesc.Height = screen_height;
	// we will calculate all lightings in linear space,
	// using SRGB to convert from linear to SRGB automatically
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	sd.BufferDesc.RefreshRate.Numerator = 60; // 目指す
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&sd,
		m_swap_chain.GetAddressOf(),
		m_device.GetAddressOf(),
		&feature_level,
		m_context.GetAddressOf()
	);

	if (FAILED(hr)) {
		// TODO メッセージボックスなどによるエラー表示
		return;
	}
}

void RenderSystem::Finalize()
{
	m_render_scene->Finalize();
	m_render_pass->Finalize();
	m_render_resource->Finalize();
	m_render_states->Finalize();
}

void RenderSystem::Draw()
{
	m_render_scene->Update();
	m_render_resource->UpdateBufferPerFrame();


	// m_render_pass->Draw();
	// m_render_path_manager->Draw(m_render_scene->GetCameraManager().GetAllCameras());
	m_render_path_manager->Draw(*m_render_scene);
}

void RenderSystem::Present()
{
	// バックバッファとフロントバッファの交換
	m_swap_chain->Present(0, 0);
}

void RenderSystem::Resize(uint32_t width, uint32_t height)
{
	m_render_scene->GetCameraManager().ResizeMainCamera(width, height);
}

//void RenderSystem::DrawLayer(RenderLayer layer)
//{
//	Scene* scene = g_global_context.m_scene_manager->GetCurrentScene();
//	if (!scene)
//	{
//		return;
//	}
//	auto& objects = scene->GetObjects();
//	for (int i = 0; i < objects.size(); i++)
//	{
//		const auto& components = objects[i]->GetComponentManager().GetComponents(ComponentLayer::RENDER);
//		for (const auto& comp : components)
//		{
//			if (auto renderComp = std::dynamic_pointer_cast<ComponentRendererSprite>(comp))
//			{
//				renderComp->GetRenderData();
//			}
//		}
//	}
//}
