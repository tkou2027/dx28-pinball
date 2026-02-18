#pragma once
#include <array>
#include <vector>
#include <memory>
#include "pass_base.h"
#include "pass_base_geometry.h"
#include "render/pass/forward/subpass_forward_unlit.h"
#include "render/pass/forward/subpass_forward_screen.h"

class SubPassForwardGlass : public PassBaseGeometry
{
public:
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* context) override;
	bool ShouldRender(const MaterialDesc& material_desc, ModelType model_type) const override;
	void SetInfoPerDraw() override;
	void SetInfoPerModel(const ModelRenderInfo& model_info) override;
private:
	void SetInfoPerMaterial(const ModelRenderKey& key) override;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps{};
};

class PassForward : public PassBase
{
public:
	enum class SubPassQueue
	{
		DEFAULT,
		DECAL,
		MAX
	};
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* context) override;
	void Draw() override;
	void SetDrawingQueue(SubPassQueue queue) { m_drawing_queue = queue; }
	void AddSubPass(std::unique_ptr<PassBaseGeometry> subpass, SubPassQueue queue);

	void ResetRenderableIndices(CameraRenderLayer layer);
	void AddRenderableIndex(size_t mesh_index, ModelType model_type, const MaterialDesc& material_desc);
	template<typename TPass>
	TPass& GetSubPass(SubPassQueue queue)
	{
		auto& subpasses = m_queues[static_cast<size_t>(queue)];
		for (const auto& subpass : subpasses)
		{
			if (auto casted = dynamic_cast<TPass*>(subpass.get()))
			{
				return *casted;
			}
		}
		assert(false);
	}
private:
	SubPassQueue m_drawing_queue{ SubPassQueue::DEFAULT };
	std::array<std::vector<std::unique_ptr<PassBaseGeometry> >,
		static_cast<size_t>(SubPassQueue::MAX)> m_queues;
};
