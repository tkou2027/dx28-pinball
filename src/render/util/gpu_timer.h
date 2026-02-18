#pragma once
// reference: GitHub MKXJun/DirectX11-With-Windows-SDK
#include <cassert>
#include <cstdint>
#include <deque>
#include "render/directx.h"

struct GpuTimerInfo
{
	D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjoint_data{};
	uint64_t start_data{ 0 };
	uint64_t stop_data{ 0 };
	Microsoft::WRL::ComPtr<ID3D11Query> disjoint_query;
	Microsoft::WRL::ComPtr<ID3D11Query> start_query;
	Microsoft::WRL::ComPtr<ID3D11Query> stop_query;
	bool stopped{ false };
};

class GpuTimer
{
public:
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* context, size_t recentCount = 0);
	void Reset(ID3D11DeviceContext* deviceContext, size_t recentCount = 0);

	HRESULT Start();
	void Stop();
	bool TryGetTime(double* pOut);
	double GetTime();
	double AverageTime()
	{
		if (m_recent_count)
		{
			return m_accum_time / m_delta_times.size();

		}
		else
		{
			return m_accum_time / m_accum_count;
		}
	}
private:
	static bool GetQueryDataHelper(ID3D11DeviceContext* pContext, bool loopUntilDone, ID3D11Query* query, void* data, uint32_t dataSize);

	std::deque<double> m_delta_times;
	double m_accum_time = 0.0;
	size_t m_accum_count = 0;
	size_t m_recent_count = 0;

	std::deque<GpuTimerInfo> m_queries;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;
};
