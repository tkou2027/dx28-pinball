#include "gpu_timer.h"

void GpuTimer::Initialize(ID3D11Device* device, ID3D11DeviceContext* context, size_t recent_count)
{
	m_device = device;
	m_context = context;
	m_recent_count = recent_count;
	m_accum_time = 0.0;
	m_accum_count = 0;
	m_delta_times.clear();
	m_queries.clear();
}

void GpuTimer::Reset(ID3D11DeviceContext* deviceContext, size_t recentCount)
{
	m_queries.clear();
	m_delta_times.clear();
	m_context = deviceContext;
	m_accum_time = 0.0;
	m_accum_count = 0;
	if (recentCount)
		m_recent_count = recentCount;
}

HRESULT GpuTimer::Start()
{
	if (!m_queries.empty() && !m_queries.back().stopped)
	{
		return E_FAIL;
	}

	m_queries.emplace_back();
	GpuTimerInfo& info = m_queries.back();

	CD3D11_QUERY_DESC queryDesc(D3D11_QUERY_TIMESTAMP);

	// create queries
	// start
	HRESULT hr = m_device->CreateQuery(&queryDesc, info.start_query.GetAddressOf());
	if (FAILED(hr))
	{
		return hr;
	}
	// stop
	hr = m_device->CreateQuery(&queryDesc, info.stop_query.GetAddressOf());
	if (FAILED(hr))
	{
		return hr;
	}
	// disjoint
	queryDesc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
	hr = m_device->CreateQuery(&queryDesc, info.disjoint_query.GetAddressOf());
	if (FAILED(hr))
	{
		return hr;
	}

	m_context->Begin(info.disjoint_query.Get());
	m_context->End(info.start_query.Get());
	return S_OK;
}

void GpuTimer::Stop()
{
	if (m_queries.empty())
	{
		return;
	}
	GpuTimerInfo& info = m_queries.back();
	m_context->End(info.disjoint_query.Get());
	m_context->End(info.stop_query.Get());
	info.stopped = true;
}

bool GpuTimer::TryGetTime(double* pOut)
{
	if (m_queries.empty())
		return false;

	GpuTimerInfo& info = m_queries.front();
	if (!info.stopped) return false;

	if (info.disjoint_query && !GetQueryDataHelper(m_context, false, info.disjoint_query.Get(), &info.disjoint_data, sizeof(info.disjoint_data)))
		return false;
	info.disjoint_query.Reset();

	if (info.start_query && !GetQueryDataHelper(m_context, false, info.start_query.Get(), &info.start_data, sizeof(info.start_data)))
		return false;
	info.start_query.Reset();

	if (info.stop_query && !GetQueryDataHelper(m_context, false, info.stop_query.Get(), &info.stop_data, sizeof(info.stop_data)))
		return false;
	info.stop_query.Reset();

	if (!info.disjoint_data.Disjoint)
	{
		double deltaTime = static_cast<double>(info.stop_data - info.start_data) / info.disjoint_data.Frequency;
		if (m_recent_count > 0)
			m_delta_times.push_back(deltaTime);
		m_accum_time += deltaTime;
		m_accum_count++;
		if (m_delta_times.size() > m_recent_count)
		{
			m_accum_time -= m_delta_times.front();
			m_delta_times.pop_front();
		}
		if (pOut) *pOut = deltaTime;
	}
	else
	{
		double deltaTime = -1.0;
		if (pOut) *pOut = deltaTime;
	}

	m_queries.pop_front();
	return true;
}

double GpuTimer::GetTime()
{
	if (m_queries.empty())
		return -1.0;

	GpuTimerInfo& info = m_queries.front();
	if (!info.stopped) return -1.0;

	if (info.disjoint_query)
	{
		GetQueryDataHelper(m_context, true, info.disjoint_query.Get(), &info.disjoint_data, sizeof(info.disjoint_data));
		info.disjoint_query.Reset();
	}
	if (info.start_query)
	{
		GetQueryDataHelper(m_context, true, info.start_query.Get(), &info.start_data, sizeof(info.start_data));
		info.start_query.Reset();
	}
	if (info.stop_query)
	{
		GetQueryDataHelper(m_context, true, info.stop_query.Get(), &info.stop_data, sizeof(info.stop_data));
		info.stop_query.Reset();
	}

	double deltaTime = -1.0;
	if (!info.disjoint_data.Disjoint)
	{
		deltaTime = static_cast<double>(info.stop_data - info.start_data) / info.disjoint_data.Frequency;
		if (m_recent_count > 0)
			m_delta_times.push_back(deltaTime);
		m_accum_time += deltaTime;
		m_accum_count++;
		if (m_delta_times.size() > m_recent_count)
		{
			m_accum_time -= m_delta_times.front();
			m_delta_times.pop_front();
		}
	}

	m_queries.pop_front();
	return deltaTime;
}

bool GpuTimer::GetQueryDataHelper(ID3D11DeviceContext* pContext, bool loopUntilDone, ID3D11Query* query, void* data, uint32_t dataSize)
{
	if (query == nullptr)
		return false;

	HRESULT hr = S_OK;
	int attempts = 0;
	do
	{
		hr = pContext->GetData(query, data, dataSize, 0);
		if (hr == S_OK)
			return true;
		attempts++;
		if (attempts > 100)
			Sleep(1);
		if (attempts > 1000)
		{
			assert(false);
			return false;
		}
	} while (loopUntilDone && (hr == S_FALSE));
	return false;
}