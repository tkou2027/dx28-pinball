#pragma once

#include "render/directx.h"
#include "render/resource/shader.h"
#include "render/resource/view_context.h"

class RenderScene;
class RenderStates;
class RenderResource;
class TextureLoader;
class ModelLoader;
class MaterialResourceManager;
class PassBase
{
public:
	PassBase() = default;
	virtual ~PassBase() = default;
	virtual void Initialize(ID3D11Device* device, ID3D11DeviceContext* context);
	virtual void Finalize() {};
	virtual void Draw() = 0;
protected:
	static constexpr int GEO_BUFFER_NUM{ 4 };
	// resources
	const RenderScene& GetRenderScene() const;
	const RenderResource& GetRenderResource() const;
	const RenderStates& GetRenderStates() const;
	TextureLoader& GetTextureLoader();
	const TextureLoader& GetTextureLoader() const;
	const MaterialResourceManager& GetMaterialResourceManager() const;
	ModelLoader& GetModelLoader();
	int GetScreenWidth() const;
	int GetScreenHeight() const;
	Microsoft::WRL::ComPtr<ID3D11Buffer> CreateConstantBuffer(size_t size) const;
	// draw
	virtual void DrawModelStatic(const struct ModelRenderInfo& model);
	virtual void DrawModelDynamic(const struct ModelRenderInfo& model);

	// device
	// 注意！初期化で外部から設定されるもの。Release不要。
	ID3D11Device* m_device{ nullptr };
	ID3D11DeviceContext* m_context{ nullptr };
};